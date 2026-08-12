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


def overrides_at(rev):
    """The manual attribution.json overrides as of `rev`, read from git rather than disk."""
    try:
        raw = subprocess.run(["git", "show", f"{rev}:attribution.json"], cwd=REPO,
                             capture_output=True, text=True, encoding="utf-8",
                             errors="replace", check=True).stdout
        data = json.loads(raw)
    except (subprocess.CalledProcessError, json.JSONDecodeError):
        return {}
    ov = data.get("overrides", {}) if isinstance(data, dict) else {}
    return {k: v for k, v in ov.items()
            if isinstance(k, str) and k.startswith("src/") and isinstance(v, str) and v}


def lineage(rev):
    """{stem-without-extension: handle} at `rev`, resolved the way the merge gate resolves it.

    This must be the COMPOSITE -- overrides, then finishers, then first_matchers -- because
    that is exactly what `validate_merge.attribution_snapshot` compares, and a gate that
    models only part of it reports clean on pushes the gate rejects.

    Checking `first_matchers` alone left a gap wide enough to lose real credit through. A
    1,798-file relocation passed this check with "0 changed, 0 lost" while quietly moving 75
    functions' composite author to whoever ran the move: `match_finishers` carried a file's
    draft history across the rename but not its finisher, so every moved file read as a fresh
    finish by the mover. The finisher layer is fixed in chaos_db_ci now; this makes the gate
    able to see that layer at all, so the next such bug fails here instead of at merge.

    Keyed on the path minus its extension rather than the full path, because a legitimate
    move or a .c -> .cpp promotion changes the path while the function -- and therefore who
    deserves credit for it -- stays the same. Comparing full paths would report every
    intentional move as a loss.
    """
    first = CDB.first_matchers(rev)
    finishers = CDB.match_finishers(rev)
    overrides = overrides_at(rev)
    out = {}
    for path in set(first) | set(finishers) | set(overrides):
        who = overrides.get(path) or finishers.get(path) or first.get(path)
        if who:
            out[path.rsplit(".", 1)[0]] = who
    return out


def basename_key(stem):
    return stem.rsplit("/", 1)[-1]


def renames_between(base, head):
    """{old basename: new basename} for renames git itself detected under src/.

    Why this exists: `lineage` keys on the basename so that a directory move or a
    `.c -> .cpp` promotion is not read as a loss. A **symbol correction** changes
    the basename itself -- `_ZN5SceneD2Ev` is really `BootScene`'s D1, so the file
    has to be called something else -- and without this the old name simply
    vanishes and reads as lost, no matter how the commits are arranged.

    That was not hypothetical. Splitting rewrite from move exactly as THE RULE
    above prescribes still reported 8 lost, because that remedy addresses
    *similarity*-based lineage loss and this is *identity* loss. #1160 hit the same
    wall (`_ZN3IRQ13DmaTimHandlerEv` -> `...Ej`) and landed with the loss recorded.

    The gate keeps its teeth: this consults git's own rename detection -- the same
    authority `first_matchers` relies on via `git log -M` -- so a delete+add git
    does NOT pair stays unpaired here, and a pairing whose credit actually moved is
    reported as changed rather than waved through. Verified by rewriting and moving
    a file in one commit: still 1 lost, still exit 1.

    Renames are collected **per commit and returned in order**, not from one
    base..head diff, and are replayed rather than composed -- see project(). That matters for exactly the sequence THE RULE prescribes: rewrite in one
    commit, move in the next. Across the whole range those two show up as a single
    change whose similarity can fall under git's 50% threshold -- which it did for 3
    of 8 real cases, the small files whose comments were rewritten most. Per commit,
    the move is the R100 git already recorded.
    """
    log = subprocess.run(
        ["git", "log", "-M", "--reverse", "--name-status", "--format=@@%H",
         f"{base}..{head}", "--", "src/"],
        cwd=REPO, capture_output=True, text=True, encoding="utf-8", errors="replace")

    steps = []
    for ln in log.stdout.splitlines():
        parts = ln.split("\t")
        if len(parts) == 3 and parts[0].startswith("R"):
            old = basename_key(parts[1].rsplit(".", 1)[0])
            new = basename_key(parts[2].rsplit(".", 1)[0])
            if old != new:
                steps.append((old, new))

    return steps


