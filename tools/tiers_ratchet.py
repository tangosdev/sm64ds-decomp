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

So a GONE path is resolved through the TU manifest (`config/tu_manifest.d/`, via
`tools/tu_manifest.py` -- never off the files) before it is called a deletion. If some
entry with `"status": "promoted"` lists it as a `legacy_source`, `why()` reports the
path as a MOVE naming the `promoted_source` that absorbed it, and `classify_missing()`
decides whether that move cost anything:

  * if the absorbing source's enrolled members each pass all five, that is NOT a
    backslide -- the same readable code is simply scored under member identities;
  * if any member does not, that IS a backslide and still fails. The message names the
    exact member and criterion, e.g. "src/actors/<Class>.cpp#<symbol> fails: Calls things
    by real names, not mangled _Z".

WHICH MODE YOU SEE THAT IN IS NOT OBVIOUS, SO IT IS WRITTEN DOWN. Both arms belong to
`--update` now, not to `--check`. `--check` peels ORPHANED identities off first (see
STALENESS below), and an identity whose file left the tree is exactly the input that
would reach `why()`'s gone-path ladder -- so under `--check` that ladder is unreachable
by construction, and neither `MOVED -- absorbed into` nor `GONE -- not a tracked source
file any more` can appear. Measured across three gone-path shapes: zero occurrences of
either under `--check`, and both under `--update`'s refusal, which is the one caller
that still hands `why()` an untracked path.

A promotion is therefore never silently free. In practice it lands in the second case
by construction: a reconstructed TU MUST spell vague-linkage symbols directly
(`_ZN7fBase_cnwEj`, `_ZN8dActor_cC2Ev`, `_ZN8dActor_cD2Ev`) or its range will not link,
so `no_mangled_refs` cannot pass for one. That is structural, not sloppiness, and it is
not fixed by exempting mangled refs -- byte-match outranks readability here, and the
override below is where that trade is recorded with a name against it. Measured
2026-09-10: 129 of the 136 destinations a promotion names fail at least one criterion,
which is the same fact from the other end.

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

     STALENESS HAS ONE EXCEPTION, AND IT COST A DAY. A banked identity whose FILE part
     leaves the tree is an ORPHAN: it names nothing, so `scan()` never scores it,
     `_failures()` returns None for it, and no criterion can fail. It sits in the
     baseline claiming coverage it does not provide. `--check` computes
     `orphaned_identities()` FIRST, removes the result from the backslide accounting,
     and fails on it -- see that function's docstring for why this is not the question
     `classify_missing()` answers, and why the split on `#` has to come first.

     THE REMEDY DEPENDS ENTIRELY ON WHAT A REWRITE WOULD CREATE, and the answers want
     different commands. `report_orphans()` therefore prints a diagnostic per class,
     keyed first on whether the TU manifest names a destination and then on
     `target_state()` -- the state of the ONE identity the block tells the reader to
     create, which is `rewrite_target()`'s, not the destination file's.

     THOSE TWO ARE NOT THE SAME QUESTION, AND CONFLATING THEM COST TWO ROUNDS. A
     multi-member destination is scored per member: it can hold a member that passes
     all five and a sibling that fails, and then "does the file pass" and "does the
     identity I am about to create pass" have different answers. Measured 2026-09-10:
     7 of the 136 distinct promoted destinations pass all five and 129 do not, but of
     the 2,329 banked-path-to-destination pairs at one of those 129, 620 name a rewrite
     target that passes on its own -- spread over 122 of the 129. Judging those by the
     file called a clean rewrite a backslide 620 times.

     THE UNIT OF THOSE COUNTS IS THE DISTINCT (banked path, destination) PAIR, which
     is what `promoted_moves()` yields: 2,359 of them, 2,329 at a failing destination.
     Counting raw manifest FUNCTION rows instead gives 2,401 / 2,371 / 661, because
     one TU repeats a single `legacy_source` across 42 of its rows and another repeats
     one across 2. The destination-level figures -- 7, 129, 122 -- are identical under
     both. Say which unit before comparing a re-derivation to these.

       - THE TARGET PASSES AND SO DOES ITS FILE: a TU promotion absorbed the file and
         the readable code is demonstrably still there. `tools/tu_promote.py:
         converted_baseline_update` REWRITES the banked identity onto the destination,
         leaving `count` unchanged -- the same readable code under a new name. A
         promotion carried out by hand that never runs it leaves the identity dangling.
         Eight did exactly that (four ukishima, four kurumajiku) and no gate saw them
         until PR #2530 rewrote them by hand; `classify_missing()` was forgiving them as
         `absorbed_clean` ownership transitions and `--check` was returning 0.

         `--update` is the WRONG tool for this class, and not for the reason you might
         expect. Measured on the pre-#2530 baseline: `--update` with no `--reason` exits
         0, prints "8 of those left by TU promotion into a CONVERTED file (a move, not a
         removal)", writes no exception row, and the following `--check` passes. It does
         not refuse -- it ABSORBS the orphan, erasing the identity instead of rewriting
         it, and the evidence of the defect with it. Rewrite; do not re-bank.

         `--check` failing here does NOT prevent that. One `--update` still launders
         every orphan of this class, and the result is green. Closing it means teaching
         `--update` to rewrite onto the manifest destination rather than absorb, which
         this change does not do and which rule 1 above constrains: the fix cannot work
         by refusing. Until then the gate makes the defect loud, not impossible.

       - THE TARGET PASSES BUT ITS FILE DOES NOT: 620 of the 2,329 pairs above. The
         rewrite creates an identity that is in this scan and passes all five, so
         nothing regresses -- and that is a claim about the identity, checked, not about
         the file around it. What fails is a different member of the same file, which
         was already failing before the rewrite.

         `--update` is still wrong here, and this is the one place where the file is the
         right unit: `classify_missing()` absorbs a moved path only when EVERY enrolled
         member of the destination is CONVERTED, so these reach `removed` -- exit 2
         without a `--reason`, and with one a row recording the LEGACY path as REMOVED,
         which is false and permanent. One paragraph of that report therefore keys its
         two halves on different units on purpose, and says so; a reader who spots the
         mismatch without the explanation will assume one half is wrong.

         ORDER IS THE REMEDY. Rewrite the baseline entry onto the target FIRST and the
         next `--check` exits 0 with no exception row and no `--update` at all;
         `--update` first writes the false row and drops the identity.

       - THE TARGET ITSELF FAILS A CRITERION: the ordinary case, 1,709 of the 2,329.
         The rewrite is still correct -- the identity belongs on the file that owns the
         code -- but nothing here licenses "nothing regressed", and an earlier version
         of this tool printed that sentence over this class too. The rewritten identity
         is a backslide the moment it exists, which is the true state of the tree rather
         than an artifact of the rewrite. `--update` behaves as in the class above, for
         the same file-level reason. Rewrite first even so: banked as an orphan, the
         exception row names the LEGACY path and is false; banked after the rewrite it
         names the REWRITTEN identity, which is a real backslide and true.

       - THE TARGET CANNOT BE VOUCHED FOR: the destination is not tracked, or no
         manifest row here enrolls the orphan under a symbol, or the `#symbol` carried
         on the banked identity is not one the destination defines. Zero cases today.
         The last of the three is the one to watch: a carried symbol is never verified
         by the manifest, only by looking the resulting identity up in the scan, and
         rewriting onto a symbol the file does not define recreates the orphan under a
         new name. Do not rewrite onto an identity nothing scored.

       - NO DESTINATION IS NAMED: this tool cannot tell a promotion from a rename from a
         deletion, and must not pretend otherwise. `classify_missing()` has no rename
         detection, so the entry reaches `backslid` and `--update` REFUSES it without a
         `--reason` (measured: exit 2). If the file was renamed, apply the same rewrite
         onto the new path. If the code genuinely left the tree, that is a real removal
         and `--update --reason` is the correct command -- it is also the ONLY caller of
         `append_exceptions()`, so it is the only road to
         `config/converted-backslide-exceptions.jsonl`.

     A NOTE ON `count`. It is metadata. `load_baseline()` reads it back solely to catch a
     hand-edit that contradicts the array; `tu_promote.py` writes it; nothing else in the
     tree reads it. A falling `count` is therefore not, by itself, a signal any gate acts
     on -- do not reason about these remedies as if it were.

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

