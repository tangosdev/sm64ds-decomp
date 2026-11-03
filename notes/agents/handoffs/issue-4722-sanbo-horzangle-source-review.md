# Handoff: issue-4722-sanbo-horzangle-signature-1103

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/4722; `issue-4722-sanbo-horzangle-signature-1103`; producer; `codex-sanbo-horzangle-1103c`; Codex.
- Source branch and previous accepted input SHA: producer branch; `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`.
- Original source base SHA and installed workflow/tool SHA: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`; `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Separate evidence commits and required artifacts in this commit: none; the handoff records the local checks and exact scope.
- Next action, responsible role and blockers: An independent verifier should accept this exact commit, review the helper signature and four angle callsites, then repeat the TU, relocation and full-ROM gates. No known byte or link blocker remains.
- Status: verified candidate; independent source review is pending.
- Remaining uncommitted/local-only material and where it is preserved: generated logs and baseline/candidate ELF comparison helpers are under ignored `build/issue-4722-*` and `build/sanbo-*.o`; no source changes remain uncommitted.

## What changed and why

- Class/TU/symbol and module-qualified ROM scope: `daSanbo_c`, `ov096/daSanbo_c`; affected methods cover `ov096:34822344..34822472`, `34822472..34823724`, `34823932..34824388`, `34824500..34824804`, and `34825524..34825684`.
- Reserved source/header/config surfaces actually touched: `src/game/actors/d_a_sanbo.cpp` and `config/tu_manifest.d/ov096/daSanbo_c.json`.
- ROM observations: `Vec3_HorzAngle` is defined and declared by `include/math/CalcLib.h` as `short(const Vector3 *, const Vector3 *)`. The prior local `const void *` declaration erased that shared type. Four callsites pass contiguous position triples, so casts now mark those vector boundaries explicitly.
- Lineage evidence or structural inference: the helper signature is taken from the shared declaration and definition. Existing position triples and actor layout were not changed.
- Hypothesized names/filenames, explicitly not recovered facts: none introduced.
- Compiler experiments and measured barriers: including `CalcLib.h` also replaces Sanbo's inaccurate local declarations for `Vec3_Asr`, `RandomIntInternal`, and `Vec3_HorzDist`. The pinned compiler consequently renumbers per-TU `@NNN` locals. Each of the fourteen manifest aliases was mapped to the candidate symbol by comparing baseline and candidate ELF section index, kind, size, contents, and relocation targets. No ROM destination identity changed.

## Reconstruction dimensions

- Exact function/byte and relocation coverage: 36/36 Sanbo TU functions match; object isolation and relocation destinations are clean. `prepush_linkcheck` reports 36 VERIFIED, the separately configured static initializer as BLIND-3, and zero blocking findings.
- Genuine methods; remaining free-function/ABI bridges: no method conversion was attempted; existing method identities are unchanged.
- Recovered layout/fields; remaining shadow structs/raw offsets: no layout changes; existing raw vector triples remain represented by explicit boundary casts.
- Lifecycle, vtable/RTTI, initializer and data ownership: preserved. The full ROM build's intact-TU gates report zero new symbol errors and exact storage aliases.
- Attribution preserved through each move/rename: no source moves or attribution changes.
- Remaining agreed issue scope: independent review of this focused type correction; broader Sanbo reconstruction remains outside this task.

## Proof

- Full-ROM build: `python tools/rombuild.py -j16 --no-rom`, exit 0; 106/106 modules exact, 11,395 source functions reproducing, zero mismatches. ROM data check: 2,200 symbols verified, 690 partial, zero differing, zero unnamed by config. Intact-TU gates passed with zero new symbol errors and exact storage aliases.
- Explicit function/consumer relocation checks: `python tools/prepush_linkcheck.py --files src/game/actors/d_a_sanbo.cpp`; 36/36 functions VERIFIED, one BLIND-3 initializer warning, zero blockers.
- Complete emitted TU and data/metadata checks: `python tools/tubuild.py verify ov096/daSanbo_c --no-write`; 36/36 MATCH, object isolation clean, relocation destinations clean, expected emission order. The checker reports the separately recorded initializer as an extra `.text` symbol; `rombuild` and prepush checks independently cover the configured initializer and its relocations.
- Shared-header consumer expansion: no shared header changed. `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements and no new local redeclarations of header-declared symbols.
- Port/path/reference and other applicable static gates: `python tools/source_style.py --base origin/main --files src/game/actors/d_a_sanbo.cpp` reports zero findings; `python tools/check_dead_references.py` reports no new dead references or broken links; `git diff --check` passes. No path or symbol rename was made.
- Private validation, if run, and the exact PR head/base it tested: not run; this commit is intended for draft review and hosted validation.
