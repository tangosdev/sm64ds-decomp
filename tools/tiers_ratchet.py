#!/usr/bin/env python3
"""Backslide gate for the CONVERTED tier: readable source ownership may not regress.

WHAT IT GATES. `tools/tiers.py` scores every source file against the five CONVERTED
criteria (real function name, no raw offset arithmetic, no `unk_<off>` fields, no
codegen tricks, no calls through mangled names). This tool banks the SET of source
identities that pass all five and fails a PR when an identity LEAVES that set. A
one-function source keeps its historical path identity. A promoted TU appends
``#symbol`` to that path for each enrolled member, matching attribution's ownership unit.
It reuses tiers.score_file/score_member outright -- the classifier has exactly one
implementation, and a second copy of those regexes would be a second definition of a
published percentage.

WHY BACKSLIDE-ONLY, AND NOT A COUNT. Two reasons, and the second is the important one.

  A count ratchet ("converted may not fall") is satisfied by converting one file while
  wrecking another, which is the trade this gate exists to notice. A set ratchet names
  the source member.

  More importantly, this project's goal ordering is not negotiable: a historically
  accurate C++ source that reproduces the ROM's exact bytes comes FIRST, readability
  third. Byte-match is never traded for readability. And the two genuinely collide --
  the tree's own measurements show that whether a raw cast can become a named member is
  decided PER FUNCTION by the byte gate, not by taste: migrating one function's 13 raw
  casts to real members was free, while another function in the same family paid 0x14
  bytes to the same edit because members over-CSE'd. So a future matching fix may
  legitimately have to put offset arithmetic BACK into a file this baseline calls
  CONVERTED. A gate with no way to say that would be blocking goal 1 to protect goal 3,
  which is exactly backwards.

  Hence the override below is a real, always-available mechanism, not a note saying one
  ought to exist. The gate's job is to make a backslide LOUD and ATTRIBUTED, not to make
  it impossible.

A PATH CAN LEAVE WITHOUT ANYTHING BEING LOST. The one legitimate way is a TU
promotion: `tubuild.py` consolidates N per-symbol `src/_ZN....cpp` files into the one
`src/actors/<Class>.cpp` the original translation unit was, and git records N deletions plus
one addition. A set ratchet reads all N as `GONE`. Measured on PR #1882
(`tu/inline-dtor-order`, 9c6396c5f), 90 of 90 backslid paths were exactly that and
none was a deletion, which is a report no one can read.

So a GONE path is now resolved through the TU manifest (`config/tu_manifest.d/`, via
`tools/tu_manifest.py` -- never off the files) before it is called a deletion. If some
entry with `"status": "promoted"` lists it as a `legacy_source`, the path is reported
as a MOVE naming the `promoted_source` that absorbed it, and:

  * if the absorbing source's enrolled members each pass all five, that is NOT a
    backslide -- the same readable code is simply scored under member identities;
  * if any member does not, that IS a backslide and still fails. The message names the
    exact member and criterion, e.g. "src/actors/<Class>.cpp#<symbol> fails: Calls things
    by real names, not mangled _Z".

A promotion is therefore never silently free. In practice it lands in the second case
by construction: a reconstructed TU MUST spell vague-linkage symbols directly
(`_ZN7fBase_cnwEj`, `_ZN8dActor_cC2Ev`, `_ZN8dActor_cD2Ev`) or its range will not link,
so `no_mangled_refs` cannot pass for one. That is structural, not sloppiness, and it is
not fixed by exempting mangled refs -- byte-match outranks readability here, and the
override below is where that trade is recorded with a name against it.

HOW TO OVERRIDE (the escape hatch)

    python tools/tiers_ratchet.py --update --reason "St_WallJump_Init: named members
        cost 0x14 bytes, reverted to offset casts to hold the match"

`--update` rewrites the baseline. It refuses -- and this is its ONLY refusal -- to
REMOVE a path without `--reason`. Each removed path is appended to
`config/converted-backslide-exceptions.jsonl` with that reason, so every file that ever
left the CONVERTED set is a line in a small file with a `git blame` and a PR behind it.
Additions never need a reason and never fail anything.

TWO FAILURE MODES OF `langmode_audit.py` THIS IS BUILT NOT TO REPEAT

  1. STUCK BOTH WAYS. That ratchet's `--update` can refuse because the tree "left the
     backlog by failing differently", which leaves no way forward except re-pinning from
     a clean origin/main checkout. Here, `--update` has no state-dependent refusal at
     all: given `--reason` when it removes, it always writes. There is no tree state from
     which this tool cannot be re-pinned in one command.

  2. AN ENVIRONMENT-SPECIFIC BASELINE. The langmode baseline lives on the `chaos-data`
     branch and its consumers need a second checkout to read it. This one is a plain JSON
     file in the tree at a fixed path, readable and writable with no git plumbing, and
     `--baseline PATH` points anywhere for a CI job or a test.

     In-tree is safe here in a way a COUNTER is not, because staleness normally runs one
     way: `--check` fails on removals alone, so a baseline that has not caught up with
     newly converted files is permissive, never falsely red. No PR needs to re-bank in
     order to pass, so the baseline is not in every PR's diff and does not conflict the
     way a counter did. Re-bank it occasionally, on its own, to bank recent gains.

     STALENESS HAS ONE EXCEPTION, AND IT COST A DAY. A path that LEAVES the tree fails
     `--check` even when nothing regressed. Two ways that happens legitimately:

       - TU promotion absorbs a file. `classify_missing()` handles this: it resolves the
         gone path through `tools/tu_manifest.py` and reports
         `MOVED -- absorbed into <file> by TU promotion (<tu_id>)`. When the absorbing
         file is itself CONVERTED the removal is `absorbed_clean` -- no `--reason`, no
         exception row. The banked COUNT drops, and that is correct; a promotion batch
         is expected to lower it. There is no floor number to defend.

       - A plain file rename. `classify_missing()` has NO rename detection, so a move
         reports `GONE -- not a tracked source file any more` and demands a `--reason`,
         which would write a permanent fake backslide row. Until that is fixed, apply
         the renames to the parent's banked set FIRST, then regenerate.

WIRED INTO CI, NOT INTO THE HOOK. `.github/workflows/converted-ratchet.yml` runs
`--check` on `pull_request` and on `push: main`, over `src/**`, this file, `tiers.py`,
`delaunder.py`, the TU manifest, and the baseline itself (that workflow's `on:` block is
the authoritative list -- do not re-enumerate it here). It reads the baseline from the
tree under test; there is no second checkout.

Do NOT add it to `tools/hooks/pre-push`. The langmode ratchet's pre-push wiring is the
one that has been observed to get stuck, and a local hook a contributor cannot get past
is worse than no gate. Prove the CI job does not get stuck across a few weeks of real
PRs before anyone discusses a hook.

A CI red here is usually a BASE desync, not a regression: a PR cut before a re-bank
lands merges against a baseline that predates it and inherits the red. Fix by merging
main into the branch, never by lowering the pin.

Usage:
    python tools/tiers_ratchet.py                 # summary; no exit-code meaning
    python tools/tiers_ratchet.py --check         # exit 1 on any backslide
    python tools/tiers_ratchet.py --update        # re-bank (additions only)
    python tools/tiers_ratchet.py --update --reason "..."   # re-bank with removals
    python tools/tiers_ratchet.py --list          # current CONVERTED identities

Exit codes: 0 ok, 1 backslide detected, 2 usage/configuration error (missing baseline,
removal without a reason). It compiles nothing and reads no ROM: pure source text over
git-tracked files, about two seconds on the whole tree.
"""
import argparse
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import tiers  # noqa: E402
import tu_manifest  # noqa: E402  (legacy_source -> promoted_source; see promoted_moves)

