"""Behavioral snapshot capture -- record per-call (inputs -> outputs) test cases
for a function from a running melonDS, to be diffed across ROM builds (retail vs
your recompiled C) as an equivalence oracle. See behavior_diff.py for the compare.

Why capture instead of live-diff two runs: runtime state (RNG, frame counters,
heap addresses) drifts between runs, so you cannot diff live execution. Instead
we record, per call: the INPUTS (r0-r3, sp, and the memory each pointer arg
points to) at entry, and the OUTPUTS (r0/r1 and the SAME memory regions re-read)
at return. Two builds are equivalent iff, for identical inputs, they produce
identical outputs (esp. memory writes -- the return value can be dead for a void
function).

Exit is caught with the lr-return trick: at entry we read lr (the return
address), set a one-shot breakpoint there, and continue; the function's own
internal returns all converge on lr. Recursion is detected and that case skipped
(v1 limitation).

For a real retail-vs-recompiled comparison, capture BOTH builds from the SAME
savestate (identical starting state) so the two runs call functions with matching
inputs -- fresh-boot timing makes cross-run input overlap unreliable for
continuously-animating code. melonDS's stub is single-session and fragile under
heavy breakpoint churn, so capture a few functions per session and restart
melonDS between sessions (cases are flushed per-function as they complete).

Usage (melonDS running with a ROM/savestate loaded, stub on 3333):
  python tools/trace/behavior.py --names NormalizeVec3,MulMat4x3Mat4x3 \
      --cases 20 --out behavior/retail
"""
import argparse
import json
import pathlib
import sys
import time

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from rsp import RspClient, RspError  # noqa: E402
import bplist  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parents[2]
MAIN_RAM = (0x02000000, 0x02400000)


def looks_like_ptr(v):
    return MAIN_RAM[0] <= v < MAIN_RAM[1]


def _ptr_args(regs):
    """Which of r0-r3 look like RAM pointers -> {reg: addr}."""
    return {f"r{i}": regs[f"r{i}"] for i in range(4) if looks_like_ptr(regs[f"r{i}"])}


def _read_regions(cli, ptrs, window):
    out = {}
    for reg, addr in ptrs.items():
        try:
            out[reg] = {"addr": addr, "hex": cli.read_mem(addr, window).hex()}
        except RspError:
            pass
    return out


def capture_one(cli, tgt, n_cases, window, per_case_timeout=4.0):
    """Capture up to n_cases (inputs->outputs) for one function.

    Assumes ONLY tgt's entry breakpoint is currently armed. Uses a temporary
    lr breakpoint per call to catch the return.
    """
    entry = tgt["addr"]
    canary = tgt["canary"]
    cases = []
    skipped_recursive = 0
    while len(cases) < n_cases:
        # --- wait for an entry hit (canary-clean) ---
        try:
            cli.wait_for_stop()
        except (RspError, OSError):
            break  # no more hits within the socket timeout
        regs = cli.read_registers()
        if regs["pc"] != entry:
            cli.cont()
            continue
        try:
            if cli.read_mem(entry, 8).hex() != canary:
                cli.cont()  # aliased overlay
                continue
        except RspError:
            cli.cont()
            continue

        in_ptrs = _ptr_args(regs)
        case = {
            "in": {k: regs[k] for k in ("r0", "r1", "r2", "r3", "sp")},
            "in_mem": _read_regions(cli, in_ptrs, window),
            "lr": regs["lr"],
        }
        lr = regs["lr"]
        cli.set_breakpoint(lr)
        # --- run to return ---
        deadline = time.time() + per_case_timeout
        got = False
        recursed = False
        while time.time() < deadline:
            cli.cont()
            try:
                cli.wait_for_stop()
            except (RspError, OSError):
                break
            r = cli.read_registers()
            if r["pc"] == lr:
                case["out"] = {"r0": r["r0"], "r1": r["r1"]}
                case["out_mem"] = _read_regions(cli, in_ptrs, window)
                got = True
                break
            if r["pc"] == entry:
                recursed = True  # re-entered before returning: v1 can't pair this
                break
            # some other breakpoint (shouldn't happen: only entry+lr armed)
        cli.clear_breakpoint(lr)
        if got:
            cases.append(case)
        elif recursed:
            skipped_recursive += 1
            if skipped_recursive > n_cases:
                break
        cli.cont()
    return cases, skipped_recursive


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=3333)
    ap.add_argument("--names", required=True, help="comma-separated function names")
    ap.add_argument("--cases", type=int, default=20, help="target cases per func")
    ap.add_argument("--window", type=int, default=64, help="bytes of pointer memory to snapshot")
    ap.add_argument("--out", required=True, help="output dir (e.g. behavior/retail)")
    args = ap.parse_args()

    targets, missing = bplist.from_names(args.names.split(","))
    if missing:
        print(f"  ! not in nearmiss DB: {missing}", file=sys.stderr)
    if not targets:
        return 2
    outdir = pathlib.Path(args.out)
    outdir.mkdir(parents=True, exist_ok=True)

    print(f"[*] connecting to {args.host}:{args.port} ...")
    try:
        cli = RspClient(args.host, args.port).connect()
    except OSError as e:
        print(f"[!] no GDB stub: {e} (melonDS running, stub on, ROM loaded?)", file=sys.stderr)
        return 3
    try:
        cli.read_registers()  # sanity
        cli.cont()  # kickoff: resume the target so breakpoints start firing
        for tgt in targets:
            # one function at a time: only its entry bp armed, so the lr-return
            # trick is unambiguous.
            cli.set_breakpoint(tgt["addr"])
            print(f"[*] {tgt['name']}: capturing up to {args.cases} cases ...", flush=True)
            cases, rec = capture_one(cli, tgt, args.cases, args.window)
            cli.clear_breakpoint(tgt["addr"])
            path = outdir / f"{tgt['name']}.jsonl"
            with open(path, "w", encoding="utf-8") as f:
                for c in cases:
                    f.write(json.dumps(c) + "\n")
            note = f" ({rec} recursive skipped)" if rec else ""
            print(f"    {len(cases)} cases -> {path}{note}")
    finally:
        cli.detach()
        cli.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())
