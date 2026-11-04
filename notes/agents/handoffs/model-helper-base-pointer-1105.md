# Handoff: type `func_020167a4` callers as the animation-model base

## Change and remaining caller

`func_020167a4` is defined in `src/engine/model/d_ext_model.cpp` with a `dExtAnmModel_c *`; its body reads `currFrame`, `data`, and `file` and calls `UpdateBones`. Type the four non-overlapping callers in Moray, Mario Cap, Trs, and `d_ent_obj` to that base pointer. The two derived-model callers (`dExtBlendAnmModel_c` in Moray and `d_ent_obj`) convert to the inherited base at offset zero; the other two already hold `dExtAnmModel_c` directly.

The fifth caller is in `src/game/player/d_a_ply_methods.cpp`, which is part of open PR #4799. It remains `void *` in this branch to avoid overlapping that PR; revisit after #4799 lands. No open PR overlaps the four files changed here.

## Proof on current main `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`

- Focused TU verification, all byte exact with clean object isolation, relocation destinations, and ROM emission order:
  - `ov002/daObjMarioCap_c`: 31/31; existing unlicensed `__sinit_d_a_obj_mario_cap.cpp`.
  - `ov063/daTBasket_c+daTrsIcon_c+daTrs_c`: 97/97; existing unlicensed `__sinit_d_a_trs.cpp`.
  - `ov075/UnknownVsPlayer+dEntObj_c`: 43/43; existing unlicensed `__sinit_d_ent_obj.cpp`.
  - `ov016/daMoray_c`: 21/21; existing unlicensed `__sinit_d_a_moray.cpp`.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395 source-built functions and 470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --symbol func_020167a4`: definition and all four updated declarations agree; the player call remains `void *` under #4799. `--changed a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`: no new disagreements or redeclarations.
- Changed-line source style: zero findings. `check_dead_references.py`: no new dead references or broken Markdown links.
- `python tools/prepush_linkcheck.py --range a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7..HEAD`: 196 checked, 191 verified, five BLIND warnings, zero blocking.

Independent source review remains outstanding. The focused verifier's nonzero exit on each TU is solely its policy refusal for the existing unlicensed `__sinit` symbol; none reported a byte or relocation mismatch.
