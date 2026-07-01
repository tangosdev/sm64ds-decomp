"""Fuzzy opcode-similarity scheduler (Coddog-style).

Our existing similarity lever (worklist.py --similar, recurring.py, cluster_shapes.py)
keys on the EXACT mnemonic sequence -- two functions cluster only if their opcode
lists are identical. That misses near-lookalikes: a target one or two instructions
off from an already-matched function gets no example. This tool ranks by FUZZY
opcode similarity instead (edit-distance-style ratio over the mnemonic sequence),
so every unmatched target is paired with its closest ALREADY-MATCHED sibling even
when the shapes are not identical. Two uses:

  1. Scheduling: order a fan-out batch so functions that have a close matched
     sibling go first -- they get the best in-context example, highest hit rate.
  2. Examples: attach the top-K most-similar matched sources to each row as
     few-shot scaffolding (fuzzy, not exact-shape).

Similarity = difflib.SequenceMatcher ratio over the opcode-mnemonic token sequence
(Ratcliff-Obershelp; a stand-in for Levenshtein -- pip install python-Levenshtein to
swap in true edit distance). A cheap opcode-set Jaccard prefilter prunes the matched
pool before the O(n*m) ratio, so scoring a batch stays fast.

Usage:
  python tools/coddog.py --module ov006 --min 0x120 --max 0x180 --limit 24 --out progress/wl_coddog.jsonl
  python tools/coddog.py --explain func_ov006_021200dc          # show a target's closest matched siblings
  python tools/coddog.py --min 0x120 --max 0x180 --limit 24 --spread --out progress/wl_coddog.jsonl
"""
import argparse, difflib, json, pathlib, sys, time
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import swarm as S
import relocs as R
import modules as MOD
import sweep
import knowledge as KB
import worklist as WL

REPO = pathlib.Path(__file__).resolve().parent.parent


def opseq(tgt):
    return tuple(i.mnemonic for i in S.md.disasm(tgt, 0))


def jaccard(a, b):
    return len(a & b) / len(a | b) if (a or b) else 0.0


def load_parked():
    """(module, addr) pairs to keep out of the target pool: NONMATCHING floor and the
    near-miss backlog. nearmiss/db.jsonl is committed (shared); nonmatching.jsonl is
    local -- both are used if present."""
    parked = set()
    files = [REPO / "progress" / "nonmatching.jsonl", REPO / "nearmiss" / "db.jsonl"]
    for p in files:
        if not p.is_file():
            continue
        for line in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            if not line.strip():
                continue
            try:
                r = json.loads(line)
                a = r["addr"]
                a = int(a, 0) if isinstance(a, str) else a
                parked.add((r.get("module", "arm9"), a))
            except Exception:
                pass
    return parked


def build_corpus():
    """Return (matched, unmatched, relocs_by_mod). Each entry:
    {name,module,addr,size,ops,opset,tgt,src?}. A function counts as MATCHED (example-
    eligible) iff it has a committed src/ file -- that is the SHARED source of truth in
    the repo, unlike the gitignored matched.jsonl, so this works on any fresh clone."""
    parked = load_parked()
    matched, unmatched, relocs_by_mod = [], [], {}
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        relocs_by_mod[label] = mod
        data = mod["bin"].read_bytes()
        for name, addr, size in sweep.funcs(mod):
            tgt = data[addr - mod["base"]:addr - mod["base"] + size]
            ops = opseq(tgt)
            if not ops:
                continue
            rec = {"name": name, "module": label, "addr": addr, "size": size,
                   "ops": ops, "opset": frozenset(ops), "tgt": tgt}
            src = WL.read_src_text(name)
            if src is not None:
                rec["src"] = src
                matched.append(rec)
            elif (label, addr) not in parked and not S.is_thunk(list(S.md.disasm(tgt, 0))):
                unmatched.append(rec)
    return matched, unmatched, relocs_by_mod


