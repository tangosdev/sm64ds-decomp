"""Independently verify a fan-out's candidate wins and bank the ones that pass.

The subagents self-verify, but nothing is trusted on their say-so: every returned
candidate is re-compiled and re-checked against the ROM here before it is recorded.
This is the verify-and-bank step of the crack loop, made permanent so it is one
command instead of a hand-written script per run.

Input is the workflow result JSON (it returns {wins:[{name,c_source}, ...]}, possibly
under a top-level "result" key) plus the worklist that produced the batch (for each
function's addr/size/module/target_hex). Passing candidates are written to
src/<name>.c|cpp and appended to progress/matched.jsonl tagged "agent-llm".

Usage:
    python tools/bank.py --result <workflow-output.json> --worklist progress/wl.jsonl
    python tools/bank.py --result r.json --worklist wl.jsonl --dry-run   # verify only
"""
import argparse
import json
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import swarm as S
import triage

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"
LEDGER = REPO / "progress" / "matched.jsonl"
NEARMISS = REPO / "progress" / "nearmiss.jsonl"


def load_wins(result_path):
    obj = json.loads(pathlib.Path(result_path).read_text())
    inner = obj.get("result", obj)
    # accept both schemas: older "wins" and the current fan-out's "matches"
    wins = inner.get("wins") or inner.get("matches")
    if wins is None and isinstance(obj, list):
        wins = obj
    return wins or []


def load_near_misses(result_path):
    o = json.loads(pathlib.Path(result_path).read_text())
    o = o.get("result", o)
    return o.get("nearMisses") or o.get("near_misses") or []


def load_meta(worklist_path):
    meta = {}
    for line in pathlib.Path(worklist_path).read_text().splitlines():
        if not line.strip():
            continue
        r = json.loads(line)
        meta[r["name"]] = (int(r["addr"], 16), int(r["size"], 16),
                           r["module"], bytes.fromhex(r["target_hex"]))
    return meta


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--result", required=True)
    ap.add_argument("--worklist", required=True)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    wins = load_wins(args.result)
    meta = load_meta(args.worklist)
    print(f"candidates returned: {len(wins)}")

    passed, rejects = [], []
    for w in wins:
        name, src = w["name"], w["c_source"]
        if name not in meta:
            rejects.append((name, "not-in-worklist"))
            continue
        addr, size, mod, tb = meta[name]
        try:
            ok = S.oracle_ok(src, name, tb)
        except Exception as e:
            ok = False
            rejects.append((name, f"error:{str(e)[:40]}"))
            continue
        (passed if ok else rejects).append((name, addr, size, mod, src) if ok
                                           else (name, "oracle-False"))

    print(f"VERIFIED {len(passed)}/{len(wins)}")
    for r in rejects:
        print(f"  REJECT {r[0]}  {r[1]}")
    if args.dry_run:
        print("(dry-run: nothing banked)")
        return

    banked_names = {p[0] for p in passed}
    for name, addr, size, mod, src in passed:
        ext = "cpp" if src.startswith("//cpp") else "c"
        (SRC / f"{name}.{ext}").write_text(src if src.endswith("\n") else src + "\n")
        with LEDGER.open("a") as f:
            f.write(json.dumps({"addr": f"0x{addr:08x}", "name": name, "size": size,
                                "module": mod, "versions": ["agent-llm"]}) + "\n")
    print(f"banked {len(passed)}")

    # Near-misses: the agents' closest COMPILING attempts that did not byte-match.
    # Keep the ones that genuinely compile and don't already match, and write them as
    # permuter seeds (tools/permuter/batch.py --seeds) -- the permuter finishes the
    # coloring/ordering for free. (If a "near-miss" actually matches, bank it as a bonus.)
    near = load_near_misses(args.result)
    seeds, bonus = [], 0
    for nm in near:
        name, src = nm.get("name"), nm.get("c_source")
        if not name or not src or name not in meta or name in banked_names:
            continue
        addr, size, mod, tb = meta[name]
        try:
            if S.oracle_ok(src, name, tb):           # actually a match the agent under-reported
                ext = "cpp" if src.startswith("//cpp") else "c"
                (SRC / f"{name}.{ext}").write_text(src if src.endswith("\n") else src + "\n")
                with LEDGER.open("a") as f:
                    f.write(json.dumps({"addr": f"0x{addr:08x}", "name": name,
                                        "size": size, "module": mod,
                                        "versions": ["agent-llm"]}) + "\n")
                banked_names.add(name)
                bonus += 1
                continue
            code, _ = triage.compile_candidate(src, name)   # keep only if it compiles
            if code is None:
                continue
        except Exception:
            continue
        seeds.append({"module": mod, "addr": f"0x{addr:08x}", "name": name,
                      "size": size, "c_source": src})
    if bonus:
        print(f"banked {bonus} extra from near-misses that actually matched")
    if near:
        NEARMISS.write_text("".join(json.dumps(s) + "\n" for s in seeds))
        print(f"near-misses: {len(near)} returned, {len(seeds)} compiling seeds -> {NEARMISS.name}")
        if seeds:
            print(f"  permute them: python tools/permuter/batch.py --seeds {NEARMISS} --secs 120")


if __name__ == "__main__":
    main()
