# Handoff: koopa-calclib-distance-type-1104

## Identity and resumption

- Scope: `src/game/actors/d_a_nknk.cpp`, Koopa (`daNknk_c`) in ov062, combined TU `Koopa+KoopaSmall`.
- Source branch: `nknk-calclib1104` in the audit namespace, based on current `origin/main` `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Open-PR and queue check: no open PR changes this source; PR #4106 is a broad data recompose and its file list does not include it. No active queue reservation for `daNknk_c` was found.
- Status: byte-verified candidate; independent source review and hosted PR checks remain outstanding.
- Next action: review the shared helper call boundaries and the 11 measured `defined_symbol_renames` entries on the exact committed head; then continue auditing remaining raw vector views in the class.

## What changed and why

The local `Vec3_Dist(void *, void *)` declaration disagreed with `math/CalcLib.h`, which declares `int Vec3_Dist(const Vector3 *, const Vector3 *)`. The TU now includes that shared declaration and removes the conflicting local declaration and its obsolete local-extern warning. Four calls over actor positions use the existing `dActor_c::mPos` union overlays. The separate home-position scalar triples keep their explicit `Vector3` views because `Koopa.h` declares them as three scalar fields, without an overlay.

Removing the two named parameters from the private `Vec3_Dist` declaration shifts this TU's compiler-local `@NNN` spellings down by two. Baseline and candidate objects were compiled separately. All 11 static-object definitions pair at equal ELF section index and size; the manifest `from` names were updated while every existing canonical `to` target stayed fixed. That correction restores the production rename policy and the `__sinit` relocation destination.

## Proof on current main base

- `python tools/tubuild.py verify ov062/Koopa+KoopaSmall --no-write --verdict`: 39/39 functions match; object isolation, relocation destinations and ROM emission order are clean. It exits 1 only because `__sinit_d_a_nknk.cpp` (0x1c8 bytes) is an unlicensed `.text` symbol, so promotion remains refused by policy.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_nknk.cpp`: 40 checked, 39 verified, one `__sinit` BLIND-13 warning, zero blockers.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements or header-declared local redeclarations.
- `python tools/source_style.py --base origin/main --files src/game/actors/d_a_nknk.cpp`: zero findings, zero formatting deferrals.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

These checks were run on the candidate based on the SHA above. Re-run the byte and relocation gates on the committed PR head before accepting it. The remaining `__sinit` policy caveat is separate from the 39 matched functions.
