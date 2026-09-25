# Handoff: readable-ov098-wbm-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3081, task `readable-ov098-wbm-0925`, stage `revise`, role producer, round-1 session `claude-prod-readable-ov098-wbm-0925`, round-2 session `claude-prod-readable-ov098-wbm-0925-r2`, round-3 session `claude-prod-readable-ov098-wbm-0925-r3`, round-4 session `claude-prod-readable-ov098-wbm-0925-r4`, Claude Code.
- Source branch and previous accepted input SHA: `readable/readable-ov098-wbm-0925`. Round 1 took input `7423f8d73f391870e7244a19599eba122f3596d5` (origin/main at enqueue). Round 2 takes the round-1 candidate `6e3545f1c7176cb7c406064db0cd762ec4aaecf0` as input and fixes verifier finding F10. Round 3 takes the round-2 candidate `25c57be5d59d1f4c27ff5e2e59b6d3443e5f0c73` as input and fixes verifier finding F11 in the manifest notes and this handoff only. Round 4 takes the round-3 candidate `8a6b2958ab69669a3d1fcdd1fd809b49b340f3ba` as input and fixes verifier findings F12 and F13 in this handoff only. Every gate range still uses base `7423f8d73f`.
- Original source base SHA and installed workflow/tool SHA: both `7423f8d73f391870e7244a19599eba122f3596d5`.
- Separate evidence commits and required artifacts in this commit: none. The compiler experiments are recorded below only as prose (the source change tried and the measured result); no experiment artifact is committed.
- Next action, responsible role and blockers: independent verification, round 4: handoff accuracy (F12, F13) of this commit. No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted/local-only material and where it is preserved: none.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `ov098/daWbm_c`, `src/actors/daWbm_c.cpp`, ov098 .text 0x0213b47c..0x0213bf60 (14 functions).
- Reserved source/header/config surfaces actually touched: `src/actors/daWbm_c.cpp`, `include/daWbm_c.h` (five member declarations added), `config/arm9/overlays/ov098/symbols.txt` (five rows renamed in place), `config/tu_manifest.d/ov098/daWbm_c.json` (rounds 1 and 2: five `functions[].symbol` fields, plus the rename of `func_ov098_0213b9d8` to `_ZN7daWbm_c9StateDropEv` inside `notes[2]` and inside the `_ZN7Vector3D1Ev` `compiler_only_output[0].reason`; round 3: `notes[0]`, `notes[1]`, `notes[2]`, `notes[4]`, `notes[5]`, `boundary_evidence[1]` and `boundary_evidence[3]`, see F11; every changed key is listed under Changed paths), this handoff, and this class's five keys in the `attribution.json` ledger.
- ROM observations: none new. The class identity and layout evidence is unchanged from the promotion.
- Lineage evidence or structural inference: the functions at 0x0213b520 and 0x0213b63c (legacy `func_ov098_0213b520` and `func_ov098_0213b63c`, now Land and Burst) take the object in r0 and are only called with it, so they are written as members. The three state bodies are called through the pointer-to-member table with the object in r0, and state 1 at 0x0213bb1c calls state 2 at 0x0213b7e8 directly with it, so they are written as members too. Bytes and relocations are unchanged.
- Hypothesized names/filenames, explicitly not recovered facts: `daWbm_c::Land` (0x0213b520, symbol `_ZN7daWbm_c4LandEv`), `daWbm_c::Burst` (0x0213b63c, `_ZN7daWbm_c5BurstEv`), `daWbm_c::StateBounce` (0x0213b7e8, `_ZN7daWbm_c11StateBounceEv`), `daWbm_c::StateDrop` (0x0213b9d8, `_ZN7daWbm_c9StateDropEv`) and `daWbm_c::StateFall` (0x0213bb1c, `_ZN7daWbm_c9StateFallEv`) are coined names. The source says so. Each state name follows its body: state 0 waits for a player below and spawns a bomb, state 1 waits for the ground, and state 2 runs the squash, stretch and hop.
- Compiler experiments and measured barriers: see the table below. Each was run in round 1 with `tools/tubuild.py verify ov098/daWbm_c` against this TU at the round-1 input commit and then reverted, except E4, which is adopted. None was rerun in rounds 2, 3 or 4.

