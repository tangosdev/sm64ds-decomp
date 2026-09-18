# Handoff: attribution-promotion-repair-0918

This document describes this commit. The coordinator records its immutable output SHA.

## Identity and resumption

- Related source PR: https://github.com/tangosdev/sm64ds-decomp/pull/2739.
- Task/stage: `attribution-promotion-repair-0918` / `repair` (producer).
- Session/harness: `codex-attribution-tools-0918`, Codex child `/root/repair2747`.
  This is the same actual session that previously produced Birds work as
  `codex-repair-2747-0918`; the alias does not make it an independent verifier.
- Branch/worktree: `fix/attribution-promotion-0918` at
  `C:/tmp/sm64ds-credit-tools-0918`.
- Input/base and installed tool source: `496c3bd14da75125926712534fb3f55619f36017`.
- Task workflow identity: `162d1796ff405da01b39d21ad753ab40aac71acb`.
- Status: producer candidate, pending independent verification;
  `formal_acceptance: false`. No remote publication or queue handoff performed.
- Next owner: coordinator assigns an independent tooling verifier, then integration.
- Local-only evidence is under this worktree's `build/`; scripts also remain in
  the primary checkout's `build/reviewer-0918/`. No global attribution data changed.

## What changed and why

Only the three reserved tools and this handoff changed.

`prepush_attribution.py` previously compared a surviving factory basename as a
file before reaching promotion checks. When several functions were consolidated
behind that filename, unrelated file-level credit could reject correct explicit
member ownership, or hide an incorrect factory member override. The repaired
route sends a surviving source that moves into a configured multi-function TU,
or expands its owned function set during consolidation, through the existing
strict module/address/size/member-override checks. It reuses one pair of function
snapshots and avoids those scans when no paths moved or disappeared. Ordinary
single-function moves retain their existing lineage behavior. Ambiguous addresses,
changed extents, missing member credit and wrong authors remain failures.

`chaos_db_ci.py` previously ignored `path#symbol` when writing function authors and
the contributor chart. It now resolves member override, then path override, then
finisher, then first matcher, and applies the canonical alias once to the chosen
identity. No path-wide workaround or attribution-file edit is included.

`test_attribution.py` adds actual Git-history and configuration fixtures for a
same-basename mixed-author promotion, in-place consolidation, missing/wrong member
overrides, changed extents, ambiguous addresses, ordinary moves and the no-move
scan guard. A generated-output regression invokes `chaos_db_ci.main()` and inspects
both serialized function records and the actual contributor chart. It covers
member/path/history priority, case-insensitive aliases, one-step alias handling,
and exclusion of draft functions from contributor credit.

## Reconstruction dimensions

- Source, class layouts, lifecycle/data ownership, function bytes and ROM enrollment:
  unchanged; this is a separate tooling repair.
- Contributor identities remain authoritative per function. The Water source
  proposal is a dependency/example, not an accepted source change in this commit.
- Full-ROM, relocation, header-consumer and port-move checks: not applicable to the
  changed surfaces and not run as acceptance tests. Wired setup's 2004/b56 compiler
  canary passed; that is environment evidence only.
- Remaining scope: independent tooling review; the coordinator must separately
  reserve and apply the four Water metadata corrections and verify that source PR.

## Proof

Commands ran in the tooling worktree unless noted. Logs are local evidence, not
private validation or independent Source review.

- Before the fix, the expanded `python -m unittest tools.test_attribution` ran
  50 tests with seven expected failures, exposing both positive and negative
  defects. Log: `build/attribution-tests-before.log`.
- After the fix, the same command exited 0: 50 tests passed in 39.887 seconds.
  Log: `build/attribution-tests-after.log`.
- `python -m unittest tools.test_bytegate tools.test_progress tools.test_validate_merge`
  exited 0: 193 tests passed in 73.719 seconds.
  Log: `build/attribution-related-tests.log`.
- `git diff --check` passed.
- Read-only real Water history regression used base
  `c67ec9fe1195e8594bae076451a2bbde670fa98e` and head
  `a726cdbceb5c0198e374ffbd70fc00c11ba654bd`. Running the repaired consumer against
  the committed metadata still exited 1, with exactly four changed owners, zero
  lost and three preserved consolidations. Substituting only the proposed four
  member overrides in memory exited 0, with zero changed/lost and all seven
  consolidations preserved. No path-wide override was added, and no metadata
  proposal was applied or committed. Script:
  `build/verify_attribution_fix_water.py`; summary:
  `build/water-fixed-tools-summary.json`; detailed reports/logs:
  `build/water-fixed-tools-actual_committed_metadata.{json,log}` and
  `build/water-fixed-tools-four_member_corrections_in_memory_only.{json,log}`.
- The task's immutable input and the Water example identities are separate.
  The in-memory Water exercise establishes tool behavior only; it does not
  establish source acceptance, current-main composition, or private CI results.
- Private validation / independent verifier: pending; no self acceptance.
