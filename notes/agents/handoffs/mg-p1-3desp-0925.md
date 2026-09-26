# Handoff: mg-p1-3desp-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3171,
  `mg-p1-3desp-0925`, stage `revise` (producer, rework round 2), session
  `claude-prod-mg-p1-3desp-0925-r2`, Claude Code. Round 1 was session `claude-prod-mg-p1-3desp-0925`.
- Source branch and previous accepted input SHA: `readable/mg-p1-3desp-0925`, input
  `68f14c35c56c96930e9eeb9bcc9c0abca93c1b19`, the round-1 candidate, which verification returned
  with finding V1 open.
- Original source base SHA and installed workflow/tool SHA: both `e63828444b7ad3fc20afb58de29857b618dddbcf`.
- Separate evidence commits and required artifacts in this commit: none; this note only.
- Next action, responsible role and blockers: independent verification, round 2 (byte,
  relocation, whole-object and source review of this exact commit). No blockers.
- Status: verified candidate (evidence below).
- Remaining uncommitted/local-only material and where it is preserved: none needed; the
  object comparison and each experiment variant are described below and can be repeated
  from the base and this commit.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `dScMg3DEsp_c` (scene 0x185, profile
  `MG_3DESP`), its eight single-function files in ov006 (`0x020e7660..0x020e777c` for D1
  and D0, `0x020e9c10..0x020ea1f0` for the six methods), and `include/dScMg3DEsp_c.h`.
- Reserved source/header/config surfaces actually touched: the eight `src` files, the
  header and this note. No config, symbols, delinks, attribution or baseline file changed.
- ROM observations: none new. The pass is pass 1: no names coined, no symbols renamed.
- Lineage evidence or structural inference: `unk_51e4` sits at `0x51e4 - 0x4fd8 = 0x20c`
  inside the 0x21c-byte block at `0x4fd8`, so it is that `dMg3DEspModel_c`'s `unk_20c`.
  The header comment says so; the field keeps its name.
- Hypothesized names/filenames, explicitly not recovered facts: none added.
- Compiler experiments and measured barriers: see the finding list, the theme results and
  the V1 experiments under Proof.

### Findings

- **V1, fixed.** Verification of `68f14c35c5` found the comment above `struct M48` in
  `InitResources` false: a `Matrix4x3` copy compiles to the same object. `InitResources` now
  declares `extern volatile Matrix4x3 data_ov006_0213c88c`, types its temporary `Matrix4x3`
  and assigns `mat4x3` by name. `M48` is gone. The comment claims only the two shapes
  re-measured on this form (R5).
- **R1, fixed.** `InitResources` reached every field through `char *c` plus an offset.
  It now uses the header names (`mCameraEye*`, `mCameraTarget*`, `mCameraAngle`,
  `pad_4660`, `mModel1`, `mModel2`, `pad_4fd8`, `mTextureTransformer`, `unk_0a4`,
  `unk_0a8`, `unk_0ac`, `unk_553c`, `mHudScore`). The alias is gone.
- **R2, fixed.** `InitResources` called slot 18 through a hand-rolled 19-slot `Obj` view,
  and its banner said no header declared that slot. It now calls `OnYoshiTryEat(-1)`.
  The view and the stale banner are deleted.
- **R3, fixed.** The mangled `TextureTransformer::Prepare` bridge is now a call through
  `TextureTransformer.h`.
- **R4, deferred.** `TextureTransformer::SetFile` keeps its scalar-speed mangled bridge.
  Through the header with a `Fix12<int>` by value, the function grows by 12 bytes
  (member assignment) or by 20 bytes plus a `.data` section (aggregate initializer).
  A one-line comment records this.
- **R5, fixed.** The two matrix copies target `Model::mat4x3` by name, as `Matrix4x3`
  copies (V1). The `volatile` extern and the stack copy stay, with one comment. Measured
  on this form, a non-volatile extern adds 32 bytes, with or without the casts, and
  copying both models straight from the extern removes 32 bytes. The `(Matrix4x3&)`
  casts strip the `volatile`: without them mwcc rejects the copy (`illegal operands`).
- **R6, fixed.** The `zero` temporary is gone; literal zeros match.
- **R7, fixed.** Ten local redeclarations in `InitResources` repeated `decl_common.h`
  exactly (`InitialiseVramGlobals`, `func_ov004_020b04d0`, `Deallocate`, `func_02056374`,
  `func_ov004_020b0cac`, three `data_ov006_*` handles, `data_ov004_020bc880` and
  `data_ov004_020bc884`). They are deleted. Each symbol keeps a plurality that agrees
  with its definition.
