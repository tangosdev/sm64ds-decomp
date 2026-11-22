# Handoff: data-trampoline2-wave-tables1017

This document describes this commit. The queue records its immutable output SHA.

## Identity and scope

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4058; producer session `codex-trampoline2-wave-tables1017` (Codex).
- Branch: `refs/heads/data/trampoline2-wave-tables1017`; input/base `1b98141e598440a0432317b58678f3c5c088d558` (accepted ALIGNALL tooling, PR #4055). Workflow `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Predecessors: `issue-2497-trampoline2-composition-review-0908` and `asset-readability1014`. PR #3937 at `7268d28db7515fd30509c1f261b532407dd297d1` was a preserved research reference, not transplanted source or accepted proof.
- Verified producer candidate; independent source review, verifier production proof, PR publication and CI remain pending. No merge is authorized. Root is the intended independent verifier.
- Changed surfaces are the Trampoline2 source/manifest, OV006 symbol names and one complete delink range, plus these notes. No shared header, unrelated source, author ledger or tooling changes.

## Native data and ownership

OV006 `.rodata [0x0212f0f0,0x0212f3ac)` is now owned by `src/minigames/d_s_mg_trampoline2.cpp`: 28 native objects contain 670 bytes, separated by 30 bytes of normal linker alignment. Previously this entire 700-byte interval came from a ROM gap object. The 24 native pointer relocations in six wave records resolve to the exact module-qualified ROM targets. The later bytes at/after `0x0212f3ac` are outside this slice.

The local `Trampoline2WaveDef` replaces an integer address plus anonymous short pointers with a count, position pointer, and pointers to angles, item types and motion types. StatePlay consumes 2/3/5 entries according to each record. `TrmpItem_Init` copies positions/normals and stores the last two arguments in its model-size selector and motion selector. `func_0203cc28` rotates vector components 0/1, so these angles describe the XY plane, not yaw. Eleven 12-byte random spawn positions are proven by SpawnWave's selection bound and stride. The brush upload consumes 32 bytes, represented as sixteen native 16-bit palette values; the spinner uses four 16-bit colors.

Twenty-five same-address names are improved, including `sTrmp2ItemKinds` becoming `sTrmp2RandomSpawnPositions`. A complete source/include/config text scan finds no retired names; all configured relocation files were searched for OV006 references to their addresses. The 25 incoming references are the 24 owned wave pointers and one existing SpawnWave reference. No numeric address or relocation destination changes.

The arrays use demonstrated consumed counts, not guessed historical padding elements. Fifteen two-byte gaps follow the pinned linker's `ALIGNALL(4)` semantics with natural ELF alignment and no explicit alignment attributes or invented padding objects. Names, original declared lengths/grouping, and original whole-TU boundaries are not recoverable from a byte match alone.

## Existing code and compiler policies

All 71 previously emitted definitions, including all 43 functions and the 64-byte initializer, retain their payload, size, type, binding, alignment and relocation tuples modulo two exact identities: the semantic random-position rename and one compiler-local destructor-registration node. Giving the wave record a named type shifts the raw node from `@1111` to `@1110`. It is the unique 12-byte local NOBITS object referenced at initializer offset 60 by `R_ARM_ABS32`, addend zero. All five initializer relocation sites otherwise agree. The manifest maps that measured node to the unchanged configured name `@1086`; there is no generic temporary-name masking.

Existing intact-TU policies remain in force. Production still owns 328 data bytes, the complete 152-byte vtable including its preamble, the 64-byte initializer, its four-byte constructor slot and 20 BSS bytes. The full vtable and all 700 new bytes are separately checked against the actual final module and source-provider map.

## Measured gates

Private artifacts are under the producer worktree `build/`; their hashes and reproducible commands are in `notes/experiments/data-trampoline2-wave-tables1017.json` and the queue evidence.

- Fresh pinned `2004/b56` native object comparison: 71 prior definitions / 43 functions preserved; all 28 new objects, complete 700-byte range and 24 relocation destinations match.
- `tubuild compile` succeeds. Standalone `tubuild verify` matches 42/42 declared functions and passes object-isolation/relocation checks, but exits 1 for ten unlicensed raw sections (initializer plus nine inherited RTTI copies), zero policy errors. This refusal is explicit; normal intact preparation supplies the relevant policies.
- `prepush_linkcheck --files`: 42 strict VERIFIED, one initializer BLIND-3 warning, zero blocking. The complete initializer proof is separate and does not relabel that warning.
- Fresh final-config baseline and candidate `tubuild linkcheck`: all six licensed ranges, all 106 modules and full ROM match. Nine global symbol errors are baseline-identical, zero new; the strict global symbol command itself is not green.
- Normal `run-trampoline-provenance.py -j16 --report build/trampoline-production.json`: 44 requested actual source providers pass, 106/106 modules, 11,334/11,334 functions, 339/339 data claims, 245 BSS claims; rebuilt 16 MiB ROM SHA256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
- Actual final ELF/map/module check: all 28 new objects, 700-byte range, thirty alignment bytes, complete 152-byte vtable, initializer and constructor slot reproduce. The provider wrapper binds actual source/config/tools before compilation and the real linker inputs/outputs; missing provenance modules come from PR #4015 tooling commit `b045eaba5fdc50e097a78be7f7ac55e0da308816` and are identified separately.
- Declaration ratchet: no new disagreement or duplicate local extern. Source style: zero findings/deferrals. Port references, rename ledger, recorded tubuild conflicts and committed whitespace pass. No shared-header edit or unrelated manifest change is included; ten routine serialization changes were preserved privately, proven JSON-equal and restored before the production freeze.
- Broad optional ROM-data diagnostics retain an unrelated mismatch and partial/unnamed entries. That report is not a claim of whole-repository data completion.

## Findings and continuation

The experiment retains all seven predecessor findings and their exact historical wording, plus three scoped findings (ten total). `TRAMP2-05` is corrected as a current-state description: five state bodies and scalar/tail fields are already reconstructed at the tested base. Remaining debt is raw `mState[2]`, P2/S2/double-union copies, conditional PMF scaffolding and remaining raw accesses. `TRAMP2-06` retains local Model/SharedFilePtr and other ABI/lifecycle debt. Issue #4058 continues these partial reconstruction items; this table slice neither fixes them nor restarts known matching floors. Asset naming limitations remain deferred.

Next: independently claim the immutable verifier offer, review source and all ten findings, fresh-compile the object, run an own final-config control and normal production/provider proof, then publish only the accepted exact candidate. Preserve matching authorship and the private probe/control/failed-or-warning receipts.
