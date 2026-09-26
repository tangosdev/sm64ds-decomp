# Handoff: mg-p1-factories-0926

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3171, task `mg-p1-factories-0926`, stage `revise`, role producer, session `claude-prod-mg-p1-factories-0926`, Claude Code.
- Source branch and previous accepted input SHA: `readable/mg-p1-factories-0926`, input `4e48c28b97ef78d3b9314dd082f6325ecec6057a` (origin main at claim).
- Original source base SHA and installed workflow and tool SHA: both `4e48c28b97ef78d3b9314dd082f6325ecec6057a`.
- Separate evidence commits and required artifacts in this commit: none. The source and theme change is `de8581365e4dc9d963b2e75ec6cb6386f798b12f`; this handoff is the only change after it.
- Next action, responsible role and blockers: independent verification of the six applied factories and the seven measurements. No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted or local-only material: the candidate sources, the two scratch header copies, the base and after objects and the build logs lived in the producer worktree's ignored build folder and are not preserved. Each is reproducible from the text below.

## What changed and why

- Scope: theme 4 of `notes/minigames/themes.md` over 13 ov006 factories. Six now read `return new dScMgX_c;`; seven keep the hand-built form.
- Surfaces touched: `src/d_s_mg_bomroom.c`, `src/d_s_mg_curling.c`, `src/d_s_mg_curling2.c`, `src/d_s_mg_pachinko.c`, `src/d_s_mg_panel.c`, `src/d_s_mg_teresa.c`, `notes/minigames/themes.md` and this handoff. No header, config, attribution or declaration baseline change.
- Form: `//cpp` on the first line, the class header, then `extern "C" void *dScMgX_c_classInit() { return new dScMgX_c; }`. Every path is kept, so the `.c` files stay `.c`.
- Comments: each `// @symbol` line and "Reconstructed source-style name" comment is kept; Bomroom gains the `// @symbol` line it lacked. The "STILL MACHINE-SHAPED" banner is dropped: both of its items (the unnamed `data_ov006_*` label and the mangled calls) are gone. The `recovered:` and `vtable identified: VT0 = data_ov006_*` tags described that label and are dropped with it.
- Return type: `int *` becomes `void *`. `check_decl_agreement.py --symbol` finds no other declaration of any of the six.
- ROM observation: each `_ZTV<C>` shares its address with the old `data_ov006_*` label in ov006 `symbols.txt`.
- Names: none coined.

## Per-file results

Each `new` form was compiled with `tools/match.py`'s pinned `2004/b56` flags (C++ adds `-Cpp_exceptions off`) and compared word by word with the base build's object, whose words outside relocations are the ROM's. Sizes are the `new` form against the ROM. The first differing word is given as ROM against `new` form. Where it is a literal load at `+0x04`, only the pool offset differs, because the body length changed; the last column names what the `new` form leaves out.