- **R8, deferred.** The `G2S::GetBG2CharPtr`, `GXS::LoadBGPltt`, `GXS::LoadOBJPltt` and
  `G3X::SetFog` calls stay mangled: no header declares those namespaces. A local
  namespace spelling of the first three was measured byte-identical and then reverted.
- **R9, deferred.** The register-named locals `r5v` and `r4v` in `InitResources` would
  need new names; that is pass 2 work.
- **R10, deferred.** `LoadFile` is defined returning `int`, so the `int` handles and the
  `(void*)f` casts agree with it. Typed handles would add declaration disagreements.
- **R11, fixed.** `OnYoshiTryEat` drops its `char *c` and `self` aliases, and its
  `int *p` increment is now `unk_0bc++`.
- **R12, fixed.** `OnYoshiTryEat` clears the singleton's score as
  `((dScMgBase_c*)data_ov004_020beb68)->mHudScore` instead of `+ 0xb4`.
- **R13, kept with a comment.** The clamp right after `unk_0bc = 0` looks dead, but
  removing it shortens the function by 16 bytes. The cap `0x270e` is written `9998`.
- **R14, fixed.** `Render` drops its `char *c` alias, a local `TextureTransformer`
  shadow struct and a hand-rolled `Obj` view. It now uses `TextureTransformer.h`, and
  `Model::Render(0)`, slot 5, is a real virtual call. Its local `Camera_UpdateMatrices`
  declaration now takes `void *`, as the one in `InitResources` does, which heals a
  banked `char *` disagreement.
- **R15, kept with a comment.** `Render` passes `*(ModelComponents *)(mModel1 + 8)`.
  Spelling it `((Model *)mModel1)->data` makes mwcc add the 8 in a separate `add`
  (same size, one pool word fewer), so the offset form stays.
- **R16, fixed.** `Behavior` calls `Advance()` through the header instead of the mangled
  `Animation::Advance` bridge. It drops its `idx` temporary and its unused
  `decl_common.h` include. Its banner claimed the receiver type sets the table's stride.
  Measured here, a complete and an incomplete receiver compile to the same object, and
  the banner now says so.
- **R17, fixed.** D1 and D0 destroy the members with qualified explicit destructor calls
  (`TextureTransformer::~TextureTransformer`, `~dMg3DEspModel_c`, `Model::~Model`)
  instead of three `extern "C"` mangled bridges. The header's bridge declarations were
  then unused and are deleted.
- **R18, fixed.** The header banner is rewritten short. It drops a citation of a deleted
  pre-migration file and the historical rename narrative, and it states the layout and
  why the members stay raw.
- **R19, fixed.** The `CleanupResources`, `Virtual50`, D1 and D0 comments are shortened
  to their facts.
- **R20, deferred.** Calls to the ov006 helpers cast `(char *)this`: `decl_common.h`
  declares them with `char *`, and their C definitions take `char *` (one takes
  `Outer *`). This is why the `(char *)this` count in `Render` rises from 1 to 4 while
  its alias goes away.
- **R21, deferred.** The four members stay raw storage. The factory constructs them in
  declaration order and the destructor tears them down in reverse, so they can become
  typed members once the out-of-scope factory changes with the destructor.

### Theme results

- Theme 1, inherited base fields by name: **proven here** in `dScMg3DEsp_c::InitResources`.
- Theme 2, dropping `char *c`: **proven here** in `InitResources`, `OnYoshiTryEat` and
  `Render`, each converted whole.
- Theme 3, mangled calls to real methods: **proven here** for `TextureTransformer::Prepare`
  (`InitResources`), `Animation::Advance` (`Behavior`) and the three qualified destructor
  calls (D1, D0). **Measured negative here** for `TextureTransformer::SetFile` with
  `Fix12<int>` by value (`InitResources`). SDK namespaces with no header are **not
  applied**; a local spelling was byte-identical in `InitResources`.
- Theme 5, pointer-to-member tables: complete receiver **proven here** in `Behavior`; an
  incomplete receiver is byte-identical there too.
- Theme 7, the scene singleton: **proven here** through `dScMgBase_c *` in `OnYoshiTryEat`.
- Theme 11, hand-rolled vtable views: **proven here** for `OnYoshiTryEat(-1)`
  (`InitResources`) and `Model::Render(0)` (`Render`).
- Theme 13, resource loading: **not applied**. `LoadFile`'s definition returns `int`, so
  the existing handles agree with it.
