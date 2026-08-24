#!/usr/bin/env python3
"""Restack a TU-pilot PR branch onto origin/main across the manifest split.

TRANSITIONAL. The manifest used to be one `config/tu_manifest.json` holding an
`entries` list, and every TU PR appended one object to it. That made all of
those PRs conflict pairwise: each went DIRTY the moment any other landed. The
conflict was never semantic -- entries are keyed by `id` and no two PRs touch
the same one -- but JSON cannot use .gitattributes' `merge=union` the way the
JSONL ledgers can, and GitHub does not run the union driver server-side anyway.

`config/tu_manifest.d/` (one file per entry) removes the cause, so a PR written
against the new shape does not need this script. What still needs it is any
branch cut BEFORE the split, whose commit edits the old single file. This
resolves that conflict structurally rather than by editing hunks: three-way
merge the `entries` list by `id`, then hand the result to tu_manifest.save,
which writes it out in whichever shape the branch now has.

A conflict on an individual `config/tu_manifest.d/*.json` is NOT auto-resolved:
two branches touching one entry file means they really do claim the same TU,
which is a question for a human, not a merge strategy.

Usage:
    python tools/restack_tu_pr.py --all [--push]
    python tools/restack_tu_pr.py 1719 1724 [--push]
    python tools/restack_tu_pr.py --all --dry-run
"""
from __future__ import annotations

import argparse
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
LEGACY_MANIFEST = "config/tu_manifest.json"
MANIFEST_DIR = "config/tu_manifest.d"
DEFAULT_BRANCH_PREFIX = "cpp/tu-batch"


def git(*args, check=True, capture=True):
    r = subprocess.run(["git", *args], cwd=REPO, check=False,
                       capture_output=capture, text=True)
    if check and r.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)} failed:\n{r.stdout}\n{r.stderr}")
    return r


def gh_json(*args):
    r = subprocess.run(["gh", *args], cwd=REPO, check=True,
                       capture_output=True, text=True)
    return json.loads(r.stdout)


def discover():
    prs = gh_json("pr", "list", "--limit", "100", "--json",
                  "number,headRefName,mergeable,mergeStateStatus")
    return [p for p in prs if p["headRefName"].startswith(DEFAULT_BRANCH_PREFIX)]


def entries_by_id(rev):
    r = git("show", f"{rev}:{LEGACY_MANIFEST}")
    return {e["id"]: e for e in json.loads(r.stdout)["entries"]}


def stage_entries(stage):
    """One merge stage of the legacy single file, or None if that stage has none.

    Stage 2 ("ours") is absent once the split has landed: main deleted the single
    file, so the branch's edit to it arrives as a delete/modify conflict rather
    than a content one.
    """
    r = git("show", f":{stage}:{LEGACY_MANIFEST}", check=False)
    return json.loads(r.stdout) if r.returncode == 0 else None


def resolve_manifest():
    """Three-way merge the entries list by id. Returns a summary dict."""
    base = stage_entries(1) or {"entries": []}
    theirs = stage_entries(3) or {"entries": []}
    # "Ours" is whatever main holds NOW: the split directory once that landed,
    # otherwise this conflict's own stage 2.
    ours = stage_entries(2)
    if ours is None:
        sys.path.insert(0, str(REPO / "tools"))
        import tu_manifest as TUM
        ours = TUM.load(REPO / MANIFEST_DIR)
    bi = {e["id"]: e for e in base["entries"]}
    oi = {e["id"]: e for e in ours["entries"]}
    ti = {e["id"]: e for e in theirs["entries"]}

    added = [i for i in ti if i not in bi]
    removed = [i for i in bi if i not in ti]
    modified = [i for i in ti if i in bi and ti[i] != bi[i]]

    result, replaced = [], []
    for e in ours["entries"]:
        i = e["id"]
        if i in removed:
            continue
        result.append(ti[i] if i in modified else e)
    for i in added:
        if i in oi:
            # The branch's TU already reached main by another route. Its entry
            # is a replacement, not an append -- the caller must eyeball which
            # of the two records is the better one.
            result = [ti[i] if e["id"] == i else e for e in result]
            replaced.append(i)
        else:
            result.append(ti[i])

    merged = dict(ours)
    for k in set(list(ours) + list(theirs)):
        if k != "entries" and theirs.get(k) != base.get(k):
            merged[k] = theirs[k]
    merged["entries"] = result

    ids = [e["id"] for e in result]
    if len(ids) != len(set(ids)):
        raise RuntimeError("duplicate ids after merge")

    # Write through tu_manifest so the result lands in whichever shape this
    # branch now has -- the old single file, or the split directory.
    sys.path.insert(0, str(REPO / "tools"))
    import tu_manifest as TUM
    root = REPO / (MANIFEST_DIR if (REPO / MANIFEST_DIR).is_dir() else LEGACY_MANIFEST)
    TUM.save(merged, root)

    return {"added": [i for i in added if i not in replaced],
            "replaced": replaced, "modified": modified,
            "removed": removed, "total": len(result)}


