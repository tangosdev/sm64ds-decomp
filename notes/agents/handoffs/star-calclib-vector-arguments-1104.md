# Handoff: star-calclib-vector-arguments-1104

## Identity and resumption

- Scope: `src/game/actors/d_a_star.cpp`, the `daStarBase_c` + `daStar_c` TU in ov002.
- Source branch: `star-horz-dist-type1107` in the audit namespace, based on current `origin/main` `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Related open work: PR #4794 changes star ID typing in `d_s_stage_methods.cpp`, a separate source file and symbol range. Historical queue tasks mentioning `daStar_c` are cancelled; no active reservation was found.
- Status: byte-verified candidate; independent source review and hosted PR checks remain outstanding.
- Next action: review the helper boundaries and the `defined_symbol_renames` table against the exact candidate; then continue auditing the remaining raw vector views in this TU.

## What changed and why

`math/CalcLib.h` declares `Vec3_HorzDist` and `Vec3_Dist` with `const Vector3 *` parameters. This TU had private declarations with a different `Vec3 *` / `void *` surface. The local declarations are removed in favor of the shared declaration. Calls on actor positions now use the existing `dActor_c::mPos` union member, and the already-typed `Vector3 *` path parameter is passed directly. This names the established three-word position view without constructing new vector locals or changing object layout.

The TU has 23 compiler-local `@NNN` aliases in its manifest. Including CalcLib shifted these labels. The candidate updates all 23 aliases by matching their object symbol section indexes and sizes against the licensed aliases; full link and ROM checks below pass with that mapping.

## Proof on current main base

- `python tools/tubuild.py verify ov002/daStarBase_c+daStar_c --no-write --verdict`: 77/77 functions match; object isolation, relocation destinations, and ROM emission order are clean. It exits 1 only because the pre-existing `__sinit_d_a_star.cpp` (0x2d8 bytes) is unlicensed by the TU manifest, so promotion remains refused by policy.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_star.cpp`: 78 checked, 77 verified, one `__sinit` BLIND-11 warning, zero blockers.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements or local redeclarations of header-declared symbols.
- `python tools/source_style.py --base origin/main --files src/game/actors/d_a_star.cpp`: zero findings, zero formatting deferrals.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

These checks were run against the uncommitted candidate based on the SHA above. Re-run the byte and relocation gates on the committed PR head before accepting it. Do not treat the clean full ROM result as licensing the separate `__sinit` contribution.
