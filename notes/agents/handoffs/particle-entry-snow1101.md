# Handoff: particle-entry-snow1101

This document describes this commit; the queue records its immutable output SHA.

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/4570; producer `codex-particle-cont1101` (Codex).
- Branch `cleanup/particle-shadow1101`; base/workflow `ad5a9c03fffe174eaec000ce99ab55a5325c3b9f`; input `2a6794d6220f7f475771ef6db224257895802e80`.
- Producer candidate; independent source acceptance, current-base integration, and PR are pending. Earlier changes and limitations are recorded in preceding handoffs.

Use `Particle__System.h` instead of local partial particle declarations in `d_ent_obj.cpp` and `d_a_snm_bth.cpp`. Existing call sites are unchanged. Re-key existing manifest rename sources to compiler-emitted names, leaving destinations untouched. Remove comments claiming the removed local declarations remain.

The complete base/head object comparison (`build/particle-actor-object-comparison.py`) passes: entry scene 125 allocated sections, 475 relocations and 38 local symbols; snowman 35 allocated sections, 140 relocations and one local symbol. The check includes section type, flags, alignment and bytes and pairs renumbered local symbols by observed layout.

`tubuild.py verify --no-write --verdict` matches 43/43 entry-scene functions and 17/17 snowman functions with clean relocation destinations after re-keying. Both checks exit 1 for the same single unlicensed initializer section reported on the unchanged input. Logs and base/head objects are in `build/particle-d_ent_obj-*` and `build/particle-d_a_snm_bth-*`.

`python tools/rombuild.py -j16 --no-rom`: exit 0 with all nine replacements together, 106/106 modules exact, 11,395 source-built functions matched and ROM data 0 differ. Log `build/particle-entry-snow-rombuild.log`. The exact final composition still needs range-scoped linkcheck, declaration agreement, current-base revalidation and independent source review. The remaining particle candidates require their own work and coordination.