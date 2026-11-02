# Handoff: particle-system-camera-shadow1101

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570; producer session `codex-particle-shadow1101` (Codex).
- Branch `cleanup/particle-shadow1101`; input, base and workflow commit `5fea26a75f26021b33f63662fdb8667fd2d2833c`.
- Next: independent verifier to review source and check bytes, relocation targets and data on this exact candidate and its tested base; then integrator to recheck the current base. Remaining particle declarations need their own scoped work.
- Status: producer candidate; independent acceptance and integration pending.
- Logs and queue receipts live in this worktree's ignored `build/` directory.

## Change and evidence

`src/engine/view/d_camera.cpp` declared a partial `Particle::System` solely for `NewWeather`. The shared `include/Particle__System.h` declares the same method and types; include it and delete the local class and now-unneeded forward declaration. Both call sites already use that signature. This does not reconstruct or change the particle object layout. No new field names, lifecycle claims, symbol changes, or file moves are introduced. The source scope is arm9 camera TU 0x020072c4..0x0200e494.

The rest of the particle shadow candidates are not addressed. Many use integer stand-ins for `Fix12i`; changing those calls needs separate ABI and emitted-code proof. The source and shared header were reserved in the v2 queue before the edit.

## Producer proof

- `python tools/rombuild.py -j16 --no-rom`: exit 0, 106/106 exact, 11,395 source-built functions match, ROM data 0 differ. `build/particle-camera-rombuild.log`.
- `python tools/tubuild.py verify arm9/dCamera_c --no-write --verdict`: exit 1; 142/142 functions match, relocation destinations and objisolate clean. Promotion refused because `__sinit_d_camera.cpp` is reported as an unlicensed section. Untouched input has the identical result; no manifest was changed. This is not a clean whole-TU promotion verdict.
- `python tools/check_decl_agreement.py --changed origin/main`: exit 0, no new disagreements or local redeclarations.
- `git diff --check`: exit 0.
- Relocation-aware prepush result and independently reviewed source acceptance are recorded separately after this commit. Private CI has not been run.
- No header definition, path, or port reference was changed. No ownership, attribution, or asset payload was moved.