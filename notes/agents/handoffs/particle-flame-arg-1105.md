# Handoff: flame-thrower particle wrapper signatures

## Change

`func_02022774` and `func_02022864` are defined in `src/engine/particle/Particle.cpp` with `(Fix12i x, Fix12i y, Fix12i z, s16 scale, s16 velocity)`. The flame-thrower caller declared the last argument as `int` and called the short parameters `ang1`/`ang2`. Correct both widths and normalize the four sibling wrapper prototypes (`func_020226fc` and `func_020227ec` were already `s16`) to the implementation's `scale`/`velocity` names. The helpers store those arguments into particle callback `scale` and `velocity` fields before calling `Particle::System::New`.

No shared header declares these helper symbols, and no open PR touches either relevant source file.

## Proof on current main `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`

- `python tools/tubuild.py verify ov095/daObjFlamethrower_c --no-write --verdict`: 5/5 functions exact; object isolation, relocation destinations, and ROM order clean; promotable.
- `python tools/rombuild.py -j16 --no-rom`: PASS; all 106 modules exact, 11,395 functions and 470 source-owned data claims exact, zero mismatches.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_obj_flamethrower.cpp`: 5 checked, 4 verified, one class-init BLIND-1 warning, zero blockers.
- Declaration agreement: zero disagreements for `func_02022774` and `func_02022864`, no new branch disagreements or header redeclarations.
- Changed-line source style: zero findings. `check_dead_references.py`: no new dead references or broken Markdown links.

Independent source review remains outstanding.
