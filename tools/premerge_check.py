#!/usr/bin/env python3
"""Run the static gates on the MERGE RESULT, before the merge, and diff the verdicts.

WHAT IT GATES
-------------
Not a branch. Not main. The tree `git merge` would produce if this PR landed right
now -- `git merge-tree --write-tree origin/main <head>` -- which is a tree that has
never existed on disk and that nothing in this repository has ever run a gate over.
Every gate is run twice, once on that tree and once on an export of `origin/main`
itself, and the two verdicts are printed side by side. The only thing this tool
fails on is a gate that is GREEN on the base and RED on the merge result.

WHY EVERY OTHER GATE IS BLIND TO THIS
-------------------------------------
`.github/workflows/pr-validate.yml` -- the job that submits to the external ROM
validator, and the only job in this tree that builds anything -- has exactly one
trigger:

    on:
      pull_request_target:

There is no `push: branches: [main]`. So **no ROM is built after a merge, ever**,
and every PR is judged against ITS OWN BASE. Two PRs that are each honestly green
can combine into a red main and nothing observes it until a person notices. That
has happened twice in four days here (#1987, #1990, both `tools/`-only changes that
landed against a base that had moved).

`.github/workflows/source-coverage.yml` is the one job that partly closes the hole:
it also runs on push to main against `github.event.before`, main's previous tip. But
that fires AFTER the merge. It tells you main is broken; it cannot tell you not to
break it. And it is one metric out of the eight gates below.

The gap this fills is therefore narrow and specific: the static gates, on the merge
result, BEFORE the merge.

WHAT THIS IS NOT: tools/validate_merge.py
------------------------------------------
`validate_merge.py` also reasons about a merge, and the overlap is worth stating so
this is not read as a second copy of it. That tool builds the report the EXTERNAL
validator publishes: the private worker owns the ROM and the compiler, commits a test
merge, runs `rombuild.py` on both sides and hands the JSON to `validate_merge.py` to
turn into a verdict. It is the authority on bytes, it is the only thing that can be,
and none of it runs here -- it needs the ROM, the compiler, and a worker this repo
does not control. It also runs on `pull_request_target`, which means it too is
answering the question against the PR's own base.

This tool runs on a laptop, in a second, over a tree that has no commit, and answers
only what text can answer. The two do not overlap; they do not even share a question.

THIS IS NOT HYPOTHETICAL
------------------------
Run by hand on 2026-08-29 against PR #2001: green on its own PR, clean-merging, no
conflict. The merge result turned `converted-ratchet` RED, because
`src/actors/ActorDerived.cpp` had landed in one of the sixteen commits the PR was
behind. Neither side shows this. The PR's tree is green. Main is green. Only the
tree that does not yet exist is red.

WHY IT NEVER BLAMES A PR FOR A RED BASE
---------------------------------------
The exit code is driven by the DIFF of two verdicts, never by the merge result's
verdict alone. A gate that is already red on `origin/main` stays red on every merge
result computed from it, and reporting that as this PR's failure would be wrong four
times over: it is not the PR's bug, the PR cannot fix it, the author would have to
learn to ignore the tool to get anything landed, and a tool people ignore gates
nothing. So a red base is printed, prominently, as `pre-existing` -- and contributes
zero to the exit code.

The four outcomes, and what each one means:

    base    merge   verdict         what to do
    ----    -----   -------         ----------
    pass    pass    ok              nothing
    pass    FAIL    REGRESSION      this is the PR's, and only this fails the run
    FAIL    FAIL    pre-existing    main is broken; not this PR's to own
    FAIL    pass    fixed           the PR repairs a broken base; still exit 0

WHY IT COMPILES NOTHING, AND WHY THAT IS A HARD REQUIREMENT
-----------------------------------------------------------
It never invokes `tools/rombuild.py` or `tools/eligible.py`, and it must not be
extended to. `build/` is shared between worktrees in this repo's normal working
setup, so two concurrent ROM builds clobber each other's intermediate state and
produce failures that belong to neither. A gate that needed a ROM build could not
run alongside the work it gates -- the same reasoning `source-coverage.yml` records
for staying on a GitHub runner. Everything below is `git cat-file` plus text
parsing.

The cost of that is stated honestly rather than hidden: this tool cannot see a byte
regression. A merge result that breaks the ROM link, or that makes a function stop
reproducing, is invisible here and always will be. What it sees is the eight static
gates, which is eight more than run today.

THE OTHER BLIND SPOT: THE MERGE TREE'S OWN GATES ARE WHAT RUN
--------------------------------------------------------------
Every gate below is invoked as `python tools/<gate>.py` INSIDE the exported merge
result, so the script that judges the merge is the script the merge contains. That is
the correct design -- it is the same choice #1994 made for the external validator, so
that a PR which changes a gate is tested by the gate it ships rather than by the one
it replaces -- but it is a real hole and it is named here rather than discovered
later: **a merge result that WEAKENS a gate reports pass on both sides.** A commit
that makes `check_dead_references.py` exit 0 unconditionally is, to this tool,
indistinguishable from a tree with no dead references. Nothing here can close that;
reading the gate's own diff is the only thing that can. `--json` carries every gate's
`rc` and `summary` so a reviewer can at least see when a gate's output changed shape.

THE GATES, AND WHICH ONES WERE VERIFIED TOOLCHAIN-FREE
------------------------------------------------------
Each was read before inclusion, because several tools in this tree self-`return`
when the compiler or `extracted/` is missing and report a PASS rather than a skip --
a fast green that means nothing. Included:

  converted-ratchet   tools/tiers_ratchet.py --check
                      Pure. `tiers.py` imports `delaunder` for `code_mask` only and
                      `demangle` for string work; neither reads a ROM. Needs
                      `git ls-files src` -- see THE `git init` STEP. Refuses an
                      absent baseline with exit 2 rather than treating it as an
                      empty set, so it cannot pass vacuously. ~70s, the slow one.

  source-coverage     tools/source_coverage.py --check --base <base> --ref <tree>
                      A DELTA gate, handled specially: see BYTES, BELOW.

  langmode-ratchet    tools/langmode_audit.py --check langmode-baseline.json
                      Pure text over `git ls-files` and the tree. A ratchet against a
                      banked baseline, which is exactly the class of gate a moved base
                      breaks, so it belongs here more than most. ~70s, the other slow
                      one -- this gate and converted-ratchet are essentially the whole
                      runtime. See THE LANGMODE BASELINE IS NOT ALWAYS THERE.

  dead-references     tools/check_dead_references.py
                      Pure text over prose files. Carries its own anti-hollow guard
                      (`SCAN TOO SMALL`, exit 2) so a scan that found nothing cannot
                      report clean. Needs `git ls-files -z`.

  src-tu-refs         tools/check_src_tu.py
                      Resolves mangled references in `src_tu/` against
                      `config/**/symbols.txt`. Text only -- the compiling half of
                      this check lives in `check_src_tu_compiles.py`, which is
                      EXCLUDED below for exactly that reason.

  header-offsets      tools/check_header_offsets.py <every tracked header>
                      Pure text. Refuses an empty file list ("an empty check is not
                      a pass"), which this tool relies on: if `git ls-files` yields
                      no headers the gate is recorded as an ERROR, not a pass.
                      Invoked in batches to stay under the Windows argv limit.

  layout-check        tools/layout_check.py
                      Filesystem and `config/**/delinks.txt` only.

  duplicate-sources   tools/check_duplicate_sources.py
                      Filesystem only. Worth its second in this particular tool: a
                      doubled stem is a *merge-shaped* defect. One PR renames
                      `X.c` -> `X.cpp`, another edits `X.c`; git merges both
                      happily and the result has both halves, one of which is
                      compiled by nothing and checked by nothing.

Deliberately EXCLUDED, each for a stated reason -- none of these is toolchain-free,
and including one would buy a green that means nothing:

  check_references.py         reads build/rombuild-eligibility.json, which only
                              `eligible.py` produces, and then validates its commit
                              stamp against HEAD. An exported tree has no HEAD.
  check_data_definitions.py   reads the objects `eligible.py` leaves in
                              build/eligible-scratch/, or rombuild.py in build/src/.
  check_layout_free.py        reads `extracted/` (the ROM dump) and needs PyYAML.
  check_python_names.py       needs pyflakes, which is not stdlib.
  check_src_tu_compiles.py    needs mwccarm.
  rombuild.py, eligible.py    forbidden outright, see above.

BYTES, BELOW: WHY source_coverage IS RUN WITH `--check`, AND HOW
-----------------------------------------------------------------
THE UNIT IS THE COVERED BYTE **SET**, NOT A BYTE COUNT. An earlier draft of this
tool ran `source_coverage.py --json` on both trees and subtracted `currentBytes` and
the `bySection` totals. That is a strictly weaker question than the one CI asks, and
the gap was measured rather than argued: relocating a single `.rodata` range in
`config/arm9/delinks.txt` by `+0x400000` -- 71,984 bytes of claimed coverage moved
to an address the cartridge does not have -- leaves `currentBytes` at 2,067,148 on
both sides, every `bySection` total identical, and the count-based check saying `ok`,
while `source_coverage.py --check` reports 71,984 B handed back to the cartridge.
Two PRs editing the same module's `delinks.txt` merge cleanly at the text level and
can trade ranges exactly like that, which makes it merge-shaped and therefore this
tool's business.

So the comparison is not reimplemented here. `source_coverage.py` merges each side
into disjoint intervals keyed by `(module, section)` and subtracts the SETS, honours
`config/source-coverage-exceptions.jsonl`, and blames each lost range on the entry
that used to own it. All of that is reused verbatim, because a second implementation
of interval arithmetic is a second thing to drift:

    <merge export>/tools/source_coverage.py --check --json
        --repo <this repo> --base <base sha> --ref <merge tree sha>

The one thing the earlier draft got right about `--check` was that it needs a base;
what it got wrong was that the merge result has nothing to hand it. `--ref` is passed
straight to `git ls-tree`, which resolves a bare TREE perfectly well, so the merge
tree that has never had a commit is a legitimate `--ref`. `--base` does take a
commit, and `origin/main` is one. That pairing asks precisely the question the
push-to-main job in `source-coverage.yml` asks -- "did main lose bytes at the moment
this merged?" -- one merge earlier.

The base side gets the same command with `--ref <base tree>`, which is a
self-comparison and therefore zero by construction. It is run anyway, and it is not
ceremony: it is the reference that keeps a broken `source_coverage.py` ON MAIN from
being charged to a PR. If the base self-check errors, coverage is `pre-existing` and
cannot fail the run. If the base self-check passes and the merge check errors, that
IS a fact about the merge result and it is a REGRESSION -- the same rule `classify()`
applies to every other gate, and the reason coverage is no longer routed around it.
An unmeasurable gate is never a pass here, and `--json` says so explicitly in
`coverageMeasured`.

`entries` and `modules` are still measured, on both sides, and are printed as context
that can never fail a run. That distinction is not fastidiousness, it is a
measurement. The first version of this tool failed on a decrease in ANY of the four
numbers `--json` reports, and the first time it was pointed at real PRs it called two
of them red -- #2023 (`entries` 11025 -> 11011) and #2024 (11025 -> 11017) -- with
every byte total UNCHANGED. Both are TU promotions, and that is exactly the false
alarm `source_coverage.py` exists to avoid: promotion re-partitions the address
space, so thirty-eight per-function delinks entries become three merged ones covering
the same bytes and the entry count falls by construction with nothing handed back to
the cartridge. A gate that goes red on every TU promotion is a gate people learn to
click past, which is the same failure as a gate that never goes red at all.

THE LANGMODE BASELINE IS NOT ALWAYS THERE
------------------------------------------
`langmode-ratchet.yml` reads a ROOT `langmode-baseline.json` when one is present and
otherwise falls back to `_chaos_data/langmode-baseline.json`, checked out from the
`chaos-data` BRANCH. That fallback cannot be reproduced here: `_chaos_data/` is not
tracked, so it is absent from any `checkout-index` export, and the root file is a
temporary escape hatch that is meant to be deleted once a booked rise has landed.
When the root file is missing the gate is recorded as an ERROR, never a pass -- and
because the base export is missing it too in that case, the pair classifies as
`pre-existing` and no PR is blamed for it. What this must never do is report a green
langmode ratchet because it could not find the thing to ratchet against.

THE `git init` STEP, WHICH IS NOT OPTIONAL
------------------------------------------
A tree exported with `git checkout-index --prefix=` is a plain directory, and four
of the gates above shell out to `git -C <repo> ls-files`, which fails there with
exit 128 -- reported by the gate as an error, or in the worst shape as an empty file
list. So each export gets a `git init` and a copy of the temporary index the export
was made from. That index is the whole point: it is what makes `ls-files` enumerate
the merge result's files rather than nothing.

AND THE EXPORT IS COUNTED, FOR THE SAME REASON check_dead_references COUNTS
---------------------------------------------------------------------------
An export that is empty, truncated, or left over from a previous run makes every gate
ERROR on BOTH sides, which classifies as `pre-existing`, exits 0, and prints "Safe to
merge". That is the hollow green this whole tool exists to prevent, arriving through
its own front door. So `export_tree` asserts three things before handing the
directory back: the tree names at least EXPORT_FLOOR files (main carries ~12,770, and
anything under a few thousand is not this repository); the index the export carries
enumerates exactly the tree's path set; and every one of those paths is on disk. A
short export is a TOOL ERROR, exit 2, not a gate verdict.

Each export directory is also removed before it is written, not merged into -- the
first version used `mkdir(exist_ok=True)` plus `checkout-index -f`, which overwrites
every file the new tree has and silently keeps every file it does NOT, so a reused
directory reported gate verdicts for a tree corresponding to no commit. And each run
gets its own subdirectory under `--tmp`, so two concurrent runs sharing one `--tmp`
neither collide nor `rmtree` each other's exports on the way out.

WHAT A LEGITIMATE FAILURE LOOKS LIKE
-------------------------------------
`REGRESSION` on a gate means: rebase the PR onto current main (or merge main into
it) and fix what the combination broke, exactly as if CI had failed -- because the
combination genuinely is broken, and merging it makes main red for everybody. The
usual cause is a baseline or a ratchet: the PR was cut when the banked set was
smaller, and a file that landed meanwhile no longer passes.

`CONFLICT` is reported separately and is not a gate failure. It means `git
merge-tree` could not produce a tree at all, so there was nothing to run gates on;
the PR needs a rebase before this question can even be asked.

ADVISORY, ON PURPOSE
--------------------
Nothing in `.github/workflows/` runs this. It is a command you run before you press
merge, or before you tell somebody else to. Wiring it into CI is a separate decision
with its own costs -- ~90 seconds per PR, a second full checkout, and a gate that
goes red for reasons the PR author did not write -- and that decision is not made
here.

USAGE
-----
    python tools/premerge_check.py 2001                 # a PR number
    python tools/premerge_check.py 2001 2003 2011       # several; one base export
    python tools/premerge_check.py <head-sha>           # any commit-ish
    python tools/premerge_check.py origin/main          # must be all-green, exit 0
    python tools/premerge_check.py 2001 --fetch         # pull the head object first
    python tools/premerge_check.py 2001 --json
    python tools/premerge_check.py 2001 --keep-trees    # leave the exports on disk

`gh` knows a PR's head sha; this clone may not have the OBJECT. Without `--fetch`
that is a named error with the `git fetch origin pull/N/head` to fix it, not a
`merge-tree` failure about "something we can merge". The default is not to touch the
network -- or the object store -- unless asked.

`--fetch` ALSO refreshes the base. That matters more than it sounds: the entire
premise of this tool is "your base moved and you did not notice", and reading a
stale `origin/main` out of the local object store answers the question against a main
that no longer exists. Without `--fetch` the base's age is printed on every run, and
a base older than BASE_STALE_HOURS is called out by name.

The base export and its gate run happen ONCE and are reused by every target, so six
PRs in one invocation cost roughly one base run plus six merge runs, not twelve runs.

Exit codes:
    0   no gate went green -> red
    1   at least one REGRESSION
    2   the tool could not answer (bad ref, gh missing, git too old, export failed)
    3   at least one target CONFLICTS and nothing regressed

ONE exit code covers the WHOLE run, and it is the worst target's. `premerge_check.py
2001 2024` exits 1 because #2001 regresses, even though #2024 is clean. That is the
right answer for a person -- the run as a whole is not safe to act on -- but anything
consuming the exit code alone cannot tell which target was at fault. Pass one target
per invocation, or read `--json`, where every target carries its own `outcome`.

Stdlib only. Exported trees are removed with `shutil.rmtree`; git's own removal is
never used, because in this repo a worktree's `extracted/` is an NTFS junction to
the only surviving ROM dump and git's remover recurses through it.
"""
import argparse
import json
import os
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile
import time

