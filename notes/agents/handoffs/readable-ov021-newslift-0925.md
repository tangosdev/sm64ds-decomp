# Handoff: readable-ov021-newslift-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3073,
  `readable-ov021-newslift-0925`, stage `revise`, session `claude-prod-readable-ov021-newslift-0925`,
  Claude Code (Opus 5.5).
- Source branch and previous accepted input SHA: `readable/readable-ov021-newslift-0925`, input
  `7423f8d73f391870e7244a19599eba122f3596d5`.
- Original source base SHA and installed workflow/tool SHA: both `7423f8d73f391870e7244a19599eba122f3596d5`.
- Separate evidence commits and required artifacts in this commit: none. Every experiment below was
  scratch-applied and reverted; its command and measured size are recorded here.
- Next action, responsible role and blockers: independent verification (verifier) of this commit:
  byte, relocation, whole-object and source review. There are no blockers.
- Status: verified candidate. Every local gate listed below exits 0.
- Remaining uncommitted or local-only material and where it is preserved: none.

## What changed and why

- Class, TU, symbol and module-qualified ROM scope: `ov021/daObjCvNewsLift_c`, ov021 `.text`
  `[0x021111a0, 0x021121e8)`, 19 functions.
- Reserved surfaces actually touched: `src/actors/daObjCvNewsLift_c.cpp`, `include/daObjCvNewsLift_c.h`,
  `config/arm9/overlays/ov021/symbols.txt` (12 rows renamed in place),
  `config/tu_manifest.d/ov021/daObjCvNewsLift_c.json` (12 symbol fields plus two evidence notes) and
  this class's 12 override keys in `attribution.json` (re-keyed in place, credit unchanged).
  The ov021 `delinks.txt` needs no change: its row for this TU is an address range with no symbol names.
- ROM observations: InitResources loads the five callback addresses (0x02112128, 0x02111fe4,
  0x02111f8c, 0x02111f34, 0x02111edc) as literal-pool words (the `kind:load` rows in
  `config/arm9/overlays/ov021/relocs.txt`), and every other call to the renamed functions is an
  `arm_call` inside this TU. Those rows are keyed by address, so the rename leaves them intact.
  Each helper takes the object in r0, so member form is the same ABI.
  UpdateModelTransforms ends `ldrh; cmp #0x2d; pophs; ands #1; pop`: the blink test Render makes,
  with nothing after it.
- Lineage or structural inference: the four platform setters differ only in the index they store,
  and the callback that calls each one is registered on the collider of the same index.
- Hypothesized names, explicitly not recovered facts: `UpdateClsnTransforms`,
  `UpdateModelTransforms`, `OnMainMeshRide`, `OnPlatform0Ride` to `OnPlatform3Ride`,
  `MainMeshCallback` and `Platform0Callback` to `Platform3Callback` are all coined, and the header
  says so. `UpdateModelTransforms` follows the tree's `daObjCasket_c::UpdateModelTransform`,
  pluralised because it writes five model matrices.
- Compiler experiments and measured barriers (pinned mwccarm 2004/b56,
  `python tools/match.py --c src/actors/daObjCvNewsLift_c.cpp --func <symbol> --addr <addr> --size <size> --version 2004/b56 --module ov021 --strict-relocs --brief`;
  the committed form is MATCH in each case):
  - `mMeshCollider.SetFile(file, mClsnMat, scale, mAngleY, clps)` and the matching platform-loop
    call, with a `Fix12<int> scale` whose `val` is 0x199: InitResources is 0x26c bytes against the
    ROM's 0x258, and tubuild reports 12 wrong reloc destinations. This agrees with
    `notes/experiments/batch2-2685-dossunbar-setfile.md`.
  - `mPlatformMats[i] = data_020a0e68;` in UpdateClsnTransforms: 8 words differ (the two pointer
    increments swap registers). `*platformMat = data_020a0e68;`: 0x194 bytes against 0x198.
  - The committed `platform->mPlatformMats[0] = ...` with `platform` stepped one Matrix4x3 at a
    time: MATCH. It replaces the raw `+0x460` store and `+0x30` step.
  - UpdateModelTransforms tail: `if (mResetTimer < 45 && (mResetTimer & 1)) {}` and the nested-if
    form both drop the test (0x18c against 0x1a8). The two separate early returns, and a single
    `if (mResetTimer < 45 && (mResetTimer & 1)) return;`, both MATCH. The single test is committed.
  - Behavior: plain reads of `data_02092768` in the reset give 0x594 against 0x5a4; dropping the
    `(u16)(s16)` cast on `mWobblePhase` gives 0x59c. Both are kept, and the source comment gives
    the sizes.

