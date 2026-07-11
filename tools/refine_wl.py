"""Export the closest FIXABLE near-misses from nearmiss/db.jsonl into a fan-out
worklist for the refine specialist (tools/refine_run.js).

Routing (validated, see notes/crack-loop-runbook.md): structural miss categories go
to the refine agents; "register allocation" / "instruction reorder" go to the
permuter; "base materialization / addressing" is the compiler floor and is skipped.
Classification compiles each candidate, so results are cached in
progress/nm_categories.json keyed by (module, addr, divergences) - a re-ingested
better draft reclassifies automatically.

  python tools/refine_wl.py --max-div 6 --limit 20 --out progress/wl_refine.jsonl

Rows come out in the same shape as the coddog worklist, so abrow.py / abverify.py /
bank_run.py all work against them with --wl progress/wl_refine.jsonl. Each row also
carries the stored draft (abrow prints it) plus its category and divergence count.
Selected names are appended to progress/refine_attempted.txt so a later export skips
them (one refine shot per draft; improved drafts land back in the DB for the
permuter / hand-fix tiers).
"""
import argparse, json, pathlib, sys
REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import categorize_misses as CAT
import knowledge as KB
import ledger as L
import modules as MOD
import relocs as R
import worklist as WL

REFINE_CATS = {
    "different op / idiom", "missing logic (ROM does more)",
    "extra logic (you do more)", "constant / value", "predicate vs branch",
    "push-set / frame",
}
CACHE = REPO / "progress" / "nm_categories.json"
ATTEMPTED = REPO / "progress" / "refine_attempted.txt"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max-div", type=int, default=6)
    ap.add_argument("--limit", type=int, default=20)
    ap.add_argument("--min-size", default=None,
                    help="only near-misses with size >= this (hex ok, e.g. 0x100); "
                         "keeps parallel workers on disjoint size bands")
    ap.add_argument("--max-size", default=None,
                    help="only near-misses with size < this (hex ok, e.g. 0x100)")
    ap.add_argument("--out", default=str(REPO / "progress" / "wl_refine.jsonl"))
    ap.add_argument("--include-all-cats", action="store_true",
                    help="skip category routing (take everything under --max-div)")
    ap.add_argument("--only-category", default=None,
                    help="substring filter: take ONLY drafts whose category matches "
                         "(e.g. 'base materialization' for a 6g-trigger recheck batch)")
    ap.add_argument("--include-attempted", action="store_true",
                    help="do not skip names in refine_attempted.txt (mass sweeps by a "
                         "DIFFERENT model tier that never saw them)")
    args = ap.parse_args()

    rows = [json.loads(l) for l in (REPO / "nearmiss" / "db.jsonl")
            .read_text(encoding="utf-8").splitlines() if l.strip()]
    for r in rows:                       # addr/size are hex strings in some records
        r["addr"] = int(r["addr"], 0) if isinstance(r["addr"], str) else r["addr"]
        r["size"] = int(r["size"], 0) if isinstance(r["size"], str) else r["size"]

    min_size = int(args.min_size, 0) if args.min_size is not None else None
    max_size = int(args.max_size, 0) if args.max_size is not None else None

    parked = L.nonmatching_set()
    attempted = set()
    if ATTEMPTED.exists() and not args.include_attempted:
        attempted = {l.strip() for l in ATTEMPTED.read_text().splitlines() if l.strip()}

    def unmatched(name):
        # a src file only disqualifies a candidate if it is a real byte-match;
        # a "// NONMATCHING" hatch (PR #84 draft bank) is still fair game
        text = WL.read_src_text(name)
        return text is None or "NONMATCHING" in text[:200]

    pool = [r for r in rows
            if r.get("divergences") and 0 < r["divergences"] <= args.max_div
            and r["name"] not in attempted
            and (r["module"], r["addr"]) not in parked
            and (min_size is None or r["size"] >= min_size)
            and (max_size is None or r["size"] < max_size)
            and unmatched(r["name"])]
    pool.sort(key=lambda r: r["divergences"])

    cache = json.loads(CACHE.read_text()) if CACHE.exists() else {}
    chosen, checked = [], 0
    for r in pool:
        if len(chosen) >= args.limit:
            break
        checked += 1
        key = f"{r['module']}:{r['addr']}:{r['divergences']}"
        cat = cache.get(key)
        if cat is None:
            try:
                cat = CAT.classify_entry(r["c_source"], r["name"],
                                         bytes.fromhex(r["target_hex"]))
            except Exception:
                cat = "error"
            cache[key] = cat
        r["category"] = cat
        if args.only_category:
            if args.only_category.lower() in cat.lower():
                chosen.append(r)
        elif args.include_all_cats or cat in REFINE_CATS:
            chosen.append(r)
    CACHE.parent.mkdir(exist_ok=True)
    CACHE.write_text(json.dumps(cache))
    sys.stderr.write(f"pool {len(pool)} candidates, classified {checked}, "
                     f"chose {len(chosen)} refine-routable\n")
    if not chosen:
        sys.stderr.write("nothing to refine at this --max-div\n")
        return

    gsyms = R.load_all_syms()
    kb = KB.build_kb()
    mods = {("arm9" if m["name"] == "main" else m["name"]): m for m in MOD.modules()}
    with open(args.out, "w", encoding="utf-8") as f:
        for r in chosen:
            mod = mods[r["module"]]
            relocs = R.load_relocs_file(mod["relocs"])
            tgt = bytes.fromhex(r["target_hex"])
            lines, callees, poolsyms = WL.annotate(r["name"], r["addr"], r["size"],
                                                   tgt, relocs, gsyms)
            sigs = {c: v for c in callees if (v := KB.sig_for(c, kb))}
            f.write(json.dumps({
                "module": r["module"], "name": r["name"],
                "addr": f"0x{r['addr']:08x}", "size": f"0x{r['size']:x}",
                "target_hex": r["target_hex"], "self": KB.sig_for(r["name"], kb),
                "callees": callees, "signatures": sigs, "pool": poolsyms,
                "disasm": lines, "draft": r["c_source"],
                "draft_divergences": r["divergences"], "category": r["category"],
            }) + "\n")
    if not args.include_attempted:  # off-tier mass sweeps must not consume the pool
        with ATTEMPTED.open("a", encoding="utf-8") as f:
            for r in chosen:
                f.write(r["name"] + "\n")
    names = [r["name"] for r in chosen]
    sys.stderr.write(f"wrote {len(names)} rows -> {args.out}\n")
    print(f'Workflow({{ scriptPath: "tools/refine_run.js", args: {json.dumps(names)} }})')


if __name__ == "__main__":
    main()