BASELINE = REPO / "config" / "converted-baseline.json"
EXCEPTIONS = REPO / "config" / "converted-backslide-exceptions.jsonl"

NOTE = ("The CONVERTED source/member identity set, banked. One-function sources use "
        "their path; promoted TU members append #symbol to that path. "
        "tools/tiers_ratchet.py --check fails when an identity no longer passes all "
        "five criteria in tools/tiers.py. Removals need --reason and land in "
        "config/converted-backslide-exceptions.jsonl. Regenerate with "
        "`python tools/tiers_ratchet.py --update`.")


def tracked_sources():
    """Every git-tracked C/C++ file under src/, as repo-relative posix paths.

    tiers.converted() walks the filesystem instead, which is right for a MEASUREMENT --
    it should describe the tree in front of you. A GATE wants the tree git agrees on, so
    an untracked scratch file cannot enter the baseline and then "backslide" by being
    deleted. Both enumerations currently return the same 11,287 files.
    """
    out = subprocess.run(["git", "-C", str(REPO), "ls-files", "src"],
                         capture_output=True, text=True, check=True).stdout.split()
    return sorted(p for p in out if p.endswith((".c", ".cpp")))


def promoted_moves(root=None):
    """`legacy_source` -> (tu_id, promoted_source) for every PROMOTED TU entry.

    A TU promotion is the one way a banked path legitimately stops existing without
    anything being deleted. `tubuild.py` consolidates N per-symbol `src/_ZN....cpp`
    files into the single `src/actors/<Class>.cpp` the original translation unit was, and
    git records that as N deletions plus one addition -- so every one of the N banked
    paths reads to a set ratchet exactly like a file someone threw away. Measured on
    PR #1882 (`tu/inline-dtor-order`, 9c6396c5f): 90 of 90 backslid paths were TU
    `legacy_source` entries whose TU is `"status": "promoted"` and whose
    `promoted_source` exists on the branch. Zero were real deletions. A gate whose
    entire output is 90 false alarms teaches people to re-bank without reading it,
    which is the failure mode that costs a ratchet its value.

    So the checker resolves a GONE path through the manifest before calling it a
    deletion. It does NOT make the promotion free -- see `classify_missing`. It only
    lets the report say WHICH of the two things happened.

    Read through tools/tu_manifest.py, never off `config/tu_manifest.d/**/*.json`
    directly: that directory's on-disk shape is this repo's second attempt at it (it
    used to be one file) and tu_manifest is the only place allowed to know the layout.
    """
    moves = {}
    try:
        data = tu_manifest.load(root)
    except (OSError, ValueError):
        return moves
    for entry in data.get("entries") or []:
        if entry.get("status") != "promoted":
            continue
        dest = entry.get("promoted_source")
        if not dest:
            continue
        for fn in entry.get("functions") or []:
            legacy = fn.get("legacy_source")
            if legacy and legacy != dest:
                moves[legacy] = (entry.get("id"), dest)
    return moves