| ID | Finding | Change tried | Result |
|---|---|---|---|
| E1 | F4 int flag | In `func_ov098_0213b6e0`, `if (actorID == 0xbf)` and `if (actorID == 0xce)` in place of the `int is` flag | 13/14; `func_ov098_0213b6e0` 0x108 -> 0xf0 |
| E1b | F4 int flag | The same tests as `switch (other->actorID)` with `case 0xbf` and `case 0xce` | 13/14; `func_ov098_0213b6e0` 0x108 -> 0xf4 |
| E2 | F4 one-step arithmetic | State 0 (then `func_ov098_0213b9d8`, now `StateDrop`) folded: `a = (u16)player->mPrevAngleY >> 4; lead = (player->mHorzSpeed >> 12) * 0x1c + 0x64; v.x += lead * cos; v.y = playerY + 0x480000; v.z += lead * sin;` | 13/14; state 0 0x144 -> 0x13c |
| E2b | F4 one-step arithmetic | Partly folded: `a = (u16)angle >> 4; lead = (speed >> 12) * 0x1c; ... lead += 0x64;`, rest unchanged | 13/14; state 0 0x144 -> 0x13c |
| E3 | F4 include order | `#include "daWbm_c.h"` ahead of `#include "common.h"` | 13/14; `func_ov098_0213b584` 0xb8 -> 0xec, and an extra `_ZN9Matrix4x3D1Ev` is emitted |
| E3b | F4 include order | `#include "common.h"` removed | 13/14; `func_ov098_0213b584` does not match |
| E4 | F3 SetFile bridge | `mModel.SetFile(bmd, 1, 0x16)` in both InitResources calls; the `_ZN9ModelBase7SetFileEP8BMD_Fileii` prototype deleted | 14/14, undefined symbols identical. Adopted. |
| E5 | F3 Fix12 bridges | `Fix12<int> r = {...}, h = {...}; mdCcAc_c.Init(this, r, h, flags, 0);` for both dCcAc_c::Init calls | 13/14; `_ZN7daWbm_c13InitResourcesEv` 0x1fc -> 0x21c |

The three F4 shapes each stay, each with one short comment naming what it forces. The StateDrop and include-order comments give the measured size; the hit-check comment says only that the other spellings are shorter.
E5 confirms wall 6az for this TU. The other four Fix12 bridges have the same by-value Fix12 parameter shape and were not tried one by one.

## Deferred findings from #3081

