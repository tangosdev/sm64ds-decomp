# Handoff: issue-4570-ov007-pathpoint-vector3-1106

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4570; `issue-4570-ov007-pathpoint-vector3-1106`; producer `reconstruct`; session `codex-ov007-pathpoint-vector3-1106`; Codex.
- Source branch and previous accepted input SHA: task branch; `3154d95553ffa4c331f8d99e38c104743363de55`.
- Original source base SHA and installed workflow/tool SHA: source base `3154d95553ffa4c331f8d99e38c104743363de55`; workflow/tool instructions pinned at `682321c64de078383e3c66cea08b0a3cd451a5ef`.
- Separate evidence commits and required artifacts in this commit: none; generated build reports are local and ignored.
- Next action, responsible role and blockers: an independent verifier should review the type inference and repeat the object, relocation and full ROM checks at this exact candidate. Source acceptance and PR checks are pending.
- Status: verified candidate for byte and relocation correctness; independent source review is pending.
- Remaining uncommitted/local-only material and where it is preserved: no source changes; ignored build outputs and private queue receipt remain under this worktree's `build/`.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `func_ov007_020c24d0`, ov007 `0x020c24d0..0x020c25fc` (one 0x12c-byte function).
- Reserved source/header/config surfaces actually touched: `src/unnamed/ov007/func_ov007_020c24d0.c`; `include/Ov007StrokePath.h` and `include/types.h` were inspected but not edited. The path header already includes `types.h`.
- ROM observations: the path struct's alternate point storage is allocated and copied in 12-byte units; this function removes one point by assigning the next 12-byte point over it.
- Lineage evidence or structural inference: the deleted local `Ov007PathPoint` was exactly three `int` members (`x`, `y`, `z`), and `find_shadow_types.py --twins` identified it as a POD twin of `Vector3`. The shared C-compatible `Vector3` has three signed 32-bit `Fix12i` members. This is a layout/type reconstruction; the original source identifier is not recovered.
- Hypothesized names/filenames, explicitly not recovered facts: none.
- Compiler experiments and measured barriers: replacing the local type with `Vector3` in the two casts yields a compiler object identical to the base object, including relocations.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: the changed function's complete object is identical; the explicit linker check verifies its ROM destinations.
- Genuine methods; remaining free-function/ABI bridges: unchanged; this is a C free function.
- Recovered layout/fields; remaining shadow structs/raw offsets: the per-function local point twin is removed. `Ov007StrokePath::pointStorage` remains `void *`; typing that shared field is outside this change.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged.
- Attribution preserved through each move/rename: no function ownership or source movement.
- Remaining agreed issue scope: other ov007 path views and callers are outside this function-scoped change.

## Proof

Tested against current main `3154d95553ffa4c331f8d99e38c104743363de55` with compiler `2004/b56`.

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom` — exit 0; 11,395/11,395 source-built functions reproduced, 106/106 modules exact, 470/470 source-owned data claims reproduced, 302 BSS claims, 0 data differences, and 0 symbol errors. The baseline control also passed 106/106.
- Explicit function/consumer relocation checks: `python tools/prepush_linkcheck.py --files src/unnamed/ov007/func_ov007_020c24d0.c` — exit 0; 1 checked, 1 verified, zero warnings and blockers.
- Complete emitted object check: `python tools/tu_object_diff.py --base HEAD --numbered src/unnamed/ov007/func_ov007_020c24d0.c` — exit 0, `IDENTICAL`, digest `8f7d7c8d9ce860df`; no numbered symbols.
- Shared-header consumer expansion: no header was edited; the changed TU already includes `types.h` through `Ov007StrokePath.h`.
- Port/path/reference and other applicable static gates: `python tools/check_decl_agreement.py --changed origin/main` — exit 0, no new disagreements; `python tools/port_refcheck.py` — exit 0, 217 references resolve; `git diff --check` — clean.
- Private validation, if run, and the exact PR head/base it tested: not run; PR not yet opened.
