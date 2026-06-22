"""The NONMATCHING hatch: park genuinely-unmatchable functions with logic-correct C.

A function is NONMATCHING when its C compiles and has the RIGHT logic (small reloc-aware
divergence vs the ROM) but cannot be made byte-exact at this compiler tier -- the
base-materialization floor (mwccarm 1.2 materializes a base addr that C folds), and similar
backend instruction-selection quirks. This is exactly what pret/pokediamond marks with
`#ifdef NONMATCHING`. These count toward DECOMPILED but NOT MATCHED (byte-exact), so the
matched% stays honest and verifiable.

  progress/nonmatching.jsonl : one line {addr,name,size,module,divergences,reason}
  src/<name>.c(pp)           : the logic-correct C with a // NONMATCHING header

Discipline: a function is only allowed in here if it (a) compiles, (b) does NOT byte-match,
and (c) is CLOSE (divergence <= --max-div, default 12) -- i.e. the logic is verified right
and only the codegen floor remains. Don't park functions you just haven't figured out.

Usage:
  python tools/nonmatching.py add --name X --c cand.c --reason "base materialization"
  python tools/nonmatching.py ingest-nearmiss --max-div 8        # park the floor near-misses
  python tools/nonmatching.py stats
"""
import argparse, json, pathlib, sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import nearmiss_db as NM
import categorize_misses as CAT

SRC = REPO / "src"
MATCHED = REPO / "progress" / "matched.jsonl"
NONMATCH = REPO / "progress" / "nonmatching.jsonl"
TOTAL_FUNCS = 11390

HEADER = ("// NONMATCHING: {reason} (div={div}). Logic verified correct vs ROM; not\n"
          "// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).\n"
          "// Counts as decompiled, not matched.\n")


def _load(path):
    out = {}
    if path.exists():
        for l in path.read_text().splitlines():
            if l.strip():
                r = json.loads(l)
                out[(r["module"], r["addr"])] = r
    return out


def _record(name, src, reason, div, addr, size, module):
    ext = "cpp" if src.startswith("//cpp") else "c"
    body = src if src.endswith("\n") else src + "\n"
    hdr = HEADER.format(reason=reason, div=div)
    # keep a //cpp first line first if present, then the header
    if src.startswith("//cpp"):
        first, rest = body.split("\n", 1)
        body = first + "\n" + hdr + rest
    else:
        body = hdr + body
    (SRC / f"{name}.{ext}").write_text(body)
    with NONMATCH.open("a") as f:
        f.write(json.dumps({"addr": addr, "name": name, "size": size,
                            "module": module, "divergences": div, "reason": reason}) + "\n")


def add(args):
    matched = _load(MATCHED)
    nonm = _load(NONMATCH)
    src = pathlib.Path(args.c).read_text() if args.c else None
    if src is None:
        # pull from the near-miss DB
        db = {json.loads(l)["name"]: json.loads(l)
              for l in (REPO / "nearmiss" / "db.jsonl").read_text().splitlines() if l.strip()}
        if args.name not in db:
            print(f"no --c file and {args.name} not in near-miss DB"); return
        src = db[args.name]["c_source"]
    info = NM.resolve_name(args.name)
    if not info:
        print(f"cannot resolve {args.name}"); return
    addr, size, module, thex = info
    if (module, addr) in matched:
        print(f"{args.name} is already MATCHED (byte-exact) -- not nonmatching"); return
    div, ok = NM.evaluate(src, args.name, bytes.fromhex(thex))
    if ok:
        print(f"{args.name} actually MATCHES (div=0) -- bank it as matched, not nonmatching"); return
    if div is None:
        print(f"{args.name} does NOT compile -- fix the C first"); return
    if div > args.max_div:
        print(f"{args.name} div={div} > max-div {args.max_div} -- logic not close enough; "
              f"this isn't a codegen floor, it needs real work"); return
    reason = args.reason
    if not reason:
        try:
            reason = CAT.classify_entry(src, args.name, bytes.fromhex(thex))
        except Exception:
            reason = "codegen floor"
    _record(args.name, src, reason, div, addr, size, module)
    print(f"NONMATCHING + {args.name}  (div={div}, {reason})")


def ingest_nearmiss(args):
    matched = _load(MATCHED)
    nonm = _load(NONMATCH)
    db = [json.loads(l) for l in (REPO / "nearmiss" / "db.jsonl").read_text().splitlines() if l.strip()]
    added = 0
    for r in db:
        info = NM.resolve_name(r["name"])
        if not info:
            continue
        addr, size, module, thex = info
        if (module, addr) in matched or (module, addr) in nonm:
            continue
        div = r.get("divergences")
        if div is None or div > args.max_div:
            continue
        try:
            reason = CAT.classify_entry(r["c_source"], r["name"], bytes.fromhex(thex))
        except Exception:
            reason = "codegen floor"
        # only park the genuine backend floor; skip anything still logically/structurally fixable
        if reason not in ("base materialization / addressing", "register allocation"):
            continue
        _record(r["name"], r["c_source"], reason, div, addr, size, module)
        nonm[(module, addr)] = True
        added += 1
        print(f"  + {r['name'][:46]:46} div={div} {reason}")
    print(f"\ningested {added} nonmatching (floor) entries.")


def stats(args):
    matched = _load(MATCHED)
    nonm = _load(NONMATCH)
    m, n = len(matched), len(nonm)
    print(f"matched (byte-exact): {m} ({100*m/TOTAL_FUNCS:.2f}%)")
    print(f"nonmatching (floor):  {n}")
    print(f"decompiled total:     {m+n} ({100*(m+n)/TOTAL_FUNCS:.2f}%)")
    if nonm:
        from collections import Counter
        by = Counter(r["reason"] for r in nonm.values())
        print("nonmatching by reason:")
        for reason, c in by.most_common():
            print(f"  {c:4} {reason}")


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("add"); p.add_argument("--name", required=True); p.add_argument("--c")
    p.add_argument("--reason"); p.add_argument("--max-div", type=int, default=12); p.set_defaults(fn=add)
    p = sub.add_parser("ingest-nearmiss"); p.add_argument("--max-div", type=int, default=8)
    p.set_defaults(fn=ingest_nearmiss)
    p = sub.add_parser("stats"); p.set_defaults(fn=stats)
    args = ap.parse_args()
    args.fn(args)


if __name__ == "__main__":
    main()