- Theme 14, Fix12: `Fix12<int>` by value at a call site is **measured negative here**
  (`InitResources`, `SetFile`).
- Also measured in `InitResources`: a `Matrix4x3` struct copy in place of the `M48`
  word array is **proven here** and applied (V1). A non-volatile matrix extern (+32
  bytes) and dropping the stack copy (-32 bytes) are each negative. In `Render`,
  `Model::data` by name is negative. In `OnYoshiTryEat`, the post-zero clamp is
  load-bearing.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: all eight functions, every byte and
  relocation. The comparison is object-level, not masked.
- Genuine methods; remaining free-function/ABI bridges: all eight are real methods.
  Remaining bridges are `SetFile` (R4) and the G2S, GXS and G3X calls (R8).
- Recovered layout/fields; remaining shadow structs/raw offsets: no local shadow struct
  remains; the matrix copies use `Matrix4x3` (V1). Raw offsets remain for `Model::data`
  (R15), the halfword at `0x5548` (inside `pad_5540`) and the SharedFilePtr file word (its
  header declares no fields).
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The destructor is
  still the key function; the D1 and D0 raw objects still emit the same vtable and RTTI.
- Attribution preserved through each move/rename: no moves or renames.
- Remaining agreed issue scope: R4, R8, R9, R10, R15, R20 and R21 for later passes.

## Proof