## Findings from #3073

All findings below are reconstruction findings. Owner of each deferred one: the coordinator
`claude-promo-coord-0923`, tracked on https://github.com/tangosdev/sm64ds-decomp/issues/3073.

| ID | Finding | Outcome | Owner | Issue | Reason |
|---|---|---|---|---|---|
| R1 | mangled `dBgW_KcMbg::SetFile` bridge; unnamed `func_020393c4` and `func_020393d4` | still deferred, now measured | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3073 | The member call measures 0x26c against 0x258 (above), and the source comment now says so. `func_020393c4` and `func_020393d4` are arm9 symbols defined by `src/func_020393c4.c` and `src/func_020393d4.c`, outside this reservation. |
| R2 | raw `+0x460` and `+0x30` walk in UpdateClsnTransforms | partially fixed | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3073 | The store now names `mPlatformMats` and the cursor steps one Matrix4x3, with no raw offsets. A this-based cursor remains because `mPlatformMats[i]` (8 words) and `*platformMat` (0x194) both miss; the comment records both. |
| R3 | dead early returns in UpdateModelTransforms | partially fixed | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3073 | The two returns and the `unsigned` local are now one `if (mResetTimer < 45 && (mResetTimer & 1)) return;`, Render's own test. An empty-bodied `if` drops the test (0x18c), so the trailing return stays. |
| R4 | twelve unnamed `func_ov021_*` helpers | fixed | - | https://github.com/tangosdev/sm64ds-decomp/issues/3073 | All twelve are now daObjCvNewsLift_c members: two non-virtual transform updates, five ride setters and five static collision callbacks. The symbols.txt rows, manifest symbols and attribution keys are renamed in place. The build is byte- and relocation-identical, and the undefined-symbol set is identical. |
| R5 | `include/decl_common.h` `int` placeholders | still deferred | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3073 | The file is held by `jump-contract-repair-0918`. Since this commit, its lines for `func_ov021_02112128`, `func_ov021_02111fe4`, `func_ov021_02111f8c`, `func_ov021_02111f34`, `func_ov021_02111edc`, `func_ov021_0211129c` and `func_ov021_02111434` name no defined symbol. Nothing uses them, so the build is unaffected; delete them when the file is free. `data_ov021_02114a20` is unchanged. |
| R6 | `NewsLiftFile` shadow of SharedFilePtr, `(Vector3 *)&mPosX` casts, volatile levers | partially fixed | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3073 | The shadow's `unk_00` and `file` are now `fileID`, `numRefs` and `filePtr`, the layout of `_ZN13SharedFilePtr4LoadEv`'s definition. The shadow itself stays because `include/SharedFilePtr.h` declares no fields and is a shared header. `include/dActor_c.h` has only position scalars, so the casts stay. The volatile reads and the double cast were re-measured (above). |
| R7 | file-scope statics initialised by `__sinit_ov021_02113500` | still deferred | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3073 | That `.init` function (0x188 bytes) is its own shard, `src/__sinit_ov021_02113500.c`, outside this reservation. It also references the `data_ov021_*` objects by name, which blocks renaming them here. |
| R8 | header does not classify `mHomePos` (0xc40) | fixed | - | https://github.com/tangosdev/sm64ds-decomp/issues/3073 | The header comment now says the names from 0xc40 on are inferred. |
| C1 | 41 decl-baseline rows and one attribution key under retired shard paths | still deferred | claude-promo-coord-0923 | https://github.com/tangosdev/sm64ds-decomp/issues/3073 | `config/decl-agreement-baseline.json` is outside this reservation, and this task must not edit it. The rows are inert. |

