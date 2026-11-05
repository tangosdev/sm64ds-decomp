# Handoff: issue-4570-koopa-homepos-vector-view-1105

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: issue #4570; `issue-4570-koopa-homepos-vector-view-1105`; producer; `codex-koopa-homepos-vector-view-1105`; Codex.
- Source branch and previous accepted input SHA: `refs/heads/audit/koopa-homepos-vector-view1105`; `c6bfb72fc09c4b14c2cb344b4f628f649148985e`.
- Original source base SHA and installed workflow/tool SHA: `c6bfb72fc09c4b14c2cb344b4f628f649148985e` for both.
- Separate evidence commits and required artifacts in this commit: none. Local build artifacts remain ignored under `build/`; ROM/compiler inputs are linked, not included.
- Next action, responsible role and blockers: independent verifier accepts this exact commit and re-runs the focused TU and full stock ROM proof; source-quality review remains pending.
- Status: verified candidate for independent review. The standalone `tubuild verify` reports the existing unlicensed `__sinit_d_a_nknk.cpp` output, which is present on the base too; the full production ROM analysis passes.
- Remaining uncommitted/local-only material and where it is preserved: none after this commit; the private queue receipts stay under ignored `build/`.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daNknk_c`, TU `ov062/Koopa+KoopaSmall`, ROM range `ov062:0x021174cc..0x021197ac`.
- Reserved source/header/config surfaces actually touched: `src/game/actors/d_a_nknk.cpp`, `include/Koopa.h`, and `config/tu_manifest.d/ov062/Koopa+KoopaSmall.json`.
- ROM observations: `mHomePosX/Y/Z` are initialized from `mPosX/Y/Z`; the values bound the Koopa leash and supply its return heading. The three words are contiguous at offsets `0x39c`, `0x3a0`, and `0x3a4`.
- Lineage evidence or structural inference: the typed `mHomePos` view overlays the existing scalar fields in an anonymous union; `Koopa_size_must_be_0x3d0` remains and was compiled by MWCC. No fields move.
- Hypothesized names/filenames, explicitly not recovered facts: none. `mHomePos` is an established field name in the header comments and its use is consistent with every inspected access.
- Compiler experiments and measured barriers: adding the nontrivial `Vector3` union member emits an unnamed 4-byte D1 thunk. ELF inspection found `STB_LOPROC`/`STT_FUNC`, body `bx lr`, and no inbound relocations; there is no `symbols.txt` row for it. The manifest deadstrips this compiler-only output. The same change renumbers eleven local BSS labels; the manifest rekeys them by matching baseline/candidate section index, offset, size and binding while preserving each canonical destination.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: the focused TU reports 39/39 MATCH, clean object isolation, clean relocation destinations and ROM-ascending emission.
- Genuine methods; remaining free-function/ABI bridges: no function ownership or method signatures changed.
- Recovered layout/fields; remaining shadow structs/raw offsets: the home position now has a typed `Vector3` view while preserving scalar aliases. Other Koopa reconstruction is outside this slice.
- Lifecycle, vtable/RTTI, initializer and data ownership: class size remains `0x3d0`; no data ownership or RTTI/vtable claim changed. The focused tool still reports the base-existing `__sinit_d_a_nknk.cpp` section as unlicensed; the stock production link accepts the candidate.
- Attribution preserved through each move/rename: no source moves or function ownership changes.
- Remaining agreed issue scope: independent review and terminal PR validation.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`; exit 0. Source-built functions 11,395, mismatches 0; module fidelity 106/106 exact; source data 470/470 reproduces; BSS claims 302; ROM data report 2,228 symbols verified, 690 partial, 0 differ. `--no-rom` skips writing a final NDS image.
- Explicit function/consumer relocation checks: `python tools/tubuild.py verify ov062/Koopa+KoopaSmall --no-write --verdict`; 39/39 match, object isolation and relocation destinations clean, emission order correct. Exit 1 only for the pre-existing unlicensed `__sinit_d_a_nknk.cpp`; no new policy failure remains.
- Complete emitted TU and data/metadata checks: focused TU proof above; no data ownership changed. The `Koopa_size_must_be_0x3d0` compile-time assertion and MWCC compilation passed.
- Shared-header consumer expansion: `python tools/affected_src.py include/Koopa.h`; only `src/game/actors/d_a_nknk.cpp` consumes the header.
- Port/path/reference and other applicable static gates: `python tools/check_decl_agreement.py --changed origin/main` exit 0, no new local redeclarations or declaration disagreements; `python tools/port_refcheck.py` exit 0, 217 references resolve; `git diff --check` exit 0.
- Private validation: not run; PR validation remains pending after publication.