def top_siblings(target, matched, k, jmin, lenlo, lenhi):
    """K most opcode-similar matched functions to `target`, fuzzy-ranked."""
    L = len(target["ops"])
    cands = []
    for m in matched:
        lm = len(m["ops"])
        if lm < L * lenlo or lm > L * lenhi:          # length-window prune
            continue
        j = jaccard(target["opset"], m["opset"])
        if j < jmin:                                  # opcode-set prune
            continue
        cands.append((j, m))
    # score survivors by the real sequence ratio (autojunk off = stable on code)
    scored = []
    for j, m in cands:
        r = difflib.SequenceMatcher(None, target["ops"], m["ops"], autojunk=False).ratio()
        scored.append((r, m))
    scored.sort(key=lambda x: -x[0])
    return scored[:k]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--module", default=None)
    ap.add_argument("--min", type=lambda x: int(x, 0), default=0x8)
    ap.add_argument("--max", type=lambda x: int(x, 0), default=0x200)
    ap.add_argument("--limit", type=int, default=24)
    ap.add_argument("--k", type=int, default=2, help="examples attached per row")
    ap.add_argument("--jmin", type=float, default=0.5, help="opcode-set Jaccard prefilter floor")
    ap.add_argument("--lenlo", type=float, default=0.55)
    ap.add_argument("--lenhi", type=float, default=1.8)
    ap.add_argument("--spread", action="store_true", help="round-robin modules instead of best-first global")
    ap.add_argument("--explain", default=None, help="print one function's closest matched siblings, then exit")
    ap.add_argument("--out", default=None, help="write worklist JSONL (fuzzy-scheduled). omit = summary only")
    args = ap.parse_args()

    t0 = time.time()
    gsyms = R.load_all_syms()
    kb = KB.build_kb()
    matched, unmatched, mods = build_corpus()
    sys.stderr.write(f"corpus: {len(matched)} matched(w/ src)  {len(unmatched)} unmatched  "
                     f"({time.time()-t0:.1f}s)\n")

    if args.explain:
        tgt = next((u for u in unmatched if u["name"] == args.explain), None)
        if not tgt:
            print(f"{args.explain}: not found among unmatched"); return
        sibs = top_siblings(tgt, matched, 8, args.jmin, args.lenlo, args.lenhi)
        print(f"{tgt['name']}  {tgt['module']}  size {tgt['size']:#x}  ({len(tgt['ops'])} ops)")
        print("closest matched siblings (fuzzy opcode ratio):")
        for r, m in sibs:
            print(f"  {r:.3f}  {m['name']:32} {m['module']:6} {m['size']:#5x}")
        return

    # target pool: size-filtered unmatched, optionally one module
    pool = [u for u in unmatched
            if args.min <= u["size"] <= args.max and (not args.module or u["module"] == args.module)]

    scored = []
    for u in pool:
        sibs = top_siblings(u, matched, args.k, args.jmin, args.lenlo, args.lenhi)
        best = sibs[0][0] if sibs else 0.0
        scored.append((best, u, sibs))
    scored.sort(key=lambda x: -x[0])

    if args.spread:
        from collections import defaultdict, deque
        buckets = defaultdict(deque)
        for row in scored:
            buckets[row[1]["module"]].append(row)
        order, out = list(buckets), []
        while len(out) < args.limit and any(buckets.values()):
            for mlabel in order:
                if buckets[mlabel]:
                    out.append(buckets[mlabel].popleft())
                    if len(out) >= args.limit:
                        break
        chosen = out
    else:
        chosen = scored[:args.limit]

    sys.stderr.write("top of schedule (best fuzzy sibling first):\n")
    for best, u, sibs in chosen[:min(len(chosen), 12)]:
        sib = f"{sibs[0][1]['name']} ({best:.2f})" if sibs else "(none)"
        sys.stderr.write(f"  {best:.3f}  {u['name']:30} {u['module']:6} {u['size']:#5x}  <- {sib}\n")

    if not args.out:
        return
    written = 0
    with open(args.out, "w") as f:
        for best, u, sibs in chosen:
            mod = mods[u["module"]]
            relocs = R.load_relocs_file(mod["relocs"])
            lines, callees, pool2 = WL.annotate(u["name"], u["addr"], u["size"], u["tgt"], relocs, gsyms)
            sigs = {c: KB.sig_for(c, kb) for c in callees}
            sigs = {c: v for c, v in sigs.items() if v}
            row = {"module": u["module"], "name": u["name"], "addr": f"0x{u['addr']:08x}",
                   "size": f"0x{u['size']:x}", "target_hex": u["tgt"].hex(),
                   "self": KB.sig_for(u["name"], kb), "callees": callees, "signatures": sigs,
                   "pool": pool2, "disasm": lines,
                   "coddog_sim": round(best, 3),
                   "siblings": [{"name": m["name"], "sim": round(r, 3)} for r, m in sibs]}
            ex = []
            for r, m in sibs:
                src = m.get("src") or WL.read_src_text(m["name"])
                if src:
                    ex.append({"name": m["name"], "c_source": src})
            if ex:
                row["examples"] = ex
            f.write(json.dumps(row) + "\n")
            written += 1
    sys.stderr.write(f"wrote {written} fuzzy-scheduled rows -> {args.out}  ({time.time()-t0:.1f}s)\n")


if __name__ == "__main__":
    main()
