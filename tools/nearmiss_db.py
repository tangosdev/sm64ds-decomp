"""Persistent near-miss database: never throw away a close attempt again.

Every fan-out emits "near_misses" -- compiling C that didn't byte-match, usually only a
few instructions off (codegen shape). Those are the most valuable byproduct we have:
logically-correct, already-compiling, often 1-2 fixes from done. This stores them forever,
keyed by (module, addr), keeping the CLOSEST candidate per function (fewest diverging
instructions). When a better model or a new idiom lands, we start from 95%-done, not scratch.

The DB lives at nearmiss/db.jsonl (committed, so the work survives). Each record:
  {module, addr, name, size, target_hex, lang, divergences, c_source, source}

Usage:
  python tools/nearmiss_db.py ingest --result <fanout-output.json> --worklist progress/wl.jsonl
  python tools/nearmiss_db.py ingest --seeds <seeds.jsonl>
  python tools/nearmiss_db.py stats
  python tools/nearmiss_db.py list --max-div 12
  python tools/nearmiss_db.py export-close --max-div 8 --out progress/close.jsonl   # permuter seeds
  python tools/nearmiss_db.py bank-matches      # re-check every entry; bank any that now score 0
"""
import argparse
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import match as M
import swarm as S

DB = REPO / "nearmiss" / "db.jsonl"
SRC = REPO / "src"
LEDGER = REPO / "progress" / "matched.jsonl"
LOCKDIR = REPO / "nearmiss" / ".lock"
REG = re.compile(r"\b(r\d+|sb|sl|fp|ip|sp|lr|pc)\b")


def norm_addr(a):
    """Canonical '0x%08x' form for an addr that may arrive as int or hex string.
    Workflow results and older DB rows mixed the two, which made (module, addr)
    keys silently miss each other."""
    return f"0x{(int(a, 0) if isinstance(a, str) else int(a)):08x}"


class locked:
    """Cross-process mutex (atomic mkdir) so multiple cruncher instances can safely
    read-modify-write the DB and ledger. Hold it only for the brief write, never while
    permuting."""
    def __enter__(self):
        import time, os
        LOCKDIR.parent.mkdir(parents=True, exist_ok=True)
        for _ in range(1200):
            try:
                os.mkdir(LOCKDIR)
                return self
            except FileExistsError:
                time.sleep(0.1)
        raise TimeoutError("could not acquire nearmiss db lock")

    def __exit__(self, *a):
        import os
        try:
            os.rmdir(LOCKDIR)
        except OSError:
            pass


def _disasm(code, relocs):
    out = []
    for ins in S.code_insns(list(S.md.disasm(code, 0))):
        if ins.address in relocs:
            out.append("reloc")
        else:
            op = re.sub(r"\[pc, #-?0x[0-9a-fA-F]+\]", "[pc,#pool]", ins.op_str)
            out.append(f"{ins.mnemonic} {op}".rstrip())
    return out


def evaluate(src, name, target):
    """Return (divergences, ok) for a candidate vs the target bytes. divergences is the
    count of differing instructions (reloc-wildcarded); ok is a true RELOC-AWARE oracle
    match (swarm.oracle_ok -- NOT exact byte-equality, since reloc slots are wildcarded).
    Returns (None, False) if it does not compile or the function is absent."""
    import tempfile, os, difflib
    cpp = src.startswith("//cpp")
    try:
        ok = S.oracle_ok(src, name, target)        # definitive, reloc-aware
    except Exception:
        ok = False
    if ok:
        return 0, True
    fd, tmp = tempfile.mkstemp(suffix=".cpp" if cpp else ".c")
    os.close(fd)
    pathlib.Path(tmp).write_text(src)
    try:
        obj = M.compile_c(pathlib.Path(tmp), M.CANONICAL, S.CPP_FLAGS if cpp else M.DEFAULT_FLAGS)
    finally:
        pathlib.Path(tmp).unlink(missing_ok=True)
    if obj is None:
        return None, False
    cand, crel = M.extract_func(obj, name)
    if cand is None:
        return None, False
    crel = crel or set()
    c = _disasm(cand, crel)
    t = _disasm(target, crel)
    sm = difflib.SequenceMatcher(a=c, b=t, autojunk=False)
    div = sum(max(i2 - i1, j2 - j1) for op, i1, i2, j1, j2 in sm.get_opcodes() if op != "equal")
    return div, False


def load_db():
    db = {}
    if DB.exists():
        for l in DB.read_text(encoding="utf-8").splitlines():
            if l.strip():
                r = json.loads(l)
                r["addr"] = norm_addr(r["addr"])
                db[(r["module"], r["addr"])] = r
    return db


