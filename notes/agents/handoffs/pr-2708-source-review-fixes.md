# Handoff: pr-2708-source-review-fixes

This document describes this commit. It is the partial-scope disposition record
that [PR #2708](https://github.com/tangosdev/sm64ds-decomp/pull/2708)'s
`CHANGES_REQUESTED` review asked for under
[notes/agents/SOURCE-REVIEW-CUTOVER.md](../SOURCE-REVIEW-CUTOVER.md). It does not
publish a source-review pass and does not claim one.

## Identity

- Durable thread: PR #2708. No class issue exists for `daDsnBase_c`; the PR is the
  durable thread until one is opened, the same convention
  `pr-2450-source-review-fixes` used.
- Reviewed candidate at the blocking review: `04be8b484ff09afd7426f0e4fad92595bd47dffe`.
  Reviewer re-check head: `7fe8968965a3ca72626c4f32a849be69550448e1`.
- This candidate: the commit that adds this file, on branch `cr/pr2708-0917`.
- PR base at writing: `491c2eea764f01e498611270e565c814afb928b4`; `origin/main` tip
  `0fb9ed8384aa8503a40e68665b4c2ca747a2da2c`. A composition against a newer main
  needs its own review and validation.
- Compiler pin `2004/b56`, unchanged.
- `completion: partial`. The `deferred` rows below are producer-proposed. The
  cutover document's acceptance table reserves `deferred` to the reviewer, who
  must explicitly accept the partial scope; this producer does not approve its own
  deferral.
- Reviewed paths in this candidate: `src/actors/daDsnBase_c.cpp`,
  `include/daDsnBase_c.h`, `config/tu_manifest.d/ov091/daDsnBase_c.json`,
  `config/decl-agreement-baseline.json`, the six `dsn-2708` experiment notes under
  `notes/experiments/`, and this handoff.

## Finding dispositions

| Finding | Location | Kind | Disposition | Reason |
| --- | --- | --- | --- | --- |
| DSN-2708-01 | `src/actors/daDsnBase_c.cpp:85-91`, calls at `:215`, `:265`, `:278` | correctness | `fixed` | The declaration is spelled from the definition (`src/DecIfAbove0_Byte.c`: `unsigned char DecIfAbove0_Byte(unsigned char *)`), the three callers cast, and the ABI comment now says a call site cannot evidence either the parameter or the return type. Landed at `3fab96894` and independently re-checked by the reviewer at `7fe896896`. Re-proved here, and the manifest note that still carried the old call-site rationale is corrected in this commit. |
| DSN-2708-02 | `src/actors/daDsnBase_c.cpp:30-33`; sites throughout the seven helpers | reconstruction | `deferred` (proposed) | `0x360..0x39f` is still read through `char *` plus literal offsets: the shadow `Matrix4x3` at `0x360`, the shadow position words `0x384`/`0x388`/`0x38c`, the top height `0x390`, the ground height `0x394`, the state at `0x398` and the timer at `0x39e`. The storage is not this class's: `include/daDsnBase_c.h` closes `daDsnBase_c` at `0x360` on its minimal reading and asserts `sizeof == 0x360`, and each leaf declares the span for itself (`include/daDkk_c.h` `pad_360`/`mProbeHeight`/`unk_394`/`mState`/`unk_39c`/`unk_39e`/`unk_39f`; `include/Thwomp.h` `pad_360`/`mState`/`mRetrigger`). Typing them here means moving the shared words up into `daDsnBase_c`, which changes that size assert, edits both leaf headers and re-verifies `ov025/daDkk_c` and `ov091/Thwomp`. Outside this candidate's reserved file set. |
| DSN-2708-03 | `src/actors/daDsnBase_c.cpp:26-28`; the seven `func_ov091_*` definitions | reconstruction | `deferred` (proposed) | The seven helpers keep ROM labels, C linkage and `char *` parameters. `include/decl_common.h` already declares all seven that way, and `daDkk_c::Behavior` (ov025) plus `Thwomp::Behavior` call them by name across the TU boundary, so giving them real member identities is a cross-module symbol migration, not a local rename. |
| DSN-2708-04 | `src/actors/daDsnBase_c.cpp:62-77` | reconstruction | `deferred` (proposed) | `DsnBaseFileTable` is a TU-local six-word shape read off its consumers, not a recovered original type. The two real tables (`data_ov091_02135138`, `data_ov025_02113814`) are owned by the leaf overlays and are never defined here. Promoting it to a shared header belongs with whichever pass owns those two data symbols. |
| DSN-2708-05 | `src/actors/daDsnBase_c.cpp:53-60` | reconstruction | `deferred` (proposed) | `kYoshiEggActorID` and `kDosunActorID` are TU-local coined names. No header carries actor-ID vocabulary and cleaned callers still pass raw hex; the two values are cited to the ROM's own debug table through `symbols/profile_reconstruction_registry.tsv`. A shared enum is a tree-wide naming pass. |
| DSN-2708-06 | `src/actors/daDsnBase_c.cpp:83-113` | reconstruction | `deferred` (proposed) | `RandomIntInternal`, `data_0209e650`, `Vec3_Dist`, `func_0201267c` and the three mangled `_ZN8dActor_c...` / `_ZN8Particle6System...` entries are declared in this TU's own `extern "C"` block because no header declares them. `func_0201267c`'s `void` return is confirmed by the enrolled definition at `src/func_0201267c.cpp`; the rest remain reconstructed interfaces reached by address identity. Three of them are additionally pinned by DSN-2708-07. |
| DSN-2708-07 | `src/actors/daDsnBase_c.cpp:107-113`, the `DropShadowScaleXYZ` call in `func_ov091_02133098`, the `Earthquake` call in `func_ov091_02132f04` | reconstruction | `compiler_constraint` | The mangled TU-local spellings of `dActor_c::DropShadowScaleXYZ`, `dActor_c::Earthquake` and `Particle::System::NewSimple` are kept because the `Fix12<int>`-by-value member forms change codegen. Measured under the pin, artifacts committed on this branch: `notes/experiments/dsn-2708-dropshadow.md` (`func_ov091_02133098` `0x120` -> `0x134`, 999 words differ) and `notes/experiments/dsn-2708-earthquake.md` (`func_ov091_02132f04` `0xf0` -> `0xfc`, 999 words differ). Both were run at `7027888ce` with `tools/match.py --strict-relocs`. |
| DSN-2708-08 | `src/actors/daDsnBase_c.cpp`, `daDsnBase_c::CleanupResources` | reconstruction | `compiler_constraint` | `CleanupResources` reloads `mFileTable` after each `Release` because a `Release` clobbers it; the three loads are the ROM's. Artifact `notes/experiments/dsn-2708-cleanup-reloads.md`: hoisting the load into a single local gives `0x58` -> `0x50`, two loads gone, 999 words differ. |
| DSN-2708-09 | `src/actors/daDsnBase_c.cpp`, the landing tail of `func_ov091_02132f04` | reconstruction | `fixed` | The `isDosun` temporary no longer doubles as the `mVertSpeed` zero. `notes/experiments/dsn-2708-isdosun.md` measured the plain split spelling as DISPROVED (same bytes), and `7fe896896` applied it. |
| DSN-2708-10 | `src/actors/daDsnBase_c.cpp`, the `aimPos` block in `func_ov091_02132dc0` | reconstruction | `fixed` | `aimPos` is a plain `Vector3` local again. `notes/experiments/dsn-2708-volatile-aimpos.md` measured `volatile` as DISPROVED (same bytes), and `7fe896896` applied it. The block is still dead by value -- the distance check reads the actor origin, not `aimPos` -- and the source comment says so; the three stores and the `OnAimedAtWithEgg` call are the ROM's. |

DSN-2708-02 through DSN-2708-06 are what "the retained field/state reconstruction"
covers, and they are why this candidate is `completion: partial`. Their named next
owner is the `cleanup-dadkk` producer, through @andrewboudreau as integrator, on
PR #2708 until a `daDsnBase_c` class issue exists. DSN-2708-02 is the one the PR
description already named as waiting on `cleanup-dadkk`; DSN-2708-03 through
DSN-2708-06 are registered here so the deferral is explicit rather than implied by
the file header's `Leftover:` list.

The destructor pair's `deadstrip-duplicate` licensing and the narrowed
`0x02132dc0..0x02133254` run are unchanged promoted-manifest policy that predates
this PR; they are recorded in the manifest's own `boundary_evidence` and are not
re-litigated here.

## Adjacent defect observed, not a finding on this candidate

`include/Thwomp.h`'s comment on `pad_360[0x38]` says "nothing in the tree reads
`0x360..0x398` on either leaf". This TU's `func_ov091_02133098` writes `0x360`,
`0x384`, `0x388` and `0x38c` and reads `0x394`, and `func_ov091_02133020` reads
`0x390` -- on both leaves, because both `Behavior`s call these helpers. The claim
was already inaccurate on main for the legacy shards this TU merged, and this
candidate neither makes nor repeats it. Correcting it edits a shared header
consumed by a different TU, so it belongs with the DSN-2708-02 move-up rather than
with this disposition record.

## What changed in this candidate

`src/` is byte-identical to `7fe896896`; no source edit was needed, because
DSN-2708-01 had already landed at `3fab96894`.

- `config/tu_manifest.d/ov091/daDsnBase_c.json`: the `DecIfAbove0_Byte` note still
  said the kept `char *` spelling was justified because every call site in the
  merged TU passes a `char *` plus `0x39e`. That is the reasoning DSN-2708-01
  rejected, and it contradicted the source. The note now records the definition as
  the evidence.
- `config/tu_manifest.d/ov091/daDsnBase_c.json`: `verification.compilerOnlyOutput.droppedSections`
  loses a stale `35`. `tools/tubuild.py verify` rewrites that block from the object
  it just compiled; the entry had not been refreshed since `7fe896896` removed the
  `volatile` local. Emitted function bodies are unchanged.
- This handoff.

## Proof

Run in `C:/tmp/sm64ds-cr2708` on this candidate, compiler pin `2004/b56`
(`python tools/build_pin.py verify` -> `verify: 2004/b56`).

| Check | Result |
| --- | --- |
| `python tools/rombuild.py -j16 --no-rom` | exit 0. `[3/6] mwccarm: 8760 enrolled source file(s)` -- `8759 reused from cache, 1 compiled`; `src/actors/daDsnBase_c.cpp` is enrolled in `config/arm9/overlays/ov091/delinks.txt`. 11,206 source-built functions, 11,206 reproducing, 0 mismatching; 26 source-owned data claims, 0 mismatching; module fidelity 106/106 exact; ROM-build analysis PASS. The nine `[7/8] dsd check symbols --fail` errors are the baseline control's identical nine: pre-existing and tree-wide. |
| `python tools/tubuild.py verify ov091/daDsnBase_c` | exit 0. 9/9 MATCH, objisolate clean, relocation destinations clean, emission order ROM-ascending -> TEXT-VERIFIED. `compiler-only: exact deadstrip ['_ZN11daDsnBase_cD0Ev', '_ZN11daDsnBase_cD1Ev', '_ZN7Vector3D1Ev']`. |
| `python tools/linkcheck.py --c src/actors/daDsnBase_c.cpp --module ov091`, one run per manifest symbol | 9 of 9 `VERIFIED`, `diffs: []`, `blind: 0`, including `func_ov091_02132e64` at `0x02132e64` size `0x34`. Read from the printed verdict, not the exit code: this tool exits non-zero on a single-name run that verifies. |
| Emitted function bodies | The compiled object carries exactly 12 `STT_FUNC` bodies: the 9 licensed symbols plus `_ZN11daDsnBase_cD1Ev`, `_ZN11daDsnBase_cD0Ev` and `_ZN7Vector3D1Ev`, which is what the manifest licenses and deadstrips. None added, none removed. |

No ROM was packed for this record (`--no-rom`), and no private validation, CI
result or merge is claimed. An independent verifier must still review this
candidate at its final head against the current base and decide whether the
`deferred` scope above is acceptable.
