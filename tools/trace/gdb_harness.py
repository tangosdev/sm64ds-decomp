"""Phase-0 spike: attach to a melonDS/DeSmuME GDB stub, break at known-address
functions, and prove attach + breakpoint + canary + register/memory dump work
end-to-end. See notes/emu-trace-build.md section 5.

This is a SPIKE, not the collector. It targets always-resident arm9 functions
(ground truth: we know their address and their first bytes from the nearmiss DB)
so the overlay-residency variable is removed. Its job is to answer:
  1. Does the register-packet layout parse to r0..r15 (+cpsr) correctly?
  2. Does the canary (first 8 bytes == target_hex[:16]) hold for a resident
     func, and reject a bogus one?
  3. What is the breakpoint-hit throughput (hits/sec)?  Gate: >= 5 hits/sec.

Usage (melonDS running with GdbEnabled=1, GdbPortARM9=3333, JIT_Enable=0, a
savestate loaded so the game is actually executing):

  python tools/trace/gdb_harness.py --duration 20
  python tools/trace/gdb_harness.py --names func_02043288,func_02058568
  python tools/trace/gdb_harness.py --negative 0x02043288   # canary-reject test
"""
import argparse
import json
import pathlib
import sys
import time

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from rsp import RspClient, RspError  # noqa: E402
import symindex  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parents[2]
MAIN_RAM = (0x02000000, 0x02400000)

# Default ground-truth targets: small always-resident arm9 funcs from nearmiss DB.
DEFAULT_NAMES = [
    "func_02043288", "func_0204335c", "func_0204322c",
    "func_02058568", "DMASyncFillTransfer", "func_0206e3dc",
]


def load_targets(names):
    """Join requested names against the nearmiss DB for addr + canary bytes."""
    db = REPO / "nearmiss" / "db.jsonl"
    by_name = {}
    for line in db.read_text(encoding="utf-8", errors="replace").splitlines():
        if not line.strip():
            continue
        try:
            r = json.loads(line)
        except json.JSONDecodeError:
            continue
        by_name[r["name"]] = r
    targets = []
    for n in names:
        r = by_name.get(n)
        if not r:
            print(f"  ! {n}: not in nearmiss DB, skipping", file=sys.stderr)
            continue
        targets.append({
            "name": n,
            "addr": int(str(r["addr"]), 16),
            "size": r.get("size"),
            "module": r.get("module", "arm9"),
            "canary": r["target_hex"][:16].lower(),
        })
    return targets


def load_all_arm9():
    """Every arm9 (always-resident) nearmiss func -- maximizes hit coverage."""
    db = REPO / "nearmiss" / "db.jsonl"
    out = []
    for line in db.read_text(encoding="utf-8", errors="replace").splitlines():
        if not line.strip():
            continue
        try:
            r = json.loads(line)
        except json.JSONDecodeError:
            continue
        if r.get("module", "arm9") != "arm9":
            continue
        out.append({
            "name": r["name"],
            "addr": int(str(r["addr"]), 16),
            "size": r.get("size"),
            "module": "arm9",
            "canary": r["target_hex"][:16].lower(),
        })
    return out


def looks_like_ptr(v):
    return MAIN_RAM[0] <= v < MAIN_RAM[1]


def dump_hit(cli, tgt, syms, deref_len=64):
    regs = cli.read_registers()
    # canary: read first 8 bytes at the function address
    raw = cli.read_mem(tgt["addr"], 8)
    canary_seen = raw.hex()
    canary_ok = canary_seen == tgt["canary"]
    stack = []
    try:
        sp = regs["sp"]
        stack = [w for w in _words(cli.read_mem(sp, 32))]
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
        "canary_ok": canary_ok,
        "canary_seen": canary_seen,
        "pc": regs["pc"],
        "regs": {k: regs[k] for k in ("r0", "r1", "r2", "r3", "sp", "lr", "pc")},
        "cpsr": regs["cpsr"],
        "cpsr_src": regs["cpsr_src"],
        "reg_words": len(regs["words"]),
        "caller": syms.resolve(regs["lr"]),
        "stack": stack,
        "derefs": derefs,
    }


