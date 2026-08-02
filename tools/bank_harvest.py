"""Central harvest banker. Collects {name,c_source} JSONL produced by fan-out agents,
INDEPENDENTLY re-verifies each via the oracle (never trusts agent say-so), checks that
every relocation the object emits points where config/**/relocs.txt says (the byte
compare wildcards reloc slots, so a wrong same-shaped callee would otherwise pass --
--no-strict skips this), and banks the real byte-matches to src/<name>.c(pp) +
progress/matched.jsonl (tag 'harvest'). Resilient
to agents that died mid-run -- it banks whatever verified lines they saved.

Dry-run by default; pass --apply to bank.

Usage:
  python tools/bank_harvest.py --glob "C:/tmp/h3_*.jsonl"           # verify only
  python tools/bank_harvest.py --glob "C:/tmp/h*_*.jsonl" --apply   # verify + bank
"""
import argparse, json, glob, pathlib, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import swarm as S
import nearmiss_db as NM
import ledger as L
import reloc_audit as RA


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--glob", required=True, help="glob for agent result files")
    ap.add_argument("--apply", action="store_true", help="bank the verified matches")
    ap.add_argument("--dry-run", action="store_true",
                    help="no-op; dry-run is the default (kept for compatibility)")
    ap.add_argument("--no-strict", action="store_true",
                    help="skip the reloc-destination gate (bytes-only banking)")
    args = ap.parse_args()
    do_apply = args.apply and not args.dry_run

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

    # matched_set, NOT load_done: a stale NONMATCHING park must not block banking a
    # function that has since been cracked (ledger.bank drops the park on success)
    done = L.matched_set()
    banked, rejected, dup = [], [], 0
    for name, src in cands.items():
        info = NM.resolve_name(name)
        if not info:
            rejected.append((name, "resolve")); continue
        addr, size, module, thex = info
        if L.make_key(module, addr) in done:
            dup += 1; continue
        try:
            ok, obj = S.oracle_check(src, name, bytes.fromhex(thex))
        except Exception as e:
            rejected.append((name, f"exc {e}")); continue
        if not ok:
            rejected.append((name, "oracle FALSE")); continue
        if not args.no_strict:
            # the byte oracle wildcards reloc slots; refuse a candidate whose
            # relocations point somewhere other than config/**/relocs.txt records
            bad = RA.gate_wrong_dests(obj, name, L.norm_addr(addr), size, module)
            if bad is None:
                # gate_wrong_dests returns None when it could not check at all -- an
                # unknown module ID, or the symbol missing from the object. None is
                # falsy, so this used to fall through `if bad:` and bank the candidate
                # as strict-verified having verified nothing.
                rejected.append((name, "reloc-destination check could not run "
                                       f"(module {module!r} unknown, or symbol absent)"))
                continue
            if bad:
                rejected.append((name, f"WRONG-DEST reloc: {bad[0]['cand']} "
                                       f"({bad[0]['cand_addr']}) != config {bad[0]['cfg']}"))
                continue
        if do_apply:
            status = L.bank({"addr": addr, "name": name, "size": size,
                             "module": module, "versions": ["harvest"]}, src)
            if status == "dup":                    # a concurrent banker landed it
                dup += 1; continue
            if status == "refused":
                rejected.append((name, "ledger refused (see stderr)")); continue
        done.add(L.make_key(module, addr))
        banked.append(name)

    verb = "BANKED" if do_apply else "WOULD BANK"
    print(f"{verb} {len(banked)} (independently verified); dup-skipped {dup}; rejected {len(rejected)}")
    for n, why in rejected[:20]:
        print(f"   reject: {n}: {why}")
    if not do_apply and banked:
        print("(dry-run: re-run with --apply to bank)")


if __name__ == "__main__":
    main()
