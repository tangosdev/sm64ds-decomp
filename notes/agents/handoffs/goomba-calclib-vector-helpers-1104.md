# Handoff: goomba-calclib-vector-helpers-1104

## Identity and resumption

- Scope: `src/game/actors/d_a_krb.cpp`, the Goomba class in ov084.
- Source branch: `krb-calclib1104` in the audit namespace, based on current `origin/main` `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Open-PR and queue check: PR #4106 does not change this source file; no active `daKrb_c` reservation was found.
- Status: byte-verified candidate; independent source review and hosted PR checks remain outstanding.
- Next action: review the shared declarations, actor-position overlays and alias pairing on the exact committed head; continue the remaining vector-view audit afterward.

## What changed and why

This TU had local declarations for CalcLib helpers with widened `void *` parameters, named parameters that shifted compiler-local names, and return types copied from local aliases. It now includes `math/CalcLib.h` and uses its declarations for `Vec3_Dist`, `Vec3_HorzAngle`, `Vec3_HorzLen`, `Vec3_HorzDist`, `RandomIntInternal`, `ApproachAngle`, `DecIfAbove0_Byte` and `DecIfAbove0_Short`. Four horizontal-distance calls now pass `dActor_c::mPos`. The signed 16-bit `mWanderRerollTimer` storage is viewed as `unsigned short *` at the decrement-helper boundary, matching the shared prototype without changing its bits.

Removing the named parameters from those local prototypes reduces the TU's compiler-local numbering by 15. Baseline and candidate objects were compiled separately. The ten mapped local data objects pair at equal ELF section index and size (nine 12-byte BSS objects and one 8-byte data object); the existing canonical `to` targets remain unchanged. The mapping also restores the initializer's correct relocation destination.

## Proof on current main base

- Focused `tubuild.py verify` for the Koopa TU in overlay 084: 35/35 functions match; object isolation, relocation destinations and ROM emission order are clean. It exits 1 only because `__sinit_d_a_krb.cpp` (0x1fc bytes) is not licensed by the TU manifest, so promotion remains refused by policy.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_krb.cpp`: 36 checked, 35 verified, one `__sinit` BLIND-11 warning, zero blockers.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements or header-declared local redeclarations.
- The prior producer check recorded zero style findings. A fresh local recheck cannot run in this environment because clang-format 19.1.7 is unavailable; confirm the hosted source-style result on the PR.
- `python tools/port_refcheck.py`: 217/217 references resolve.
- `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

These checks were run on the candidate based on the SHA above. Re-run the byte and relocation gates on the committed PR head before accepting it. The `__sinit` policy caveat is outside the 35 licensed function matches.
