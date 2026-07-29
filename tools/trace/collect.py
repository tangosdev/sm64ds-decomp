"""Phase-1 trace collector.

Attaches to a running melonDS GDB stub (see tools/trace/README.md for the
setup + the stub's quirks), arms breakpoints for a worklist of functions, and
captures N canary-clean hits per function -- entry regs, stack, pointer derefs,
and the resolved caller. Once a function reaches its hit cap its breakpoint is
removed (z0) so rarer functions in the tail still get airtime.

Output:
  traces/<module>/<name>.jsonl   one line per captured hit
  traces/coverage.jsonl          per-func {hits, alias_rejects, dark} ledger

Coverage depends on gameplay REACHING each function while its overlay is
resident -- drive the emulator (or load savestates) to cover different scenes.
Whatever isn't reached is recorded as dark, never silently dropped.

Usage (melonDS running with a ROM/savestate loaded, stub on 3333):
  python tools/trace/collect.py --wl worklist.txt --hits 5
  python tools/trace/collect.py --nearmiss-div 26 --module ov006 --hits 5
  python tools/trace/collect.py --nearmiss-div 26 --module arm9 --duration 60
"""
import argparse
import json
import pathlib
import sys
import time

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from rsp import RspClient, RspError  # noqa: E402
import bplist  # noqa: E402
import symindex  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parents[2]
TRACES = REPO / "traces"
MAIN_RAM = (0x02000000, 0x02400000)


def looks_like_ptr(v):
    return MAIN_RAM[0] <= v < MAIN_RAM[1]


def capture(cli, tgt, syms, deref_len=96, frame=None):
    """Build one hit record from the halted target (pc == tgt['addr'])."""
    regs = cli.read_registers()
    stack = []
    try:
        stack = _words(cli.read_mem(regs["sp"], 32))
    except RspError:
        pass
    derefs = {}
    for i in range(4):
        v = regs[f"r{i}"]
        if looks_like_ptr(v):
            try:
                derefs[f"r{i}"] = cli.read_mem(v, deref_len).hex()
            except RspError:
                pass
    return {
        "name": tgt["name"],
        "module": tgt["module"],
        "regs": {k: regs[k] for k in ("r0", "r1", "r2", "r3", "sp", "lr", "pc", "cpsr")},
        "caller": syms.resolve(regs["lr"]),
        "stack": stack,
        "derefs": derefs,
        "frame": frame,
    }


def _words(b):
    return [int.from_bytes(b[i:i + 4], "little") for i in range(0, len(b) - 3, 4)]