def save_db(db):
    """Atomic rewrite (temp + replace): the DB is committed work; a crash mid-write
    must not truncate it."""
    import os
    DB.parent.mkdir(parents=True, exist_ok=True)
    items = sorted(db.values(), key=lambda r: (r.get("divergences") if r.get("divergences") is not None else 1e9))
    tmp = DB.with_suffix(".jsonl.tmp")
    tmp.write_text("".join(json.dumps(r) + "\n" for r in items), encoding="utf-8")
    os.replace(tmp, DB)


def load_meta(worklist):
    meta = {}
    if worklist and pathlib.Path(worklist).exists():
        for l in pathlib.Path(worklist).read_text().splitlines():
            if l.strip():
                r = json.loads(l)
                meta[r["name"]] = (r["addr"], int(r["size"], 16), r["module"], r["target_hex"])
    return meta


_NAME_IDX = None
_AMBIG = set()


def _build_name_idx():
    global _NAME_IDX
    if _NAME_IDX is None:
        import modules as MOD
        import sweep
        _NAME_IDX = {}
        for mod in MOD.modules():
            label = "arm9" if mod["name"] == "main" else mod["name"]
            data = mod["bin"].read_bytes()
            for fn, addr, size in sweep.funcs(mod):
                rec = (f"0x{addr:08x}", size, label,
                       data[addr - mod["base"]:addr - mod["base"] + size].hex())
                if fn in _NAME_IDX and _NAME_IDX[fn][:3] != rec[:3]:
                    _AMBIG.add(fn)
                _NAME_IDX[fn] = rec
    return _NAME_IDX


def resolve_name(name):
    """Find a function's (addr_hex, size, module, target_hex) by name across all modules.
    Lets us backfill near-misses whose original worklist is gone. For a name defined
    in more than one module this returns the LAST module's entry -- check
    is_ambiguous() before trusting the module attribution (bank paths must)."""
    return _build_name_idx().get(name)


def is_ambiguous(name):
    """True if the name is defined at different (addr, module) in multiple modules.
    Banking such a name by resolve_name() alone risks overwriting a same-named
    sibling's src/ file with a different function's source."""
    _build_name_idx()
    return name in _AMBIG


def _done_set():
    done = set()
    if LEDGER.exists():
        for l in LEDGER.read_text().splitlines():
            if l.strip():
                d = json.loads(l)
                done.add((d["module"], norm_addr(d["addr"])))
    return done


def ingest(args):
    db = load_db()
    meta = load_meta(args.worklist)
    done = _done_set()
    items = []
    if args.result:
        o = json.loads(pathlib.Path(args.result).read_text())
        o = o.get("result", o)
        for nm in (o.get("nearMisses") or o.get("near_misses") or []):
            items.append((nm.get("name"), nm.get("c_source")))
    if args.seeds:
        for l in pathlib.Path(args.seeds).read_text().splitlines():
            if l.strip():
                d = json.loads(l)
                items.append((d.get("name"), d.get("c_source")))
    added = improved = 0
    for name, src in items:
        if not name or not src:
            continue
        m = meta.get(name) or resolve_name(name)
        if m:
            addr, size, mod, thex = m
        else:
            ex = next((r for r in db.values() if r["name"] == name), None)
            if not ex:
                continue
            addr, size, mod, thex = ex["addr"], ex["size"], ex["module"], ex["target_hex"]
        addr = norm_addr(addr)
        key = (mod, addr)
        if key in done:                 # already matched -- not a pending near-miss
            db.pop(key, None)
            continue
        div, ok = evaluate(src, name, bytes.fromhex(thex))
        if div is None or ok:           # ok shouldn't happen for an unmatched func; skip if so
            continue
        cur = db.get(key)
        if cur is None or div < (cur.get("divergences") if cur.get("divergences") is not None else 1e9):
            db[key] = {"module": mod, "addr": addr, "name": name, "size": size,
                       "target_hex": thex, "lang": "cpp" if src.startswith("//cpp") else "c",
                       "divergences": div, "c_source": src, "source": args.label or "fanout"}
            if cur is None:
                added += 1
            else:
                improved += 1
    save_db(db)
    print(f"ingested: +{added} new, {improved} improved. DB now {len(db)} entries.")


def stats(args):
    db = load_db()
    ds = [r["divergences"] for r in db.values() if r.get("divergences") is not None]
    ds.sort()
    if not ds:
        print(f"DB: {len(db)} entries (none scored)")
        return
    b = {"0 (bankable!)": sum(d == 0 for d in ds), "1-4": sum(1 <= d <= 4 for d in ds),
         "5-12": sum(5 <= d <= 12 for d in ds), "13-30": sum(13 <= d <= 30 for d in ds),
         ">30": sum(d > 30 for d in ds)}
    print(f"DB: {len(db)} entries. median divergences={ds[len(ds)//2]}, min={ds[0]}")
    for k, v in b.items():
        print(f"  {k:14} {v}")


