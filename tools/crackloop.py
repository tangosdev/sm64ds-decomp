"""One-command driver for the crack loop. Wraps the full runbook sequence so a
batch is exactly three steps: prep -> Workflow -> land.

  python tools/crackloop.py prep --min 0x100 --max 0x280 --limit 30
      coddog-schedules a fresh spread worklist to progress/wl_ab.jsonl, locks the
      module spans via the claims service (skipped gracefully on a public clone
      without tools/claims.py), trims the worklist to the modules actually locked,
      and prints the names array to paste into the Workflow launch:
        Workflow({ scriptPath: "tools/sched_run.js", args: <names> })

  python tools/crackloop.py land --output <the Workflow task .output file>
      banks the result (independent re-verify, parks misses, ingests near-misses),
      runs the free post-pass (clone + paramclone), releases the claims, and
      prints fresh progress.

  python tools/crackloop.py refine --max-div 6 --limit 20
      exports the closest fixable near-misses from nearmiss/db.jsonl (category-routed,
      see tools/refine_wl.py) and prints the refine_run.js launch line. Land a refine
      batch with:  python tools/crackloop.py land --output <out> --refine

Every underlying tool still works standalone; this only sequences them.
"""
import argparse
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
TOOLS = REPO / "tools"
WL = REPO / "progress" / "wl_ab.jsonl"


def run(script, *args, check=True):
    cmd = [sys.executable, str(TOOLS / script), *args]
    print(f"\n>> {script} {' '.join(args)}")
    return subprocess.run(cmd, check=check).returncode


def _paths(tag):
    """(worklist, claims_active) paths for a batch tag; 'ab' keeps legacy names."""
    wl = REPO / "progress" / (f"wl_{tag}.jsonl")
    ca = REPO / "progress" / ("claims_active.json" if tag == "ab"
                              else f"claims_active_{tag}.json")
    return wl, ca


def prep(a):
    wl, active = _paths(a.tag)
    if active.exists():
        print(f"a '{a.tag}' batch appears to be in flight ({active.name} exists).\n"
              "Regenerating the worklist now would swap target bytes under running agents.\n"
              f"Land it first (crackloop.py land --tag {a.tag}) or release the claims.")
        sys.exit(1)
    cd = ["--min", hex(a.min), "--max", hex(a.max), "--limit", str(a.limit),
          "--out", str(wl)]
    if a.module:
        cd += ["--module", a.module]
    if not a.no_spread:
        cd += ["--spread"]
    if a.draft:
        cd += ["--draft"]
    run("coddog.py", *cd)

    rows = [json.loads(l) for l in wl.read_text(encoding="utf-8").splitlines() if l.strip()]
    if not rows:
        print("worklist is empty - nothing to do"); sys.exit(1)

    if a.no_claims:
        print("--no-claims: skipping the lock step (coordinate via CLAIMS.md); "
              "use when the local claims key is expired")
        active.write_text("{}")
    else:
        try:
            sys.path.insert(0, str(TOOLS))
            import claims
            held = claims.lock_worklist(str(wl))   # trims conflicted rows from the file itself
            active.write_text(json.dumps(held))
        except ImportError:
            print("tools/claims.py not present (public clone) - skipping the lock step; "
                  "coordinate via CLAIMS.md")

    # re-read: the lock step drops conflicted rows from the worklist file in place
    rows = [json.loads(l) for l in wl.read_text(encoding="utf-8").splitlines() if l.strip()]
    if not rows:
        print("no lockable candidates left - either another agent holds this band "
              "(re-run with a different band or a higher --limit for slack) or the "
              "claims service is unreachable (python tools/claims.py check ...)")
        sys.exit(1)

    names = [r["name"] for r in rows]
    launch = {"names": names}
    if a.tag != "ab":
        launch["wl"] = f"progress/wl_{a.tag}.jsonl"
    print(f"\n{len(names)} functions ready. Launch:")
    print(f'Workflow({{ scriptPath: "tools/sched_run.js", args: {json.dumps(launch)} }})')


def refine(a):
    active = REPO / "progress" / "claims_active_refine.json"
    if active.exists():
        print("a refine batch appears to be in flight (claims_active_refine.json exists); "
              "land it first (crackloop.py land --refine)")
        sys.exit(1)
    run("refine_wl.py", "--max-div", str(a.max_div), "--limit", str(a.limit))
    wlr = REPO / "progress" / "wl_refine.jsonl"
    if a.no_claims:
        print("--no-claims: skipping the lock step (coordinate via CLAIMS.md)")
        active.write_text("{}")
        rows = [__import__("json").loads(l) for l in wlr.read_text(encoding="utf-8").splitlines() if l.strip()]
        names = [r["name"] for r in rows]
        print(f"\n{len(names)} drafts ready. Launch:")
        print(f'Workflow({{ scriptPath: "tools/refine_run.js", args: {json.dumps(names)} }})')
        print("\nthen land with: python tools/crackloop.py land --output <task.output> --refine")
        return
    try:
        sys.path.insert(0, str(TOOLS))
        import claims
        if wlr.exists() and wlr.read_text(encoding="utf-8").strip():
            held = claims.lock_worklist(str(wlr))   # both sides refine the shared DB head
            active.write_text(json.dumps(held))
            rows = [json.loads(l) for l in wlr.read_text(encoding="utf-8").splitlines() if l.strip()]
            if not rows:
                print("every refine draft conflicted with an active claim - the other "
                      "agent is refining this head; try later or raise --limit")
                sys.exit(1)
            names = [r["name"] for r in rows]
            print(f"\n{len(names)} drafts locked. Launch (use THIS line, post-lock):")
            print(f'Workflow({{ scriptPath: "tools/refine_run.js", args: {json.dumps(names)} }})')
    except ImportError:
        pass
    print("\nthen land with: python tools/crackloop.py land --output <task.output> --refine")


