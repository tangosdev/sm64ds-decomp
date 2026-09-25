# Handoff: readable-ov098-wbm-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3081, task `readable-ov098-wbm-0925`, stage `revise`, role producer, session `claude-prod-readable-ov098-wbm-0925`, Claude Code.
- Source branch and previous accepted input SHA: `readable/readable-ov098-wbm-0925`, input `7423f8d73f391870e7244a19599eba122f3596d5` (origin/main at enqueue).
- Original source base SHA and installed workflow/tool SHA: both `7423f8d73f391870e7244a19599eba122f3596d5`.
- Separate evidence commits and required artifacts in this commit: none. The compiler experiments are recorded below as source diffs with their measured results.
- Next action, responsible role and blockers: independent verification (byte, relocation, whole-object and source review) of this commit. No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov098/daWbm_c`, `src/actors/daWbm_c.cpp`, ov098 .text 0x0213b47c..0x0213bf60 (14 functions).
- Reserved source/header/config surfaces actually touched: `src/actors/daWbm_c.cpp`, `include/daWbm_c.h`, `config/arm9/overlays/ov098/symbols.txt` (two rows renamed in place), `config/tu_manifest.d/ov098/daWbm_c.json` (two `symbol` fields), and this class's two keys in the `attribution.json` ledger.
- ROM observations: none new. The class identity and layout evidence is unchanged from the promotion.
- Lineage evidence or structural inference: `func_ov098_0213b520` and `func_ov098_0213b63c` take the object in r0 and are only called with it, so they are written as members. Their bytes and relocations are unchanged.
- Hypothesized names/filenames, explicitly not recovered facts: `daWbm_c::Land` (0x0213b520, symbol `_ZN7daWbm_c4LandEv`) and `daWbm_c::Burst` (0x0213b63c, symbol `_ZN7daWbm_c5BurstEv`) are coined names. The source says so.
- Compiler experiments and measured barriers: see the table below. Each was run with `tools/tubuild.py verify ov098/daWbm_c` against this TU at the input commit and then reverted, except E4, which is adopted.

| ID | Finding | Change tried | Result |
|---|---|---|---|
| E1 | F4 int flag | In `func_ov098_0213b6e0`, `if (actorID == 0xbf)` and `if (actorID == 0xce)` in place of the `int is` flag | 13/14; `func_ov098_0213b6e0` 0x108 -> 0xf0 |
| E1b | F4 int flag | The same tests as `switch (other->actorID)` with `case 0xbf` and `case 0xce` | 13/14; `func_ov098_0213b6e0` 0x108 -> 0xf4 |
| E2 | F4 one-step arithmetic | `func_ov098_0213b9d8` folded: `a = (u16)player->mPrevAngleY >> 4; lead = (player->mHorzSpeed >> 12) * 0x1c + 0x64; v.x += lead * cos; v.y = playerY + 0x480000; v.z += lead * sin;` | 13/14; `func_ov098_0213b9d8` 0x144 -> 0x13c |
| E2b | F4 one-step arithmetic | Partly folded: `a = (u16)angle >> 4; lead = (speed >> 12) * 0x1c; ... lead += 0x64;`, rest unchanged | 13/14; `func_ov098_0213b9d8` 0x144 -> 0x13c |
| E3 | F4 include order | `#include "daWbm_c.h"` ahead of `#include "common.h"` | 13/14; `func_ov098_0213b584` 0xb8 -> 0xec, and an extra `_ZN9Matrix4x3D1Ev` is emitted |
| E3b | F4 include order | `#include "common.h"` removed | 13/14; `func_ov098_0213b584` does not match |
| E4 | F3 SetFile bridge | `mModel.SetFile(bmd, 1, 0x16)` in both InitResources calls; the `_ZN9ModelBase7SetFileEP8BMD_Fileii` prototype deleted | 14/14, undefined symbols identical. Adopted. |
| E5 | F3 Fix12 bridges | `Fix12<int> r = {...}, h = {...}; mdCcAc_c.Init(this, r, h, flags, 0);` for both dCcAc_c::Init calls | 13/14; `_ZN7daWbm_c13InitResourcesEv` 0x1fc -> 0x21c |

The three F4 shapes each stay, each with one short comment giving the measured size.
E5 confirms wall 6az for this TU. The other four Fix12 bridges have the same by-value Fix12 parameter shape and were not tried one by one.

## Deferred findings from #3081

