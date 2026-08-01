"""Parallel link-gate: the same per-file verdict prepush_linkcheck.py produces, but
across a thread pool instead of one file at a time.

WHY THIS EXISTS
---------------
prepush_linkcheck.py resolves and link-checks each file serially. That is fine for the
handful of files a normal match PR touches, but a readability sweep changes 100-200 files
at once, and the serial walk then dominates the batch (about 8s for 15 files, so several
minutes for a full batch on a 12-core box). Running the same linkcheck subprocesses
concurrently cuts that by roughly 4-5x with identical verdicts -- checked file-for-file
against prepush_linkcheck.py before this was used to gate anything.

It is a speed tool, not a different check: symbol resolution and the verdict come from
the same stamp_provenance/linkcheck path prepush_linkcheck uses. Use prepush_linkcheck.py
for a normal PR; use this when a sweep makes the serial cost the bottleneck.

Verdicts are the linkcheck ones (VERIFIED / BLIND-n / WRONG / NO-REPRO / NO-SYM / ...);
see tools/linkcheck.py. WRONG and NO-REPRO are false matches and must block a push.

Usage:
  python tools/pgate.py --out gate.json --jobs 10 src/a.c src/b.cpp ...
"""
import argparse
import importlib.util
import json
import pathlib
import subprocess
import sys
from collections import Counter
from concurrent.futures import ThreadPoolExecutor

REPO = pathlib.Path(__file__).resolve().parent.parent


def load_symbol_fn():
    """The (module, addr, size) resolver prepush_linkcheck.py uses."""
    spec = importlib.util.spec_from_file_location("_sp", REPO / "tools" / "stamp_provenance.py")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod.load_symbol


def run_linkcheck(module, name, addr, size):
    proc = subprocess.run(
        [sys.executable, str(REPO / "tools" / "linkcheck.py"),
         "--module", module, "--name", name, "--addr", hex(addr), "--size", hex(size)],
        cwd=REPO, capture_output=True, text=True)
    try:
        start = proc.stdout.find("{")
        if start != -1:
            rec, _ = json.JSONDecoder().raw_decode(proc.stdout[start:])
            return rec.get("verdict", "ERROR"), rec.get("blind", 0)
    except (ValueError, TypeError):
        pass
    return "ERROR", 0


def main():
    ap = argparse.ArgumentParser(description="Link-verify many src files in parallel")
    ap.add_argument("--out", required=True, help="write the JSON verdict list here")
    ap.add_argument("--jobs", type=int, default=10)
    ap.add_argument("files", nargs="+")
    args = ap.parse_args()
    load_symbol = load_symbol_fn()

    jobs, rows = [], []
    for f in args.files:
        name = pathlib.Path(f).stem
        sym = load_symbol(name)
        if not sym:
            rows.append({"file": f, "name": name, "verdict": "NO-SYM"})
        else:
            jobs.append((f, name, sym))

    def work(job):
        f, name, (module, addr, size) = job
        verdict, blind = run_linkcheck(module, name, addr, size)
        if verdict == "ERROR":  # one retry: ERROR is usually a flaky compile, not a bad file
            verdict, blind = run_linkcheck(module, name, addr, size)
        return {"file": f, "name": name, "module": module, "addr": hex(addr),
                "verdict": verdict, "blind": blind}

    with ThreadPoolExecutor(max_workers=args.jobs) as ex:
        rows.extend(ex.map(work, jobs))

    json.dump(rows, open(args.out, "w"))
    tally = Counter(r["verdict"] if not str(r["verdict"]).startswith("BLIND") else "BLIND" for r in rows)
    print("pgate:", dict(tally))
    bad = [r["file"] for r in rows if r["verdict"] in ("WRONG", "NO-REPRO")]
    if bad:
        print("FALSE MATCHES (must not be pushed):", *bad, sep="\n  ")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
