# Handoff: promote-ov055-luigi-0923

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3072,
  task `promote-ov055-luigi-0923`, stage `revise` (producer), session
  `claude-prod-promote-ov055-luigi-0923`, harness Claude Code.
- Source branch and previous accepted input SHA: `promote/promote-ov055-luigi-0923`;
  input `eb8f46d46a94a2a05cbcff9fd3d3ab228f2afe20` (origin/main at enqueue).
- Original source base SHA and installed workflow/tool SHA: both `eb8f46d46a`.
- Separate evidence commits and required artifacts in this commit: two commits on the
  branch. The first renames the class (byte-neutral). The second promotes the TU and
  carries this handoff. No other evidence commit.
- Next action, responsible role and blockers: independent verification (byte,
  relocation, whole-object and source review) of this exact commit. No blockers.
- Status: verified candidate (local proof below).
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daLuigi_c`, ov055 `.text`
  `0x021111a0..0x02111860`, 11 functions: D1, D0, `func_ov055_02111264`,
  `func_ov055_02111288`, `func_ov055_021112bc`, `func_ov055_021112c4`,
  CleanupResources, OnPendingDestroy, Render, Behavior, InitResources.
- Reserved source/header/config surfaces actually touched:
  - `include/daLuigi_c.h`, renamed from the MirrorLuigi header.
  - `src/actors/daLuigi_c.cpp` (new), `src/d_a_luigi.cpp` (rename only).
  - ov055 `delinks.txt` and `symbols.txt`, and `config/tu_manifest.d/ov055/daLuigi_c.json`.
  - Ledgers: `attribution.json`, `config/converted-baseline.json`,
    `config/decl-agreement-baseline.json`, `symbols/actor_renames.tsv`,
    `notes/data/tu-promotion-queue.tsv` and `notes/data/class-build-worklist.tsv`.
    The integrator reconciles these.
- ROM observations: `build/rtti.json` record `ov055:0x02111ab0` is `9daLuigi_c`, an
  si record with a zero-offset `dActor_c` base. Its name string is at `0x02111aa4` and
  its vtable at `0x02111ae0`, the address the tree called `_ZTV11MirrorLuigi`.
  The state table at `0x02111a94` holds two words, which relocate to `0x021112bc` and
  `0x02111288`. `__sinit_ov055_021118d4` copies it to `0x02111b70`, and InitResources
  passes that copy to `func_ov055_021112c4`. ov063 calls `func_ov055_02111264`.
- Lineage evidence or structural inference: the state setter, the two-callback table and
  the mirror callback (it negates X and the Y angle) are inferred from the code.
- Hypothesized names/filenames, explicitly not recovered facts:
  - `daLuigiState`, `daLuigiStateFn` and `daLuigiStateHost` are coined type names. The
    header says so.
  - No mangled spelling was coined. The four helpers keep their `func_ov055_` linker
    names and C linkage.
  - The file name `daLuigi_c.cpp` is the path resolver's choice, not a recovered name.
- Compiler experiments and measured barriers (each run under `tubuild verify`):
  - Inline empty destructor in the header: D1 then D0 in ROM order, no D2. 11/11 on
    the first compile.
  - `Matrix4x3` assignment in InitResources: DIFF (a different copy sequence).
    Reverted to the twelve-word `Mtx` copy.
  - Mirror callback with direct `player->mPosX` reads: DIFF. It matches when it reads
    through `int *pos = &player->mPosX`.
  - `mModel.Render(0)`, `mModelAnim.Model::Render(0)`, `TextureSequence::Update`,
    `Model::LoadFile`, `Animation::LoadFile`, `ModelBase::SetFile`,
    `ShadowModel::InitCylinder`, `TextureSequence::Prepare`, `ApproachLinear` by
    reference, and the typed pointer-to-member state table: all MATCH.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 11/11 MATCH. objisolate is clean, the
  relocation destinations are clean, and emission is in ROM order. All 72 undefined
  symbols in the TU object have a `config/**/symbols.txt` row.
- Genuine methods; remaining free-function/ABI bridges:
  - The five virtuals are real methods.
  - Real member calls: Model, Animation and TextureSequence file loading, SetFile,
    InitCylinder, Prepare, Update and both Render calls.
  - Three calls stay mangled because they pass `Fix12<int>` by value (wall 6az):
    `ModelAnim::SetAnim`, `TextureSequence::SetFile` and `dActor_c::DropShadowRadHeight`.
  - Four helpers stay C-linkage free functions with their original addresses as
    names: the state setter, the two table callbacks and the ov063 trigger.
- Recovered layout/fields; remaining shadow structs/raw offsets:
  - Header fields are unchanged and `check_header_offsets` passes.
  - `mState` is now a typed `daLuigiState *`.
  - The mirror callback uses `dActor_c` position and angle fields.
  - Still raw in Render: the player model record from `func_ov002_020e496c`, the
    player's `+0x6fb` frame byte, and the ModelComponents walk. Render keeps its
    measured 64-bit store-pointer round trip (lever 6m).
  - `FileOf` reads the loaded file one word into a SharedFilePtr, because that
    layout is not recovered.
- Lifecycle, vtable/RTTI, initializer and data ownership:
  - The inline destructor makes InitResources the key function.
  - `_ZTV9daLuigi_c` and the fBase_c, dBase_c, dActor_c and daLuigi_c RTTI records
    are licensed as deadstrip-data at their canonical homes.
  - `romdata_check` on `src/actors/daLuigi_c.cpp`: 5 VERIFIED (the 124-byte vtable
    and all four `_ZTI` records), 4 PARTIAL (the `_ZTS` strings,
    because the ROM pads them), 0 DIFFERS.
  - The static initializer and the factory stay outside this TU, where tu_map places
    them.
- Attribution preserved through each move/rename: `prepush_attribution` reports 11
  consolidated with credit intact, 0 changed and 0 lost. One `tu_promote` override was
  corrected by hand: OnPendingDestroy goes to tangosdev, which is main's credit.
- Remaining agreed issue scope:
  - Type the player record and the frame byte that Render reads.
  - Recover the SharedFilePtr layout.
  - Decide whether the table callbacks become members of a recovered class.

## Proof

All checks ran in `C:/tmp/promote-ov055-luigi-0923` on this branch, base `eb8f46d46a`.

- Full-ROM build: `python tools/rombuild.py -j16` exited 0.
  - 7595 enrolled sources; `build/src/actors/daLuigi_c.o` was freshly compiled.
  - 11,210 reproducing, 0 mismatching, 106/106 modules exact.
  - Intact TU gates: zero new symbol errors.
  - ROM sha256 `d1506e90…c478e8`, identical to stock.
  - The rename commit alone: `rombuild -j16 --no-rom` exited 0, with 11,210
    reproducing, 0 mismatching and 106/106 exact.
- Explicit function/consumer relocation checks: `python tools/prepush_linkcheck.py
  --range eb8f46d46a..HEAD` exited 0. 12 checked, 12 VERIFIED (11 TU members plus the
  factory, the header's other consumer).
- Complete emitted TU and data/metadata checks:
  - `python tools/tubuild.py verify ov055/daLuigi_c` exited 0: 11/11 MATCH,
    TEXT-VERIFIED, 0 unlicensed.
  - `romdata_check`: results as above.
  - `tubuild linkcheck` was not run on the candidate. The production proof is the
    default rombuild compiling the promoted file.
- Shared-header consumer expansion: `include/daLuigi_c.h` has two consumers, the TU
  and `src/d_a_luigi.cpp`. Both are VERIFIED above. `check_header_offsets` reports 6
  fields, 0 mismatched and 0 unparsed.
- Port/path/reference and other applicable static gates:
  - `prepush_attribution --base eb8f46d46a --head HEAD`: exit 0, 0 changed, 0 lost.
  - `check_decl_agreement --changed eb8f46d46a`: exit 0, no new disagreements. The
    full-tree run also reports none new.
    - The baseline was edited in place, not regenerated.
    - 22 entries keyed to retired shard paths were removed.
    - Six were re-keyed to the TU: the five ov002 SharedFilePtr handles and
      `func_0203c178` arity, whose ROM callers pass four arguments against a
      `void (void)` definition.
    - Two new keys:
      - `data_ov055_02111b70` is now typed `daLuigiState`, against decl_common's
        `void *`.
      - The `func_ov055_021112c4` definition takes `char *` where decl_common declares
        `int`. decl_common is a shared header outside this reservation.
  - `check_dead_references`: exit 0, none new.
  - `tiers_ratchet --check`: exit 0, PASS. Baseline 2993, current 3029. The unbanked
    gains are left for the integrator.
  - `check_tubuild_conflicts`: exit 0. All four create conflicts are marked RESOLVED.
  - `queue_audit --check-promoted`: exit 0 after flipping the daLuigi_c row to
    `already_promoted=yes`.
  - `check_src_tu_compiles`: exit 0, 269/269.
  - `port_refcheck`: exit 0, 408 references resolve.
  - `check_rename_ledger`: exit 0.
- Private validation, if run, and the exact PR head/base it tested: not run. No PR was
  opened.