| Finding | Outcome | Reason |
|---|---|---|
| F2 states and helpers are extern "C" free functions | Partially fixed | `Land` and `Burst` are members now. The three state bodies stay free functions: `src/__sinit_ov098_0213c2b4.c`, which is outside this reservation, builds the state table from their names. `func_ov098_0213b584` and `func_ov098_0213b6e0` are declared in `include/decl_common.h`, which is off limits. |
| F3 six mangled ABI bridges | Partially fixed | `ModelBase::SetFile` is now a member call (E4). `include/ModelBase.h` declares it returning int, so the old reason for the bridge no longer applied. The five Fix12-by-value bridges stay: E5 measured the wall. `dBgCh_Actr::Init` is also declared with `Fix12i` in `include/dBgCh_Actr.h`, which mangles to a different symbol. |
| F4 three shapes called compiler-forced without an experiment | Fixed as a finding (shapes kept) | E1, E1b, E2, E2b, E3 and E3b are the committed experiments. Each shape is measured as required and keeps a one-line comment. |
| F5 `unk_3b4` | Still deferred | InitResources and Land only store 0 to it. No function in the class reads it, so nothing shows what it means. |
| F6 address-named data | Still deferred | `data_ov098_0213c930` and `data_ov098_0213c91c` are also named by `src/__sinit_ov098_0213c2b4.c`, `include/decl_common.h` and the `Cannon` sources. `data_ov002_0210da38`, `data_02082214`, `data_020a0e68` and `func_0201267c` are not ov098 rows. No overlay symbols.txt gives coined names to data like `data_ov098_0213bf90`, so there is no convention to follow. |
| F7 `include/decl_common.h` | Still deferred | The file is off limits (held by `jump-contract-repair-0918`). The diff against the base is empty. |
| F9 Vector3 casts over inherited dActor_c scalars, `unk_0a4`, `unk_0ac` | Still deferred | `include/dActor_c.h` declares position, scale and speed as scalars. `unk_0a4`, `mVertSpeed` and `unk_0ac` look like a speed vector, but naming them is a shared-header change this task does not hold. |

F1 and F8 were fixed by the promotion task.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 14/14 functions, objisolate clean, relocation destinations clean, ROM order.
- Genuine methods; remaining free-function/ABI bridges: 8 member functions (D1, D0, InitResources, Behavior, Render, CleanupResources, Land, Burst) plus the factory. 5 C-linkage functions remain (see F2) and 5 mangled bridges (see F3).
- Recovered layout/fields; remaining shadow structs/raw offsets: unchanged. `unk_3b4` is still unnamed. The Vector3 casts over `mPosX` and `mScaleX` remain (F9).
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. Adding two non-virtual members changes neither layout nor vtable.
- Attribution preserved through each move/rename: the two `attribution.json` keys follow the renamed symbols with the same author. prepush_attribution reports 0 changed and 0 lost.
- Remaining agreed issue scope: F2 (five functions), F3 (five Fix12 bridges), F5, F6, F7 and F9 as in the table above.

Residue in `src/actors/daWbm_c.cpp`, counted as occurrences:

| | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
|---|---|---|---|---|---|
| Input commit | 466 | 4 | 41 | 9 | 20 |
| This commit | 461 | 4 | 29 | 7 | 19 |

## Proof

All commands were run in the worktree on the source commit this handoff sits on top of. This document is the only later change.

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov098/daWbm_c` | 0 | 14/14 MATCH, objisolate clean, reloc destinations clean, all 14 in ROM order. The input commit gives the same result. |
| Undefined symbols of `build/tu/ov098-daWbm_c/daWbm_c.o`, read with pyelftools, before and after | n/a | 74 and 74, identical set. The defined-function set differs only in the two renames. |
| `python tools/rombuild.py -j16` | 0 | 7189 enrolled, 7189 compiled and 0 reused. `build/src/actors/daWbm_c.o` was rebuilt and defines `_ZN7daWbm_c4LandEv` and `_ZN7daWbm_c5BurstEv`. 11,213 functions reproducing, 0 mismatching, 106/106 modules exact. `intactTuRom.identical` is true, sha256 `d1506e90...c478e8`. The build's own baseline control step reports 9 pre-existing `dsd check symbols` errors (arm9 main and ITCM) before any TU is substituted. |
| `python tools/prepush_linkcheck.py --range 7423f8d73f..HEAD` | 0 | 14 checked, 14 VERIFIED, 0 blocking |
| `python tools/prepush_attribution.py --base 7423f8d73f --head HEAD` | 0 | 0 changed, 0 lost |
| `python tools/check_decl_agreement.py --changed 7423f8d73f` | 0 | no new declaration disagreements |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/tiers_ratchet.py --check` | 0 | PASS, baseline 2994, current 3113 |
| `python tools/check_tubuild_conflicts.py` | 0 | 293 entries, every conflict recorded |
| `python tools/queue_audit.py --check-promoted` | 0 | every row agrees with config/tu_manifest.d |
| `python tools/check_src_tu_compiles.py` | 0 | 293/293 compile |
| `python tools/port_refcheck.py` | 0 | 408 checked, all resolve |
| `python tools/affected_src.py include/daWbm_c.h` | 0 | only `src/actors/daWbm_c.cpp` consumes it |
| `git diff 7423f8d73f..HEAD -- include/decl_common.h config/dead-reference-baseline.json config/decl-agreement-baseline.json` | 0 | empty |

- Full-ROM build: see the rombuild row.
- Explicit function/consumer relocation checks: tubuild verify and prepush_linkcheck, above.
- Complete emitted TU and data/metadata checks: this TU licenses .text only. The whole-object vtable, RTTI and romdata checks from the promotion were not rerun; the class's data and virtual members are unchanged.
- Shared-header consumer expansion: `include/daWbm_c.h` has one consumer, this TU.
- Port/path/reference and other applicable static gates: above.
- Private validation, if run, and the exact PR head/base it tested: not run.