## Reconstruction dimensions

- Exact function, byte and relocation coverage: 19 of 19 functions, full TU range.
- Genuine methods, and the remaining free-function and ABI bridges: 18 members (D1, D0, the four
  virtuals, seven non-virtual members and five static members) plus the C-linkage factory
  `daObjCvNewsLift_c_classInit`. Bridges: the mangled `dBgW_KcMbg::SetFile` (R1) and
  `func_020393c4` and `func_020393d4` (R1).
- Recovered layout and fields, and the remaining shadow structs and raw offsets: no `unk_` names
  remain. `NewsLiftFile` shadows SharedFilePtr (R6), position arguments cast over base scalars
  (R6), and the platform-matrix store uses a this-based cursor (R2).
- Lifecycle, vtable, RTTI, initializer and data ownership: unchanged from promotion. The file
  objects and tables stay `data_ov021_*` rows owned by `__sinit_ov021_02113500` (R7).
- Attribution preserved through each rename: the 12 overrides were re-keyed to the new symbols
  with the same credit.
- Residue in `src/actors/daObjCvNewsLift_c.cpp` (matching lines, base to this commit): lines 528 to
  521, `unk_` 1 to 0, `func_` 50 to 9, `extern "C"` 14 to 2, `_ZN` 7 to 20. The 12 added `_ZN`
  lines are the new marker lines, and one is the comment citing `_ZN13SharedFilePtr4LoadEv`.
  The non-marker `_ZN` lines go from 3 to 4. The 9 remaining `func_` lines are the
  `func_020393c4` and `func_020393d4` declarations and calls (R1). The header adds one `func_`
  line, a comment naming `func_020393c4`.

## Proof

All commands were run in this worktree against this commit's source tree, with base `7423f8d73f`.

- `python tools/tubuild.py verify ov021/daObjCvNewsLift_c`: exit 0. 19/19 MATCH, objisolate clean,
  reloc-destinations clean, emitted in ROM order (TEXT-VERIFIED). The baseline before any edit was
  also 19/19.
- `python tools/rombuild.py -j16`: exit 0. `enrolledFiles` 7189, 0 reused and 7189 compiled,
  including `build/src/actors/daObjCvNewsLift_c.o`. 11,213 reproducing, 0 mismatching, 106/106
  modules exact. `intactTuRom.identical` is true, sha256 d1506e90...c478e8. The baseline control's
  `dsd check symbols` errors (overlay_100, overlay_102, two data and five ITCM symbols) are
  pre-existing, and the intact gate reports zero new symbol errors.
- `python tools/prepush_linkcheck.py --range 7423f8d73f..HEAD`: exit 0. 19 checked, 19 VERIFIED, 0 blocking.
- `python tools/prepush_attribution.py --base 7423f8d73f --head HEAD`: exit 0. 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed 7423f8d73f`: exit 0, no new disagreements.
- `python tools/check_dead_references.py`: exit 0, no new dead references, no broken links.
- `python tools/tiers_ratchet.py --check`: exit 0. PASS, baseline 2994, current 3125. `--update` was not run.
- `python tools/check_tubuild_conflicts.py`: exit 0, 293 manifest entries.
- `python tools/queue_audit.py --check-promoted`: exit 0.
- `python tools/check_src_tu_compiles.py`: exit 0, 293/293.
- `python tools/port_refcheck.py`: exit 0, 408 checked, 0 stale.
- Undefined symbols of the TU object (pyelftools on `build/tu/ov021-daObjCvNewsLift_c/daObjCvNewsLift_c.o`,
  base copy against this commit): 91 before and 91 after, character-identical. The defined set
  differs only by the 12 renamed rows.
- `include/decl_common.h`, `config/dead-reference-baseline.json` and
  `config/decl-agreement-baseline.json`: empty diff against the base.
- Private validation: not run. The coordinator owns the PR.