Exit codes: 0 ok, 1 backslide or orphaned identity detected, 2 usage/configuration error
(missing baseline, removal without a reason). It compiles nothing and reads no ROM: pure
source text over git-tracked files, about two seconds on the whole tree.
"""
import argparse
import collections
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
    deleted.

    The two enumerations agree today: both return the same set, and their symmetric
    difference is empty. THAT is the durable claim; the size is not. This docstring
    carried "the same 11,287 files" long after the number had drifted to 9,089 (measured
    2026-09-10, when consolidation into promoted TUs had been shrinking it for weeks).
    Re-measure rather than quote either figure -- `len(tracked_sources())` is one line,
    and the plain no-flag report prints it.
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


def promoted_symbols(root=None):
    """`legacy_source` -> the symbol that source's function is enrolled under after
    promotion.

    Only the orphan report needs this, and only for a banked BARE legacy path: an
    identity that already carries `#symbol` brings its own. Kept as a second walk of the
    manifest rather than a third field on `promoted_moves()` on purpose -- that map's
    `(tu_id, promoted_source)` tuple is unpacked at three call sites here and by the
    verification harnesses outside this repo, and widening it for one report line would
    be a shape change with no other consumer.
    """
    symbols = {}
    try:
        data = tu_manifest.load(root)
    except (OSError, ValueError):
        return symbols
    for entry in data.get("entries") or []:
        if entry.get("status") != "promoted":
            continue
        for fn in entry.get("functions") or []:
            legacy, symbol = fn.get("legacy_source"), fn.get("symbol")
            if legacy and symbol:
                symbols.setdefault(legacy, symbol)
    return symbols

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

    ONLY `--update` STILL REACHES EITHER. `--check` peels orphaned identities off
    before it gets here, and an identity whose file left the tree is exactly the input
    that would take the `rel not in tracked` branch below -- so under `--check` those
    arms are unreachable and `report_orphans()` answers the same question with the
    state of the identity a rewrite would create, plus a remedy keyed on it. Note that
    the ladder below judges the whole destination FILE, which is the coarser unit and
    the reason `report_orphans()` does not reuse it.
    `--update`'s refusal is the one caller that
    still hands this function an untracked path. Do not describe the strings below as
    something a `--check` run prints; that mistake has been made twice.
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


def orphaned_identities(banked, tracked):
    """Banked identities whose FILE part is not a tracked source any more.

    This is the one defect a set ratchet cannot notice by scoring, because there is
    nothing left to score. `scan()` only emits identities for files git tracks, so an
    identity naming a vanished file is absent from `current` for a reason that has
    nothing whatever to do with the five criteria: it never enters `scores`,
    `_failures()` returns None for it, and no criterion can fail. The entry sits in the
    baseline watching nothing while `count` still claims it as coverage.

    SPLIT ON `#` FIRST. Most banked entries are `path#symbol`, and testing the whole
    identity against the tracked set reports every promoted-TU member as an orphan --
    651 false positives out of the 2701 identities banked when this was written.

    THIS IS NOT THE QUESTION `classify_missing()` ANSWERS, and the difference is the
    whole reason this exists. That function's `absorbed_clean` has two branches and only
    one of them concerns a file that left the tree:

      * a banked bare path whose members are now banked individually. The file STILL
        EXISTS; it is simply scored under `path#symbol` since the per-member scorer
        arrived. Not an orphan, and it must not be reported as one.
      * a banked path with a `moves` row into a CONVERTED destination. The file is gone
        and the entry IS an orphan -- but `classify_missing()` calls it a clean
        ownership transition, so `--check` prints it under a PASS and returns 0. That is
        precisely where eight identities (four ukishima, four kurumajiku) hid after two
        hand-promotions, until PR #2530 rewrote them by hand.

    So the orphan set is computed first and removed from the backslide accounting. An
    orphan is then diagnosed as an orphan, instead of being forgiven as a transition or
    -- when no manifest row names a destination -- reported as a CONVERTED backslide,
    which it is not: nothing regressed, the code moved.
    """
    return sorted(e for e in banked if e.split("#", 1)[0] not in tracked)


def orphan_destination(identity, moves=None):
    """The (tu_id, promoted_source) whose promotion absorbed this identity's file.

    None when no PROMOTED manifest entry names the file as a `legacy_source`. This one
    fact decides everything the reader is told, so it is a function rather than an
    expression repeated at each site: an orphan with a destination has a known, safe,
    mechanical repair, and an orphan without one is an open question that must not be
    answered by guessing.

    It says a destination EXISTS. It says nothing about the state of that destination
    (`destination_state()`), and still less about the one identity a rewrite would put
    there (`target_state()`). Do not assert anything about the code at the other end
    without calling the second of those: it is the only one that answers the question
    a reader acts on.
    """
    return (moves or {}).get(identity.split("#", 1)[0])


def orphan_reason(identity, moves=None):
    """Why one banked identity no longer names a tracked file, naming the destination."""
    rel = identity.split("#", 1)[0]
    moved = orphan_destination(identity, moves)
    if moved:
        tu_id, dest = moved
        return (f"ORPHANED -- {rel} was absorbed into {dest} by TU promotion "
                f"({tu_id}), but this identity was never rewritten onto it")
    return (f"ORPHANED -- {rel} is not a tracked source file and no promoted TU "
            "manifest claims it (deleted, renamed or moved)")


def destination_state(dest, tracked, scores, ownership):
    """(state, detail) for the FILE a promotion named as an orphan's destination.

    THIS ANSWERS ABOUT THE WHOLE FILE, WHICH IS NOT THE QUESTION THE READER ACTS ON.
    A rewrite creates ONE member identity; whether THAT identity passes is
    `target_state()`, and the two answers disagree constantly. Measured 2026-09-10:
    of the 2,329 banked-path-to-destination pairs whose destination FAILS, 620 name a
    rewrite target that passes all five on its own, spread over 122 of the 129 failing
    destinations. Two earlier versions of this tool keyed the reader-facing claim on
    this function and were wrong for exactly that population. Use it for the one claim
    that really is about the whole file:

      `--update` absorbs a moved path only when the whole destination is CONVERTED.
      `classify_missing()` requires `dest in current` (bare path) or every
      `dest#symbol` in `current`, which is precisely `state == "converted"` here. So
      this function, and not `target_state()`, is what predicts what `--update` does.

      "converted"  tracked, and every identity it is scored under passes all five.
      "failing"    tracked, and at least one of them fails a criterion.
      "unvouched"  not tracked, or not scored, or only PARTLY scored, so this scan did
                   not look at all of it. Say so instead of guessing.

    Measured 2026-09-10 over the real manifest: of 136 distinct promoted destinations,
    7 are converted, 129 are failing and 0 are unvouched.

    The member/path split mirrors `scan()` exactly -- more than one enrolled member
    means the destination is scored as `dest#symbol`, otherwise as the bare path. It is
    not `_failures()` plus `_member_result()` because those two build a sentence for
    `why()` about ONE identity; this needs a summary of a whole file, and a destination
    with eight members produced a 700-character line when it borrowed theirs.

    `tracked`, `scores` and `ownership` are REQUIRED and may not be None. They used to
    default to None, and the None arm returned "unvouched" -- the safe answer, but an
    answer nonetheless, reached with no evidence and reached BY DEFAULT. Nothing
    exercised that arm, so flipping it to "converted" passed the whole suite: a caller
    who forgot an argument would have been told a file nothing read passes. A verdict
    with no scan behind it is not a verdict; refuse instead of returning one.
    """
    if tracked is None or scores is None or ownership is None:
        raise ValueError(
            "destination_state() needs the tracked set, the scan's scores and the "
            "ownership index. Any answer it returned without them would be a verdict "
            "about a file nothing looked at.")
    if dest not in tracked:
        return "unvouched", "it is not a tracked source file"
    members = ownership.get(dest) or []
    if len(members) <= 1:
        score = scores.get(dest)
        if score is None:
            return "unvouched", "nothing in this scan scored it"
        failed = [k for k in tiers.CRITERIA if not score[k]]
        if failed:
            return "failing", "; ".join(tiers.CRITERION_LABEL[k] for k in failed)
        return "converted", "the file passes all five criteria"
    identities = [f"{dest}#{symbol}" for symbol in members]
    # A PARTLY SCORED FILE IS NOT A PASSING FILE. Without this guard the loop below
    # would raise on the unscored member -- or, if it skipped it, would answer
    # "converted" from the members it happened to have, which is the same claim as
    # "converted" made over a smaller file than the one on disk. Deleting this guard
    # passed the whole suite before the test named for it existed, and the failure it
    # allows is silent: `--update` would then be predicted to absorb a destination
    # `classify_missing()` will not absorb, because an unscored member is not in
    # `current`.
    if any(scores.get(i) is None for i in identities):
        missing = sum(1 for i in identities if scores.get(i) is None)
        return "unvouched", (f"{missing} of its {len(identities)} enrolled members "
                             "were not scored")
    failing, criteria = [], []
    for identity in identities:
        bad = [k for k in tiers.CRITERIA if not scores[identity][k]]
        if bad:
            failing.append(identity)
            criteria.extend(k for k in bad if k not in criteria)
    if not failing:
        return "converted", f"all {len(identities)} enrolled members pass all five"
    labels = "; ".join(tiers.CRITERION_LABEL[k] for k in criteria)
    if len(failing) == 1:
        return "failing", (f"1 of its {len(identities)} enrolled members fails -- "
                           f"{failing[0]}: {labels}")
    return "failing", (f"{len(failing)} of its {len(identities)} enrolled members "
                       f"fail: {labels}")


def rewrite_target(identity, dest, ownership, symbols):
    """The identity a rewrite must put on `dest`, in the shape `scan()` will score it.

    `scan()` IS THE AUTHORITY ON THE SHAPE, not the manifest. Two derivations exist:
    `scan()` takes the enrolled-member count from `srcpath.source_definition_index()`
    (what is defined in the file), while `tu_promote.py:converted_baseline_update`
    takes it from `len(p["functions"])` (what the manifest enrolls). Measured
    2026-09-10 they agree for all 136 promoted destinations -- but that is an
    agreement, not an invariant, and only one of them decides anything: this baseline
    is compared against `scan()`'s output, so an identity in a shape `scan()` does not
    produce can never leave the report, no matter what the manifest says. Hence the
    expression below is copied from `scan()` on purpose, fallback included. If the two
    ever diverge, `tu_promote` is the one that has to move.

    The `or [stem]` fallback matters and is not decoration: an ownership index with NO
    entry for `dest` means `scan()` scores it under the bare path, so this must return
    the bare path too. The old `len(members) <= 1` swallowed the empty case into the
    single-member case and happened to agree; it agreed for a reason it did not state,
    and only its one caller stopped an empty index reaching it.

    Measured 2026-09-10: 0 of the 136 promoted destinations own a single enrolled
    member, so every real case today takes the `#symbol` branch -- which is exactly why
    hard-coding that branch went unnoticed.

    None when the symbol cannot be established: a `dest` scored per member, reached
    from a bare legacy path whose manifest row was not handed to this report. The
    caller says so rather than printing a guess. A symbol CARRIED on the identity is
    preferred over the manifest's, and is not checked here -- `target_state()` checks
    it by looking the resulting identity up in the scan, which is the only check that
    means anything: a symbol the destination does not define is scored by nothing.
    """
    members = ownership.get(dest) or [pathlib.PurePosixPath(dest).stem]
    if len(members) <= 1:
        return dest
    rel, _, symbol = identity.partition("#")
    symbol = symbol or symbols.get(rel)
    return f"{dest}#{symbol}" if symbol else None


def target_state(identity, dest, tracked, scores, ownership, symbols):
    """(state, detail, target) for the ONE identity a rewrite of `identity` creates.

    THIS IS THE QUESTION THE READER ACTS ON. `destination_state()` answers about the
    whole destination file; the reader is not told to create a file, they are told to
    create `rewrite_target()`'s single identity, and for a multi-member destination
    those are different questions with different answers. Measured 2026-09-10: 620 of
    the 2,329 banked-path-to-destination pairs whose destination FAILS name a target
    that passes all five on its own, over 122 of the 129 failing destinations. Every
    reader-facing claim about regression must be keyed here.

      "converted"  the scan scored this exact identity and it passes all five.
                   Creating it costs nothing, whatever the rest of the file does.
      "failing"    the scan scored it and it fails. The rewrite is still the correct
                   repair -- the identity belongs on the file that owns the code -- but
                   the identity is a backslide the moment it exists.
      "unvouched"  no target could be named, or the scan never scored the one named.
                   That includes a `#symbol` carried on the banked identity that the
                   destination does not define: it names nothing, so it can be scored
                   by nothing, and rewriting onto it would recreate the orphan.

    It does NOT predict `--update`. That keys on the whole file -- see
    `destination_state()` -- and the two are allowed to disagree.
    """
    if tracked is None or scores is None or ownership is None or symbols is None:
        raise ValueError(
            "target_state() needs the tracked set, the scan's scores, the ownership "
            "index and the manifest symbols. Any answer it returned without them "
            "would be a verdict about an identity nothing looked at.")
    if dest not in tracked:
        return "unvouched", "it is not a tracked source file", None
    target = rewrite_target(identity, dest, ownership, symbols)
    if target is None:
        rel = identity.split("#", 1)[0]
        return ("unvouched",
                f"it is scored per member and no manifest row here enrolls {rel} "
                "under a symbol", None)
    score = scores.get(target)
    if score is None:
        return ("unvouched",
                f"nothing in this scan scored {target} -- the destination does not "
                "define that symbol", target)
    failed = [k for k in tiers.CRITERIA if not score[k]]
    if failed:
        return ("failing", "; ".join(tiers.CRITERION_LABEL[k] for k in failed), target)
    return "converted", "it passes all five criteria", target


OrphanRow = collections.namedtuple(
    "OrphanRow", "identity tu_id dest target target_detail dest_state dest_detail")

ORPHAN_GROUPS = ("clean", "target_only", "backslide", "unvouched", "unexplained")


def orphan_groups(orphans, moves, tracked, scores, ownership, symbols):
    """Orphans partitioned into the five groups both reports need.

    THE SPLIT IS ON THE IDENTITY THE READER IS TOLD TO CREATE, not on the destination
    file. Every row carries both verdicts because the two paragraphs of the remedy
    genuinely key on different units, and each names its own:

      `clean`        the rewrite target passes AND the whole destination passes.
      `target_only`  the rewrite target passes, but the destination as a whole does
                     not. Creating the identity regresses nothing; `--update` still
                     will not absorb it, because `classify_missing()` keys absorption
                     on the file. Measured 2026-09-10: 620 of the 2,329
                     banked-path-to-destination pairs at a failing destination, over
                     122 of the 129 failing destinations. This class had no name in
                     the two previous versions of this report, which is why both of
                     them told these readers something false.
      `backslide`    the rewrite target itself fails a criterion. Still rewrite; it is
                     a backslide the moment it exists, and that is the tree's true
                     state, not an artifact of the rewrite.
      `unvouched`    no target could be named, or nothing scored the one named.
      `unexplained`  no promoted manifest entry names the file at all.

    `unexplained` rows carry None for everything after `tu_id`. One function so the
    plain report and `--check` cannot drift into disagreeing about which class an
    orphan is in.
    """
    groups = {name: [] for name in ORPHAN_GROUPS}
    for identity in orphans:
        moved = orphan_destination(identity, moves)
        if not moved:
            groups["unexplained"].append(
                OrphanRow(identity, None, None, None, None, None, None))
            continue
        tu_id, dest = moved
        tstate, tdetail, target = target_state(
            identity, dest, tracked, scores, ownership, symbols)
        dstate, ddetail = destination_state(dest, tracked, scores, ownership)
        row = OrphanRow(identity, tu_id, dest, target, tdetail, dstate, ddetail)
        if tstate == "unvouched":
            groups["unvouched"].append(row)
        elif dstate == "converted":
            # dstate "converted" means every identity `dest` is scored under passes,
            # and `target` is one of them, so tstate cannot be "failing" here.
            groups["clean"].append(row)
        elif tstate == "converted":
            groups["target_only"].append(row)
        else:
            groups["backslide"].append(row)
    return groups


def report_orphans(orphans, moves, exceptions_path, tracked, scores, ownership,
                   symbols):
    """Print the orphan diagnostic, split by what a rewrite would actually create.

    FIVE CLASSES, AND THE REMEDY TEXT DIVERGES AT EVERY SPLIT. All five are orphans by
    the same predicate. What differs is what a reader should do, and what `--update`
    does if they ignore that -- every claim below is measured, not reasoned from the
    shape of the code:

      target passes,        `--update` exits 0, writes no exception row, and ABSORBS
      destination passes    the entry. Rewrite onto the target; nothing regressed.

      target passes,        Rewrite onto the target; nothing regressed FOR THAT
      destination does not  IDENTITY, which is the claim the reader acts on. A
                            different member of the same file fails, and that is why
                            `--update` will not absorb it: exit 2 without a `--reason`,
                            and with one a row saying the LEGACY path was REMOVED,
                            which is false.

      target fails          Rewrite, but it is a backslide the moment it exists.
                            `--update` behaves as in the row above, for the same
                            file-level reason.

      target unvouched      nothing scored the identity a rewrite would create. Do not
                            rewrite onto something this tool cannot see.

      no destination        `--update` exits 2 without a `--reason`; with one it is the
                            correct command for a file that really was deleted.

    THE FIRST THREE CLASSES ARE KEYED ON THE IDENTITY, THE `--update` SENTENCES ON THE
    FILE, and that is deliberate rather than sloppy: `classify_missing()` absorbs a
    moved path only when EVERY enrolled member of the destination is CONVERTED, so
    what `--update` does is a fact about the whole file, while what the reader creates
    is one member of it. Two earlier versions of this report keyed the regression claim
    on the file too, and were wrong for 620 of the 2,329 banked paths whose destination
    fails.

    `UpdateBehaviourPin` in the test file runs the real `--update` for these classes, so
    this prose cannot quietly drift away from the tool it describes.
    """
    groups = orphan_groups(orphans, moves, tracked, scores, ownership, symbols)

    def rows(group, target=True):
        """One block per orphan, and every line names the unit it is talking about.

        `target` is off for an identity nothing scored: printing "rewrite onto X" under
        prose that says not to rewrite onto X would be the same kind of
        self-contradiction this split exists to remove.
        """
        for row in group:
            print(f"  {row.identity}\n      {orphan_reason(row.identity, moves)}")
            if target:
                print(f"      rewrite onto {row.target} -- {row.target_detail}")
                print(f"      the file it lands in, {row.dest} -- {row.dest_detail}")
            else:
                # Nothing lands anywhere in this class, so do not say it does.
                print(f"      no rewrite target this tool can vouch for -- "
                      f"{row.target_detail}")
                print(f"      the destination named, {row.dest} -- {row.dest_detail}")

    print(f"CONVERTED baseline orphan: {len(orphans)} banked identity(ies) name a file "
          "that is\nnot a tracked source any more. An orphan is scored by nothing, so "
          "no\ncriterion can fail for it and every later --check reads it as clean.")

    if groups["clean"]:
        print(f"\n{len(groups['clean'])} whose rewrite target PASSES, in a file that "
              "passes too -- REWRITE these:\n")
        rows(groups["clean"])
        print("\n  Nothing regressed, and this report checked the identity named on\n"
              "  each row rather than assuming it: that exact identity is in this\n"
              "  scan and passes all five criteria. Only the baseline was left\n"
              "  behind. Rewrite each orphan onto its target, leaving `count`\n"
              "  unchanged. That is exactly what\n"
              "  tools/tu_promote.py:converted_baseline_update does during a\n"
              "  promotion, out of the promotion's own manifest entry; a promotion\n"
              "  carried out by hand has to perform the same rewrite by hand.\n"
              "\n"
              "  Do NOT re-bank these with --update. Measured on the eight identities\n"
              "  PR #2530 had to repair: --update with no --reason exits 0, reports\n"
              "  them as a move rather than a removal, and writes no exception row. It\n"
              "  does not refuse -- it ABSORBS the entry, dropping the identity instead\n"
              "  of rewriting it and taking the evidence of the defect with it. That\n"
              "  hole is still open; failing here makes it loud, not impossible.")

    if groups["target_only"]:
        print(f"\n{len(groups['target_only'])} whose rewrite target PASSES, in a file "
              "that does NOT -- REWRITE these:\n")
        rows(groups["target_only"])
        print("\n  Nothing regressed FOR THESE IDENTITIES. That is a claim about the\n"
              "  one identity each row tells you to create, not about the file it\n"
              "  lands in, and this report checked it: the target named above is in\n"
              "  this scan and passes all five criteria. Creating it takes nothing\n"
              "  out of the CONVERTED set. What fails is a DIFFERENT member of the\n"
              "  same file -- the last line of each row names it -- and that member\n"
              "  was already failing before the rewrite and is no worse after it.\n"
              "\n"
              "  Measured 2026-09-10: 620 of the 2,329 banked paths whose destination\n"
              "  fails are in this class, spread over 122 of the 129 failing\n"
              "  destinations. It is not a corner case, and the two earlier versions\n"
              "  of this report that judged these by the file told their readers the\n"
              "  rewrite was a backslide when it was not.\n"
              "\n"
              "  THE NEXT PARAGRAPH KEYS ON THE FILE, NOT THE IDENTITY, AND THAT IS\n"
              "  NOT A CONTRADICTION. classify_missing() absorbs a moved path only\n"
              "  when EVERY enrolled member of the destination is CONVERTED, so what\n"
              "  --update does is a fact about the whole file, while what you create\n"
              "  is one member of it. A file can hold a passing target and a failing\n"
              "  sibling at the same time; then the two answers differ, and both are\n"
              "  right about their own unit.\n"
              "\n"
              "  So --update is wrong here for the file's reason, not the identity's.\n"
              "  It does not absorb these: they reach `removed`, so it exits 2\n"
              "  without a --reason, and with one it appends a row to\n"
              f"    {exceptions_path}\n"
              "  recording the LEGACY path as REMOVED. Nothing was removed. That row\n"
              "  is a permanent false statement in a log whose whole value is that it\n"
              "  can be trusted.\n"
              "\n"
              "  ORDER IS THE WHOLE REMEDY: REWRITE FIRST, THEN RE-RUN. Measured on\n"
              "  src/_ZN7dBase_cD1Ev.cpp, whose target is\n"
              "  src/actors/ActorDerived.cpp#_ZN7dBase_cD1Ev on a destination that\n"
              "  fails. Bank it first and `--update --reason` exits 0 and writes the\n"
              "  false REMOVED row against the legacy path. Rewrite the baseline\n"
              "  entry onto the target first and the next --check exits 0 outright:\n"
              "  no exception row, no --update, nothing left to bank. Do not reach\n"
              "  for --update to make this section go away.")

    if groups["backslide"]:
        print(f"\n{len(groups['backslide'])} whose rewrite target does NOT pass -- "
              "REWRITE these too,\nbut not as a clean move:\n")
        rows(groups["backslide"])
        print("\n  The rewrite is still the correct repair: the identity belongs on the\n"
              "  file that owns the code. But \"nothing regressed\" is not available\n"
              "  here, and the reason is the identity itself rather than the file\n"
              "  around it -- the target named on each row is in this scan and fails\n"
              "  the criteria listed beside it. It is a backslide the moment it\n"
              "  exists, and that is the true state of the tree, not an artifact of\n"
              "  the rewrite.\n"
              "\n"
              "  This is the ordinary case: 1,709 of the 2,329 banked paths whose\n"
              "  destination fails. A reconstructed TU must spell vague-linkage\n"
              "  symbols directly (_ZN7fBase_cnwEj, _ZN8dActor_cC2Ev,\n"
              "  _ZN8dActor_cD2Ev) or its range will not link, so no_mangled_refs\n"
              "  cannot pass for one.\n"
              "\n"
              "  Fix the criterion named above, or bank it with a reason -- but\n"
              "  REWRITE FIRST, because the order decides whether the log tells the\n"
              "  truth. Bank the orphan as it stands and --update records the LEGACY\n"
              "  path as REMOVED: exit 2 without a --reason, and with one a permanent\n"
              "  row in\n"
              f"    {exceptions_path}\n"
              "  saying a file was removed when it was absorbed. Rewrite onto the\n"
              "  target first and the next --check fails on the REWRITTEN identity,\n"
              "  which is a real backslide; --update --reason then logs that\n"
              "  identity, which is true. Either way do not carry the dangling\n"
              "  identity forward, because it is scored by nothing as it stands.\n"
              "\n"
              "  --update does not absorb this class either, and for the file's\n"
              "  reason rather than the identity's: classify_missing() keys\n"
              "  absorption on every enrolled member of the destination, and this\n"
              "  destination does not pass.")

    if groups["unvouched"]:
        print(f"\n{len(groups['unvouched'])} whose rewrite target this tool cannot "
              "vouch for:\n")
        rows(groups["unvouched"], target=False)
        print("\n  A manifest names the destination, but the identity a rewrite would\n"
              "  create is not one this scan scored: the destination is not a tracked\n"
              "  source, or no manifest row here enrolls the orphan under a symbol,\n"
              "  or the symbol carried on the banked identity is not one the\n"
              "  destination defines. That last one matters most, because it is the\n"
              "  one a hand rewrite can create: scan() only ever scores enrolled\n"
              "  members, so an identity naming a symbol the file does not define is\n"
              "  scored by nothing, exactly as the orphan was. Measured: the next\n"
              "  --check reports it as a backslide instead, \"GONE -- <symbol> is no\n"
              "  longer an enrolled member of <destination>\" -- the same missing code\n"
              "  under a different heading, and no repair at all.\n"
              "\n"
              "  Do not rewrite onto an identity this tool cannot see. Find out what\n"
              "  became of the code first -- if it is enrolled under another symbol,\n"
              "  or the file exists under another path, rewrite onto that, exactly as\n"
              "  for a destination that is present.")

    if groups["unexplained"]:
        print(f"\n{len(groups['unexplained'])} with no TU promotion destination -- FIND "
              "OUT WHAT HAPPENED:\n")
        for row in groups["unexplained"]:
            print(f"  {row.identity}\n      {orphan_reason(row.identity, moves)}")
        print("\n  Deleted, renamed, or absorbed by a promotion this tool cannot see:\n"
              "  nothing here can tell those apart, so do not guess. Find out what\n"
              "  became of the file first, then:\n\n"
              "    renamed or moved -- rewrite the identity onto the new path, exactly\n"
              "      as for a promotion destination. `classify_missing()` has no rename\n"
              "      detection, so nothing will do this for you.\n\n"
              "    absorbed by a promotion whose manifest entry is missing or is not\n"
              "      marked \"promoted\" -- fix the manifest first; the rewrite then\n"
              "      follows from it, and the next run will name the destination.\n\n"
              "    genuinely deleted -- then this is a real REMOVAL, not an orphan, and\n"
              "      re-banking it is right. Run\n"
              '        python tools/tiers_ratchet.py --update --reason "<why it left>"\n'
              "      which appends a row to\n"
              f"        {exceptions_path}\n"
              "      That is the only caller of append_exceptions() in this tool, so it\n"
              "      is the only road to that log.")

def classify_missing(missing, current, tracked, moves, ownership=None):
    """Split banked identities into clean ownership transitions and backslides.

    `absorbed_clean` is either a banked path rewritten as independently passing member
    identities, or a banked path that stopped existing only because a promoted TU
    absorbed it and every destination member is CONVERTED. Nothing readable left the
    set, so this is not a backslide and does not need an exception row.

    ONLY THE FIRST OF THOSE REACHES `--check` NOW. The second requires `rel not in
    tracked`, which is the definition of an orphaned identity, and `--check` removes
    those before calling this -- an entry still naming a file that a promotion deleted
    is a baseline that was never rewritten, not a transition to bless. This function
    keeps both branches because `--update` still calls it, where absorbing the path is
    the correct outcome: `--update` rewrites the whole baseline from the tree, so the
    identity is replaced rather than left dangling. See `orphaned_identities()`.

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
        # Orphans first, and out of the backslide accounting: an identity whose file is
        # not tracked cannot have failed a criterion, so calling it a backslide would
        # name the wrong defect and send the reader to the wrong remedy.
        orphans = orphaned_identities(banked, tracked_set)
        live = banked - set(orphans)
        left = sorted(live - current)
        absorbed_clean, missing = classify_missing(left, current, tracked_set, moves)
        gained = len(current - banked)
        if orphans:
            report_orphans(orphans, moves, args.exceptions, tracked_set, scores,
                           ownership, promoted_symbols())
            if missing:
                print()
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
            # Guarded on being a promotion DESTINATION, not on being in `moves`. The
            # old guard tested `rel in moves`, whose keys are legacy sources -- and a
            # legacy source that is still tracked cannot reach this list (0 of the 2359
            # promoted legacy entries are tracked today), while one that is not tracked
            # is peeled off as an orphan above. So it could only ever have fired in a
            # run that printed no MOVED line at all, which is what it claimed to
            # annotate. This tests the thing the paragraph is actually about.
            destinations = {dest for _tu, dest in moves.values()}
            if any(rel.split("#", 1)[0] in destinations for rel in missing):
                print("\nLines above name a promoted TU: the file absorbed the code of\n"
                      "several legacy sources and then failed a criterion. A\n"
                      "reconstructed TU must spell vague-linkage symbols directly\n"
                      "(_ZN7fBase_cnwEj, _ZN8dActor_cC2Ev, _ZN8dActor_cD2Ev) or the range\n"
                      "will not link, so no_mangled_refs cannot pass for one. Byte-match\n"
                      "outranks readability -- bank it with that as the reason.")
            return 1
        if orphans:
            # The PASS path lists absorbed_clean; so must this one. A run carrying an
            # orphan AND a genuine lossless transition must not drop the transition
            # just because something else failed in the same run.
            if absorbed_clean:
                print(f"\n{len(absorbed_clean)} banked path(s) made a lossless "
                      "ownership transition. Not orphans,\nnot backslides, and nothing "
                      "to do about them:")
                for rel in absorbed_clean:
                    print(f"  {rel}\n      "
                          f"{why(rel, scores, tracked_set, moves, ownership)}")
            print(f"\nbaseline {len(banked)}   current {len(current)}   "
                  f"({len(orphans)} orphaned, {len(live)} still naming a tracked file)")
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
    orphans = orphaned_identities(banked, tracked_set)
    live = banked - set(orphans)
    left = sorted(live - current)
    absorbed_clean, missing = classify_missing(left, current, tracked_set, moves)
    print(f"baseline               {len(banked):6d}   {args.baseline}")
    print(f"gained, not banked     {len(current - banked):6d}")
    # Mirror --check's split, through the same function, so the two reports cannot
    # drift into disagreeing about which class an orphan is in. A bare total would hide
    # the only distinction that tells the reader what to do -- and neither "a
    # destination exists" nor "the destination passes" is that distinction: whether the
    # ONE identity a rewrite creates passes is.
    groups = orphan_groups(orphans, moves, tracked_set, scores, ownership,
                           promoted_symbols())
    print(f"ORPHANED               {len(orphans):6d}   "
          "(file part is not a tracked source)"
          f"{'   <- --check would fail' if orphans else ''}")
    if orphans:
        print(f"  target passes        {len(groups['clean']):6d}   "
              "(so does its file; rewrite, nothing regressed)")
        print(f"  target passes, file  {len(groups['target_only']):6d}   "
              "does not (rewrite; --update would log a false removal)")
        print(f"  target fails         {len(groups['backslide']):6d}   "
              "(rewrite, but it is a backslide there)")
        print(f"  target unvouched     {len(groups['unvouched']):6d}   "
              "(nothing here scored the identity a rewrite makes)")
        print(f"  investigate          {len(groups['unexplained']):6d}   "
              "(deleted, renamed, or a manifest this tool cannot see)")
    for identity in orphans[:20]:
        print(f"    {identity}\n        {orphan_reason(identity, moves)}")
    if len(orphans) > 20:
        print(f"    ... and {len(orphans) - 20} more")
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