REPO = pathlib.Path(__file__).resolve().parent.parent

# Per-gate wall clock. converted-ratchet and langmode-ratchet are ~70s each on this
# tree; the rest are seconds.
GATE_TIMEOUT = 900

# `git ls-files` can hand back 500+ headers and Windows caps a command line at 32k.
HEADER_BATCH = 120

# An export smaller than this is not this repository. main carries ~12,770 tracked
# files; the floor is deliberately far below that, because the failure this guards
# against is a truncated or empty export, not a tree that legitimately shrank.
EXPORT_FLOOR = 4000

# How old `origin/main` may be before the run says so out loud.
BASE_STALE_HOURS = 12


# --------------------------------------------------------------------------- #
# Gate definitions.
#
# `commands(tree)` returns a LIST of argv lists so a gate can be batched; the gate's
# return code is the worst of them. Everything here must be pure Python over the
# exported tree -- see the module docstring for what was excluded and why.
# --------------------------------------------------------------------------- #

def _headers(tree):
    """Every tracked header, via the index the export carries.

    Returns [] when git cannot answer, which the caller turns into an ERROR rather
    than a pass -- check_header_offsets exits 1 on an empty list by design, and
    reading that as a gate failure would blame the PR for a broken export.
    """
    out = subprocess.run(["git", "-C", str(tree), "ls-files",
                          "include/*.h", "include/**/*.h"],
                         capture_output=True, text=True)
    if out.returncode != 0:
        return []
    return [ln for ln in out.stdout.splitlines() if ln.strip()]