| Finding | Outcome | Reason |
|---|---|---|
| F2 states and helpers are extern "C" free functions | Partially fixed | `Land`, `Burst`, `StateDrop`, `StateFall` and `StateBounce` are members now (the three states in round 2, see F10). `func_ov098_0213b584` and `func_ov098_0213b6e0` stay C-linkage free functions because `include/decl_common.h` declares both by those names and is off limits. |
| F3 six mangled ABI bridges | Partially fixed | `ModelBase::SetFile` is now a member call (E4). `include/ModelBase.h` declares it returning int, so the old reason for the bridge no longer applied. The five Fix12-by-value bridges stay. E5 measured the wall for `dCcAc_c::Init` only; the other four share the by-value Fix12 parameter shape but were not tried one by one. `dBgCh_Actr::Init` is also declared with `Fix12i` in `include/dBgCh_Actr.h`, which mangles to a different symbol. |
| F4 three shapes called compiler-forced without an experiment | Deferred: experiments recorded in the handoff, no committed artifact | E1, E1b, E2, E2b, E3 and E3b in the table above are prose records of source changes tried and their measured sizes. No experiment artifact is committed under `notes/experiments/`, so the finding stays open. The three shapes stay, each with a one-line comment. |
| F5 `unk_3b4` | Still deferred | InitResources and Land only store 0 to it. No function in the class reads it, so nothing shows what it means. |
| F6 address-named data | Still deferred | `data_ov098_0213c930` is also named by `src/__sinit_ov098_0213c2b4.c` and `include/decl_common.h`; `data_ov098_0213c91c` is also named by `src/__sinit_ov098_0213c2b4.c`, the Cannon InitResources and CleanupResources shards (since promoted into `src/game/actors/d_a_cnn.cpp`). `data_ov002_0210da38`, `data_02082214`, `data_020a0e68` and `func_0201267c` are not ov098 rows. No overlay symbols.txt gives coined names to data like `data_ov098_0213bf90`, so there is no convention to follow. |
| F7 `include/decl_common.h` | Still deferred | The file is off limits (held by `jump-contract-repair-0918`). The diff against the base is empty. |
| F10 (round-1 verification): the source and the F2 row said the state bodies had to stay free functions because `src/__sinit_ov098_0213c2b4.c` and `include/decl_common.h` name them | Fixed | That reason was false. The static initializer only copies the .data words `data_ov098_0213c71c`, `data_ov098_0213c724` and `data_ov098_0213c72c`, whose ROM relocations reach the state bodies by address (the ov098 relocs.txt rows from 0x0213c71c, 0x0213c724 and 0x0213c72c), and `include/decl_common.h` declares none of the three. They are now members, with their symbols.txt rows, manifest `symbol` fields and attribution keys renamed in place. See the object comparison under Proof. The manifest's `notes` kept the false claim; that part is F11. |
| F11 (round-2 verification): manifest `notes[1]` still said the three states and four helpers keep C-linkage `func_ov098_*` names, and `notes[4]` still said `ModelBase::SetFile` stays mangled | Fixed | `notes[1]` now says the three states, Land and Burst are members under coined names and only the two helpers `include/decl_common.h` declares keep C linkage. `notes[4]` drops the SetFile clause and adds E5's measured size. A full read of the manifest also corrected `notes[0]` (it pointed at a reconciliation the source header does not contain and named the retired factory file), `notes[2]` (its sizes now match E1, E1b and E2), `notes[5]` ("named in this change" and a slash list; it also gained a sentence on why `unk_3b4` stays unnamed), and two `boundary_evidence` entries (the states called `func_ov098_*` state bodies, and "the rename commit on this branch"). A full read of this handoff corrected the F4 row, the experiments paragraph and the identity, scope and proof sections. |
| F12 (round-3 verification): the residue table gave the round-1 candidate 461 lines | Fixed | `git show 6e3545f1c7:src/actors/daWbm_c.cpp` has 460 lines. Every residue count was retaken mechanically; see the residue table and its counting rules. |
| F13 (round-3 verification): the scope line said rounds 1 and 2 changed only five `symbol` fields in the manifest | Fixed | Round 2 also renamed `func_ov098_0213b9d8` to `_ZN7daWbm_c9StateDropEv` inside `notes[2]` and inside the `_ZN7Vector3D1Ev` `compiler_only_output[0].reason`. The scope line now says so, and Changed paths lists every changed manifest key, from a parse of both JSON files. |
| F9 Vector3 casts over inherited dActor_c scalars, `unk_0a4`, `unk_0ac` | Still deferred | `include/dActor_c.h` declares position, scale and speed as scalars. `unk_0a4`, `mVertSpeed` and `unk_0ac` look like a speed vector, but naming them is a shared-header change this task does not hold. |

F1 and F8 were fixed by the promotion task.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 14/14 functions, objisolate clean, relocation destinations clean, ROM order.
- Genuine methods; remaining free-function/ABI bridges: 11 member functions (D1, D0, InitResources, Behavior, Render, CleanupResources, Land, Burst, StateDrop, StateFall, StateBounce) plus the factory. 2 C-linkage helpers remain (see F2) and 5 mangled bridges (see F3).
- Recovered layout/fields; remaining shadow structs/raw offsets: unchanged. `unk_3b4` is still unnamed. The Vector3 casts over `mPosX` and `mScaleX` remain (F9).
- Lifecycle, vtable/RTTI, initializer and data ownership: unchanged. Adding five non-virtual members changes neither layout nor vtable. The state table's .data words keep their relocations by address; the linker resolves them to the renamed rows.
- Attribution preserved through each move/rename: the five `attribution.json` keys follow the renamed symbols with the same authors (`lunavyqo` keeps state 0). prepush_attribution reports 0 changed and 0 lost.
- Remaining agreed issue scope: F2 (two helpers), F3 (five Fix12 bridges), F4 (no committed experiment artifact), F5, F6, F7 and F9 as in the table above.

Residue in `src/actors/daWbm_c.cpp`. The `lines` column is `wc -l` of `git show <rev>:src/actors/daWbm_c.cpp` (newline count; every revision ends in a newline). Each token column is the number of lines containing the token, `grep -c -F`; at all four revisions this equals the number of occurrences, `grep -o -F | wc -l`.

