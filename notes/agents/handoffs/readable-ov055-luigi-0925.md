# Handoff: readable-ov055-luigi-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3072, `readable-ov055-luigi-0925`, stage `revise`, role producer, session `claude-prod-readable-ov055-luigi-0925`, Claude Code.
- Source branch and previous accepted input SHA: `readable/readable-ov055-luigi-0925`; input `7423f8d73f391870e7244a19599eba122f3596d5` (origin/main at enqueue).
- Original source base SHA and installed workflow/tool SHA: both `7423f8d73f391870e7244a19599eba122f3596d5`.
- Separate evidence commits and required artifacts in this commit: none. The source commit and this handoff are the whole candidate.
- Next action, responsible role and blockers: independent verification (byte, relocation, whole-object and source review) of this commit. No blockers.
- Status: verified candidate, from the proof below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov055/daLuigi_c`, `.text` 0x021111a0..0x02111860, 11 functions, in `src/actors/daLuigi_c.cpp` and `include/daLuigi_c.h`.
- Reserved source/header/config surfaces actually touched (`git diff --name-only 7423f8d73f..HEAD`):
  - `attribution.json` (this class's three renamed keys only)
  - `config/arm9/overlays/ov055/symbols.txt` (three rows renamed in place)
  - `config/tu_manifest.d/ov055/daLuigi_c.json` (three function symbols and the coined-names note)
  - `include/daLuigi_c.h`
  - `notes/agents/handoffs/readable-ov055-luigi-0925.md`
  - `src/actors/daLuigi_c.cpp`

  `config/arm9/overlays/ov055/delinks.txt` holds only address ranges and needed no edit. `include/decl_common.h` is untouched.
- ROM observations:
  - ov055 `relocs.txt` reaches the state members by address: 0x02111a94 loads 0x021112bc, 0x02111a9c loads 0x02111288, and InitResources calls 0x021112c4 from 0x021117f8. Renaming the three symbol rows therefore moves no linked byte.
  - The body model that `func_ov002_020e496c` returns is read at +0x14 and +0x1c. Those are `Model::data.transforms` and `Model::mat4x3`. The bone copy reads `transforms[15]` (+0x2d0), and the loop bound is `BMD_File::numBones` (+0x4).
  - The player byte at +0x6fb is `Player::mIsVanish` in `include/Player.h`.
  - 0x02111264 has one candidate caller: an `arm_call` at ov063 0x02118004. Its relocation is recorded as `module:overlays(27,55)`, so the call's target module is ambiguous.
- Lineage evidence or structural inference: the parameter names `radius` and `depth` in the DropShadowRadHeight bridge follow the real declaration in `include/dActor_c.h`. The bridge used to call them `height, radius`, which was the wrong way round.
- Hypothesized names/filenames, explicitly not recovered facts: `SetState`, `EnterMirror`, `ExecuteMirror`, `daLuigiState` and `daLuigiStateFn` are coined names. The header and the manifest mark them as coined.
- Compiler experiments and measured barriers (each run with `python tools/tubuild.py verify ov055/daLuigi_c`):
  - Real `mModelAnim.SetAnim(file, 0, sp, 0)` with a `Fix12<int>` local: InitResources DIFF (10/11), with 10 relocation destinations wrong. The three mangled Fix12 bridges stay.
  - State members of `daLuigi_c`, and a table of `int (daLuigi_c::*)(Player *)`: MATCH. The coined `daLuigiStateHost` was not needed.
  - ExecuteMirror with direct member reads, in store order and in x, z, y order: DIFF both times. The one base pointer stays, with one comment.
  - Render, `mModelAnim.data.modelFile` and `.transforms` read directly: DIFF. The same reads through a `ModelComponents *` local: MATCH. The `(int)` launder is gone.
  - Render's bone loop as `*dst = *src`, `*dst++ = *src++`, `dst[i] = src[i]` and `*(Mtx *)(int)dst = *src`: each DIFF. The 64-bit store launder stays.
  - `Matrix4x3` assignment instead of the `Mtx` word copy: DIFF in Render (bone 15 copy) and in InitResources. `Mtx` stays.
  - `Vector3 t` instead of `int t[3]` in InitResources: the text matched, but the object gained an extra `_ZN7Vector3D1Ev`. Reverted.
  - SetState and Behavior read `mState` directly, and Behavior tests `data_ov055_02111b68` directly: MATCH. The `s`, `state` and `f` locals are gone.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 11 of 11 functions match, and every relocation destination is clean. The emitted object was compared before and after (pyelftools, `build/tu/ov055-daLuigi_c/daLuigi_c.o`). The 72 undefined symbols are identical. The defined globals are identical once the three renamed state symbols are mapped. All 21 PROGBITS and NOBITS sections are identical in bytes and relocations (offset, type and target, with the renames mapped).
- Genuine methods; remaining free-function/ABI bridges:
  - 10 of 11 functions are now `daLuigi_c` members.
  - `func_ov055_02111264` stays an `extern "C"` free function. Its only candidate caller relocates ambiguously, so its owner is unknown.
  - The remaining bridges are the three Fix12 by-value mangled calls, the unnamed arm9 helpers (`func_02016acc`, `func_02016b24`, `func_0201277c`, `func_0203c178`) and the ov002 helpers `func_ov002_020e496c` and `func_ov002_020e4374`.
- Recovered layout/fields; remaining shadow structs/raw offsets:
  - Render is now typed through `Player`, `Model`, `ModelComponents`, `BMD_File` and `Matrix4x3::t`.
  - Still raw: `FileOf` (the SharedFilePtr word at +4), the `Mtx` word copy, and Behavior's store to `data_0209f318` + 0x114 (the camera, inside `Camera::pad_114`).
  - `pad_0d0` stays, because nothing in the TU touches it.
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. The inline empty destructor still gives D1 and D0 with no D2, and InitResources is still the key function.
- Attribution preserved through each move/rename: the three `attribution.json` keys follow the symbol renames with the same credit. `prepush_attribution` reports 0 changed and 0 lost.
- Remaining agreed issue scope: see the table below. Every deferred row has the owner `coordinator claude-promo-coord-0923`, to reassign under the named reservation, and the issue https://github.com/tangosdev/sm64ds-decomp/issues/3072.

### Deferred findings from #3072

| Finding | Outcome | Reason |
|---|---|---|
| R1 raw player and model-record offsets, pointer launder in Render | partially fixed | Now typed: the player (`Player *`, `mIsVanish`), the body model (`Model *`, `data.transforms`, `mat4x3`), `mat4x3.t.x`, `transforms[15]` and `BMD_File::numBones`. The `(int)` launder on `&mModelAnim.data` is gone. The 64-bit store launder stays: four plainer loop forms each DIFF. |
| R2 three mangled Fix12 by-value calls | still deferred | The real `ModelAnim::SetAnim` call measured DIFF in InitResources, with relocation destinations wrong (wall 6az). |
| R3 PMF bridge through the coined daLuigiStateHost | fixed | The states are daLuigi_c members, and the table holds `int (daLuigi_c::*)(Player *)`. The three ov055 symbol rows were renamed in place. The object is byte and relocation identical. |
| R4 FileOf SharedFilePtr accessor | still deferred | `include/SharedFilePtr.h` leaves the handle's layout unrecovered. It is a shared header that this task does not hold. |
| R5 local Mtx copy struct | still deferred | `Matrix4x3` assignment was re-measured and DIFFs in both InitResources and Render. The comment now names both. |
| R6 unnamed globals, raw +0x114 store | still deferred | `data_ov055_02111a90`, `data_ov055_02111b60`, `data_ov055_02111b64`, `data_ov055_02111b68`, `data_ov055_02111b6c` and `data_ov055_02111b70` are also declared in `include/decl_common.h` (off limits), `src/func_ov002_020e3e00.cpp` and `src/__sinit_ov055_021118d4.c`, and this task holds neither file. A comment records their observed roles, but that is prose only. +0x114 falls inside `Camera::pad_114` in `include/Camera.h`, which is not held. |
| R7 pad_0d0 | still deferred | Nothing in the TU reads or writes 0x0d0, so there is no evidence for a name. |
| R8 two declaration-baseline keys against decl_common placeholders | still deferred | `include/decl_common.h` is held by `jump-contract-repair-0918`. Its line `extern int func_ov055_021112c4(void*, void*, int);` now names a symbol that nothing defines. The banked key `func_ov055_021112c4` no longer fires (`--symbol` reports 0 disagreements), and it was left in place. |
| R9 factory's mangled constructor calls and raw vtable store | still deferred | They are in `src/d_a_luigi.cpp`, which is outside this reservation. |
| R10 stale alias annotation in class-build-worklist.tsv | still deferred | `notes/data/class-build-worklist.tsv` line 133 still reads `[alias: header uses pre-rename name]`. That file is outside this reservation. |

Residue, base `7423f8d73f` to this commit. Counts are the lines that contain the string (`git show <rev>:<path> | grep -c -F`, with `MSYS_NO_PATHCONV=1`), and the line totals are `wc -l`:

| Measure | `src/actors/daLuigi_c.cpp` base | this commit | `include/daLuigi_c.h` base | this commit |
|---|---|---|---|---|
| lines | 231 | 232 | 74 | 82 |
| `unk_` | 0 | 0 | 0 | 0 |
| `func_` | 25 | 16 | 0 | 0 |
| `extern "C"` | 5 | 2 | 0 | 0 |
| `_ZN`, all | 12 | 15 | 0 | 0 |
| `_ZN` outside marker lines | 7 | 7 | 0 | 0 |

The `_ZN` total rises because the three state markers now carry mangled names.

## Proof

All commands were run in `C:/tmp/claude-rd-ov055` on the source commit this handoff sits on. That commit's tree matches this one's except for this file. The inputs were the pinned `tools/mwccarm` 2004/b56 compiler and the `extracted/` ROM copied from the main checkout.

- Full-ROM build: `python tools/rombuild.py -j16` exit 0.
  - 7189 enrolled source files, all 7189 compiled with 0 reused from cache. `build/src/actors/daLuigi_c.o` was rebuilt, and it defines the three renamed state symbols.
  - 11,213 functions reproduce and 0 mismatch. 106/106 modules are exact.
  - `build/rombuild-report.json` has `intactTuRom.identical: true`, sha256 d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8.
  - ROM data reports 3 differing symbols (`_ZTV10dCcAcPos_c`, `_ZTIN8daDemo_c10anmModel_cE`, `_ZTIN8daDemo_c13simpleModel_cE`). None of them belongs to ov055 or this class.
  - The baseline control's `dsd check symbols` failure has the tree's pre-existing ITCM errors.
- Explicit function/consumer relocation checks:
  - `python tools/tubuild.py verify ov055/daLuigi_c`: 11/11 MATCH, objisolate clean, reloc-destinations clean, TEXT-VERIFIED. The baseline on the untouched tree was also 11/11, and it was re-run after every edit.
  - `python tools/prepush_linkcheck.py --range 7423f8d73f..HEAD` exit 0: 12 checked, 12 VERIFIED (the TU and `daLuigi_c_classInit`), 0 warnings, 0 blocking.
- Complete emitted TU and data/metadata checks: the before-and-after object comparison described under Reconstruction dimensions. The 72 undefined symbols are identical, and the 21 sections are identical once the renames are mapped.
- Shared-header consumer expansion: `python tools/affected_src.py include/daLuigi_c.h` lists `src/actors/daLuigi_c.cpp` and `src/d_a_luigi.cpp`. Both compiled in rombuild, and both are VERIFIED by linkcheck. `python tools/check_header_offsets.py include/daLuigi_c.h`: 6 commented fields, 0 mismatched.
- Port/path/reference and other applicable static gates:
  - `python tools/prepush_attribution.py --base 7423f8d73f --head HEAD` exit 0: 0 changed, 0 lost.
  - `python tools/check_decl_agreement.py --changed 7423f8d73f` exit 0: no new declaration disagreements.
  - `python tools/check_dead_references.py` exit 0: no new dead references, no broken markdown links.
  - `python tools/tiers_ratchet.py --check` exit 0: PASS, baseline 2994, current 3117. `--update` was not run.
  - `python tools/check_tubuild_conflicts.py` exit 0: 293 entries, every conflict recorded.
  - `python tools/queue_audit.py --check-promoted` exit 0.
  - `python tools/check_src_tu_compiles.py` exit 0: 293/293.
  - `python tools/port_refcheck.py` exit 0: 408 checked, 0 stale.
  - `git diff 7423f8d73f..HEAD -- include/decl_common.h` is empty.
- Private validation, if run, and the exact PR head/base it tested: not run.