def _header_commands(tree):
    hdrs = _headers(tree)
    if not hdrs:
        return None  # -> ERROR: an empty header check is not a pass
    return [[sys.executable, "tools/check_header_offsets.py", *hdrs[i:i + HEADER_BATCH]]
            for i in range(0, len(hdrs), HEADER_BATCH)]


LANGMODE_BASELINE = "langmode-baseline.json"


def _langmode_commands(tree):
    """The ratchet, but only when the thing to ratchet against is IN the tree.

    `langmode-ratchet.yml` falls back to `_chaos_data/langmode-baseline.json` -- a
    second checkout of the `chaos-data` branch -- when the root override is absent.
    That path is untracked and therefore cannot exist in a `checkout-index` export, so
    the fallback is unreproducible here. Raising makes run_gate record an ERROR; the
    one thing that must not happen is a green langmode ratchet measured against
    nothing, which is what returning a `--check` of a missing file would produce.
    """
    if not (pathlib.Path(tree) / LANGMODE_BASELINE).is_file():
        raise RuntimeError(
            f"{LANGMODE_BASELINE} is not in this tree. CI falls back to "
            f"_chaos_data/{LANGMODE_BASELINE}, checked out from the chaos-data "
            f"BRANCH, which is untracked and cannot appear in an export -- so the "
            f"ratchet cannot be answered here. Not a pass.")
    return [[sys.executable, "tools/langmode_audit.py", "--check", LANGMODE_BASELINE]]


GATES = [
    {
        "key": "converted-ratchet",
        "commands": lambda t: [[sys.executable, "tools/tiers_ratchet.py", "--check"]],
        "note": "pure python; needs git ls-files; refuses an absent baseline",
    },
    {
        "key": "langmode-ratchet",
        "commands": _langmode_commands,
        "note": "pure python; a missing root baseline is an ERROR, not a pass",
    },
    {
        "key": "dead-references",
        "commands": lambda t: [[sys.executable, "tools/check_dead_references.py"]],
        "note": "pure text; own SCAN TOO SMALL guard",
    },
    {
        "key": "src-tu-refs",
        "commands": lambda t: [[sys.executable, "tools/check_src_tu.py"]],
        "note": "text only; the compiling half is check_src_tu_compiles.py (excluded)",
    },
    {
        "key": "header-offsets",
        "commands": _header_commands,
        "note": "pure text; batched; empty file list is an ERROR, not a pass",
    },
    {
        "key": "layout-check",
        "commands": lambda t: [[sys.executable, "tools/layout_check.py", "--quiet"]],
        "note": "filesystem + config/**/delinks.txt",
    },
    {
        "key": "duplicate-sources",
        "commands": lambda t: [[sys.executable, "tools/check_duplicate_sources.py"]],
        "note": "filesystem only; a merge-shaped defect",
    },
]

# Handled outside GATES: it is a DELTA between two trees, so its command line needs
# both shas and cannot be built from one. It goes through classify() like every other
# gate all the same. See BYTES, BELOW.
COVERAGE_GATE = "source-coverage"

# Reported, never fatal -- see coverage_context() for the measurement that proved this
# on real PRs the first time this tool was pointed at them. The verdict comes from
# source_coverage.py's own byte-SET subtraction, never from a count.
COVERAGE_CONTEXT_KEYS = ("entries", "modules")


# --------------------------------------------------------------------------- #
# Pure logic. Everything below this line is a function of data, not of the
# filesystem, so tools/test_premerge_check.py can exercise it with no git, no gh,
# no compiler and no extracted/.
# --------------------------------------------------------------------------- #

def classify(base_status, merge_status):
    """One gate's verdict pair -> the word that goes in the table.

    `pass` and anything-else. An ERROR is red: a gate that could not run on the merge
    result but ran on the base is a real signal about the merge result, and calling
    it a pass is how gates come to mean nothing.
    """
    base_ok = base_status == "pass"
    merge_ok = merge_status == "pass"
    if base_ok and merge_ok:
        return "ok"
    if base_ok and not merge_ok:
        return "REGRESSION"
    if not base_ok and merge_ok:
        return "fixed"
    return "pre-existing"


def diff_verdicts(base, merge):
    """[{gate, base, merge, verdict}] for every gate either side reports.

    `base` and `merge` are {gate_key: {"status": ...}}. A gate missing from one side
    is recorded as `absent`, which is not a pass, so a gate that vanished from the
    merge result surfaces as a REGRESSION rather than silently dropping out of the
    table.
    """
    rows = []
    for key in sorted(set(base) | set(merge)):
        b = base.get(key, {}).get("status", "absent")
        m = merge.get(key, {}).get("status", "absent")
        rows.append({"gate": key, "base": b, "merge": m,
                     "verdict": classify(b, m)})
    return rows


def regressions(rows):
    return [r for r in rows if r["verdict"] == "REGRESSION"]