def _words(b):
    return [int.from_bytes(b[i:i + 4], "little") for i in range(0, len(b) - 3, 4)]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=3333)
    ap.add_argument("--names", default=None, help="comma-separated func names")
    ap.add_argument("--arm9-all", action="store_true",
                    help="target every arm9 nearmiss func (good for throughput/coverage)")
    ap.add_argument("--duration", type=float, default=20.0, help="seconds to run")
    ap.add_argument("--negative", default=None,
                    help="addr (hex) to bp with a deliberately wrong canary test")
    ap.add_argument("--out", default=None, help="write hit records as JSONL here")
    args = ap.parse_args()

    if args.names:
        targets = load_targets(args.names.split(","))
    elif args.arm9_all:
        targets = load_all_arm9()
        print(f"[*] loaded all {len(targets)} arm9 nearmiss funcs as targets")
    else:
        targets = load_targets(DEFAULT_NAMES)
    if not targets:
        print("no valid targets", file=sys.stderr)
        return 2
    by_addr = {t["addr"]: t for t in targets}
    syms = symindex.get()

    print(f"[*] connecting to {args.host}:{args.port} ...")
    try:
        cli = RspClient(args.host, args.port).connect()
    except OSError as e:
        print(f"[!] could not connect to a GDB stub on {args.host}:{args.port}: {e}\n"
              f"    Is melonDS running with the stub enabled (GdbEnabled=1, "
              f"GdbPortARM9={args.port}, JIT_Enable=0) and a ROM/savestate loaded?",
              file=sys.stderr)
        return 3
    try:
        # The melonDS target is RUNNING when we attach and its stub is
        # single-session. We do NOT halt it (sending 0x03 resets the melonDS
        # Windows stub) -- breakpoints are set while running, and each bp HIT
        # stops the target on its own. Read regs once (while running) just to
        # report the detected register-packet layout.
        r0 = cli.read_registers()
        print(f"[*] attached (running). register packet: {len(r0['words'])} words; "
              f"pc={r0['pc']:#010x} cpsr={r0['cpsr']:#010x} (src={r0['cpsr_src']})")

        armed = 0
        for t in targets:
            if cli.set_breakpoint(t["addr"]):
                armed += 1
            else:
                print(f"  ! failed to arm bp at {t['name']} {t['addr']:#010x}",
                      file=sys.stderr)
        if args.negative:
            neg = int(args.negative, 16)
            cli.set_breakpoint(neg)
            print(f"[*] negative-canary bp armed at {neg:#010x}")
        print(f"[*] armed {armed}/{len(targets)} breakpoints; running "
              f"{args.duration:.0f}s ...")

        out_fp = open(args.out, "w", encoding="utf-8") if args.out else None
        hits = 0
        clean = 0
        rejected = 0
        per_func = {}
        t_start = time.time()
        cli.cont()
        while time.time() - t_start < args.duration:
            try:
                stop = cli.wait_for_stop()
            except (RspError, OSError) as e:
                print(f"[!] stop wait ended: {e}", file=sys.stderr)
                break
            regs = cli.read_registers()
            pc = regs["pc"]
            tgt = by_addr.get(pc)
            hits += 1
            if tgt is None:
                # could be the negative bp or an unexpected stop
                rec = {"name": "?", "pc": pc, "canary_ok": False, "unexpected": True}
            else:
                rec = dump_hit(cli, tgt, syms)
                per_func[tgt["name"]] = per_func.get(tgt["name"], 0) + 1
                if rec["canary_ok"]:
                    clean += 1
                else:
                    rejected += 1
            if out_fp:
                out_fp.write(json.dumps(rec) + "\n")
            if hits <= 12:
                _print_rec(rec)
            cli.cont()

        elapsed = time.time() - t_start
        if out_fp:
            out_fp.close()

        print("\n" + "=" * 60)
        print(f"[=] elapsed:        {elapsed:.1f}s")
        print(f"[=] total hits:     {hits}  ({hits / elapsed:.1f}/s)")
        print(f"[=] canary clean:   {clean}")
        print(f"[=] canary rejected:{rejected}")
        print(f"[=] per-func hits:  {per_func}")
        print("[=] GATE throughput >=5/s:  "
              + ("PASS" if hits / elapsed >= 5 else "FAIL (see pivot in build plan)"))
        print("[=] GATE canary:            "
              + ("PASS (had clean hits)" if clean else "no clean hits -- check savestate/targets"))
    finally:
        # Clean teardown: `D` (detach) makes the stub drop its breakpoints and
        # resume. No 0x03 break (that resets the melonDS Windows stub). Errors
        # here are harmless -- we're exiting, and melonDS is restarted between
        # trace sessions anyway (to cycle savestates).
        cli.detach()
        cli.close()
    return 0


def _print_rec(rec):
    if rec.get("unexpected"):
        print(f"  hit @ {rec['pc']:#010x}  <unexpected / negative bp>")
        return
    reg = rec["regs"]
    mark = "OK " if rec["canary_ok"] else "XX "
    caller = rec.get("caller") or "?"
    print(f"  {mark}{rec['name']:<28} r0={reg['r0']:#010x} r1={reg['r1']:#010x} "
          f"lr={reg['lr']:#010x}<-{caller} derefs={list(rec.get('derefs', {}))}")


if __name__ == "__main__":
    sys.exit(main())
