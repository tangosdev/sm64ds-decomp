"""Bank a coddog fan-out workflow result in one step: independently re-verify and
bank the matches (via bank_harvest), then park the misses into nonmatching.jsonl so
they are not re-selected. Point --output at the Workflow task's .output file.

  python tools/bank_run.py --output <task.output> [--wl progress/wl_ab.jsonl]
Then run the free post-pass:  python tools/clone.py && python tools/paramclone.py
"""
import argparse, json, pathlib, shutil, subprocess, sys, tempfile
REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import ledger as L


def find_result(x):
    if isinstance(x, dict):
        if "landed" in x and "sources" in x:
            return x
        for v in x.values():
            r = find_result(v)
            if r:
                return r
    if isinstance(x, str):
        try:
            return find_result(json.loads(x))
        except Exception:
            return None
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--output", required=True, help="Workflow task .output file")
    ap.add_argument("--wl", default=str(REPO / "progress" / "wl_ab.jsonl"))
    ap.add_argument("--no-park", action="store_true",
                    help="do not append misses to nonmatching.jsonl (refine batches: "
                         "the near-miss DB + refine_attempted.txt already track them)")
    args = ap.parse_args()

    res = find_result(json.load(open(args.output, encoding="utf-8")))
    if not res:
        print("no {landed,sources} result found in output"); sys.exit(1)
    print(f"model={res.get('model','?')}  landed {res['landed']}/{res['attempted']}  "
          f"tok/landed {res.get('tokensPerLanded')}")

    # bank the matches through the independent re-verifier.
    # Own directory per run: this was one shared <temp>/coddog_bank.jsonl, so two bank_run
    # invocations overlapping (two agents, or two repos driven from one console) wrote the same
    # path and each handed bank_harvest whatever the other had just written.
    bankdir = pathlib.Path(tempfile.mkdtemp(prefix="coddog_bank_"))
    tmp = bankdir / "candidates.jsonl"
    try:
        with open(tmp, "w", encoding="utf-8") as f:
            for n, s in res["sources"].items():
                f.write(json.dumps({"name": n, "c_source": s}) + "\n")
        subprocess.run([sys.executable, str(REPO / "tools" / "bank_harvest.py"),
                        "--glob", str(tmp), "--apply"], check=True)
    finally:
        shutil.rmtree(bankdir, ignore_errors=True)

    # link gate: the oracle wildcards reloc slots, so a candidate calling a wrong
    # same-shaped callee still byte-"matches". linkcheck reconstructs the linked
    # bytes and compares to the ROM - run it on each just-banked function and be loud.
    for n in res["sources"]:
        subprocess.run([sys.executable, str(REPO / "tools" / "linkcheck.py"),
                        "--name", n], check=False)

    # park the misses so coddog does not re-select them
    rows = {}
    for l in open(args.wl, encoding="utf-8"):
        if l.strip():
            r = json.loads(l); rows[r["name"]] = r
    divs = {r["name"]: r.get("divergences") for r in res["results"]}
    miss = [r["name"] for r in res["results"] if not r["matched"]]
    if args.no_park:
        print(f"{len(miss)} misses NOT parked (--no-park)")
    else:
        # Park ONLY close, compiling misses (the nonmatching.py bar: divergences <= 12).
        # Parking is a PERMANENT exclusion from coddog scheduling, so a function that
        # merely failed this run (never compiled, timed out, wild draft) must fall back
        # into the pool for a future batch/model instead.
        parked = skipped = 0
        for n in miss:
            r = rows.get(n)
            d = divs.get(n)
            if not r:
                continue
            if d is None or not (0 < d <= 12):
                skipped += 1
                continue
            parked += L.append_nonmatching(
                {"addr": r["addr"], "name": n, "size": int(r["size"], 16),
                 "module": r["module"], "divergences": d,
                 "reason": f"fan-out miss ({res.get('model','?')} "
                           f"{res.get('tokensPerLanded')}/landed)"})
        print(f"parked {parked} close misses -> progress/nonmatching.jsonl; "
              f"{skipped} far/non-compiling misses left in the pool")

    # ingest near-misses into the committed DB (standing rule: never discard a close attempt).
    # Matched names are fed through too: ingest's done-check pops their stale DB entries.
    nms = list(res.get("nearMisses") or [])
    nms += [{"name": n, "c_source": s} for n, s in res.get("sources", {}).items()]
    if nms:
        # Same shared-path hazard as the bank file above, and worse here: a collision loses near
        # misses, which the repo's standing rule says are never to be discarded.
        nmdir = pathlib.Path(tempfile.mkdtemp(prefix="coddog_nearmiss_"))
        tmp2 = nmdir / "seeds.jsonl"
        try:
            with open(tmp2, "w", encoding="utf-8") as f:
                for x in nms:
                    f.write(json.dumps({"name": x["name"], "c_source": x["c_source"]}) + "\n")
            subprocess.run([sys.executable, str(REPO / "tools" / "nearmiss_db.py"), "ingest",
                            "--seeds", str(tmp2), "--worklist", args.wl,
                            "--label", f"fanout-{res.get('model', '?')}"], check=True)
        finally:
            shutil.rmtree(nmdir, ignore_errors=True)


if __name__ == "__main__":
    main()
