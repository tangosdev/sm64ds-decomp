"""Bank a coddog fan-out workflow result in one step: independently re-verify and
bank the matches (via bank_harvest), then park the misses into nonmatching.jsonl so
they are not re-selected. Point --output at the Workflow task's .output file.

  python tools/bank_run.py --output <task.output> [--wl progress/wl_ab.jsonl]
Then run the free post-pass:  python tools/clone.py && python tools/paramclone.py
"""
import argparse, json, pathlib, subprocess, sys, tempfile
REPO = pathlib.Path(__file__).resolve().parent.parent


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
    args = ap.parse_args()

    res = find_result(json.load(open(args.output, encoding="utf-8")))
    if not res:
        print("no {landed,sources} result found in output"); sys.exit(1)
    print(f"model={res.get('model','?')}  landed {res['landed']}/{res['attempted']}  "
          f"tok/landed {res.get('tokensPerLanded')}")

    # bank the matches through the independent re-verifier
    tmp = pathlib.Path(tempfile.gettempdir()) / "coddog_bank.jsonl"
    with open(tmp, "w", encoding="utf-8") as f:
        for n, s in res["sources"].items():
            f.write(json.dumps({"name": n, "c_source": s}) + "\n")
    subprocess.run([sys.executable, str(REPO / "tools" / "bank_harvest.py"),
                    "--glob", str(tmp)], check=True)

    # park the misses so coddog does not re-select them
    rows = {}
    for l in open(args.wl, encoding="utf-8"):
        if l.strip():
            r = json.loads(l); rows[r["name"]] = r
    miss = [r["name"] for r in res["results"] if not r["matched"]]
    nm = REPO / "progress" / "nonmatching.jsonl"
    with nm.open("a", encoding="utf-8") as f:
        for n in miss:
            r = rows.get(n)
            if r:
                f.write(json.dumps({"addr": r["addr"], "name": n,
                                    "size": int(r["size"], 16), "module": r["module"],
                                    "divergences": 2,
                                    "reason": f"fan-out miss ({res.get('model','?')} {res.get('tokensPerLanded')}/landed)"}) + "\n")
    print(f"parked {len(miss)} misses -> progress/nonmatching.jsonl")


if __name__ == "__main__":
    main()