def coverage_lines(findings, limit=12):
    """`source_coverage.py --check --json` findings -> the prose a reader needs.

    Presentation only. Every number here was computed by `source_coverage.py` from the
    subtraction of two merged interval SETS, and every `why` string is its own
    `blame()`. Nothing in this file re-derives a byte, on purpose: the measurement
    that motivated this shape -- a `.rodata` range relocated by +0x400000, 71,984 B
    handed back, with `currentBytes` and every `bySection` total IDENTICAL on both
    sides -- is invisible to any comparison of counts, and the way not to have two
    interval implementations drift apart is not to write the second one.
    """
    if not findings:
        return []
    lost = findings.get("unwaivedLostBytes") or 0
    if not lost:
        return []
    out = [f"{lost:,} B stopped being built from source "
           f"(base {findings.get('baseBytes', 0):,} B -> "
           f"{findings.get('currentBytes', 0):,} B)"]
    ranges = findings.get("lost") or []
    for r in ranges[:limit]:
        out.append(f"-{r.get('bytes', 0):>8,} B  {r.get('module', '?'):<8} "
                   f"{r.get('section', '?'):<7} {r.get('start')}-{r.get('end')}")
        for owner in (r.get("owners") or [])[:2]:
            out.append(f"             was: {owner.get('path')}")
            out.append(f"             why: {owner.get('why')}")
    if len(ranges) > limit:
        out.append(f"... and {len(ranges) - limit} more range(s); run "
                   f"source_coverage.py --check for the lot")
    waived = findings.get("waived") or []
    if waived:
        out.append(f"({sum(w.get('bytes', 0) for w in waived):,} B in {len(waived)} "
                   f"range(s) waived by config/source-coverage-exceptions.jsonl)")
    return out


def coverage_context(base_metrics, merge_metrics, byte_regressions=()):
    """`entries` and `modules` movement. Printed, never fatal, and here is why.

    The first version of this tool failed a run on any decrease in any of the four
    numbers `source_coverage.py --json` reports. Pointed at real PRs on 2026-08-30 it
    immediately called two of them red -- #2023 (`entries` 11025 -> 11011) and #2024
    (11025 -> 11017) -- with `currentBytes` and every per-section total UNCHANGED.

    Both are TU-promotion PRs, and that is precisely the false alarm
    `source_coverage.py` was written to avoid; its own docstring spells it out. TU
    promotion re-partitions the address space: thirty-eight per-function delinks
    entries become three merged ones covering the same bytes. The entry count drops
    by construction and NOTHING has been handed back to the cartridge. Comparing
    entry counts reports every consolidation as a loss.

    So the unit is the covered byte SET, exactly as it is in the tool being wrapped --
    and it is that tool's own subtraction that produces it, see coverage_lines().
    `entries` and `modules` are still shown, because a large swing in either is worth
    a glance, but they cannot fail a run. A gate that reddens on every TU promotion
    is a gate people learn to click past.

    `byte_regressions` is coverage_lines()' output: the reassuring "consolidation"
    reading is only available when the byte set really is flat.
    """
    if base_metrics is None or merge_metrics is None:
        return []
    out = []
    for key in COVERAGE_CONTEXT_KEYS:
        b, m = base_metrics.get(key), merge_metrics.get(key)
        if isinstance(b, int) and isinstance(m, int) and m != b:
            # The reassuring reading is only available when the bytes really are
            # flat. If bytes ALSO fell, the drop is corroborating a real loss and
            # must not be labelled a consolidation.
            note = ("  (consolidation, not a loss -- bytes are flat)"
                    if m < b and not byte_regressions else "")
            out.append(f"{key}: {b} -> {m} ({m - b:+d}){note}")
    return out


def parse_merge_tree(rc, stdout):
    """`git merge-tree --write-tree` output -> (tree_sha, conflicts).

    Exit 0 is a clean merge and stdout's first line is the tree. Exit 1 means git
    still wrote a tree but recorded conflicts; the first line is that tree and the
    rest describes them. Anything else is git refusing to answer -- no tree, and the
    caller must report a TOOL ERROR, not a conflict and not a gate failure.
    """
    lines = stdout.splitlines()
    first = lines[0].strip() if lines else ""
    is_oid = bool(re.fullmatch(r"[0-9a-f]{40,64}", first))
    if rc == 0:
        if not is_oid:
            return None, ["git merge-tree exited 0 but wrote no tree oid"]
        return first, []
    if rc == 1:
        detail = [ln for ln in lines[1:] if ln.strip()]
        return (first if is_oid else None), detail or ["conflict (no detail reported)"]
    return None, None  # None conflicts == "git could not answer at all"


def is_pr_number(spec):
    return bool(re.fullmatch(r"#?\d+", spec.strip()))


def resolve_pr(spec, runner=subprocess.run, repo=None):
    """PR number -> (head_sha, label, info). ValueError with a sentence, never a traceback.

    `repo` is the LOCAL checkout, and it is passed as gh's working directory. Without
    it gh resolves the PR against whatever repository the shell happens to be sitting
    in -- so `--repo /some/other/clone` silently asked GitHub about the wrong project,
    or about no project at all, and the failure message blamed authentication for what
    was a working-directory problem. `gh` derives owner/name from that directory's
    remotes, which is the right answer for every clone of this repo including a
    worktree.

    `info` carries `state` and `baseRefName`, which the caller acts on: a CLOSED or
    MERGED PR, or one targeting a branch other than the base being merged into, would
    otherwise produce a confident green about a merge nobody is going to perform.

    `runner` is injected so the failure paths -- gh absent, gh unauthenticated, PR
    not found -- are testable with no gh installed. That matters more than usual
    here: an unauthenticated gh is the single most likely way a person meets this
    tool for the first time, and a traceback would read as the tool being broken.
    """
    num = spec.strip().lstrip("#")
    argv = ["gh", "pr", "view", num, "--json", "headRefOid,title,state,baseRefName"]
    kwargs = {"capture_output": True, "text": True}
    if repo is not None:
        kwargs["cwd"] = str(repo)
    try:
        out = runner(argv, **kwargs)
    except FileNotFoundError:
        raise ValueError(
            f"cannot resolve PR #{num}: the `gh` CLI is not on PATH.\n"
            f"  Install it, or pass the head sha directly: "
            f"premerge_check.py <sha>")
    except OSError as exc:
        raise ValueError(f"cannot resolve PR #{num}: could not run gh ({exc})")
    if out.returncode != 0:
        err = (out.stderr or "").strip().splitlines()
        hint = err[-1] if err else f"gh exited {out.returncode}"
        where = f" (asked from {repo})" if repo is not None else ""
        raise ValueError(
            f"cannot resolve PR #{num}: {hint}{where}\n"
            f"  gh reads the repository from that directory's git remotes. If that is\n"
            f"  not the project the PR lives in, pass the head sha directly instead of\n"
            f"  a number. If it IS, and this is an auth problem, `gh auth login`.")
    try:
        data = json.loads(out.stdout)
    except (ValueError, TypeError):
        raise ValueError(f"cannot resolve PR #{num}: gh returned no usable JSON")
    sha = data.get("headRefOid")
    if not sha:
        raise ValueError(f"cannot resolve PR #{num}: gh reported no headRefOid")
    title = (data.get("title") or "").strip()
    label = f"#{num} {title}"[:72] if title else f"#{num}"
    info = {"state": (data.get("state") or "").strip().upper() or None,
            "baseRefName": (data.get("baseRefName") or "").strip() or None}
    return sha, label, info


