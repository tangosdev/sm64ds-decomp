"""Pre-push gate: catch a push that would move contributor credit off its matcher.

WHY THIS EXISTS
---------------
`validate` fails a merge outright when credit moves:

    Validation failed: contributor attribution changed or was lost
    Contributor credit: 0 added, 2 changed, 0 lost

That verdict arrives from the private build box after a full merge validation -- twenty
minutes, and only once the PR is open. The failure is also invisible in review: every file
still byte-matches, every module is still exact, and the diff looks like ordinary readable
work. Nothing on your machine says otherwise. This closes that gap: same computation as the
gate, run locally in seconds.

WHAT GOES WRONG
---------------
`chaos_db_ci.first_matchers()` credits each surviving src/ path to the first contributor who
landed the match it descends from. Git's own classification decides the lineage:

    rename        -> credit carries to the new path
    delete + add  -> lineage ends; the adder becomes the new owner

#938 added a rescue for the common promotion case: a delete and an add IN ONE COMMIT whose
paths share a stem are paired as a rename, so `src/F.c -> src/F.cpp` keeps its credit. But
that stem includes the directory, so it cannot follow a file that MOVES. And `git log -M`
gives up on its own once content churn drops similarity below 50%.

So the trap is specific: **rewriting a file's contents and moving it in the same commit.**
Either alone is safe. Together, similarity falls below the threshold, the stem changes so
the pairing cannot help, and credit silently re-points to whoever pushed.

PR #993 is the worked example, and it shows how narrow the margin is. Three files moved in
one commit. The Mad Piano source survived at R056 -- barely over the threshold -- while two
message sources were rewritten a little more heavily, fell under it, and lost their lineage.
Same commit, same kind of move; the outcome turned purely on how much text changed.

THE RULE
--------
A commit may rewrite a file, or move it. Not both.

Split them: rewrite in place first, then move with an empty diff. Git records R100 and the
credit follows. That is the #869-then-#970 sequence, and it is what this check enforces.

Usage:
  python tools/prepush_attribution.py                          # origin/main..HEAD
  python tools/prepush_attribution.py --base origin/main
  python tools/prepush_attribution.py --json report.json

Exit status is 1 if any credit changed or was lost, so it can gate a push.
"""
import argparse
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import chaos_db_ci as CDB  # noqa: E402


def lineage(rev):
    """{stem-without-extension: handle} at `rev`.

    Keyed on the path minus its extension rather than the full path, because a legitimate
    move or a .c -> .cpp promotion changes the path while the function -- and therefore who
    deserves credit for it -- stays the same. Comparing full paths would report every
    intentional move as a loss.
    """
    out = {}
    for path, who in CDB.first_matchers(rev).items():
        out[path.rsplit(".", 1)[0]] = who
    return out


def basename_key(stem):
    return stem.rsplit("/", 1)[-1]


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--base", default="origin/main", help="revision to compare against")
    ap.add_argument("--head", default="HEAD")
    ap.add_argument("--json", help="write a JSON report here")
    args = ap.parse_args()

    try:
        subprocess.run(["git", "rev-parse", "--verify", args.base], cwd=REPO, check=True,
                       capture_output=True)
    except subprocess.CalledProcessError:
        sys.exit(f"cannot resolve {args.base} -- fetch first?")

    before, after = lineage(args.base), lineage(args.head)

    # Directory moves change the stem but not the filename, so compare on the filename to
    # tell "this file moved" apart from "this file's credit moved".
    before_by_name = {basename_key(s): (s, w) for s, w in before.items()}
    after_by_name = {basename_key(s): (s, w) for s, w in after.items()}

    changed, lost, moved_ok = [], [], []
    for name, (new_stem, new_who) in after_by_name.items():
        if name not in before_by_name:
            continue                                   # genuinely new work
        old_stem, old_who = before_by_name[name]
        if old_who != new_who:
            changed.append((name, old_stem, new_stem, old_who, new_who))
        elif old_stem != new_stem:
            moved_ok.append((name, old_stem, new_stem, old_who))
    for name, (old_stem, old_who) in before_by_name.items():
        if name not in after_by_name:
            lost.append((name, old_stem, old_who))

    for name, old_stem, new_stem, old_who, new_who in changed:
        print(f"  CREDIT CHANGED  {name}")
        print(f"      {old_stem}  [{old_who}]")
        print(f"   -> {new_stem}  [{new_who}]")
    for name, old_stem, old_who in lost:
        print(f"  CREDIT LOST     {name}  was {old_stem} [{old_who}]")
    for name, old_stem, new_stem, who in moved_ok:
        print(f"  moved, credit intact: {name}  [{who}]")

    print(f"\n{len(after_by_name)} tracked, {len(moved_ok)} moved with credit intact, "
          f"{len(changed)} changed, {len(lost)} lost")

    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(
            {"changed": changed, "lost": lost, "moved_ok": moved_ok}, indent=2),
            encoding="utf-8")

    if changed or lost:
        print("\nA commit may rewrite a file, or move it -- not both. Split them: rewrite in")
        print("place, then move with an empty diff, so git records R100 and credit follows.")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
