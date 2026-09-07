# tools/ archive log

Generated 2026-08-14 by an automated triage pass (6 parallel haiku agents,
one per ~30-file slice of `tools/`'s top level, each checking for a matching
`test_*.py`, inbound references across the repo, CI-workflow/pre-push usage,
and git history shape before proposing anything). Default bias was KEEP;
archiving required zero references anywhere plus a one-off/scratch shape.
Subdirectories (`dataset/`, `ghidra/`, `hooks/`, `mwccarm/`, `permuter/`,
`probes/`, `trace/`, `bin/`) were out of scope and untouched.

Nothing was deleted. Archiving is a plain filesystem move; git still has full
history for every file under its original path.

## Archived (13 of 178 reviewed)

All 13 are one-off sweep/batch/crack scripts from closed matching campaigns,
each with zero references anywhere else in the repo and no corresponding test:

| File | Reason |
|---|---|
| `_overnight_batch16.py` | One-off batch-16 sweep script, underscore-prefixed scratch naming, zero references. |
| `_sweep4_batch16.py` | Same batch-16 sweep family, zero references. |
| `_sweep5_batch16.py` | Same batch-16 sweep family, zero references. |
| `_sweep6_batch16.py` | Same batch-16 sweep family, zero references. |
| `_sweep7_batch16.py` | Same batch-16 sweep family, zero references. |
| `_sweep8_bc3dc.py` | One-off sweep keyed to a specific target hash (`bc3dc`), zero references. |
| `_sweep9_bf36c.py` | One-off sweep keyed to a specific target hash (`bf36c`), zero references. |
| `_sweep_bc3dc_asm.py` | Companion ASM sweep for the same closed campaign, zero references. |
| `_sweep_ov062.py` | One-off sweep for overlay 062, zero references. |
| `_sweep_ov062b.py` | Variant of the ov062 sweep, zero references. |
| `crack_pr104.js` | One-off Fable crack script for PR #104's `func_ov006_02115b0c`; that function matched and landed in commit `5fa17d52f`; never re-run since, no other tool references it. |
| `gen_sinit_disasm.py` | Orphaned `__sinit` disasm-to-C generator; one commit, no callers anywhere (workflows, docs, notes, or other tools). |

## Kept (165 of 178)

Everything else was either directly referenced elsewhere (imported by another
tool, invoked from a GitHub workflow or `tools/hooks/pre-push`), backed by a
maintained `test_*.py`, or is part of an active pipeline (rombuild, RTTI
extraction, matching/marker/evidence tooling, etc.). Full per-file reasoning
from each review agent is in the task transcript; nothing here was judged
dead with the same confidence as the 13 above.

## Rejected: test-file archive proposal (not executed, and shouldn't be)

One review agent (scoped to `tools/test_match_attempts.py`..`tools/worklist.py`)
proposed archiving 7 test files as testing "modules that no longer exist":
`test_attribution.py`, `test_fdiff_version.py`, `test_layout.py`,
`test_pr_linkcheck_renames.py`, `test_pr_linkcheck_verdict.py`,
`test_reloc_audit_modules.py`, `test_reloc_audit_sections.py`.

This is wrong for at least 3 of the 7: `test_attribution.py`,
`test_fdiff_version.py`, and `test_layout.py` were outside that agent's
assigned range in the first place (they belong to the adjacent slice), and
the agent that *was* assigned them independently confirmed their target
modules (`chaos_db_ci.py`, `fdiff.py`, `layout_check.py`) are present and
load-bearing. `pr_linkcheck.py` and `reloc_audit.py` (targets of the other 4)
also both still exist and are themselves marked KEEP elsewhere in this pass.
The move was never executed (the agent hit a permission failure), so no
action is needed — all 7 remain exactly where they were.

## Side effect worth knowing about

`config/tu_manifest.json` has an uncommitted diff (promoting `LevelObjects`
from `text-verified` to `link-verified`, plus a new PoleLift `linkcheck`
entry that failed to link). This looks like the byproduct of a review agent
actually *running* `tools/tubuild.py linkcheck` while checking whether
`tubuild.py` was still in active use, rather than just reading it. The result
looks like legitimate data, not corruption, but it wasn't something this
cleanup task asked for — left in place for you to review/keep or revert with
`git checkout -- config/tu_manifest.json`.

## Incident: unreliable execution layer, not unreliable judgment

Two agents (notes group D, tools group 6) correctly *identified* archive
candidates but their move commands silently failed ("permission constraints"
on the Bash/PowerShell tool) — nothing was moved for those. A third agent's
move command partially malfunctioned and renamed a *different* file to a
garbage name instead of moving its intended target (see
`notes/archive/ARCHIVE_LOG.md`). None of the 13 files listed above have this
problem — each was independently confirmed via `git status` and a byte-size/
content check against the pre-existing file before being trusted.