| | lines | `unk_` | `func_` | `extern "C"` | `_ZN` |
|---|---|---|---|---|---|
| Base `7423f8d73f` | 466 | 4 | 41 | 9 | 20 |
| Round-1 candidate `6e3545f1c7` | 460 | 4 | 29 | 7 | 19 |
| Round-2 candidate `25c57be5d5` | 459 | 4 | 16 | 4 | 22 |
| This commit | 459 | 4 | 16 | 4 | 22 |

Rounds 3 and 4 do not touch the source; the counts were retaken on this commit.

The `_ZN` count: round 1 drops the three `_ZN9ModelBase7SetFileEP8BMD_Fileii` lines (the prototype and both calls) and adds the Land and Burst marker lines, 20 to 19; round 2 adds the three state marker lines, 19 to 22. No mangled bridge was added.

## Changed paths

`git diff --name-only 7423f8d73f..HEAD` lists six paths:

| Path | Rounds | Change |
|---|---|---|
| `attribution.json` | 1, 2 | five keys renamed with their symbols, authors unchanged |
| `config/arm9/overlays/ov098/symbols.txt` | 1, 2 | five rows renamed in place: 0x0213b520, 0x0213b63c (round 1), 0x0213b7e8, 0x0213b9d8, 0x0213bb1c (round 2) |
| `config/tu_manifest.d/ov098/daWbm_c.json` | 1, 2, 3 | the keys below |
| `include/daWbm_c.h` | 1, 2 | declarations of Land and Burst (round 1), StateDrop, StateFall and StateBounce (round 2) |
| `notes/agents/handoffs/readable-ov098-wbm-0925.md` | 1, 2, 3, 4 | this handoff |
| `src/actors/daWbm_c.cpp` | 1, 2 | the source changes above |

Every changed key in `config/tu_manifest.d/ov098/daWbm_c.json`, from a python parse of the file at each revision, comparing leaves:

| Key | Round | Change |
|---|---|---|
| `functions[2].symbol` | 1 | `func_ov098_0213b520` to `_ZN7daWbm_c4LandEv` |
| `functions[4].symbol` | 1 | `func_ov098_0213b63c` to `_ZN7daWbm_c5BurstEv` |
| `functions[6].symbol` | 2 | `func_ov098_0213b7e8` to `_ZN7daWbm_c11StateBounceEv` |
| `functions[7].symbol` | 2 | `func_ov098_0213b9d8` to `_ZN7daWbm_c9StateDropEv` |
| `functions[8].symbol` | 2 | `func_ov098_0213bb1c` to `_ZN7daWbm_c9StateFallEv` |
| `compiler_only_output[0].reason` | 2 | `func_ov098_0213b9d8` to `_ZN7daWbm_c9StateDropEv` in the list of functions with Vector3 locals |
| `notes[2]` | 2, 3 | round 2: `func_ov098_0213b9d8` to `_ZN7daWbm_c9StateDropEv`; round 3: sizes from E1, E1b and E2 (F11) |
| `notes[0]` | 3 | F11 |
| `notes[1]` | 3 | F11 |
| `notes[4]` | 3 | F11 |
| `notes[5]` | 3 | F11 |
| `boundary_evidence[1]` | 3 | F11 |
| `boundary_evidence[3]` | 3 | F11 |

No other key changed: 13 changed leaves in all, 2 in round 1, 5 in round 2 and 7 in round 3 (`notes[2]` counts in both).

## Proof

### Round 4

Round 4 changes only this handoff. The commands below were run in the worktree on this commit.

| Command | Exit | Result |
|---|---|---|
| `git diff 8a6b2958ab..HEAD --name-only` | 0 | only `notes/agents/handoffs/readable-ov098-wbm-0925.md` |
| `python tools/tubuild.py verify ov098/daWbm_c` | 0 | 14/14 MATCH, objisolate clean, reloc destinations clean, all 14 in ROM order |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |

### Round 3

