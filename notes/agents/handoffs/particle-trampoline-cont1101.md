# Handoff: particle-trampoline-cont1101

This document describes this commit; the queue records the immutable output SHA.

## Identity and resumption

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/4570; producer session `codex-particle-cont1101` (Codex).
- Branch `cleanup/particle-shadow1101`; base and workflow commit `ad5a9c03fffe174eaec000ce99ab55a5325c3b9f`; input `a020b58c502e8211633948a17867ba0f82618cf8`.
- Next: independent source and byte review of both camera and trampoline changes, then current-base integration. Remaining particle candidates are separate work.
- Status: producer candidate. No independent acceptance, PR, or integration yet. Logs and private receipts remain in ignored `build/`.

## Change

Replace the trampoline TU's local `Particle::System` declaration with `Particle__System.h`. Its `New` parameters resolve to the same scalar types, including `Fix12i`. All eight calls remain unchanged. The input also carries the camera replacement described in `particle-system-camera-shadow1101.md`.

The included header shifts 26 compiler-local initializer registration symbols by 56: base `@2141`..`@2166` become `@2197`..`@2222`. Comparing unmodified base and candidate objects shows the same 26 symbols in the same order, each in `.bss`, `SHT_NOBITS`, writable/allocated flags 3, and size 12. Map the first 14 to their former names and re-key the existing last 12 mappings, retaining all configured destination names. This is compiler bookkeeping, not a claim about original identifiers or data layout.

## Producer proof

- On the candidate before remapping, `tubuild verify` matched 88/89, but the initializer had 14 wrong relocation destinations and 26 unlicensed symbols. The unchanged input passed 89/89, demonstrating the regression.
- `python tools/tubuild.py verify ov006/dMgTrmpln2Mario_c+dMgTrmpln3DMario_c --no-write --verdict`: exit 0 after remapping; 89/89 match, objisolate and relocation destinations clean, zero unlicensed and policy failures. Logs: `build/trampoline-base-tu.log`, `build/particle-trampoline-tu.log`, `build/particle-trampoline-tu-fixed.log`. Compiled objects: `build/trampoline-base.o` and `build/trampoline-head.o`.
- `python tools/rombuild.py -j16 --no-rom`: exit 0, 106/106 modules exact, 11,395 source functions matched and ROM data 0 differ. Log `build/particle-trampoline-rombuild.log`.
- Camera TU verification and base control: 142/142 functions and relocations match; both input and candidate report the same pre-existing unlicensed initializer section. No claim of a clean promotion verdict for camera.
- Prepush relocation check, declaration agreement, and independent review of this exact candidate remain to be recorded separately. No shared header was edited; no paths or ownership changed.