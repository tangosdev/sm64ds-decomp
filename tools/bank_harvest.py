"""Central harvest banker. Collects {name,c_source} JSONL produced by fan-out agents,
INDEPENDENTLY re-verifies each via the oracle (never trusts agent say-so), checks that
every relocation the object emits points at the destination config/**/relocs.txt records
(the byte compare wildcards reloc slots, so a wrong same-shaped callee would otherwise
pass -- see tools/reloc_audit.py), and banks the real matches to src/<name>.c(pp) +
progress/matched.jsonl (tag 'harvest'). Resilient to agents that died mid-run -- it
banks whatever verified lines they saved.

Usage:
  python tools/bank_harvest.py --glob "C:/tmp/h3_*.jsonl"
  python tools/bank_harvest.py --glob "C:/tmp/h*_*.jsonl" --dry-run
"""
import argparse, json, glob, pathlib, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import swarm as S
import nearmiss_db as NM
import reloc_audit as RA

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
                    d.add((r["module"], NM.norm_addr(r["addr"])))
    return d


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--glob", required=True, help="glob for agent result files")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--no-strict", action="store_true",
                    help="skip the reloc-destination gate (bytes-only banking)")
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
        if NM.is_ambiguous(name):
            # same name defined in multiple modules: resolve_name's module pick is
            # arbitrary and src/<name> would clobber the sibling -- bank by hand
            rejected.append((name, "ambiguous name (multi-module) -- bank by hand")); continue
        addr, size, module, thex = info
        if (module, addr) in done:
            dup += 1; continue
        try:
            ok, obj = S.oracle_check(src, name, bytes.fromhex(thex))
        except Exception as e:
            rejected.append((name, f"exc {e}")); continue
        if not ok:
            rejected.append((name, "oracle FALSE")); continue
        if not args.no_strict:
            bad = RA.gate_wrong_dests(obj, name, int(addr, 0), size, module)
            if bad:
                rejected.append((name, f"WRONG-DEST reloc: {bad[0]['cand']} "
                                       f"({bad[0]['cand_addr']}) != config {bad[0]['cfg']}"))
                continue
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