Round 3 changes only `config/tu_manifest.d/ov098/daWbm_c.json` (commit `c3dadda822`) and this handoff. The manifest `notes` and `boundary_evidence` are prose and do not reach the compiler. The commands below were run in the worktree on `c3dadda822`; this handoff is the only later change.

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov098/daWbm_c` | 0 | 14/14 MATCH, objisolate clean, reloc destinations clean, all 14 in ROM order |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/check_tubuild_conflicts.py` | 0 | 293 entries, every conflict recorded |
| `python tools/queue_audit.py --check-promoted` | 0 | every row agrees with config/tu_manifest.d |
| `python tools/prepush_linkcheck.py --range 7423f8d73f..HEAD` | 0 | 14 checked, 14 VERIFIED, 0 blocking |
| `python tools/check_decl_agreement.py --changed 7423f8d73f` | 0 | no new declaration disagreements |
| `git diff 25c57be5d5..HEAD --stat` | 0 | only `config/tu_manifest.d/ov098/daWbm_c.json` and this handoff |

`python tools/rombuild.py -j16` was not rerun in round 3; the round-2 run below stands for the unchanged source.

### Round 2

All commands were run in the worktree on the round-2 source commit `b38aa9af78`.

| Command | Exit | Result |
|---|---|---|
| `python tools/tubuild.py verify ov098/daWbm_c` | 0 | 14/14 MATCH, objisolate clean, reloc destinations clean, all 14 in ROM order. The round-1 candidate gives the same result. |
| Object comparison of `build/tu/ov098-daWbm_c/daWbm_c.o`, round-1 candidate against this commit, read with pyelftools, with the three old state names mapped to the new ones | n/a | 27 of 27 sections byte-identical; 19 of 19 relocation sections identical in offset, type, addend and target name; 74 and 74 undefined symbols, an identical set; 51 of 51 defined function and object symbols identical in binding, type, value and size. Without the name mapping, the only differences are the three renamed definitions. |
| `python tools/rombuild.py -j16` | 0 | 7189 enrolled, 7189 compiled and 0 reused. `build/src/actors/daWbm_c.o` was rebuilt and defines `_ZN7daWbm_c9StateDropEv`, `_ZN7daWbm_c9StateFallEv` and `_ZN7daWbm_c11StateBounceEv`. 11,213 functions reproducing, 0 mismatching, 106/106 modules exact. `intactTuRom.identical` is true, sha256 `d1506e90...c478e8`. The build's own baseline control step reports the same 9 pre-existing `dsd check symbols` errors (arm9 main and ITCM) before any TU is substituted. |
| `python tools/prepush_linkcheck.py --range 7423f8d73f..HEAD` | 0 | 14 checked, 14 VERIFIED, 0 blocking |
| `python tools/prepush_attribution.py --base 7423f8d73f --head HEAD` | 0 | 0 changed, 0 lost |
| `python tools/check_decl_agreement.py --changed 7423f8d73f` | 0 | no new declaration disagreements |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/tiers_ratchet.py --check` | 0 | PASS, baseline 2994, current 3116 |
| `python tools/check_tubuild_conflicts.py` | 0 | 293 entries, every conflict recorded |
| `python tools/queue_audit.py --check-promoted` | 0 | every row agrees with config/tu_manifest.d |
| `python tools/check_src_tu_compiles.py` | 0 | 293/293 compile |
| `python tools/port_refcheck.py` | 0 | 408 checked, all resolve |
| `python tools/affected_src.py include/daWbm_c.h` | 0 | only `src/actors/daWbm_c.cpp` consumes it |
| `git grep` for the three old state names in `src/`, `include/` and `config/`, excluding the append-only `.jsonl` logs | 0 | only the three manifest `legacy_source` paths, which name the retired one-function files and stay as history, as they do for Land and Burst |
| `git diff 7423f8d73f..HEAD -- include/decl_common.h config/dead-reference-baseline.json config/decl-agreement-baseline.json` | 0 | empty |

- Full-ROM build: see the rombuild row.
- Explicit function/consumer relocation checks: tubuild verify and prepush_linkcheck, above. The state table's .data relocations resolve by address to the renamed rows, and the intact-TU ROM is identical.
- Complete emitted TU and data/metadata checks: this TU licenses .text only. The whole-object vtable, RTTI and romdata checks from the promotion were not rerun; the class's data and virtual members are unchanged.
- Shared-header consumer expansion: `include/daWbm_c.h` has one consumer, this TU.
- Port/path/reference and other applicable static gates: above. `config/match_attempts.jsonl` and `config/match_provenance.jsonl` still carry `func_ov098_0213b9d8` in their `name` fields. Their rows are keyed by `ov098:0x0213b9d8`, and editing an append-log row is left to its owners.
- Private validation, if run, and the exact PR head/base it tested: not run.
