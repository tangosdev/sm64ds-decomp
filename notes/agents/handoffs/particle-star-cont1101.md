# Handoff: particle-star-cont1101

This document describes this commit; the queue records its immutable output SHA.

## Identity and status

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/4570; producer session `codex-particle-cont1101` (Codex).
- Branch `cleanup/particle-shadow1101`; base and workflow `ad5a9c03fffe174eaec000ce99ab55a5325c3b9f`; input `90ea056fc49be538fd7d4d0b96f87742221dd3e2`.
- Producer candidate only; independent acceptance, current-base integration and PR are pending. Additional particle shadows remain.
- Predecessor camera and trampoline evidence is recorded in the preceding handoffs and ignored build logs.

## Change

Replace the Power Star TU's local `Particle::System` declaration with `Particle__System.h`. Existing calls are unchanged. In its manifest, re-key 23 compiler-local symbol sources by +52 while retaining the existing destinations. The base/head objects have matching section order, size, type and writable/allocated flags for all 23 paired symbols: nine 12-byte `SHT_NOBITS` BSS registration nodes and fourteen eight-byte writable `SHT_PROGBITS` data records. Allocated bytes match. No original variable names are inferred.

## Producer proof

- `python tools/tubuild.py verify ov002/daStarBase_c+daStar_c --no-write --verdict`: 77/77 match and clean relocation destinations, exit 1 because the tool reports an unlicensed `__sinit_d_a_star.cpp` section. The untouched input has exactly the same warning and result; the manifest is not promoted or declared clean. See `build/particle-star-base.log` and `build/particle-star-tu-fixed.log`.
- `python tools/rombuild.py -j16 --no-rom`: exit 0, 106/106 modules exact, 11,395 source-built functions match, zero differing ROM data. `build/particle-star-rombuild.log`.
- Complete base/head object comparison using `build/particle-object-comparison.py`: star 129 allocated sections and 694 relocations identical after the verified renumbering. The same check for trampoline covers 201 sections and 995 relocations, resolving the scope of the separate prepush tool's `BLIND-17` initializer warning by comparison with its clean base TU. Objects are in `build/particle-star-{base,head}.o` and `build/trampoline-{base,head}.o`.
- Prepush check of the earlier camera+trampoline candidate: 232 checked, 231 verified, one `BLIND-17` warning, zero blocking. Declaration agreement had no new discrepancies. Re-run the gates for this exact final candidate and current base; independent review remains required.
- This change edits no shared header or source paths and makes no asset, source ownership or original naming claim.