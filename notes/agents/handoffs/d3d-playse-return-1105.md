# Handoff: type Trampoline `D3D_PlaySeAtX` declaration

## Change and evidence

In `src/minigames/unit020cd744.cpp`, the namespace-local `D3D_PlaySeAtX` declaration said `void`, while `src/minigames/d_s_mg_d3d_base.cpp` defines it as `int` and `include/decl_common.h` declares `int`. The five local calls ignore the return value. Correct the local declaration to `int` and mark why it remains local.

The TU already includes `decl_common.h`. Removing the namespace-local declaration and relying on the header changes the compiler-generated local symbol/relocation mapping: `tubuild` then reports wrong destinations in four functions and misses `__arraydtor$2193`. Retaining the namespace-local declaration with the corrected return type preserves the retail TU byte and relocation map. Do not remove it or rewrite its calls without re-verifying the full TU.

## Proof on current main `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`

- `python tools/tubuild.py verify ov006/unit020cd744 --no-write --verdict`: 59/59 functions match; object isolation, relocation destinations, and ROM emission order are clean. It exits 1 only because `__sinit_unit020cd744.cpp` (0xcc bytes) is not licensed in the TU manifest; promotion remains refused by policy.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395 functions and 470 source-owned data claims exact, zero mismatches.
- `python tools/prepush_linkcheck.py --files src/minigames/unit020cd744.cpp`: 60 checked, 59 verified, one `__sinit` BLIND-4 warning, zero blocking.
- `python tools/check_decl_agreement.py --symbol D3D_PlaySeAtX`: zero disagreements; `--changed a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`: no new disagreements or redeclarations.
- `python tools/source_style.py --base a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`: zero findings. `check_dead_references.py`: no new dead references or broken Markdown links.

No open PR currently changes `src/minigames/unit020cd744.cpp`. Independent source review remains outstanding.