| File | Result | Size | First differing word | What differs |
|---|---|---|---|---|
| `src/d_s_mg3_d_esp.cpp` | kept | 0x50 against 0x90 | `+0x04` `ldr r0, [pc, #0x64]` against `#0x34` | stops after the `_ZTV12dScMg3DEsp_c` store: no `Model` constructors at 0x4f38 and 0x4f88, none for `dMg3DEspModel_c` at 0x4fd8 or `TextureTransformer` at 0x51f4 |
| `src/d_s_mg_bomroom.c` | applied | 0x30, identical | none | linked vptr word `0x0213bbb4`, as before |
| `src/d_s_mg_coin.cpp` | kept | 0x34 against 0x48 | `+0x04` `ldr r0, [pc, #0x30]` against `#0x20` | stops after the vptr store, without `func_0203b9b4(this + 0x51c4, 1)` |
| `src/d_s_mg_curling.c` | applied | 0x34, identical | none | linked vptr word `0x0213c304`, as before |
| `src/d_s_mg_curling2.c` | applied | 0x34, identical | none | linked vptr word `0x0213c510`, as before |
| `src/d_s_mg_flower.cpp` | kept | 0x50 against 0x94 | `+0x04` `sub sp, sp, #8` against `ldr r0, [pc, #0x34]` | the ROM reserves stack for `__cxa_vec_ctor`'s fifth argument; neither that call nor `func_ov006_020c3f54` is emitted |
| `src/d_s_mg_pachinko.c` | applied | 0x34, identical | none | linked vptr word `0x0213d9cc`, as before |
| `src/d_s_mg_panel.c` | applied | 0x34, identical | none | linked vptr word `0x0213e24c`, as before |
| `src/d_s_mg_slot3.c` | kept | 0x50 against 0x60 | `+0x04` `ldr r0, [pc, #0x40]` against `#0x34` | no `func_ov006_020c221c` at 0x4f38 |
| `src/d_s_mg_snowball.cpp` | kept | 0x60 against 0x34 | `+0x00` `stmdb sp!, {lr}` against `push {r4, lr}` | `new` inlines a constructor (base C2, `SysTracker`, `Model`) where the ROM calls `func_ov006_021295ac` |
| `src/d_s_mg_teresa.c` | applied | 0x34, identical | none | linked vptr word `0x0213fa0c`, as before |
| `src/minigames/d_s_mg_roulette.cpp` | kept | 0x4c against 0xac | `+0x00` `push {r4, r5, lr}` against `push {r4, lr}` | no table, racer-array or `Model` constructors |
| `src/minigames/d_s_mg_slot1.cpp` | kept | 0x58 against 0x88 | `+0x04` `ldr r0, [pc, #0x5c]` against `#0x38` | the base C2 and the three vptr stores come out in the ROM's order; the calls `func_ov006_0210c2b0` (0x4684, 0x4690) and `func_ov006_0210c208` (0x469c) do not |

What each kept header would need (headers are reserved elsewhere; none was edited):

- Coin: a constructor that makes the call. In a scratch copy of `include/dScMgCoin_c.h`, `dScMgCoin_c() { func_0203b9b4((int *)pad_51c4, 1); }` plus an `extern "C"` declaration of `func_0203b9b4` gives MATCHING at 0x48. This is the inline-constructor shape `daFish_c` uses; a member type at 0x51c4 whose default constructor makes the call was not measured.
- 3DEsp: typed members for the four objects above, each with its out-of-line constructor. The header already says the destructor must change with them.
- Flower: `mArray` as 0x16 elements of a 0x20-byte type (constructor `func_ov006_0212b88c`, destructor `func_ov006_0212a650`) and a typed object at 0x51f8 (`func_ov006_020c3f54`). The header records why `mArray` stays bytes: a typed element adds a second teardown pass.
- Slot3: a typed member at 0x4f38 constructed by `func_ov006_020c221c`.
- Roulette: typed `mTable` (`func_ov006_020c1d80`), `mArray` (five elements of 0x34, constructor `func_ov006_0210a4ac`, destructor `func_ov006_021079c8`) and two `Model` members. The header records that typed members destroy in a different order from the ROM's destructor.
- Slot1: typed members at 0x4684 and 0x4690 (`func_ov006_0210c2b0`) and at 0x469c (`func_ov006_0210c208`).
- Snowball: an out-of-line `dScMgSnowball_c();`. In a scratch copy of the header, `match.py --no-strict-relocs` gives MATCHING at 0x34. The link then needs 0x021295ac named `_ZN15dScMgSnowball_cC1Ev` in ov006 `symbols.txt` and defined under that name by `src/func_ov006_021295ac.cpp`, both outside this reservation.
- The Single3D children's `new` objects also carry a weak `_ZN19dScMgSingle3DBase_cD2Ev` (0x38), and Slot1's carries the inline `betIcon_c` and `dThIcon_c` destructors. A standalone factory there would need objisolate to drop them. Memory2 avoids this by living in its class's TU.

## Relocations, before and after

`src/d_s_mg_bomroom.c`, `dScMgBomroom_c_classInit` at ov006 0x020d9574. "Raw" is the compiler's object; "isolated" is the copy rombuild links.

| Offset | Type | Before | After, raw | After, isolated | Resolves to |
|---|---|---|---|---|---|
| +0x08 | `R_ARM_PC24` | `_ZN7fBase_cnwEj` - 8 | same | same | 0x02043444 |
| +0x14 | `R_ARM_PC24` | `_ZN11dScMgBase_cC2Ev` - 8 | same | same | 0x020b2adc |
| +0x2c | `R_ARM_ABS32` | `data_ov006_0213bbb4` + 0 | `_ZTV14dScMgBomroom_c` + 8 | `_ZTV14dScMgBomroom_c` + 0 | 0x0213bbb4 |

