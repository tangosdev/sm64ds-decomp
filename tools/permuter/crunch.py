"""Local near-miss cruncher: grind the closest near-misses through the permuter for free.

Pulls the closest entries from the near-miss DB (tools/nearmiss_db.py), runs each through
the permuter on this PC (no LLM tokens), and:
  - if the permuter reaches a byte match -> oracle-verify and BANK it (src/ + matched.jsonl),
    remove from the DB;
  - else, if the permuter found a CLOSER candidate than the DB's, save that back to the DB
    (so the function inches toward done over repeated runs).

Run it whenever the PC is idle. It only ever banks oracle-verified matches, so it is safe.

Usage:
  python tools/permuter/crunch.py --max-div 8 --secs 180 --limit 20
  python tools/permuter/crunch.py --loop          # keep going over the closest pile
"""
import argparse
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(REPO / "tools"))
sys.path.insert(0, str(REPO / "tools" / "permuter"))
import import_func as IMP
import nearmiss_db as DB
import ledger as L


def best_output(work):
    """Lowest-score output-<score>-<n> dir the permuter wrote, as (score, source_text)."""
    best = None
    for od in work.glob("output-*"):
        parts = od.name.split("-")
        if len(parts) >= 2 and parts[1].isdigit():
            score = int(parts[1])
            src = od / "source.c"
            if src.exists() and (best is None or score < best[0]):
                best = (score, src.read_text())
    return best


def run_one(rec, secs, jobs):
    """Permute one DB record. Returns 'banked' | 'improved' | 'nochange'.
    Ledger writes go through ledger.bank (its own lock + under-lock dup check);
    DB writes happen under the DB lock (DB.locked)."""
    a = rec["addr"]
    found = IMP.find_func(rec["module"], int(a, 0) if isinstance(a, str) else a, rec["name"])
    if not found:
        return "nochange"
    # A //cpp near-miss whose body is C-compatible is converted to plain C so the
    # C-only permuter parser can mutate it (byte-identical, verified in permutable_base).
    base_src = IMP.permutable_base(rec["c_source"], rec["name"])
    out, name, addr, size, _ = IMP.setup_dir(found, base_src)
    # Popen + tree-kill on timeout: the permuter spawns -j worker PROCESSES that a plain
    # timeout would orphan (they pile up and eat RAM). taskkill /T kills the whole tree.
    p = subprocess.Popen(
        [sys.executable, str(IMP.PERM_DIR / "permuter.py"), str(out),
         "--stop-on-zero", "-j", str(jobs), "--quiet"],
        cwd=str(IMP.PERM_DIR), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        p.wait(timeout=secs)
    except subprocess.TimeoutExpired:
        subprocess.run(["taskkill", "/F", "/T", "/PID", str(p.pid)],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        try:
            p.wait(timeout=10)
        except Exception:
            pass
    except Exception:
        return "nochange"
    bo = best_output(out)
    if bo is None:
        return "nochange"
    score, src = bo
    div, ok = DB.evaluate(src, name, bytes.fromhex(rec["target_hex"]))
    if ok:
        st = L.bank({"addr": rec["addr"], "name": name, "size": size,
                     "module": rec["module"], "versions": ["permuter"]}, src)
        if st == "refused":
            return "nochange"
        with DB.locked():                       # dup = another shard banked it
            db = DB.load_db()
            db.pop((rec["module"], rec["addr"]), None)
            DB.save_db(db)
        return "banked"
    if div is not None and div < rec.get("divergences", 1e9):
        with DB.locked():
            db = DB.load_db()
            r = db.get((rec["module"], rec["addr"]))
            if r and div < r.get("divergences", 1e9):
                r["divergences"] = div
                r["c_source"] = src
                r["source"] = "permuter-improved"
                DB.save_db(db)
        return "improved"
    return "nochange"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max-div", type=int, default=12, help="only crunch entries this close")
    ap.add_argument("--secs", type=int, default=180, help="permuter budget per function")
    ap.add_argument("--limit", type=int, default=40)
    ap.add_argument("-j", "--jobs", type=int, default=3, help="permuter threads per function")
    ap.add_argument("--shard", default="0/1", help="i/N: this instance handles entries where idx%%N==i")
    ap.add_argument("--loop", action="store_true")
    ap.add_argument("--category", default=None,
                    help="comma list of category substrings to keep (route 'register "
                         "allocation,instruction reorder' here; structural goes to refine)")
    ap.add_argument("--skip-attempted", action="store_true",
                    help="skip names in progress/refine_attempted.txt (e.g. while a "
                         "refine batch is in flight on them)")
    ap.add_argument("--module", default=None,
                    help="comma list of modules to keep (e.g. 'arm9'). Without it the pile "
                         "is every module, so a module-specific push spends most of its "
                         "budget off-target.")
    args = ap.parse_args()
    mods = {m.strip() for m in args.module.split(",")} if args.module else None
    i, n = (int(x) for x in args.shard.split("/"))

    attempted = set()
    if args.skip_attempted:
        ap_file = REPO / "progress" / "refine_attempted.txt"
        if ap_file.exists():
            attempted = {l.strip() for l in ap_file.read_text().splitlines() if l.strip()}

    while True:
        db = DB.load_db()
        pile = sorted((r for r in db.values()
                       if r.get("divergences") is not None and 0 < r["divergences"] <= args.max_div
                       and r["name"] not in attempted
                       and (mods is None or r.get("module") in mods)),
                      key=lambda r: (r["divergences"], r["module"], str(r["addr"])))
        if args.category:
            import json as _json
            import categorize_misses as CAT
            cachep = REPO / "progress" / "nm_categories.json"
            cache = _json.loads(cachep.read_text()) if cachep.exists() else {}
            want = {c.strip().lower() for c in args.category.split(",")}

            def catof(r):
                a = r["addr"]
                key = f"{r['module']}:{int(a, 0) if isinstance(a, str) else a}:{r['divergences']}"
                if key not in cache:
                    try:
                        cache[key] = CAT.classify_entry(r["c_source"], r["name"],
                                                        bytes.fromhex(r["target_hex"]))
                    except Exception:
                        cache[key] = "error"
                return cache[key]
            pile = [r for r in pile if any(w in catof(r).lower() for w in want)]
            cachep.write_text(_json.dumps(cache))
        # skip seeds the permuter already failed at this exact divergence; a seed is
        # retried only once its stored draft improves (div drops)
        import json as _json2
        triedp = REPO / "progress" / "permuter_tried.json"
        tried = _json2.loads(triedp.read_text()) if triedp.exists() else {}
        pile = [r for r in pile if tried.get(r["name"]) != r["divergences"]]

        mine = [r for k, r in enumerate(pile) if k % n == i][:args.limit]
        if not mine:
            print(f"[shard {i}/{n}] nothing in range.")
            return
        banked = improved = 0
        for rec in mine:
            res = run_one(rec, args.secs, args.jobs)
            mark = {"banked": "CRACKED+BANKED", "improved": "improved", "nochange": "--"}[res]
            print(f"[{i}/{n}] div={rec.get('divergences'):<3} {rec['name'][:44]:46} {mark}", flush=True)
            banked += res == "banked"
            improved += res == "improved"
            if res == "nochange":
                tried[rec["name"]] = rec["divergences"]
                triedp.write_text(_json2.dumps(tried))
        print(f"[shard {i}/{n}] pass done: {banked} banked, {improved} improved.")
        if not args.loop or (banked == 0 and improved == 0):
            return


if __name__ == "__main__":
    main()