def project(before_by_name, steps):
    """Replay `steps` in COMMIT ORDER onto a moving {name: credit} map.

    This used to compose the steps into a single {old: final} map and chase it
    forward, which is right for a file that really moved A -> B -> C over time and
    WRONG for a permutation. Renaming a family of classes so each takes the next
    one's name produces, in this order:

        commit 1   VirtualDoor -> Exit
        commit 3   CameraTag   -> VirtualDoor

    Chased forward, those compose to `CameraTag -> Exit`: the map says CameraTag's
    credit ended up on Exit, when Exit's credit is VirtualDoor's and CameraTag's is
    on VirtualDoor. Every name in the cycle then reads as CREDIT CHANGED even though
    git recorded an R100 for each step and no author lost anything. Order is the
    whole point -- step 2 only means what it means because step 1 already vacated
    the name.

    Replaying is also strictly stricter than composing, not looser: a rewrite-and-
    move in one commit is still not a rename to git, so it contributes no step, the
    old name is still projected as present, and it is still reported lost.
    """
    state = dict(before_by_name)
    origin = {name: name for name in state}
    for old, new in steps:
        if old not in state:
            continue
        state[new] = state.pop(old)
        origin[new] = origin.pop(old, old)
    return state, origin


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

    steps = renames_between(args.base, args.head)
    # Where each name's credit SHOULD have ended up, following git's own rename
    # detection step by step. Comparing `after` against this instead of against
    # `before` is what lets a deliberate symbol correction pass while a
    # rewrite-and-move in one commit still fails.
    projected, origin = project(before_by_name, steps)

    changed, lost, moved_ok, renamed_ok = [], [], [], []
    for name, (new_stem, new_who) in after_by_name.items():
        if name not in projected:
            continue                                   # genuinely new work
        old_stem, old_who = projected[name]
        came_from = origin.get(name, name)
        if old_who != new_who:
            changed.append((name, old_stem, new_stem, old_who, new_who))
        elif came_from != name:
            renamed_ok.append((came_from, name, old_stem, new_stem, old_who))
        elif old_stem != new_stem:
            moved_ok.append((name, old_stem, new_stem, old_who))
    for name, (old_stem, old_who) in projected.items():
        if name not in after_by_name:
            lost.append((origin.get(name, name), old_stem, old_who))

    for name, old_stem, new_stem, old_who, new_who in changed:
        print(f"  CREDIT CHANGED  {name}")
        print(f"      {old_stem}  [{old_who}]")
        print(f"   -> {new_stem}  [{new_who}]")
    for name, old_stem, old_who in lost:
        print(f"  CREDIT LOST     {name}  was {old_stem} [{old_who}]")
    for name, old_stem, new_stem, who in moved_ok:
        print(f"  moved, credit intact: {name}  [{who}]")
    for name, target, old_stem, new_stem, who in renamed_ok:
        print(f"  renamed, credit intact: {name} -> {target}  [{who}]")

    print(f"\n{len(after_by_name)} tracked, {len(moved_ok)} moved with credit intact, "
          f"{len(renamed_ok)} renamed with credit intact, "
          f"{len(changed)} changed, {len(lost)} lost")

    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(
            {"changed": changed, "lost": lost, "moved_ok": moved_ok,
             "renamed_ok": renamed_ok}, indent=2),
            encoding="utf-8")

    if changed or lost:
        print("\nA commit may rewrite a file, or move it -- not both. Split them: rewrite in")
        print("place, then move with an empty diff, so git records R100 and credit follows.")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