def land(a):
    # snapshot the done-set so the link-gate sweep below can tell exactly what
    # this run banked, whichever tier banked it
    try:
        sys.path.insert(0, str(TOOLS))
        import ledger as L
        pre = L.matched_set()
    except Exception:
        pre = None
    wl = str(REPO / "progress" / ("wl_refine.jsonl" if a.refine else f"wl_{a.tag}.jsonl"))
    bank = ["--output", a.output, "--wl", a.wl or wl]
    if a.refine:
        bank.append("--no-park")
    run("bank_run.py", *bank, check=False)
    # Free tiers bank again: clone/paramclone now refuse a template whose config
    # reloc destinations differ from the candidate's (PR #64) - the wrong-symbol
    # class that put 24 wrong-table PMF clones in two trees on 2026-07-02. They
    # stay dry-run by default, so land passes --apply explicitly.
    run("clone.py", "--apply", check=False)
    run("paramclone.py", "--apply", check=False)
    # Whole-run link gate: sweep EVERYTHING banked in this run, whichever tier
    # banked it. bank_run linkchecks only its own sources; this is the backstop
    # that catches any banker the source-level gates miss (decision on PR #64).
    if pre is not None:
        try:
            import ledger as L
            import linkcheck as LC
            import reloc_audit as RA
            new_rows, seen = [], set()
            for r in L.read_records(L.MATCHED):
                try:
                    key = L.key_of(r)
                except Exception:
                    continue
                if key in pre or key in seen:
                    continue
                seen.add(key)
                new_rows.append(r)
            if new_rows:
                print('\n'
                      f"link-gate sweep: {len(new_rows)} function(s) banked this run")
                ni = RA.build_name_index()
                wrong = []
                for r in new_rows:
                    mod, addr = L.key_of(r)
                    size = r["size"] if isinstance(r["size"], int) else int(r["size"], 0)
                    v = LC.linkcheck(r["name"], addr, size, mod, ni)
                    print(f"  {v['verdict']:>10}  {mod}  {r['name']}")
                    if v["verdict"] == "WRONG":
                        wrong.append(r["name"])
                        for d in v["diffs"]:
                            print(f"        {d['off']}: {d['sym']} -> {d['target']}")
                if wrong:
                    print('\n'
                          f"*** LINK GATE: {len(wrong)} WRONG bank(s) this run: "
                          f"{', '.join(wrong)} -- unbank (remove src/<name>.c[pp] and "
                          f"the matched.jsonl rows) before committing ***")
        except Exception as e:
            print(f"link-gate sweep failed ({e}); run tools/linkcheck.py by hand")
    try:
        sys.path.insert(0, str(TOOLS))
        import claims
        if a.refine:
            p = REPO / "progress" / "claims_active_refine.json"
            if p.exists():
                ids = json.loads(p.read_text())
                for mod, v in ids.items():
                    for cid in (v if isinstance(v, list) else [v]):
                        claims.release(cid)
                p.unlink()
                print("released refine claims")
        else:
            _, active = _paths(a.tag)
            if active.exists():
                ids = json.loads(active.read_text())
                for mod, v in ids.items():
                    for cid in (v if isinstance(v, list) else [v]):
                        claims.release(cid)
                active.unlink()
                print(f"released '{a.tag}' claims")
    except ImportError:
        pass
    run("progress.py", check=False)
    print("\nbatch landed. Commit when ready: git add src/ nearmiss/ README.md && "
          "git commit -m \"Match N functions via coddog fan-out (XX.X%)\"")


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("prep", help="build + claim + trim a fresh worklist")
    p.add_argument("--min", type=lambda x: int(x, 0), default=0x100)
    p.add_argument("--max", type=lambda x: int(x, 0), default=0x280)
    p.add_argument("--limit", type=int, default=30)
    p.add_argument("--module", default=None)
    p.add_argument("--no-spread", action="store_true")
    p.add_argument("--draft", action="store_true",
                   help="attach m2c semantic drafts to scaffold-less rows "
                        "(coddog_sim < 0.5, size > 0x300); use for LARGE bands. "
                        "Needs vendor/m2c (notes/m2c-setup.md)")
    p.add_argument("--no-claims", action="store_true",
                   help="skip claims locking (e.g. expired local key); coordinate via CLAIMS.md")
    p.add_argument("--tag", default="ab",
                   help="batch tag: distinct worklist + claims files so several "
                        "fresh batches can run in parallel (default 'ab')")
    p.set_defaults(fn=prep)
    p = sub.add_parser("refine", help="export closest fixable near-misses for refine_run.js")
    p.add_argument("--max-div", type=int, default=6)
    p.add_argument("--limit", type=int, default=20)
    p.add_argument("--no-claims", action="store_true",
                   help="skip claims locking (e.g. expired local key)")
    p.set_defaults(fn=refine)
    p = sub.add_parser("land", help="bank + free post-pass + release + progress")
    p.add_argument("--output", required=True, help="Workflow task .output file")
    p.add_argument("--refine", action="store_true",
                   help="refine batch: use wl_refine.jsonl, no parking, no claims")
    p.add_argument("--wl", default=None, help="explicit worklist override")
    p.add_argument("--no-claims", action="store_true",
                   help="skip claims locking (e.g. expired local key); coordinate via CLAIMS.md")
    p.add_argument("--tag", default="ab", help="batch tag used at prep time")
    p.set_defaults(fn=land)
    a = ap.parse_args()
    a.fn(a)


if __name__ == "__main__":
    main()