def _list(args):
    db = load_db()
    rows = sorted(db.values(), key=lambda r: (r.get("divergences") if r.get("divergences") is not None else 1e9))
    for r in rows:
        if args.max_div is not None and (r.get("divergences") or 1e9) > args.max_div:
            continue
        print(f"  div={r.get('divergences'):<4} {r['module']:6} {r['name'][:46]:46} {r['lang']}")


def export_close(args):
    db = load_db()
    out = [r for r in db.values()
           if r.get("divergences") is not None and 0 < r["divergences"] <= args.max_div]
    if args.category:
        # category-routed export (permuter wants "register allocation" / "instruction
        # reorder"); uses the classification cache refine_wl.py maintains, classifying
        # on demand for uncached entries.
        import categorize_misses as CAT
        cachep = REPO / "progress" / "nm_categories.json"
        cache = json.loads(cachep.read_text()) if cachep.exists() else {}
        want = {c.strip().lower() for c in args.category.split(",")}
        kept = []
        for r in out:
            a = r["addr"]
            key = f"{r['module']}:{int(a, 0) if isinstance(a, str) else a}:{r['divergences']}"
            cat = cache.get(key)
            if cat is None:
                try:
                    cat = CAT.classify_entry(r["c_source"], r["name"],
                                             bytes.fromhex(r["target_hex"]))
                except Exception:
                    cat = "error"
                cache[key] = cat
            if any(w in cat.lower() for w in want):
                kept.append(r)
        cachep.write_text(json.dumps(cache))
        out = kept
    out.sort(key=lambda r: r["divergences"])
    pathlib.Path(args.out).write_text(
        "".join(json.dumps({"module": r["module"], "addr": r["addr"], "name": r["name"],
                            "c_source": r["c_source"]}) + "\n" for r in out))
    print(f"exported {len(out)} close seeds (div<={args.max_div}) -> {args.out}")


def bank_matches(args):
    """Re-evaluate every entry; bank any that now byte-match (score 0)."""
    db = load_db()
    banked = skipped = 0
    for key, r in list(db.items()):
        div, ok = evaluate(r["c_source"], r["name"], bytes.fromhex(r["target_hex"]))
        if ok:
            if is_ambiguous(r["name"]):
                print(f"  SKIP {r['name']}: name exists in multiple modules -- bank by hand")
                skipped += 1
                continue
            if not getattr(args, "no_strict", False):
                import reloc_audit as RA
                _, obj = S.oracle_check(r["c_source"], r["name"], bytes.fromhex(r["target_hex"]))
                bad = RA.gate_wrong_dests(obj, r["name"], int(r["addr"], 0),
                                          r["size"], r["module"]) if obj else None
                if bad:
                    print(f"  SKIP {r['name']}: bytes match but {len(bad)} reloc "
                          f"destination(s) WRONG (e.g. {bad[0]['cand']} != {bad[0]['cfg']})")
                    skipped += 1
                    continue
            ext = r["lang"]
            (SRC / f"{r['name']}.{ext}").write_text(
                r["c_source"] if r["c_source"].endswith("\n") else r["c_source"] + "\n")
            with LEDGER.open("a") as f:
                f.write(json.dumps({"addr": r["addr"], "name": r["name"], "size": r["size"],
                                    "module": r["module"], "versions": ["nearmiss-db"]}) + "\n")
            del db[key]
            banked += 1
        elif div is not None:
            r["divergences"] = div
    save_db(db)
    print(f"banked {banked} now-matching entries; skipped {skipped}; DB now {len(db)}.")


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("ingest"); p.add_argument("--result"); p.add_argument("--seeds")
    p.add_argument("--worklist"); p.add_argument("--label"); p.set_defaults(fn=ingest)
    p = sub.add_parser("stats"); p.set_defaults(fn=stats)
    p = sub.add_parser("list"); p.add_argument("--max-div", type=int); p.set_defaults(fn=_list)
    p = sub.add_parser("export-close"); p.add_argument("--max-div", type=int, default=8)
    p.add_argument("--out", default="progress/close.jsonl")
    p.add_argument("--category", default=None,
                   help="comma list of category substrings to keep (e.g. "
                        "'register allocation,instruction reorder' for permuter seeds)")
    p.set_defaults(fn=export_close)
    p = sub.add_parser("bank-matches")
    p.add_argument("--no-strict", action="store_true",
                   help="skip the reloc-destination gate (bytes-only banking)")
    p.set_defaults(fn=bank_matches)
    args = ap.parse_args()
    args.fn(args)


if __name__ == "__main__":
    main()