The other five have the same three relocations, with the literal at `+0x30`, and the same result: the two calls unchanged, and the vptr literal moving from the `data_` label + 0 to `_ZTV<C>` + 0 at the same address (`_ZTV14dScMgCurling_c` 0x0213c304, `_ZTV15dScMgCurling2_c` 0x0213c510, `_ZTV15dScMgPachinko_c` 0x0213d9cc, `_ZTV12dScMgPanel_c` 0x0213e24c, `_ZTV13dScMgTeresa_c` 0x0213fa0c). Offsets, types and resolved destinations are unchanged in all six, and the six `.text` sections are byte-identical to the base build's. The seven kept objects are byte-identical to the base build's.

## Proof

All commands ran in the producer worktree with the pinned `2004/b56` compiler. The gate runs marked "at base" were repeated with the six source edits and the theme edit stashed.

| Command | Exit | Result |
|---|---|---|
| `git fetch origin` | 0 | origin main `4e48c28b97` |
| `git worktree add -f C:/tmp/claude-mgp-factories -b readable/mg-p1-factories-0926 4e48c28b97...` | 0 | worktree at the base |
| `python tools/classqueue.py v2 claim mg-p1-factories-0926 --stage revise --role producer ...` | 0 | lease running on input `4e48c28b97` |
| `python tools/rombuild.py -j8 --no-cache` at base | 0 | `intactTuRom.identical` true, ROM sha256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`; 11,214 of 11,214 source functions reproducing, 0 mismatching; 106 of 106 modules exact |
| `python tools/check_decl_agreement.py --symbol dScMgX_c_classInit`, 13 runs | 0 | 0 disagreements; only the defining file declares each |
| `python tools/match.py --c <candidate> --func dScMgX_c_classInit --addr <addr> --size <size> --version 2004/b56 --module ov006`, 13 runs | 0 | MATCHING for the six applied; "none" with the size differences above for the seven kept |
| the same for Coin with `--include-dir` on the scratch header | 0 | MATCHING |
| the same for Snowball with `--include-dir` and `--no-strict-relocs` | 0 | MATCHING |
| `python tools/rombuild.py -j8 --no-cache` after | 0 | identical figures: `intactTuRom.identical` true, same sha256, 11,214 reproducing, 0 mismatching, 106 of 106 exact |
| byte comparison of the 106 linked module binaries against the base build's | 0 | 0 differ; the link map places the six factories at the same addresses and sizes |
| `python tools/prepush_linkcheck.py --range 4e48c28b97..HEAD` | 0 | 6 checked, 6 verified, 0 warnings, 0 blocking |
| `python tools/check_decl_agreement.py --changed 4e48c28b97` | 0 | 6 files in scope, 0 disagreements, no new |
| `python tools/check_decl_agreement.py`, at base and after | 0, 0 | both 14,534 disagreements with 1,384 banked rows gone; no new. Declarations fall from 35,770 to 35,762 |
| `python tools/check_decl_agreement.py --list`, at base and after | 0, 0 | identical apart from that count, so no banked row is healed and `config/decl-agreement-baseline.json` is unchanged |
| `python tools/prepush_attribution.py --base 4e48c28b97 --head HEAD` | 0 | 0 changed, 0 lost |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/tiers_ratchet.py --check`, at base and after | 0, 0 | CONVERTED PASS, 3222 at base, 3228 after |
| `python tools/check_src_tu_compiles.py --quiet` | 0 | 302 of 302 compile |
| `python tools/port_refcheck.py` | 0 | 408 references resolve |
| `python tools/check_profile_campaign.py`, at base and after | 0, 0 | identical output: 391 rows complete, 0 pending, 0 diverged |
| `python tools/langmode_audit.py --check langmode-baseline.json` | 0 | ratchet PASS |
| `python tools/langmode_audit.py --json`, at base and after | 0, 0 | identical metrics |
| `git diff --check 4e48c28b97 HEAD` | 0 | clean |
| `python -m unittest tools.test_objisolate` | 0 | 43 tests OK |

Independent verification of this exact candidate is a separate acceptance gate.