def pr_warnings(num, info, base_ref):
    """The `state`/`baseRefName` facts worth interrupting a green run for.

    Both were already being requested from gh and neither was read. A merged PR gates
    a merge that has already happened -- and whose result may be the base itself -- and
    a PR targeting `port` or a stacked branch is not merging into `origin/main` at all,
    so a clean verdict against origin/main is an answer to a question nobody asked.
    Reported as warnings rather than errors: the run is still meaningful, it just is
    not the run the reader thinks it is.
    """
    out = []
    state = (info or {}).get("state")
    if state and state != "OPEN":
        out.append(f"PR #{num} is {state}, not OPEN. This gates a merge that is not "
                   f"pending; if it has already landed, the base below may contain it.")
    target = (info or {}).get("baseRefName")
    if target:
        short = base_ref.rsplit("/", 1)[-1]
        if target != short:
            out.append(f"PR #{num} targets `{target}`, but this run merges it into "
                       f"`{base_ref}`. That is not the merge GitHub would perform.")
    return out


# --------------------------------------------------------------------------- #
# git plumbing.
# --------------------------------------------------------------------------- #

def git(repo, *args, check=False):
    out = subprocess.run(["git", "-C", str(repo), *args],
                         capture_output=True, text=True)
    if check and out.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)}: {out.stderr.strip()}")
    return out


def base_age(repo, sha, now=None):
    """(hours_old_or_None, a phrase for the header line).

    `origin/main` is read out of the local object store and NOTHING refreshes it. A
    base fetched three days ago is a base three days of merges out of date, and every
    verdict below is measured against it. The tool cannot make the fetch decision for
    the reader, but it can refuse to let the reader assume.
    """
    out = git(repo, "log", "-1", "--format=%ct", sha)
    if out.returncode != 0 or not out.stdout.strip().isdigit():
        return None, "(age unknown)"
    hours = ((now if now is not None else time.time())
             - int(out.stdout.strip())) / 3600.0
    if hours < 0:
        return 0.0, "(committed just now)"
    if hours < 1:
        return hours, f"(committed {hours * 60:.0f}m ago)"
    if hours < 48:
        return hours, f"(committed {hours:.0f}h ago)"
    return hours, f"(committed {hours / 24:.1f} DAYS ago)"


def git_supports_write_tree(repo):
    out = git(repo, "merge-tree", "-h")
    return "--write-tree" in (out.stdout + out.stderr)


def _paths_z(out):
    return {p for p in out.split("\0") if p}


def verify_export(repo, tree_sha, dest, floor=EXPORT_FLOOR):
    """Raise unless `dest` is a COMPLETE materialisation of `tree_sha`.

    The same reasoning `check_dead_references.py` records for its own `SCAN TOO SMALL`
    guard, applied one level up. An empty or truncated export makes every gate ERROR
    on BOTH sides, `classify()` reads that as `pre-existing`, the run exits 0 and
    prints "Safe to merge". A gate that cannot see the tree must say so, not shrug.

    Three assertions, each catching a different way it has actually gone wrong:
      * the FLOOR    -- a tree with a few hundred files is not this repository, so
                        something upstream (a bad tree sha, a partial merge result)
                        handed us the wrong object;
      * INDEX vs TREE -- the copied index is what `git ls-files` answers from, and four
                        gates shell out to it. If the copy failed or came from another
                        export, the gates enumerate somebody else's file list;
      * TREE vs DISK -- `checkout-index` stopping part-way leaves an index that still
                        promises files nothing wrote, which is precisely the shape that
                        makes a gate ERROR for a reason that has nothing to do with the
                        merge.
    """
    listing = subprocess.run(
        ["git", "-C", str(repo), "ls-tree", "-r", "-z", "--name-only", tree_sha],
        capture_output=True, text=True)
    if listing.returncode != 0:
        raise RuntimeError(f"export {tree_sha[:12]}: cannot list the tree: "
                           f"{listing.stderr.strip()}")
    expected = _paths_z(listing.stdout)
    if len(expected) < floor:
        raise RuntimeError(
            f"export {tree_sha[:12]}: EXPORT TOO SMALL -- the tree names "
            f"{len(expected)} file(s), under the {floor} floor. This repository "
            f"carries ~12,770. Refusing to run gates over it: every gate would error "
            f"on both sides and the run would report 'pre-existing' and exit 0.")

    indexed = subprocess.run(["git", "-C", str(dest), "ls-files", "-z"],
                             capture_output=True, text=True)
    if indexed.returncode != 0:
        raise RuntimeError(f"export {tree_sha[:12]}: `git ls-files` does not work in "
                           f"the export: {indexed.stderr.strip()}")
    got = _paths_z(indexed.stdout)
    if got != expected:
        only_tree = sorted(expected - got)[:5]
        only_idx = sorted(got - expected)[:5]
        raise RuntimeError(
            f"export {tree_sha[:12]}: the export's index does not match the tree "
            f"({len(got)} indexed vs {len(expected)} in the tree). "
            f"missing: {only_tree or '-'}; unexpected: {only_idx or '-'}")

    on_disk = set()
    dest = pathlib.Path(dest)
    for root, dirs, files in os.walk(dest):
        if ".git" in dirs:
            dirs.remove(".git")
        base = pathlib.Path(root)
        for f in files:
            on_disk.add((base / f).relative_to(dest).as_posix())
    absent = expected - on_disk
    if absent:
        raise RuntimeError(
            f"export {tree_sha[:12]}: {len(absent)} of {len(expected)} tracked file(s) "
            f"are in the index but not on disk -- a truncated checkout-index. "
            f"e.g. {sorted(absent)[:5]}")
    return len(expected)


def export_tree(repo, tree_sha, dest, scratch, floor=EXPORT_FLOOR):
    """Materialise a tree at `dest`, with a working `git ls-files`.

    Four steps, and the last two are the ones people rediscover the hard way.

    `dest` is REMOVED first, never merged into. `mkdir(exist_ok=True)` plus
    `checkout-index -a -f` overwrites every path the new tree has and silently keeps
    every path it does not, so a reused directory is the union of two trees and
    corresponds to no commit. A file deleted by the merge is still there, and a gate
    that reads the filesystem -- duplicate-sources, layout-check -- reports on it.

    And a tree checked out with `checkout-index --prefix=` is a plain directory: `git
    -C dest ls-files` exits 128 there, and four of the gates shell out to exactly that.
    So the directory gets its own `.git` and the index the export was made FROM is
    copied into it. Nothing is ever committed; the index alone answers ls-files.

    Finally verify_export() refuses to hand back a short or inconsistent export.
    """
    dest = pathlib.Path(dest)
    if dest.exists():
        # shutil, never `git worktree remove`: in this repo an export's neighbours can
        # be NTFS junctions to the only surviving ROM dump and git's remover recurses
        # through them. This directory is ours and holds nothing else.
        shutil.rmtree(dest, ignore_errors=True)
        if dest.exists():
            raise RuntimeError(f"export {tree_sha[:12]}: {dest} already exists and "
                               f"could not be removed; refusing to write a tree into "
                               f"another tree's directory")
    dest.mkdir(parents=True)
    idx = pathlib.Path(scratch) / (dest.name + ".index")
    env = dict(os.environ, GIT_INDEX_FILE=str(idx))

    for args in (["read-tree", tree_sha],
                 ["checkout-index", "-a", "-f",
                  "--prefix=" + dest.as_posix().rstrip("/") + "/"]):
        out = subprocess.run(["git", "-C", str(repo), *args],
                             capture_output=True, text=True, env=env)
        if out.returncode != 0:
            raise RuntimeError(f"export {tree_sha[:12]}: git {args[0]}: "
                               f"{out.stderr.strip()}")

    out = subprocess.run(["git", "init", "-q", str(dest)],
                         capture_output=True, text=True)
    if out.returncode != 0:
        raise RuntimeError(f"export {tree_sha[:12]}: git init: {out.stderr.strip()}")
    try:
        shutil.copyfile(idx, dest / ".git" / "index")
    except OSError as exc:
        # Callers catch RuntimeError. An OSError escaping here reached the top level
        # as a traceback and exit 1 -- indistinguishable, to anything reading the exit
        # code, from a REGRESSION the PR is answerable for.
        raise RuntimeError(f"export {tree_sha[:12]}: could not install the index "
                           f"({exc}); `git ls-files` would enumerate nothing and four "
                           f"gates would silently check an empty file list")
    verify_export(repo, tree_sha, dest, floor)
    return dest


