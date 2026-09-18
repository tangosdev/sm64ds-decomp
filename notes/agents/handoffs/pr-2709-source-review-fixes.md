# Handoff: pr-2709-source-review-fixes

Continuation of the existing daTrsTrap_c deslop in
[PR #2709](https://github.com/tangosdev/sm64ds-decomp/pull/2709), not a new class
campaign. It records the finding dispositions for the blocking source review of
`e406d2aa0d5f6cccd7e801dc356fd228d2e33c93` and re-proves the bytes at the current
head.

## Identity

- Branch: `cleanup-datrstrap`; review branch `cr/pr2709-0917` in
  `C:/tmp/sm64ds-cr2709`.
- Reviewed candidate: `e406d2aa0d5f6cccd7e801dc356fd228d2e33c93`.
- Head this handoff describes: `1477adc72dd9d2b59c0b9cca5f84524430b37a38` plus
  this commit.
- Source base: `491c2eea764f01e498611270e565c814afb928b4`.
- Compiler: `2004/b56`, the tracked `tools/mwccarm` install.
- Owned span: ov063 `0x0211cdec..0x0211d270` (`d_a_trs_trap.cpp`, 5 functions),
  `0x0211d2a0..0x0211d3a0` (`d_a_trs_trap_classinits.cpp`, 4 factories), and the
  two single-function destructor shards at `0x0211c600` and `0x0211c638`.
  11 defined functions total. Unchanged by this commit.
- Status: the blocking finding is closed; whole-candidate source acceptance and
  current-main composition remain the reviewer's, not claimed here.

## Finding dispositions

| Finding | Disposition | Evidence |
| --- | --- | --- |
| TRAP-2709-01 (opaque `C` PMF receiver + the false representation claim) | `fixed` | Corrected by `395312cb4`; `notes/experiments/trap-2709-dispatch.md`. Re-proved below. |
| `dBgW_KcMbg::SetFile` mangled TU-local spelling | `compiler_constraint` | `notes/experiments/trap-2709-setfile.md` -- method form measured at size `0x388` vs the ROM's `0x370`. |
| Spawn offset table referenced by symbol, not a named local | `compiler_constraint` | `notes/experiments/trap-2709-offstable.md` -- named local measured, 7 words differ (table load hoisted to `+0x294`). |
| `v16` field-write order x, y, z | `compiler_constraint` | `notes/experiments/trap-2709-v16order.md` -- y/z swap measured, 2 words differ at `+0x2a0`/`+0x2a4`. |
| Synthesized ctor's vague-linkage `_ZN9Matrix4x3D1Ev` | `compiler_constraint` (packaging form) | `notes/experiments/trap-2709-matrixd1.md` -- no source-side alternative exists to measure; the durable artifact is the committed `compiler_only_output` deadstrip row in `config/tu_manifest.d/ov063/daTrsTrap_c_classInit.json`. The reviewer decides whether that substitution holds or the entry is re-labelled `deferred`. |
| `func_020393d4`/`func_020393c4` callback-word stores; `SharedFilePtr+4` re-read; `+0x418` / `+0x154` / S14 | `deferred`, `completion: partial` | `notes/experiments/trap-2709-notexperimentable.md`. Each item names the work that would unblock it. Durable thread: this PR, until a daTrsTrap_c class issue exists (sibling ov063 classes carry #2720 and #2721). Next owner: the humanizer integrator through @andrewboudreau, who is coordinating this PR's integration. |
| Behavior's dual `mStateTimer` spelling | `fixed` | `notes/experiments/trap-2709-statetimer.md` disproved the "one instruction short" claim; `1477adc72` adopted the unified spelling and deleted the claim. |

Every artifact above is committed on this branch: the experiment notes by
`57e7b31a59f2b03217c6408cb27a519e5c1f530c`, the manifest license row by the
branch's original manifest commit. None of them carry ROM, compiler, receipts or
credentials.

## What this commit changes

Nothing that can move a byte.

- `notes/experiments/trap-2709-statetimer.md` said the disproved dual spelling
  was "left in place" and that "the branch comment over-claims". `1477adc72`
  had already adopted the unified spelling, so the note contradicted the source
  it documents. It now records the variant as the shipped form.
- `d_a_trs_trap.cpp`'s InitResources comment cited the swapped `strh` at
  `+0x29c`/`+0x2a0`. The pinned experiment measured `+0x2a0`/`+0x2a4`. The
  comment now matches its own evidence; `trap-2709-v16order.md` drops the
  stale "off by one slot" aside.
- Each experiment note states its disposition in the vocabulary
  `notes/agents/SOURCE-REVIEW-CUTOVER.md` defines, and the deferred entry names
  its durable thread, its next owner and the work that unblocks each item.

## Re-proof at this head

Independent of the review's numbers.

- `python tools/rombuild.py -j16 --no-rom --no-cache`: all 8,754 enrolled source
  files compiled from source, no cache. 11,207 source-built functions
  reproducing, 0 mismatching; 26 source-owned data claims reproducing, 0
  mismatching; module fidelity 106/106 exact, 100.000000% of compared bytes;
  intact TU gates `dsd modules PASS`, zero new symbol errors, storage aliases
  exact; ROM-build analysis PASS. Re-run cached after the comment correction
  recompiles this file and reports the same verdicts.
- `python tools/linkcheck.py --name _ZN11daTrsTrap_c8BehaviorEv --module ov063
  --addr 0x0211ce74 --size 0x8c --c src/game/actors/daTrsTrap_c/d_a_trs_trap.cpp`:
  verdict `VERIFIED`, `diffs: []`, `blind: 0`. Driven the same way over all 11
  of the class's configured functions -- the five methods here, both destructor
  shards and the four factories -- every one returns `VERIFIED`, `diffs: []`,
  `blind: 0`.
- `python tools/check_dead_references.py`: no new dead references, no broken
  markdown links.
- No function added or removed. Compiling `e406d2aa0`'s pre-fix source (opaque
  `struct C` receiver, dual `mStateTimer`) and this head's source with the
  build's exact flags yields the byte-identical object,
  sha256 `240001deaa87109e2bdc68ad52c44e805ad1391f7f0ae8c60037981aa20bd8c3`,
  3,648 bytes -- same bodies, same relocation records. That object defines
  exactly the five configured methods, `0x44 + 0x4 + 0x40 + 0x8c + 0x370 =
  0x484`, which is exactly the delinks range `0x0211cdec..0x0211d270`. Nothing
  extra is emitted and nothing is missing.

`dsd check symbols --fail` reports 9 pre-existing errors in the baseline control
run with no TU substitution, so they belong to the tree and not to this
candidate.