def conflicted():
    # ls-files -u, not diff --diff-filter=U: a delete/modify conflict has no
    # stage-2 blob, and that is exactly the shape a pre-split branch takes once
    # the split has landed.
    r = git("ls-files", "-u")
    paths = []
    for line in r.stdout.splitlines():
        if "\t" in line:
            p = line.split("\t", 1)[1].strip()
            if p not in paths:
                paths.append(p)
    return paths


def restack(pr, branch, push, dry_run):
    print(f"\n=== PR #{pr}  ({branch})")
    git("checkout", "--quiet", "--detach", "origin/main")
    r = git("cherry-pick", f"origin/{branch}", check=False)
    notes = []

    if r.returncode != 0:
        stuck = conflicted()
        if LEGACY_MANIFEST in stuck:
            summary = resolve_manifest()
            # Once the split has landed main no longer has the single file, and
            # the conflict leaves the branch's copy sitting in the worktree.
            # Staging it back would resurrect the file this PR set out to remove.
            if git("cat-file", "-e", f"origin/main:{LEGACY_MANIFEST}",
                   check=False).returncode != 0:
                (REPO / LEGACY_MANIFEST).unlink(missing_ok=True)
            git("add", "--all", "--", LEGACY_MANIFEST, MANIFEST_DIR)
            stuck.remove(LEGACY_MANIFEST)
            for i in summary["added"]:
                print(f"    append   {i}")
            for i in summary["replaced"]:
                notes.append(f"REPLACES an entry already on main: {i}")
                print(f"    REPLACE  {i}   <-- already on main, review this")
            for i in summary["modified"]:
                print(f"    modify   {i}")
            for i in summary["removed"]:
                print(f"    remove   {i}")

        for path in list(stuck):
            # add/add on a src_tu file: happens when the TU reached main by
            # another route. Only auto-resolvable when the branch side is a
            # strict superset of main's; otherwise stop and let a human look.
            ours = git("show", f":2:{path}", check=False)
            theirs = git("show", f":3:{path}", check=False)
            if ours.returncode == 0 and theirs.returncode == 0:
                o, t = ours.stdout.splitlines(), theirs.stdout.splitlines()
                if set(o) <= set(t):
                    git("checkout", "--theirs", "--", path)
                    git("add", path)
                    stuck.remove(path)
                    notes.append(f"{path}: took branch side (superset of main's)")
                    print(f"    superset {path}")

        if stuck:
            print(f"    !! UNRESOLVED: {stuck}")
            git("cherry-pick", "--abort", check=False)
            return {"pr": pr, "branch": branch, "status": "MANUAL", "files": stuck}

        env_git = subprocess.run(["git", "-c", "core.editor=true", "cherry-pick",
                                  "--continue"], cwd=REPO, capture_output=True, text=True)
        if env_git.returncode != 0:
            git("commit", "--no-edit", "-q", check=False)

    head = git("rev-parse", "HEAD").stdout.strip()
    stat = git("diff", "--stat", "origin/main", "HEAD").stdout.strip()
    print("    " + stat.replace("\n", "\n    "))

    if push and not dry_run:
        git("push", "--force-with-lease", "origin", f"{head}:refs/heads/{branch}",
            capture=False)
        print(f"    pushed -> {branch}")
    elif dry_run:
        print("    (dry run, not pushed)")

    return {"pr": pr, "branch": branch, "status": "OK", "head": head, "notes": notes}


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("prs", nargs="*", type=int, help="PR numbers (default: --all)")
    ap.add_argument("--all", action="store_true",
                    help=f"every open PR whose branch starts {DEFAULT_BRANCH_PREFIX}")
    ap.add_argument("--push", action="store_true", help="force-with-lease push each result")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    if git("status", "--porcelain", "--untracked-files=no").stdout.strip():
        sys.exit("working tree is dirty -- refusing to run")

    git("fetch", "origin", "--quiet", capture=False)
    open_prs = discover()
    if not args.all:
        wanted = set(args.prs)
        if not wanted:
            sys.exit("give PR numbers or --all")
        open_prs = [p for p in open_prs if p["number"] in wanted]

    results = [restack(p["number"], p["headRefName"], args.push, args.dry_run)
               for p in sorted(open_prs, key=lambda p: p["number"])]

    print("\n" + "=" * 60)
    for r in results:
        print(f"  #{r['pr']:<6} {r['status']:<7} {r['branch']}")
        for n in r.get("notes", []):
            print(f"           {n}")
    manual = [r for r in results if r["status"] == "MANUAL"]
    return 1 if manual else 0


if __name__ == "__main__":
    sys.exit(main())