# --------------------------------------------------------------------------- #
# Running the gates.
# --------------------------------------------------------------------------- #

def worst_rc(codes):
    """The return code that should decide a batched gate's verdict.

    NOT `max()`. On POSIX a child killed by a signal has a NEGATIVE returncode, so
    `max(0, -11)` is 0 and a batch that was OOM-killed among passing batches reported
    a clean PASS. `header-offsets` runs five batches of ~120 headers and CI is Linux,
    which is exactly where that arrives first. Any non-zero code is a failure; a
    negative one, or anything above 1, is an ERROR rather than a gate saying `no`.
    """
    nonzero = [c for c in codes if c != 0]
    if not nonzero:
        return 0
    abnormal = [c for c in nonzero if c < 0 or c > 1]
    return abnormal[0] if abnormal else 1


def status_for_rc(rc):
    """0 pass, 1 fail, anything else -- including a negative signal code -- error."""
    if rc == 0:
        return "pass"
    return "fail" if rc == 1 else "error"


def run_gate(gate, tree):
    """One gate against one exported tree -> {status, rc, summary, output}."""
    try:
        cmds = gate["commands"](tree)
    except Exception as exc:                       # noqa: BLE001 - report, never crash
        return {"status": "error", "rc": None,
                "summary": f"could not build the command line: {exc}", "output": ""}
    if not cmds:
        return {"status": "error", "rc": None,
                "summary": "no work to do -- refusing to report a pass "
                           "over an empty input set", "output": ""}

    codes, chunks = [], []
    for argv in cmds:
        try:
            out = subprocess.run(argv, cwd=str(tree), capture_output=True,
                                 text=True, errors="replace", timeout=GATE_TIMEOUT)
        except subprocess.TimeoutExpired:
            return {"status": "error", "rc": None,
                    "summary": f"timed out after {GATE_TIMEOUT}s", "output": ""}
        except OSError as exc:
            return {"status": "error", "rc": None,
                    "summary": f"could not run: {exc}", "output": ""}
        chunks.append((out.stdout or "") + (out.stderr or ""))
        codes.append(out.returncode)

    text = "\n".join(c for c in chunks if c.strip())
    lines = [ln for ln in text.splitlines() if ln.strip()]
    worst = worst_rc(codes)
    status = status_for_rc(worst)
    if worst < 0:
        lines.append(f"a batch was killed by signal {-worst}")
    return {"status": status, "rc": worst,
            "summary": summarise(lines, status), "output": text}


# The line a reader wants when a gate goes red. The LAST line is right for a gate
# that ends with a verdict (`CONVERTED backslide: ...`, `check_src_tu: N unresolved`)
# and useless for a batched one -- header-offsets ends on whichever header happened
# to sort last, which says nothing about why it failed. So on a red gate the first
# line that looks like the complaint wins, and the last line is the fallback.
_COMPLAINT = re.compile(
    r"FAIL|REFUS|ERROR|backslide|unresolved|DUPLICATE|TOO SMALL|"
    r"do(es)? not (exist|resolve)|[1-9]\d* mismatched", re.I)


def summarise(lines, status):
    if not lines:
        return "(no output)"
    if status != "pass":
        for ln in lines:
            if _COMPLAINT.search(ln):
                return ln.strip()[:200]
    return lines[-1].strip()[:200]


def run_all_gates(tree):
    return {g["key"]: run_gate(g, tree) for g in GATES}


def _run_coverage(tree, extra):
    """<tree>'s own source_coverage.py -> (rc, parsed_json_or_None, output_text).

    The child's stdout AND stderr are kept. The first version of this threw them away
    and returned a bare None, so `not measured on both trees -- see above` printed with
    nothing above it: the one thing a reader needed -- the Python traceback, the
    `unrecognized arguments`, the malformed-waiver complaint -- had been discarded by
    the function that saw it.
    """
    argv = [sys.executable, "tools/source_coverage.py", *extra]
    try:
        out = subprocess.run(argv, cwd=str(tree), capture_output=True, text=True,
                             errors="replace", timeout=GATE_TIMEOUT)
    except subprocess.TimeoutExpired:
        return None, None, f"source_coverage.py timed out after {GATE_TIMEOUT}s"
    except OSError as exc:
        return None, None, f"could not run source_coverage.py: {exc}"
    text = (out.stdout or "") + (out.stderr or "")
    try:
        data = json.loads(out.stdout)
    except ValueError:
        data = None
    return out.returncode, data, text


def coverage_metrics(tree):
    """The no-base measurement of one tree -> {"data": .. or None, "output": str}.

    Context ONLY -- `entries` and `modules`. It carries no verdict and cannot fail a
    run; coverage_check() is what decides. Kept separate because the two questions are
    genuinely different: this one is "how big is this tree", that one is "what did the
    merge hand back".
    """
    rc, data, text = _run_coverage(tree, ["--json"])
    if rc != 0:
        data = None
    if rc == 0 and data is not None:
        summary = ""
    elif rc is None:
        summary = text.strip()[:200]
    else:
        summary = (f"source_coverage.py --json exited {rc}: "
                   + " / ".join(ln.strip() for ln in text.splitlines()
                                if ln.strip())[:160])
    return {"data": data, "output": text, "summary": summary}


def coverage_check(tree, repo, ref, base_ref):
    """The VERDICT. `<tree>`'s own source_coverage.py, comparing base_ref -> ref.

    `--ref` goes straight to `git ls-tree`, which resolves a bare TREE, so the merge
    result -- a tree with no commit -- is a perfectly good `--ref`. `--base` wants a
    commit and gets one. Everything about the comparison, including the interval
    subtraction and `config/source-coverage-exceptions.jsonl`, is the wrapped tool's;
    nothing is re-derived here.

    -> {status, rc, summary, output, findings}. `error` is not `pass`: see classify().
    """
    rc, data, text = _run_coverage(
        tree, ["--check", "--json", "--repo", str(repo), "--base", base_ref,
               "--ref", ref])
    if rc is None:
        return {"status": "error", "rc": None, "summary": text.strip()[:200],
                "output": text, "findings": None}
    status = status_for_rc(rc)
    if status != "error" and data is None:
        # rc said it ran; the document is unreadable. That is not a pass either.
        status, summary = "error", ("source_coverage.py exited "
                                    f"{rc} but wrote no usable JSON")
    elif status == "error":
        summary = (f"source_coverage.py --check exited {rc}: "
                   + " / ".join(ln.strip() for ln in text.splitlines()
                                if ln.strip())[:160])
    else:
        lost = (data or {}).get("unwaivedLostBytes") or 0
        summary = (f"{lost:,} B stopped being built from source" if lost
                   else f"source-built bytes flat or up "
                        f"({(data or {}).get('currentBytes', 0):,} B)")
    return {"status": status, "rc": rc, "summary": summary, "output": text,
            "findings": data}


# --------------------------------------------------------------------------- #
# Reporting.
# --------------------------------------------------------------------------- #

def coverage_row(base_check, merge_check):
    """The coverage gate's row, through the same classify() as everything else.

    It used to be routed around classify() entirely, with a base verdict of `(ref)` and
    an `error` merge verdict that contributed nothing to the exit code -- the precise
    treatment classify()'s own docstring forbids, in the same file. It is a gate. It is
    classified like a gate.
    """
    b = (base_check or {}).get("status", "absent")
    m = (merge_check or {}).get("status", "absent")
    return {"gate": COVERAGE_GATE, "base": b, "merge": m, "verdict": classify(b, m)}