def load_targets(args):
    if args.wl:
        targets, missing = bplist.from_worklist(args.wl)
        if missing:
            print(f"  ! {len(missing)} worklist names not in nearmiss DB "
                  f"(e.g. {missing[:3]})", file=sys.stderr)
        return targets
    if args.nearmiss_div is not None or args.module:
        return bplist.from_nearmiss(min_div=args.nearmiss_div, module=args.module,
                                    limit=args.limit)
    print("specify --wl, or --nearmiss-div / --module", file=sys.stderr)
    return []


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=3333)
    ap.add_argument("--wl", default=None, help="worklist file of function names")
    ap.add_argument("--nearmiss-div", type=int, default=None,
                    help="target nearmiss funcs with divergences >= this")
    ap.add_argument("--module", default=None, help="restrict to a module (arm9/ovNNN)")
    ap.add_argument("--limit", type=int, default=None, help="cap number of targets")
    ap.add_argument("--hits", type=int, default=5, help="hits to capture per func")
    ap.add_argument("--duration", type=float, default=120.0, help="max seconds")
    ap.add_argument("--idle", type=float, default=20.0,
                    help="stop after this many seconds with no NEW func captured")
    ap.add_argument("--out", default=str(TRACES), help="output dir (default traces/)")
    args = ap.parse_args()

    targets = load_targets(args)
    if not targets:
        return 2
    by_addr = {t["addr"]: t for t in targets}
    syms = symindex.get()
    outdir = pathlib.Path(args.out)
    outdir.mkdir(parents=True, exist_ok=True)

    hits = {t["name"]: 0 for t in targets}
    alias = {t["name"]: 0 for t in targets}
    fps = {}  # name -> open file handle (lazy)

    def fp_for(t):
        if t["name"] not in fps:
            d = outdir / t["module"]
            d.mkdir(parents=True, exist_ok=True)
            fps[t["name"]] = open(d / f"{t['name']}.jsonl", "a", encoding="utf-8")
        return fps[t["name"]]

    print(f"[*] {len(targets)} targets across "
          f"{len(bplist.group_by_module(targets))} module(s); "
          f"hit-cap={args.hits}")
    print(f"[*] connecting to {args.host}:{args.port} ...")
    try:
        cli = RspClient(args.host, args.port).connect()
    except OSError as e:
        print(f"[!] no GDB stub on {args.host}:{args.port}: {e}\n"
              f"    Is melonDS running (stub on, JIT off, ROM/savestate loaded)?",
              file=sys.stderr)
        return 3

    remaining = len(targets)
    captured_total = 0
    try:
        r0 = cli.read_registers()
        print(f"[*] attached (running); pc={r0['pc']:#010x}. arming {len(targets)} bps ...")
        armed = sum(1 for t in targets if cli.set_breakpoint(t["addr"]))
        print(f"[*] armed {armed}/{len(targets)}; collecting "
              f"(<= {args.duration:.0f}s, idle-stop {args.idle:.0f}s) ...")
        cli.cont()

        t_start = time.time()
        t_last_new = t_start
        while remaining > 0 and time.time() - t_start < args.duration:
            if time.time() - t_last_new > args.idle:
                print(f"[*] idle {args.idle:.0f}s with nothing new -> stopping")
                break
            try:
                cli.wait_for_stop()
            except (RspError, OSError):
                continue  # socket read timeout between hits; keep polling
            regs = cli.read_registers()
            pc = regs["pc"]
            tgt = by_addr.get(pc)
            if tgt is None:
                cli.cont()
                continue
            # canary-gate against overlay aliasing
            seen = ""
            try:
                seen = cli.read_mem(pc, 8).hex()
            except RspError:
                pass
            if seen != tgt["canary"]:
                alias[tgt["name"]] += 1
                cli.cont()
                continue
            if hits[tgt["name"]] < args.hits:
                rec = capture(cli, tgt, syms)
                fp_for(tgt).write(json.dumps(rec) + "\n")
                hits[tgt["name"]] += 1
                captured_total += 1
                if hits[tgt["name"]] == 1:
                    t_last_new = time.time()
                if hits[tgt["name"]] >= args.hits:
                    cli.clear_breakpoint(tgt["addr"])  # tail-friendly: free the slot
                    remaining -= 1
            cli.cont()

        elapsed = time.time() - t_start
        print(f"\n[=] elapsed {elapsed:.1f}s; captured {captured_total} hits; "
              f"{len([n for n,c in hits.items() if c>0])}/{len(targets)} funcs reached")
    finally:
        for f in fps.values():
            f.close()
        cli.detach()
        cli.close()

    # coverage ledger (append-friendly: one snapshot line per run per func)
    cov = outdir / "coverage.jsonl"
    with open(cov, "a", encoding="utf-8") as c:
        for t in targets:
            c.write(json.dumps({
                "name": t["name"], "module": t["module"],
                "hits": hits[t["name"]], "alias_rejects": alias[t["name"]],
                "dark": hits[t["name"]] == 0,
            }) + "\n")
    reached = len([n for n, c in hits.items() if c > 0])
    print(f"[=] coverage: {reached}/{len(targets)} reached "
          f"({100*reached/len(targets):.0f}%); dark logged to {cov}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
