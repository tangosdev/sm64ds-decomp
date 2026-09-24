# Handoff: promote-ov021-newslift-0923

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3073,
  task `promote-ov021-newslift-0923`, stage `revise`, role producer, session
  `claude-prod-promote-ov021-newslift-0923`, harness Claude Code.
- Source branch and previous accepted input SHA: `promote/promote-ov021-newslift-0923`, input
  `eb8f46d46a94a2a05cbcff9fd3d3ab228f2afe20` (origin main at enqueue).
- Original source base SHA and installed workflow and tool SHA: both `eb8f46d46a`; the branch
  descends from it directly and was not rebased.
- Separate evidence commits and required artifacts in this commit: none. The logs cited below
  are local build outputs and are not committed.
- Next action, responsible role and blockers: independent verification of this commit
  (byte, relocation, whole-object and source review). No blocker is known for that.
- Status: verified candidate on the producer's local gates; the independent verification has
  not run yet.
- Remaining uncommitted or local-only material and where it is preserved: none needed to
  reproduce. Experiment variants of the TU were scratch files and are not part of the evidence.

## What changed and why

- Class, TU, symbol and module-qualified ROM scope: `daObjCvNewsLift_c` in ov021. The promoted
  TU `src/actors/daObjCvNewsLift_c.cpp` owns `.text 0x021111a0..0x021121e8`, 19 functions,
  text-only route (no `.data` claim).
- Reserved surfaces actually touched: the class header (renamed to
  `include/daObjCvNewsLift_c.h`), the ov021 `delinks.txt` and `symbols.txt`, the class manifest
  `config/tu_manifest.d/ov021/daObjCvNewsLift_c.json`, `attribution.json`,
  `config/decl-agreement-baseline.json` (in-place insertions only), the converted baseline,
  `symbols/actor_renames.tsv`, `notes/data/class-build-worklist.tsv` and
  `notes/data/tu-promotion-queue.tsv`. No other shared header was edited.
- ROM observations: `_ZTI17daObjCvNewsLift_c` at 0x02114748 and `_ZTS17daObjCvNewsLift_c` at
  0x02114754 name the class. The typeinfo is referenced from the vtable header word of the
  table whose address point is 0x0211478c, which is exactly the address the tree carried as
  `_ZTV12WorkElevator`. The string `WorkElevator` does not occur in the cartridge. The vtable
  storage starts at 0x02114784 and has 32 slots: slot 0 InitResources 0x02111c6c, slot 3
  CleanupResources, slot 6 Behavior, slot 9 Render, slots 16 and 17 D1 and D0, and slot 31
  `dBgActor_c::Kill` 0x020ee55c. 32 slots is the `dBgActor_c` shape.
- Lineage evidence or structural inference: the rename came from `tools/class_rename.py`
  (without `--with-derived`). Its incidental rewrites of `notes/data/c-cpp-classification.tsv`,
  `notes/data/tu-merge-candidates.json` and `notes/experiments/` were reverted. The in-place
  rewrite of `symbols/actor_renames.tsv` erased the coined lineage; this commit restores it as
  "(was ...)" suffixes, the same shape as the landed `daChair_c` rows.
- Hypothesized names, explicitly not recovered facts: every field name from 0xc40 on
  (`mHomePos`, `mRotation`, `mTargetRotation`, `mGroundY`, `mMotorSound`, `mResetTimer`,
  `mWobblePhase`, `mWobbleTimer`, `mLoweredPlatform`, `mPrevLoweredPlatform`,
  `mTiltHoldTimer`, `mBumped`, `mPlayerOnMesh`) is inferred from use. The file name
  `daObjCvNewsLift_c.cpp` is the tool's default path, not a recovered file name.
- Compiler experiments and measured barriers:
  - The destructor is inline in the header. The out-of-line form emits D0 ahead of D1 (the
    reverse of the cartridge) and a homeless D2. Inline, the order is D1 then D0, D2 is
    dead-stripped, and InitResources is the key function, so the vtable is emitted here.
  - `func_ov021_02111434` previously "matched" through a `static asm` helper whose relocated
    words were wildcards. It now matches in plain C++ as a `void` function; the ROM's dead
    `ands` comes from the two early returns in its tail.
  - Kept because measured: the `(u16)(s16)mWobblePhase` double cast, the volatile reads of
    `data_02092768` in the reset path, and the per-element pointer writes in Behavior. The
    0x30-stepped `platform` walker in `func_ov021_0211129c` stores through
    `(char *)platform + 0x460`: `mPlatformMats[i]` misses by 8 words and a second pointer
    reallocates the loop.
  - The five collision callbacks are `void` and use a local `int isPlayer`. The one-argument,
    comma-expression and `bool` spellings each measured different register allocation.

## Reconstruction dimensions

- Exact function, byte and relocation coverage: 19 of 19 functions, all relocation
  destinations checked (below). Compiler-only output is licensed in the manifest:
  `_ZN10dBgActor_cD2Ev` dead-stripped, `_ZN7Vector3D1Ev` as a duplicate of arm9 0x020072c0,
  the ancestor typeinfo chain copied from the `daObjCvShutter_c` manifest, and the class's own
  `_ZTI`, `_ZTS` and `_ZTV` at their ROM addresses.