def print_table(rows, cov_row, cov_lines=(), cov_context=(), cov_detail=(), say=print):
    width = max([len(r["gate"]) for r in rows] + [len(COVERAGE_GATE), 18])
    say(f"  {'gate'.ljust(width)}  {'base':<7}  {'merge':<7}  verdict")
    say(f"  {'-' * width}  {'-' * 7}  {'-' * 7}  {'-' * 12}")
    for r in rows:
        say(f"  {r['gate'].ljust(width)}  {r['base']:<7}  {r['merge']:<7}  "
            f"{r['verdict']}")
    if cov_row is None:
        return
    say(f"  {cov_row['gate'].ljust(width)}  {cov_row['base']:<7}  "
        f"{cov_row['merge']:<7}  {cov_row['verdict']}")
    for line in cov_lines:
        say(f"  {' ' * width}      {line}")
    # The reason a gate could not be measured, printed HERE, where the row is. The
    # first version said "see above" and had thrown the child's output away.
    for line in cov_detail:
        say(f"  {' ' * width}      ! {line}")
    for line in cov_context:
        say(f"  {' ' * width}      info: {line}")


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__.splitlines()[0],
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Advisory. Nothing in .github/workflows/ runs this; it is a command "
               "you run before you press merge.")
    ap.add_argument("targets", nargs="+", metavar="PR|SHA",
                    help="PR numbers (resolved through `gh`) and/or commit-ishes")
    ap.add_argument("--base", default="origin/main", metavar="REF",
                    help="the ref the merge is computed against (default origin/main)")
    ap.add_argument("--json", action="store_true", help="emit the report as JSON")
    ap.add_argument("--keep-trees", action="store_true",
                    help="leave the exported trees on disk and print their paths")
    ap.add_argument("--repo", default=str(REPO), metavar="PATH",
                    help="repository to read (default: this file's repo)")
    ap.add_argument("--tmp", default=None, metavar="DIR",
                    help="where to export trees (default: a system temp dir)")
    ap.add_argument("--fetch", action="store_true",
                    help="refresh the BASE ref, and fetch a PR head that is not in the "
                         "local object store (`git fetch origin pull/N/head`). Off by "
                         "default: a tool that reads should not touch the network, or "
                         "the object store, unless asked -- but a stale base is the "
                         "exact failure this tool exists to catch, so its age is "
                         "always printed")
    args = ap.parse_args(argv)

    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass

    # --json means the caller is a program. The narrative goes away entirely rather
    # than being interleaved with the document, so `| jq` works without a --quiet.
    def say(*a, **k):
        # Flushed: a run is minutes long and its output is usually redirected to a
        # file or a PR body. A block buffer would show nothing at all until the end.
        if not args.json:
            print(*a, flush=True, **k)

    repo = pathlib.Path(args.repo)
    if git(repo, "rev-parse", "--git-dir").returncode != 0:
        print(f"{repo} is not a git repository", file=sys.stderr)
        return 2
    if not git_supports_write_tree(repo):
        print("this git has no `merge-tree --write-tree` (needs git >= 2.38).\n"
              "  Without it there is no way to compute a merge result without "
              "checking one out,\n  and checking one out is exactly what this tool "
              "exists to avoid.", file=sys.stderr)
        return 2

    # The base, and how old it is. The whole premise of this tool is "your base moved
    # and you did not notice", so answering the question against a base that moved
    # last week would be the tool committing its own subject matter. --fetch refreshes
    # it; without --fetch its age is printed and a stale one is named.
    if args.fetch and "/" in args.base:
        remote, _, branch = args.base.partition("/")
        git(repo, "fetch", "--quiet", remote, branch)
    base_out = git(repo, "rev-parse", args.base + "^{commit}")
    if base_out.returncode != 0:
        print(f"base ref {args.base!r} does not resolve. Try `git fetch origin`.",
              file=sys.stderr)
        return 2
    base_sha = base_out.stdout.strip()
    base_age_hours, base_age_note = base_age(repo, base_sha)

    # Resolve every target BEFORE exporting anything: a typo on the third argument
    # should not cost a 90-second base export first.
    targets = []
    warnings = []
    for spec in args.targets:
        if is_pr_number(spec):
            try:
                sha, label, info = resolve_pr(spec, repo=repo)
            except ValueError as exc:
                print(str(exc), file=sys.stderr)
                return 2
            warnings += pr_warnings(spec.strip().lstrip("#"), info, args.base)
            # `gh` knows the head sha; this clone may not have the object. Say so
            # precisely, with the command that fixes it, rather than letting
            # merge-tree fail with "not something we can merge".
            num = spec.strip().lstrip("#")
            if git(repo, "cat-file", "-e", sha + "^{commit}").returncode != 0:
                if not args.fetch:
                    print(f"PR #{num}'s head {sha[:12]} is not in this repository's "
                          f"object store.\n"
                          f"  Fetch it, or re-run with --fetch:\n"
                          f"    git -C {repo} fetch origin pull/{num}/head",
                          file=sys.stderr)
                    return 2
                f = git(repo, "fetch", "--quiet", "origin", f"pull/{num}/head")
                if f.returncode != 0 or git(
                        repo, "cat-file", "-e", sha + "^{commit}").returncode != 0:
                    print(f"could not fetch PR #{num}'s head: "
                          f"{(f.stderr or '').strip()}", file=sys.stderr)
                    return 2
        else:
            out = git(repo, "rev-parse", spec + "^{commit}")
            if out.returncode != 0:
                print(f"{spec!r} is neither a PR number nor a commit-ish that "
                      f"resolves here.", file=sys.stderr)
                return 2
            sha, label, info = out.stdout.strip(), spec, {}
        targets.append({"spec": spec, "label": label, "head": sha, "pr": info})

    # A run-unique subdirectory, even under an explicit --tmp. `scratch/"base"` was a
    # fixed name: two concurrent runs sharing one --tmp wrote into each other's export
    # and each rmtree'd the other's on the way out.
    run_id = f"{os.getpid()}-{int(time.time()) & 0xffffff:06x}"
    root = pathlib.Path(args.tmp) if args.tmp else pathlib.Path(
        tempfile.mkdtemp(prefix="premerge-"))
    root.mkdir(parents=True, exist_ok=True)
    if args.tmp:
        # A hard kill skips the `finally`, so a --tmp accumulates ~12,700-file exports
        # from runs nobody can find again. Not removed automatically -- another run may
        # be using one right now -- but named, which is the difference between a leak
        # and a note.
        stale = sorted(p for p in root.glob("premerge-run-*") if p.is_dir())
        if stale:
            say(f"note   {len(stale)} export directory(ies) already under {root} "
                f"(e.g. {stale[0].name}).")
            say(f"       Leftovers from --keep-trees or a killed run, unless another "
                f"run is live. Remove when idle.")
    scratch = root / f"premerge-run-{run_id}"
    scratch.mkdir(parents=True, exist_ok=True)
    made = []
    report = {"base": {"ref": args.base, "sha": base_sha,
                       "ageHours": base_age_hours}, "targets": [],
              "warnings": warnings}
    exit_code = 0

    def cleanup():
        if args.keep_trees:
            return
        for path in made:
            shutil.rmtree(path, ignore_errors=True)   # never git's own remover
            try:
                (scratch / (pathlib.Path(path).name + ".index")).unlink()
            except OSError:
                pass
        shutil.rmtree(scratch, ignore_errors=True)
        if not args.tmp:
            shutil.rmtree(root, ignore_errors=True)

    try:
        base_tree_sha = git(repo, "rev-parse", base_sha + "^{tree}").stdout.strip()
        say(f"base   {args.base} @ {base_sha[:12]}   {base_age_note}")
        if base_age_hours is not None and base_age_hours > BASE_STALE_HOURS:
            say(f"       This base is {base_age_hours:.0f}h old. It is the thing every "
                f"verdict below is measured against;")
            say(f"       re-run with --fetch, or `git -C {repo} fetch origin`, if main "
                f"has moved since.")
        for w in warnings:
            say(f"       WARNING: {w}")
        try:
            base_dir = export_tree(repo, base_tree_sha, scratch / f"base-{run_id}",
                                   scratch)
        except (RuntimeError, OSError) as exc:
            print(str(exc), file=sys.stderr)
            cleanup()
            return 2
        made.append(base_dir)

        # Once, and reused by every target. This is why several PRs in one run costs
        # far less than the same PRs one at a time.
        base_verdicts = run_all_gates(base_dir)
        base_metrics = coverage_metrics(base_dir)
        base_cov = base_metrics["data"]
        # base-vs-base: zero by construction. Run anyway -- it is the reference that
        # keeps a source_coverage.py already broken ON MAIN from being charged to a PR.
        base_cov_check = coverage_check(base_dir, repo, base_tree_sha, base_sha)
        report["base"]["gates"] = {k: {kk: v[kk] for kk in ("status", "rc", "summary")}
                                   for k, v in base_verdicts.items()}
        report["base"]["coverage"] = base_cov
        report["base"]["coverageMeasured"] = base_cov_check["status"] != "error"
        report["base"]["coverageStatus"] = base_cov_check["status"]
        already_red = [k for k, v in base_verdicts.items() if v["status"] != "pass"]
        if base_cov_check["status"] == "error":
            already_red = already_red + [COVERAGE_GATE]
            say(f"       source_coverage.py cannot run on the base itself:")
            for line in (base_cov_check["output"] or "").splitlines()[:10]:
                say(f"         | {line}")
        if already_red:
            say(f"       NOTE: {len(already_red)} gate(s) already red on the base "
                  f"({', '.join(already_red)}).")
            say(f"       Those are not any PR's to own and cannot fail this run.")
        say()

        conflicted = 0
        for t in targets:
            say(f"target {t['label']}  head {t['head'][:12]}")
            mt = subprocess.run(["git", "-C", str(repo), "merge-tree", "--write-tree",
                                 base_sha, t["head"]], capture_output=True, text=True)
            tree_sha, conflicts = parse_merge_tree(mt.returncode, mt.stdout)
            if conflicts is None:
                msg = (mt.stderr or "").strip() or f"git exited {mt.returncode}"
                say(f"  TOOL ERROR: git merge-tree could not answer: {msg}\n")
                t.update(outcome="error", detail=msg)
                report["targets"].append(t)
                exit_code = max(exit_code, 2)
                continue
            if conflicts:
                # Its own outcome, deliberately: there is no merge result to gate.
                say(f"  CONFLICT: this branch does not merge into "
                      f"{args.base} cleanly.")
                for line in conflicts[:12]:
                    say(f"      {line}")
                say("  No gates run -- there is no merge result to run them on. "
                      "Rebase first.\n")
                t.update(outcome="conflict", detail=conflicts)
                report["targets"].append(t)
                conflicted += 1
                continue

            try:
                d = export_tree(repo, tree_sha,
                                scratch / f"merge-{t['head'][:12]}-{run_id}", scratch)
            except (RuntimeError, OSError) as exc:
                say(f"  TOOL ERROR: {exc}\n")
                t.update(outcome="error", detail=str(exc))
                report["targets"].append(t)
                exit_code = max(exit_code, 2)
                continue
            made.append(d)

            merge_verdicts = run_all_gates(d)
            merge_metrics = coverage_metrics(d)
            merge_cov = merge_metrics["data"]
            merge_cov_check = coverage_check(d, repo, tree_sha, base_sha)
            rows = diff_verdicts(base_verdicts, merge_verdicts)
            cov_row = coverage_row(base_cov_check, merge_cov_check)
            cov_lines = coverage_lines(merge_cov_check["findings"])
            cov_ctx = coverage_context(base_cov, merge_cov, cov_lines)
            cov_measured = merge_cov_check["status"] != "error"
            # The reason, printed where the row is. Never discarded: a coverage gate
            # that could not run is the failure mode this whole row exists to catch.
            cov_detail = []
            if not cov_measured:
                cov_detail.append(merge_cov_check["summary"])
                cov_detail += [ln for ln in
                               (merge_cov_check["output"] or "").splitlines()[:10]
                               if ln.strip()]
            if merge_cov is None and merge_metrics["output"]:
                cov_ctx = list(cov_ctx) + [
                    "entries/modules not measured: " + merge_metrics["summary"]]

            say(f"  merge tree {tree_sha[:12]}")
            print_table(rows, cov_row, cov_lines, cov_ctx, cov_detail, say)

            all_rows = rows + [cov_row]
            regs = regressions(all_rows)
            for r in regs:
                v = (merge_cov_check if r["gate"] == COVERAGE_GATE
                     else merge_verdicts.get(r["gate"], {}))
                say(f"\n  {r['gate']}: green on {args.base}, {r['merge'].upper()} on "
                      f"the merge result.")
                body = (v.get("output") or "").splitlines()
                summary = v.get("summary", "")
                # summarise() often picks out the gate's own headline, which is
                # already the body's first line. Printed twice it reads like two
                # separate findings.
                if summary and (not body or summary != body[0].strip()):
                    say(f"      {summary}")
                for line in body[:25]:
                    say(f"      | {line}")

            # A pre-existing failure contributes nothing to the exit code, and that
            # reasoning stands. But `fail`/`fail` was printed as one word and NEITHER
            # side's summary was shown, so `backslide: 1 file` on the base and
            # `backslide: 500 files` on the merge result read identically. The exit
            # code does not change; what the reader is told does.
            for r in all_rows:
                if r["verdict"] != "pre-existing":
                    continue
                bv = (base_cov_check if r["gate"] == COVERAGE_GATE
                      else base_verdicts.get(r["gate"], {}))
                mv = (merge_cov_check if r["gate"] == COVERAGE_GATE
                      else merge_verdicts.get(r["gate"], {}))
                bs, ms = bv.get("summary", ""), mv.get("summary", "")
                if bs == ms:
                    continue
                say(f"\n  {r['gate']}: red on BOTH sides, but not in the same way. "
                    f"Not this PR's to own -- and worth a look.")
                say(f"      base:  {bs}")
                say(f"      merge: {ms}")

            t.update(outcome="regression" if regs else "clean",
                     mergeTree=tree_sha,
                     gates={k: {kk: v[kk] for kk in ("status", "rc", "summary")}
                            for k, v in merge_verdicts.items()},
                     coverage=merge_cov,
                     coverageMeasured=cov_measured,
                     coverageStatus=merge_cov_check["status"],
                     coverageFindings=merge_cov_check["findings"],
                     coverageRegressions=cov_lines,
                     coverageDetail=cov_detail,
                     coverageContext=cov_ctx,
                     rows=all_rows)
            report["targets"].append(t)

            if regs:
                exit_code = max(exit_code, 1)
                say(f"\n  RESULT: {len(regs)} gate(s) go green -> red on the merge "
                      f"result. This PR is answerable for them.\n")
            else:
                say(f"\n  RESULT: nothing goes green -> red. Safe to merge as far as "
                      f"the STATIC gates can tell -- no ROM was built.\n")

        if conflicted and exit_code == 0:
            exit_code = 3
    finally:
        if args.keep_trees:
            for path in made:
                print(f"kept: {path}", file=sys.stderr)
        cleanup()

    report["exit"] = exit_code
    if args.json:
        print(json.dumps(report, indent=2, sort_keys=True))
    return exit_code


if __name__ == "__main__":
    sys.exit(main())
