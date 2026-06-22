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
    All DB/ledger writes happen under the cross-process lock (DB.locked)."""
    import json
    found = IMP.find_func(rec["module"], int(rec["addr"], 0), rec["name"])
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
        with DB.locked():
            db = DB.load_db()
            ext = "cpp" if src.startswith("//cpp") else "c"
            (DB.SRC / f"{name}.{ext}").write_text(src if src.endswith("\n") else src + "\n")
            with DB.LEDGER.open("a") as f:
                f.write(json.dumps({"addr": rec["addr"], "name": name, "size": size,
                                    "module": rec["module"], "versions": ["permuter"]}) + "\n")
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
    args = ap.parse_args()
    i, n = (int(x) for x in args.shard.split("/"))

    while True:
        db = DB.load_db()
        pile = sorted((r for r in db.values()
                       if r.get("divergences") is not None and 0 < r["divergences"] <= args.max_div),
                      key=lambda r: (r["divergences"], r["module"], r["addr"]))
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
        print(f"[shard {i}/{n}] pass done: {banked} banked, {improved} improved.")
        if not args.loop or (banked == 0 and improved == 0):
            return


if __name__ == "__main__":
    main()