- Genuine methods; remaining free-function and ABI bridges: InitResources, CleanupResources,
  Behavior, Render and both destructors are real members. classInit is `new
  daObjCvNewsLift_c()`. Twelve helpers keep their unnamed `func_ov021_*` symbols: the eight
  collision callbacks and setters from 0x02111ec4 to 0x02111fe4, `func_ov021_02112024`,
  `func_ov021_02112128`, `func_ov021_02111434` and `func_ov021_0211129c`. They take the class pointer explicitly. InitResources still calls
  the mangled `_ZN10dBgW_KcMbg7SetFile...` spelling (by-value `Fix12<int>`) and the unnamed
  `func_020393c4` and `func_020393d4`.
- Recovered layout and fields; remaining shadow structs and raw offsets: the header is a real
  `dBgActor_c` subclass of size 0xc80 with the platform arrays and every tail field typed. The
  remaining raw offset is the `+ 0x460` walker above. `NewsLiftFile` is a local layout for the
  four file records at 0x021149a0.
- Lifecycle, vtable, RTTI, initializer and data ownership: D1 and D0 are emitted from the
  inline header destructor, with no symbol marker, because they are inline in a directly
  included header. The vtable and RTTI are emitted by this TU and licensed at the ROM
  addresses. Data stays ROM-owned (text-only route).
- Attribution preserved through each move and rename: 19 `path#symbol` overrides. Behavior
  keeps its pre-existing `github-actions[bot]` override from the base. Git history for that
  shard names Tango (#2301), but changing it would be a credit change, which this task may not
  make. InitResources keeps andrewboudreau; the rest keep tangosdev.
- Remaining agreed issue scope: name the twelve helpers; replace the mangled SetFile call and
  the unnamed `func_020393c4` and `func_020393d4` with header declarations; fix
  `include/decl_common.h`'s `int` declarations of the ov021 helpers (shared header, needs a
  reservation); decide whether the TU boundary belongs in `tu_map` (see below).

## Proof

The full-ROM build and `tubuild verify` ran on the source and config state of this commit,
before this note and the `actor_renames.tsv` lineage suffixes were added. Neither of those
files is compiled. The other gates were re-run on this commit's tree.

- Full-ROM build: `python tools/rombuild.py -j16`, exit 0. 7588 enrolled; 11,211 source-built
  functions reproducing, 0 mismatching; module fidelity 106 of 106 exact; ROM-build analysis
  PASS; intact TU gates PASS with zero new symbol errors.
- Explicit function and consumer relocation checks: `python tools/prepush_linkcheck.py --range
  eb8f46d46a..HEAD`, exit 0, 19 checked, 19 verified, 0 blocking.
- Complete emitted TU and data and metadata checks: `python tools/tubuild.py verify
  ov021/daObjCvNewsLift_c`, exit 0: 19 of 19 MATCH, objisolate clean, reloc-destinations clean,
  ROM-ascending emission order, TEXT-VERIFIED, compiler-only output exact. `tubuild linkcheck`
  gave SCRATCH-LINK-VERIFIED with 9 `dsd check symbols` errors, which tubuild reports as pre-existing with 0
  new, and a ROM identical to the stock control.
- Shared-header consumer expansion: only the class's own header changed; no shared header was
  edited. `python tools/check_src_tu_compiles.py`, exit 0, 269 of 269 TUs compile.
- Attribution: `python tools/prepush_attribution.py --base eb8f46d46a --head HEAD`, exit 0.
  7763 tracked, 19 consolidated with credit intact, 0 changed, 0 lost.
- Declarations: `python tools/check_decl_agreement.py --changed eb8f46d46a`, exit 0, "no new
  declaration disagreements". Before banking it reported 25. 24 were re-keyed: the baseline
  already banked the same symbol and kind under a legacy shard this promotion deletes. The
  last, `data_ov021_02114a20`, is the TU's `Vector3 []` against decl_common's `int []`
  placeholder, banked beside the existing `__sinit` entry. One re-keyed row lands on
  `include/decl_common.h`: its `int` return for `func_ov021_02111434` now disagrees with the
  corrected `void` definition. It is banked, not fixed, because the header is not reserved.
  The baseline change is 57 inserted lines and 0 removed; nothing was regenerated or re-sorted.
- Port, path, reference and other static gates:
  - `python tools/check_dead_references.py`, exit 0, no new dead references.
  - `python tools/tiers_ratchet.py --check`, exit 0, CONVERTED PASS, baseline 2993, current
    3033. The gain is not banked; `--update` was not run.
  - `python tools/check_tubuild_conflicts.py`, exit 0.
  - `python tools/queue_audit.py --check-promoted`, exit 0 after this commit set the queue
    row's `already_promoted` to `yes` (it failed with exit 1 before that).
  - `python tools/port_refcheck.py`, exit 0, 408 references resolve.
- Private validation: not run by the producer.

## Barriers and caveats

- The TU is wider than `build/tu_map.json`'s run. The manifest was extended by hand to include
  the callbacks, setters and classInit (0x02111ec4..0x021121e8). The FOLD notes in the manifest
  give the evidence.
- The Behavior attribution override names `github-actions[bot]`. This is inherited from the
  base unchanged, and is flagged for the coordinator.