def source_text(rel):
    """Text for one repo-relative path, or None if it is unreadable."""
    try:
        return (REPO / rel).read_text(errors="replace")
    except OSError:
        return None


def scan(paths=None, ownership=None):
    """(converted identities, scores by identity) for the tracked source tree.

    Physical paths remain the identity for ordinary one-function intake files so the
    existing baseline stays valid. A production TU owns several enrolled functions;
    those are independently banked as ``path#symbol`` so consolidating files cannot
    masquerade as a readability backslide or let one readable member pay for another.
    """
    scores = {}
    converted = set()
    if ownership is None:
        ownership = tiers.srcpath.source_definition_index()
    for rel in (paths if paths is not None else tracked_sources()):
        text = source_text(rel)
        if text is None:
            continue
        members = ownership.get(rel) or [pathlib.PurePosixPath(rel).stem]
        multi = len(members) > 1
        for symbol in members:
            identity = f"{rel}#{symbol}" if multi else rel
            member_score = (tiers.score_member(rel, text, symbol, REPO)
                            if multi else tiers.score_file(rel, text))
            if not multi:
                member_score["real_name"] = tiers._real_name_for_symbol(symbol)
            scores[identity] = member_score
            if all(member_score[k] for k in tiers.CRITERIA):
                converted.add(identity)
    return converted, scores


