"""Central harvest banker. Collects {name,c_source} JSONL produced by fan-out agents,
INDEPENDENTLY re-verifies each via the oracle (never trusts agent say-so), and banks the
real byte-matches to src/<name>.c(pp) + progress/matched.jsonl (tag 'harvest'). Resilient
to agents that died mid-run -- it banks whatever verified lines they saved.

Usage:
  python tools/bank_harvest.py --glob "C:/tmp/h3_*.jsonl"
  python tools/bank_harvest.py --glob "C:/tmp/h*_*.jsonl" --dry-run
"""
import argparse, json, glob, pathlib, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import swarm as S
import nearmiss_db as NM

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"
LEDGER = REPO / "progress" / "matched.jsonl"
NONMATCH = REPO / "progress" / "nonmatching.jsonl"


def done_set():
    d = set()
    for f in (LEDGER, NONMATCH):
        if f.exists():
            for l in f.read_text().splitlines():
                if l.strip():
                    r = json.loads(l)
                    d.add((r["module"], r["addr"]))
    return d


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--glob", required=True, help="glob for agent result files")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    cands = {}
    for fp in glob.glob(args.glob):
        for l in open(fp):
            l = l.strip()
            if not l:
                continue
            try:
                r = json.loads(l)
            except Exception:
                continue
            if r.get("name") and r.get("c_source"):
                cands[r["name"]] = r["c_source"]   # ranges disjoint; last wins on dup name
    print(f"collected {len(cands)} candidates from {args.glob}")

    done = done_set()
    banked, rejected, dup = [], [], 0
    for name, src in cands.items():
        info = NM.resolve_name(name)
        if not info:
            rejected.append((name, "resolve")); continue
        addr, size, module, thex = info
        if (module, addr) in done:
            dup += 1; continue
        try:
            ok = S.oracle_ok(src, name, bytes.fromhex(thex))
        except Exception as e:
            rejected.append((name, f"exc {e}")); continue
        if not ok:
            rejected.append((name, "oracle FALSE")); continue
        if not args.dry_run:
            ext = "cpp" if src.startswith("//cpp") else "c"
            (SRC / f"{name}.{ext}").write_text(src if src.endswith("\n") else src + "\n")
            with LEDGER.open("a") as f:
                f.write(json.dumps({"addr": addr, "name": name, "size": size,
                                    "module": module, "versions": ["harvest"]}) + "\n")
        done.add((module, addr))
        banked.append(name)

    verb = "WOULD BANK" if args.dry_run else "BANKED"
    print(f"{verb} {len(banked)} (independently verified); dup-skipped {dup}; rejected {len(rejected)}")
    for n, why in rejected[:20]:
        print(f"   reject: {n}: {why}")


if __name__ == "__main__":
    main()
