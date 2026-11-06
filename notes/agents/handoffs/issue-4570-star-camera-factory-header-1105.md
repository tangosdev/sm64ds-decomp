# Handoff: issue-4570-star-camera-factory-header-1105

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570; task `issue-4570-star-camera-factory-header-1105`; producer session `codex-starcam-actor-header-1105` (Codex).
- Queue input and tested base: `682321c64de078383e3c66cea08b0a3cd451a5ef`; workflow/tool pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- This commit contains `StarCamera_Spawn.cpp` and this handoff. Build logs are local under ignored `build/`.
- Next action: independent verifier reviews the base-constructor use and reruns exact TU, relocation and full-ROM proof on this commit; independent source acceptance and integration remain pending.
- Status: verified candidate. The concrete most-derived actor type remains unresolved.
- No source edits remain uncommitted; private build outputs stay in `build/`.

## What changed and why

- Scope: the `StarCamera_Spawn` factory in `src/game/actors/StarCamera_Spawn.cpp`, ov002 range `0x020ebe5c..0x020ebe8c`.
- Replace the local `struct dActor_c { dActor_c(); }` with `include/dActor_c.h`. The factory placement-constructs the shared base class after allocating `0xd4` bytes; the shared class size is `0xd0`.
- Keep the allocation size and return behavior. No most-derived vptr or identifying RTTI is evidenced, so `StarCamera` remains a profile-derived label and no concrete derived class is claimed.
- No header, config, symbol, or attribution rows changed.

## Reconstruction dimensions

- Exact function coverage: `StarCamera_Spawn`, 1/1; relocation destinations verified.
- This TU remains a one-function factory rather than a claimed class translation unit.
- Only the known `dActor_c` base constructor is named; the allocated object's derived identity remains unknown.
- Lifecycle, vtable and RTTI ownership are unchanged; the factory still returns the pointer produced by placement construction.
- Remaining #4570 shadow and padding work is not covered by this task.

## Proof

- `python tools/tu_object_diff.py --base origin/main --numbered src/game/actors/StarCamera_Spawn.cpp` — exit 0, `IDENTICAL`, object digest `5982f55b87f829b0`; no new compiler-numbered symbols.
- `python tools/tubuild.py verify ov002/StarCamera --no-write --verdict` — exit 0; 1/1 MATCH, object isolation clean, relocation destinations clean, expected emission order, promotable.
- `python tools/prepush_linkcheck.py --files src/game/actors/StarCamera_Spawn.cpp` — exit 0; 1 checked, 1 verified, 0 warnings, 0 blockers.
- `python tools/rombuild.py -j16 --no-rom` — exit 0; 11,395 functions, 0 mismatches; 106/106 modules exact; 470/470 source-owned data claims reproduce; 302 BSS claims; 0 differing ROM data symbols.
- `python tools/check_decl_agreement.py --changed origin/main` — no new disagreements or local header redeclarations.
- `python tools/port_refcheck.py` — 217 references resolve. `git diff --check` — pass.
- Private PR validation and independent source review: pending.