class BaselineError(Exception):
    """The baseline file parsed, but its contents contradict themselves.

    Deliberately NOT the None return below. None means "no usable baseline", and
    --update treats that as "bake a new one", rewriting the whole file without the
    --reason gate that removals normally require. Routing corruption through None
    would therefore let a damaged baseline be replaced by whatever the tree happens to
    look like -- turning the evidence of the damage into the new baseline. A
    self-contradicting file is a fact about the file, so it stops both paths.
    """


def load_baseline(path):
    """The banked set, or None when there is no usable baseline file.

    A malformed or absent baseline is a configuration error, never an empty set: an
    empty set makes --check pass forever, so a deleted baseline would silently disable
    the gate rather than break it.

    Raises BaselineError when the file parses but contradicts itself.
    """
    p = pathlib.Path(path)
    if not p.is_file():
        return None
    try:
        d = json.loads(p.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None
    got = d.get("converted")
    if not isinstance(got, list):
        return None

    # write_baseline() emits sorted(converted) from a set alongside count=len(converted),
    # so in a file this tool wrote, these numbers agree by construction. A disagreement
    # means something other than --update edited the file, and both shapes are silent
    # without this check: set() below swallows duplicates, and nothing has ever read
    # `count` back. That silence is a real weakening, not a tidiness issue -- an
    # identity can be dropped from the array and the drop hidden by duplicating
    # another, leaving a gate that watches less than its own count claims.
    if len(set(got)) != len(got):
        dupes = sorted({x for x in got if got.count(x) > 1})
        shown = ", ".join(dupes[:5])
        more = f", and {len(dupes) - 5} more" if len(dupes) > 5 else ""
        raise BaselineError(
            f"{path} lists {len(got)} identities but only {len(set(got))} are "
            f"distinct.\nDuplicated: {shown}{more}")
    count = d.get("count")
    if isinstance(count, int) and count != len(got):
        raise BaselineError(
            f"{path} declares count={count} but its `converted` array holds "
            f"{len(got)} identities.\nThe two are written together by --update, so "
            "they can only disagree if the file was edited by hand.")
    return set(got)


def write_baseline(path, converted):
    p = pathlib.Path(path)
    p.parent.mkdir(parents=True, exist_ok=True)
    body = {"_note": NOTE,
            "criteria": list(tiers.CRITERIA),
            "count": len(converted),
            "converted": sorted(converted)}
    with open(p, "w", encoding="utf-8", newline="\n") as f:
        f.write(json.dumps(body, indent=2) + "\n")


def append_exceptions(path, rows):
    p = pathlib.Path(path)
    p.parent.mkdir(parents=True, exist_ok=True)
    with open(p, "a", encoding="utf-8", newline="\n") as f:
        for r in rows:
            f.write(json.dumps(r, sort_keys=True) + "\n")


def _failures(identity, scores):
    """The criteria `identity` fails, or None when it passes all five."""
    s = scores.get(identity)
    if s is None:
        return None
    failed = [k for k in tiers.CRITERIA if not s[k]]
    return failed or None


def _member_result(rel, scores, ownership):
    """Return (passes, detail) for a source scored through member identities.

    A legacy baseline can still contain the physical path of a source which is now
    scored as ``path#symbol``. Keep that identity transition observable: all members
    passing is a lossless rewrite, while a mixed result names the exact member and
    criterion that regressed.
    """
    members = (ownership or {}).get(rel) or []
    if len(members) <= 1:
        return None
    failures = []
    identities = [f"{rel}#{symbol}" for symbol in members]
    for member in identities:
        score = scores.get(member)
        if score is None:
            failures.append(f"{member} is not scored")
            continue
        failed = [k for k in tiers.CRITERIA if not score[k]]
        if failed:
            failures.append(
                f"{member} fails: "
                + "; ".join(tiers.CRITERION_LABEL[k] for k in failed))
    if failures:
        return False, "; ".join(failures)
    return True, (f"rewritten as {len(identities)} independently passing member "
                  f"identities: {', '.join(identities)}")


def why(identity, scores, tracked, moves=None, ownership=None):
    """Why a banked source/member identity is no longer CONVERTED.

    A path that is GONE gets one of two answers, and the difference is the whole
    point: someone deleted readable code, or a TU promotion absorbed it into the file
    it was always part of. The second names the absorbing file and says what that file
    does with the five criteria, because THAT is the thing a reviewer has to judge.
    """
    rel, marker, symbol = identity.partition("#")
    if rel not in tracked:
        moved = (moves or {}).get(rel)
        if not moved:
            return ("GONE -- not a tracked source file any more "
                    "(deleted, renamed or moved)")
        tu_id, dest = moved
        if dest not in tracked:
            return (f"MOVED -- TU {tu_id} names {dest} as the file that absorbed it, "
                    "but that file is not tracked; treat as a deletion")
        failed = _failures(dest, scores)
        if failed:
            return (f"MOVED -- absorbed into {dest} by TU promotion ({tu_id}), which "
                    "fails: " + "; ".join(
                        tiers.CRITERION_LABEL[k] for k in failed))
        if dest in scores:
            return (f"MOVED -- absorbed into {dest} by TU promotion ({tu_id}); that "
                    "file passes all five, so nothing readable was lost")
        member_result = _member_result(dest, scores, ownership)
        if member_result:
            passes, detail = member_result
            if passes:
                return (f"MOVED -- absorbed into {dest} by TU promotion ({tu_id}); "
                        f"{detail}, so nothing readable was lost")
            return (f"MOVED -- absorbed into {dest} by TU promotion ({tu_id}); "
                    f"member regression: {detail}")
        return (f"MOVED -- absorbed into {dest} by TU promotion ({tu_id}), but the "
                "tracked destination has no source or member score")
    failed = _failures(identity, scores)
    if failed is None:
        if marker:
            return f"GONE -- {symbol} is no longer an enrolled member of {rel}"
        member_result = _member_result(rel, scores, ownership)
        if member_result:
            passes, detail = member_result
            if passes:
                return f"IDENTITY UPGRADE -- {rel} was {detail}"
            return f"IDENTITY UPGRADE INCOMPLETE -- member regression: {detail}"
        if identity not in scores:
            return "UNREADABLE -- the file could not be read"
        # Cannot happen through --check, which derives both sides from one scan; it can
        # happen if a caller passes a hand-edited path list, so say so rather than lie.
        return "no criterion fails -- baseline and scan disagree, re-run --check"
    return "; ".join(tiers.CRITERION_LABEL[k] for k in failed)


def classify_missing(missing, current, tracked, moves, ownership=None):
    """Split banked identities into clean ownership transitions and backslides.

    `absorbed_clean` is either a banked path rewritten as independently passing member
    identities, or a banked path that stopped existing only because a promoted TU
    absorbed it and every destination member is CONVERTED. Nothing readable left the
    set, so this is not a backslide and does not need an exception row.

    Everything else is `backslid`, including a path absorbed into a TU with one member
    that fails a criterion. Member scoring keeps that failure local and the diagnostic
    names it instead of averaging it away. Reconstructed members may still need direct
    vague-linkage spellings such as `_ZN7fBase_cnwEj`, `_ZN8dActor_cC2Ev` and
    `_ZN8dActor_cD2Ev` to link their range. Such a byte-match-driven regression belongs
    in the exception log; it must not be hidden by the ownership transition.
    """
    if ownership is None:
        ownership = tiers.srcpath.source_definition_index()
    absorbed_clean, backslid = [], []
    for rel in missing:
        members = ownership.get(rel) or []
        member_ids = {f"{rel}#{symbol}" for symbol in members}
        if len(members) > 1 and member_ids.issubset(current):
            # The per-member scorer was introduced after some multi-function sources
            # had already been banked by their physical path. Treat the first path ->
            # member-identity rewrite as a lossless identity upgrade only when every
            # enrolled member independently remains CONVERTED.
            absorbed_clean.append(rel)
            continue
        moved = moves.get(rel)
        if moved and rel not in tracked:
            _, dest = moved
            dest_members = ownership.get(dest) or []
            dest_member_ids = {
                f"{dest}#{symbol}" for symbol in dest_members
            } if len(dest_members) > 1 else set()
            if (dest in current
                    or (dest_member_ids and dest_member_ids.issubset(current))):
                absorbed_clean.append(rel)
                continue
        backslid.append(rel)
    return absorbed_clean, backslid


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--check", action="store_true",
                    help="exit 1 if any banked identity is no longer CONVERTED")
    ap.add_argument("--update", action="store_true",
                    help="rewrite the baseline from the current tree")
    ap.add_argument("--reason", metavar="TEXT",
                    help="required by --update when it would REMOVE a path; recorded "
                         "against every removed path in the exceptions log")
    ap.add_argument("--date", metavar="YYYY-MM-DD",
                    help="optional date to stamp on exception rows. Omitted by default "
                         "on purpose: git already dates the commit that adds the row, "
                         "and a live clock would make this tool untestable")
    ap.add_argument("--list", action="store_true",
                    help="print the current CONVERTED source/member identities")
    ap.add_argument("--baseline", default=str(BASELINE), metavar="PATH")
    ap.add_argument("--exceptions", default=str(EXCEPTIONS), metavar="PATH")
    args = ap.parse_args()

    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass

    tracked = tracked_sources()
    ownership = tiers.srcpath.source_definition_index()
    current, scores = scan(tracked, ownership)
    tracked_set = set(tracked)
    moves = promoted_moves()

    if args.list:
        print("\n".join(sorted(current)))
        return 0

    try:
        banked = load_baseline(args.baseline)
    except BaselineError as e:
        print(f"baseline is internally inconsistent:\n\n{e}\n\n"
              "Refusing to run. Fix the file in git rather than re-running --update:\n"
              "--update would bank whatever the tree looks like now, which discards\n"
              "the very difference this check exists to show you.")
        return 2

    if args.update:
        left = sorted((banked or set()) - current)
        absorbed_clean, removed = classify_missing(left, current, tracked_set, moves)
        added = sorted(current - (banked or set()))
        if removed and not args.reason:
            print(f"REFUSING to bank {len(removed)} removal(s) without --reason:\n")
            for rel in removed:
                print(f"  {rel}\n      "
                      f"{why(rel, scores, tracked_set, moves, ownership)}")
            print("\nA path leaving the CONVERTED set is allowed -- byte-match outranks\n"
                  "readability and sometimes requires it -- but it is not allowed to be\n"
                  "silent. Re-run with --reason \"<why the match needed it>\"; the reason\n"
                  f"is appended to {args.exceptions} for every path above.")
            if absorbed_clean:
                print(f"\n({len(absorbed_clean)} further path(s) made a lossless "
                      "ownership transition.\nThose are not backslides and need no "
                      "reason.)")
            return 2
        if removed:
            append_exceptions(args.exceptions,
                              [dict({"path": rel, "reason": args.reason},
                                    **({"date": args.date} if args.date else {}))
                               for rel in removed])
        write_baseline(args.baseline, current)
        if banked is None:
            print(f"baked {len(current)} CONVERTED path(s) into {args.baseline} (new baseline)")
        else:
            print(f"wrote {args.baseline}: {len(banked)} -> {len(current)} "
                  f"(+{len(added)} / -{len(left)})")
            if absorbed_clean:
                print(f"{len(absorbed_clean)} of those left by TU promotion into a "
                      f"CONVERTED file (a move, not a removal)")
            if removed:
                print(f"logged {len(removed)} removal(s) to {args.exceptions}")
        return 0

    if args.check:
        if banked is None:
            print(f"no usable baseline at {args.baseline}.\n"
                  "Bake one with `python tools/tiers_ratchet.py --update`. An absent\n"
                  "baseline is a configuration error, not an empty set: treating it as\n"
                  "empty would make this gate pass forever.")
            return 2
        left = sorted(banked - current)
        absorbed_clean, missing = classify_missing(left, current, tracked_set, moves)
        gained = len(current - banked)
        if missing:
            print(f"CONVERTED backslide: {len(missing)} banked file(s) no longer pass "
                  f"all {len(tiers.CRITERIA)} criteria\n")
            for rel in missing:
                print(f"  {rel}\n      "
                      f"{why(rel, scores, tracked_set, moves, ownership)}")
            if absorbed_clean:
                print(f"\n({len(absorbed_clean)} further banked path(s) made a "
                      "lossless ownership transition.\nThey are not backslides and "
                      "are not counted above.)")
            print(f"\nbaseline {len(banked)}   current {len(current)}   "
                  f"(+{gained} gained, -{len(missing)} lost)")
            print("\nIf a byte match REQUIRED this -- and it legitimately can; raw-cast\n"
                  "versus named member is decided per function by the byte gate -- bank\n"
                  "it with a reason instead of reverting readable code:\n"
                  '  python tools/tiers_ratchet.py --update --reason "<why>"')
            if any(rel in moves for rel in missing):
                print("\nMOVED lines above are a TU promotion, not a deletion: the file\n"
                      "named absorbed the code and then failed a criterion for the whole\n"
                      "TU. A reconstructed TU must spell vague-linkage symbols directly\n"
                      "(_ZN7fBase_cnwEj, _ZN8dActor_cC2Ev, _ZN8dActor_cD2Ev) or the range\n"
                      "will not link, so no_mangled_refs cannot pass for one. Byte-match\n"
                      "outranks readability -- bank it with that as the reason.")
            return 1
        tail = f"   (+{gained} gained, not yet banked)" if gained else ""
        moved = (f"   ({len(absorbed_clean)} clean ownership transition(s))"
                 if absorbed_clean else "")
        print(f"CONVERTED ratchet PASS   baseline {len(banked)}   "
              f"current {len(current)}{tail}{moved}")
        for rel in absorbed_clean:
            print(f"  {rel}\n      "
                  f"{why(rel, scores, tracked_set, moves, ownership)}")
        return 0

    # No mode flag: a plain report. Says the same things --check would, without an
    # exit code, so it is safe to run while poking at the tree.
    print(f"tracked src files      {len(tracked):6d}")
    print(f"CONVERTED now          {len(current):6d}   "
          f"{100.0 * len(current) / len(tracked):.2f}%")
    if banked is None:
        print(f"baseline               (none at {args.baseline}; run --update)")
        return 0
    left = sorted(banked - current)
    absorbed_clean, missing = classify_missing(left, current, tracked_set, moves)
    print(f"baseline               {len(banked):6d}   {args.baseline}")
    print(f"gained, not banked     {len(current - banked):6d}")
    print(f"ownership transitions {len(absorbed_clean):6d}   "
          "(lossless TU move or path-to-member identity upgrade)")
    for rel in absorbed_clean:
        print(f"    {rel}\n        "
              f"{why(rel, scores, tracked_set, moves, ownership)}")
    print(f"BACKSLID               {len(missing):6d}"
          f"{'   <- --check would fail' if missing else ''}")
    for rel in missing[:20]:
        print(f"    {rel}\n        "
              f"{why(rel, scores, tracked_set, moves, ownership)}")
    if len(missing) > 20:
        print(f"    ... and {len(missing) - 20} more")
    return 0


if __name__ == "__main__":
    sys.exit(main())
