# Handoff: dbgch-bind1018

This document describes this commit. Source reconstruction and metadata wiring
are complete for the requested three-function scope. Full local and independent
verification were pending when this commit was written; subsequent exact-commit
results belong to the queue evidence, not a claim of approval in this document.

## Identity and scope

- Related collision-interface issue: https://github.com/tangosdev/sm64ds-decomp/issues/3854
- Task/stage: `dbgch-bind1018` / `reconstruct`; producer session
  `codex-dbgch-producer1018`, Codex. Branch: `cpp/dbgch-bind1018`.
- Source base and workflow: `3d76ff61c15e9e00d6d9ea317f4400aa1649764b`.
- Active independent source-review policy:
  `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Requested scope: reconstruct arm9 `0x020353b0..0x020353d4` as
  `dBgCh::SetActor(dActor_c*)`, and name sphere helpers
  `0x02037a6c..0x02037b1c` and `0x02037b1c..0x02037b5c`.

## Evidence and implementation

`SetActor` writes the existing `mOwner` and `mOwnerUniqueID` fields. The actor's
ID is `fBase_c::uniqueID` at offset 4. A null actor writes null and -1;
`func_02035354` compares the saved pointer and ID to exclude self-colliders.
Ground, line, and sphere setup now call the inherited member; the unused
declaration in the registry-query source is removed.

`ResetDisplacement` clears `disp` plus both accumulated push-out extrema.
It is narrower than `func_02037b5c`, which also resets hit flags, records, and
the floor normal. `ExpandPushOutBounds` merges supplied per-axis extrema.
The ITCM KCL scan computes depth-scaled surface normals, retains their extrema,
and passes those to this helper. The moving-collider variants rescale the local
query's extrema before merging them. `func_02037a38` sums min and max into the
displacement consumed by actor movement. These are push-out bounds, not
broad-phase spatial bounds; the caller comments now say so.

All three names are inferred from behavior, not recovered Nintendo identifiers.
The compiler's `mangle.py` output established their linker spellings.
`srcpath.new_path_for` selected the standalone SetActor source. The existing
`dBgCh` manifest owns a different text range; this change does not claim an
original TU boundary or broaden its range. The sphere manifest retains its
existing ranges and updates only the two member names. Layout, lifecycle,
vtable/RTTI, and unrelated ABI bridges are unchanged.

## Current proof and limits

- `linkcheck.py` on each of the three functions, with explicit arm9 address
  and size: all `VERIFIED`, no diffs, `blind: 0`; 276 bytes total. Local reports:
  `build/set-actor-link.json`, `build/reset-displacement-link.json`, and
  `build/expand-bounds-link.json`.
- A separate read-only agent reviewed the source diff and caller semantics;
  no source correction requested. This is not formal exact-commit acceptance.
- `affected_src.py include/dBgCh.h include/dBgCh_SphCrr.h --json` identifies
  195 final consumers, including the new member source.
- `port_refcheck.py`: 247 references checked, all resolve.
- `check_decl_agreement.py --changed` against the pinned base: no new local
  redeclarations or declaration disagreements. Existing disagreements remain.
- The whole-tree eligibility baseline was stopped before source edits because
  it recompiles consolidated TUs per function; no completed baseline result is
  claimed. Final production coverage and affected-consumer verification remain
  required.
- `check_rename_ledger.py`: all live mangled/vtable rows and lineage chains pass.
- `langmode_audit.py --check langmode-baseline.json`: ratchet passes.
- `tubuild.py verify arm9/dBgCh_SphCrr --no-write --verdict`: 27/27 bodies
  match; object isolation, relocation destinations, and order pass. It exits 1
  for eight unlicensed inherited RTTI outputs. The independent reviewer ran
  the same check at the exact source base and reproduced all eight, so this
  existing manifest limitation is not a regression or a new promotion claim.
- Full-ROM build, affected-file link checks, and the prepush link check are
  running on the final source. Their terminal results must be recorded in the
  producer/independent queue evidence before acceptance.

## Scope exception, compiler observation, and next action

The live task `issue2410-sound-playsub1015`, owned by
`codex-mky-finish-producer1015`, reserves `config/arm9/symbols.txt`,
`symbols/actor_renames.tsv`, `function-authors.json`, and `attribution.json`.
Its reservation and source work have not been changed. The current task
successfully reserved its nonconflicting source/header/delinks/manifest scope.

The user explicitly approved the narrow exception in this conversation on
2026-10-18. The applied patch changes only these three collision functions in
the four shared files: three configured symbols, three appended inferred-name
lineage rows, three author-record names, and two attribution keys. Every author
is preserved. No Sound source, reservation, or other metadata row was changed.

The first build exposed a pinned-compiler naming sensitivity: writing the
prototype as `SetActor(dActor_c *actor)` increments the compiler-local counter
and changes the existing Mky local destructor from
`_ZN7daMky_c21Bundle$981d_a_mky_cppD1Ev` to its `$982` spelling. The conventional
unnamed prototype `SetActor(dActor_c *)` preserves the existing `$981` spelling;
the definition still names and uses `actor`. A compiler-symbol probe measured
both forms, and the independent source reviewer accepted the unnamed prototype
as normal period C++. This avoids unrelated manifest churn. The failed first
build is not represented as a baseline failure; fresh full checks follow this
one-line correction.

Next owner: this task's producer. Wait for terminal full-ROM and caller proof,
inspect every failure/warning, then publish this immutable candidate to the
queue's independent `verify` stage. The separate `collision_review` agent uses
its own wired worktree. Final evidence must name the exact candidate/base and
remaining baseline limitations. No PR or merge is claimed. The wider collision
classes remain partially reconstructed; retained Fix12 and RecordHit ABI bridges
are outside this requested scope.