All results are from source commit `01eeb375f6` (this note's parent, identical sources),
on Windows with the pinned `2004/b56` compiler. The extracted ROM, the compiler and dsd
were copied into the worktree.

- **Baseline**, `python tools/rombuild.py -j8 --no-rom` with the nine files checked out
  from the base: exit 0. 0 reused from cache, 7069 compiled; 11,214 of 11,214 functions
  reproducing, 0 mismatching; 106 of 106 modules exact. The eight objects were saved as
  the baseline, and the same base files were raw-compiled with the production flags.
- **Full-ROM build**, `python tools/rombuild.py -j8 --no-cache`: exit 0, status passed.
  - `intactTuRom.identical` is true; the ROM sha256 is
    `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, the expected one.
  - `objectCache`: compiled 7069, reused 0, so the eight files were compiled from this
    tree. All eight are enrolled in the ov006 `delinks.txt` and listed in
    `build/objects.txt`.
  - 11,214 of 11,214 functions reproduce, 0 mismatch; module fidelity 106 of 106 exact;
    ROM-build analysis PASS.
- **Explicit relocation checks**, `python tools/prepush_linkcheck.py --range e63828444b..HEAD`:
  exit 0. The changed header fans out to 8 sources; 8 verified, 0 warnings, 0 blocking.
- **Complete emitted object checks**, pyelftools, base against candidate. Sections are
  compared by index and bytes, relocations by (section, offset, type, symbol, addend),
  defined symbols by (name, section, value, size, binding, type), and undefined symbols
  by name.
  - The eight production objects (post-isolation, from the two rombuild runs) are
    identical, and byte-identical files.
  - The eight whole raw compiles are identical, and byte-identical files. That covers
    D2, D1, D0, the vtable and RTTI in the destructor files (17 sections, 80
    relocations and 22 defined symbols each).
  - As a positive control, `mCameraAngle = 0xc01` in `InitResources` shows as a
    difference (+4 bytes).
- **V1 experiments.** Each is one edit to this commit's `InitResources`, raw-compiled with
  the production flags and compared to the base object:
  - the committed `Matrix4x3` form: identical;
  - `extern Matrix4x3` without `volatile`, casts kept: +32 bytes (0x380 to 0x3a0);
  - the same with the casts dropped: +32 bytes;
  - no `tmp`, both models copied straight from the extern: -32 bytes (0x380 to 0x360);
  - `volatile` kept with the casts dropped: mwcc rejects it with
    `illegal operands 'Matrix4x3' = 'volatile Matrix4x3'`.
- **Shared-header consumer expansion**: `python tools/affected_src.py include/dScMg3DEsp_c.h`
  exits 0 and lists exactly the eight files above, all covered.
  `python tools/check_header_offsets.py include/dScMg3DEsp_c.h`: exit 0, 9 commented
  fields, 0 mismatched, 0 unparsed, spans 0x5558. The header is unchanged in round 2.
- **Declaration gates**, no baseline edits:
  - `python tools/check_decl_agreement.py --changed e63828444b`: exit 0, no new local
    redeclarations, no new disagreements.
  - `python tools/check_decl_agreement.py`: exit 0, no new disagreements; 15,207
    disagreements, 35,881 declarations, 1,030 healed banked entries.
  - The same check with these nine files at the base: 15,213 disagreements, 35,896
    declarations, 1,024 healed banked entries. `data_ov006_0213c88c` has one
    declaration, this file's, so retyping it moves no count.
- **Other static gates**, each exit 0:
  - `python tools/prepush_attribution.py --base e63828444b --head HEAD`: 7241 tracked,
    0 changed, 0 lost.
  - `python tools/check_dead_references.py`: no new dead references, no broken links.
    It was run again with this note in the working tree, with the same result.
  - `python tools/tiers_ratchet.py --check`: CONVERTED ratchet PASS, baseline 3001,
    current 3219.
  - `python tools/check_tubuild_conflicts.py`: 302 entries, every conflict recorded.
  - `python tools/queue_audit.py --check-promoted`: 226 of 228 rows audited, all agree.
  - `python tools/check_src_tu_compiles.py`: 302 of 302 compile.
  - `python tools/port_refcheck.py`: 408 references checked, all resolve.
- **tubuild verify**: not applicable; this task has no TU.
- **Private validation**: not run.

### Residue, per file

Counted with `MSYS_NO_PATHCONV=1`: `wc -l`, then `grep -c -F` for each column, on
`git show` of the base and of this commit. The `(char *)this` column is the brief's
pattern. `InitResources` spells it `(char*)this`, once before (the alias) and once
after (one helper call). `char *` alias locals went from 3 to 0.

| File | lines | `func_` | `unk_` | `_ZN` | `extern "C"` | `goto` | `(char *)this` |
|---|---|---|---|---|---|---|---|
| `_ZN12dScMg3DEsp_c13InitResourcesEv.cpp` | 142 → 119 | 12 → 9 | 0 → 4 | 12 → 10 | 1 → 1 | 0 → 0 | 0 → 0 |
| `_ZN12dScMg3DEsp_c13OnYoshiTryEatEi.cpp` | 36 → 26 | 2 → 2 | 5 → 6 | 1 → 1 | 0 → 0 | 0 → 0 | 1 → 1 |
| `_ZN12dScMg3DEsp_c16CleanupResourcesEv.cpp` | 50 → 21 | 0 → 0 | 0 → 0 | 2 → 1 | 1 → 1 | 0 → 0 | 0 → 0 |
| `_ZN12dScMg3DEsp_c6RenderEv.cpp` | 37 → 32 | 4 → 4 | 0 → 0 | 1 → 1 | 1 → 1 | 0 → 0 | 1 → 4 |
| `_ZN12dScMg3DEsp_c8BehaviorEv.cpp` | 41 → 24 | 2 → 2 | 2 → 2 | 3 → 1 | 2 → 2 | 0 → 0 | 0 → 0 |
| `_ZN12dScMg3DEsp_c9Virtual50Ev.cpp` | 15 → 12 | 0 → 0 | 0 → 0 | 2 → 1 | 1 → 1 | 0 → 0 | 0 → 0 |
| `_ZN12dScMg3DEsp_cD0Ev.cpp` | 12 → 14 | 0 → 0 | 0 → 0 | 6 → 1 | 0 → 0 | 0 → 0 | 1 → 0 |
| `_ZN12dScMg3DEsp_cD1Ev.cpp` | 17 → 16 | 1 → 0 | 0 → 0 | 5 → 1 | 0 → 0 | 0 → 0 | 1 → 0 |
| `dScMg3DEsp_c.h` | 85 → 50 | 2 → 0 | 4 → 3 | 8 → 0 | 3 → 0 | 0 → 0 | 0 → 0 |

The `unk_` counts rise where raw offsets became existing `unk_` field names.

### Changed paths

`git diff --name-only e63828444b..HEAD`, with this note:

```
include/dScMg3DEsp_c.h
notes/agents/handoffs/mg-p1-3desp-0925.md
src/_ZN12dScMg3DEsp_c13InitResourcesEv.cpp
src/_ZN12dScMg3DEsp_c13OnYoshiTryEatEi.cpp
src/_ZN12dScMg3DEsp_c16CleanupResourcesEv.cpp
src/_ZN12dScMg3DEsp_c6RenderEv.cpp
src/_ZN12dScMg3DEsp_c8BehaviorEv.cpp
src/_ZN12dScMg3DEsp_c9Virtual50Ev.cpp
src/_ZN12dScMg3DEsp_cD0Ev.cpp
src/_ZN12dScMg3DEsp_cD1Ev.cpp
```
