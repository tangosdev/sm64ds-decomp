"""Behavioral equivalence oracle: diff two sets of captured (inputs->outputs)
cases (behavior.py output) for the same function across two ROM builds -- e.g.
retail vs your recompiled C.

Method: index each side's cases by an INPUT signature (r0-r3 + the bytes of every
pointer-arg memory region at entry). For every input present in BOTH builds,
compare the OUTPUTS:
  - memory writes: the delta (in_mem -> out_mem) for each pointer region. This is
    the robust, codegen-independent behavioral signal.
  - return value r0/r1: reported, but flagged "weak" -- r0 is dead for void funcs,
    so an r0-only difference is not conclusive.

Verdict per function:
  EQUIVALENT   -- every shared input produced identical memory writes
  DIVERGENT    -- some shared input produced different memory writes (a real bug)
  INCONCLUSIVE -- no shared inputs (drive matching scenes / capture more cases)

Usage:
  python tools/trace/behavior_diff.py behavior/retail behavior/recompiled
  python tools/trace/behavior_diff.py behavior/retail behavior/retail2 --name NormalizeVec3
"""
import argparse
import json
import pathlib
import sys


def load(path, name):
    p = pathlib.Path(path) / f"{name}.jsonl"
    if not p.is_file():
        return []
    return [json.loads(l) for l in p.read_text(encoding="utf-8").splitlines() if l.strip()]


def input_sig(case):
    r = case["in"]
    parts = [f"{r['r0']:08x}{r['r1']:08x}{r['r2']:08x}{r['r3']:08x}"]
    for reg in ("r0", "r1", "r2", "r3"):
        m = case.get("in_mem", {}).get(reg)
        if m:
            parts.append(f"{reg}:{m['hex']}")
    return "|".join(parts)


def mem_writes(case):
    """{reg: (before_hex, after_hex)} for pointer regions that CHANGED between
    entry and return -- i.e. what the function wrote through its args."""
    out = {}
    inm = case.get("in_mem", {})
    outm = case.get("out_mem", {})
    for reg in ("r0", "r1", "r2", "r3"):
        b = inm.get(reg, {}).get("hex")
        a = outm.get(reg, {}).get("hex")
        if b is not None and a is not None and b != a:
            out[reg] = (b, a)
    return out


def diff_one(name, a_cases, b_cases):
    a_by = {}
    for c in a_cases:
        a_by.setdefault(input_sig(c), c)
    shared = 0
    mem_divergent = []
    ret_divergent = []
    for c in b_cases:
        sig = input_sig(c)
        if sig not in a_by:
            continue
        shared += 1
        ca = a_by[sig]
        wa, wb = mem_writes(ca), mem_writes(c)
        if wa != wb:
            mem_divergent.append((sig, wa, wb))
        elif ca.get("out", {}).get("r0") != c.get("out", {}).get("r0") or \
                ca.get("out", {}).get("r1") != c.get("out", {}).get("r1"):
            ret_divergent.append((sig, ca.get("out"), c.get("out")))
    if shared == 0:
        verdict = "INCONCLUSIVE"
    elif mem_divergent:
        verdict = "DIVERGENT"
    else:
        verdict = "EQUIVALENT"
    return {
        "name": name, "verdict": verdict, "shared_inputs": shared,
        "a_cases": len(a_cases), "b_cases": len(b_cases),
        "mem_divergent": mem_divergent, "ret_divergent": ret_divergent,
    }


def func_names(a_dir, b_dir):
    names = set()
    for d in (pathlib.Path(a_dir), pathlib.Path(b_dir)):
        if d.is_dir():
            names.update(p.stem for p in d.glob("*.jsonl"))
    return sorted(names)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dir_a", help="baseline captures (e.g. behavior/retail)")
    ap.add_argument("dir_b", help="candidate captures (e.g. behavior/recompiled)")
    ap.add_argument("--name", default=None, help="only this function")
    ap.add_argument("--verbose", action="store_true", help="show diverging bytes")
    args = ap.parse_args()

    names = [args.name] if args.name else func_names(args.dir_a, args.dir_b)
    if not names:
        print("no captures found", file=sys.stderr)
        return 2

    tally = {"EQUIVALENT": 0, "DIVERGENT": 0, "INCONCLUSIVE": 0}
    for name in names:
        r = diff_one(name, load(args.dir_a, name), load(args.dir_b, name))
        tally[r["verdict"]] += 1
        mark = {"EQUIVALENT": "OK ", "DIVERGENT": "XX ", "INCONCLUSIVE": "-- "}[r["verdict"]]
        print(f"{mark}{name:40} {r['verdict']:12} "
              f"shared={r['shared_inputs']:3} (a={r['a_cases']} b={r['b_cases']})")
        if r["mem_divergent"]:
            print(f"      {len(r['mem_divergent'])} input(s) produced different MEMORY WRITES:")
            for sig, wa, wb in r["mem_divergent"][:5]:
                for reg in sorted(set(wa) | set(wb)):
                    print(f"        {reg}: baseline {wa.get(reg,('-','-'))[1][:32]}"
                          f" != candidate {wb.get(reg,('-','-'))[1][:32]}"
                          if args.verbose else f"        {reg}: write differs")
        elif r["ret_divergent"] and args.verbose:
            print(f"      note: {len(r['ret_divergent'])} r0/r1 differ but memory matches "
                  f"(weak signal -- likely dead retval on a void fn)")
    print(f"\n[=] {tally['EQUIVALENT']} equivalent, {tally['DIVERGENT']} DIVERGENT, "
          f"{tally['INCONCLUSIVE']} inconclusive")
    return 1 if tally["DIVERGENT"] else 0


if __name__ == "__main__":
    sys.exit(main())
