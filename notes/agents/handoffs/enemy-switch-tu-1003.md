# Handoff: enemy-switch-tu-1003

This document describes this commit. The user authorized committing and pushing
the combined branch on 2026-10-03, including the prepared bookkeeping updates.

## Identity and resumption

- Producer/integration owner: `codex-enemy-switch-1003` / `codex-tu-reduce-1002`.
- Harness: Codex. Branch: `tu/enemy-switch-1003`.
- Source and workflow input: `ef4a02ce0f5d0e0447e992eeb89f706f11a0c7af`.
- Independent source reviewer: `scene_review`; final immutable review pending.
- Worktree: `C:/tmp/sm64ds-enemy-switch-tu-1003`; private evidence under `build/`.
- Branch push is authorized; no PR, merge or issue message is requested.

## Production change and evidence

Twelve existing functions at [ov002](../../../config/arm9/overlays/ov002/symbols.txt):0x020f15fc..0x020f198c now compile from
[src/game/actors/d_a_e_switch.cpp](../../../src/game/actors/d_a_e_switch.cpp). The twelve per-function files and shadow copy
are retired: eleven fewer src inputs, or thirty-one fewer including the Scene
input commit. The interleaved methods and neighboring actor boundaries support
a combined TU; this is medium-confidence inference, not an original object map.

The cartridge RTTI names `daECreate_c` (string 0x0210b304, typeinfo 0x0210b2f8)
and `daESwitch_c` (string 0x0210b314, typeinfo 0x0210b2ec). Both inherit `dActor_c`.
Their vtable address points are 0x0210b364 and 0x0210b3e8. Historical descriptive
names remain compatibility typedefs; existing imported vtable aliases remain.
Factory/profile identifiers and field names are reconstructed, not recovered.

Both factories now use new, both destructors are inline empty native definitions,
and behavior uses named fields and native *Event/model/collider/lifecycle* calls.
Header order gives the observed destructor group order. The compiler emits all
twelve functions in ascending ROM order. Twelve exact metadata policies preserve
existing data ownership; no metadata range is newly enrolled.

One documented ABI bridge calls the existing raw-word `dCcAc_c::Init` definition.
Native `Fix12<int>` aggregate initialization, explicit `.val` assignment, and an
inline conversion helper each produced 0xc4 bytes instead of the ROM's 0xac.
Their sources are retained in `build/enemy-switch-native-*-failed.cpp`. The retained
bridge agrees with the current definition and eliminates raw object offsets.
The spawn call still views the base class's scalar coordinate fields as vectors;
[func_ov102_0214ad14](../../../src/actors/daBmb_c.cpp)(ROM Ordinal 5. see [daBmb_c.json](../../../config/tu_manifest.d/ov102/daBmb_c.json) remains unresolved. Reconstruction is therefore partial.

## Verification and limits

- Production rombuild.py -j16: exit 0, 106/106 modules exact, 11255/11255 source
  functions match, 32/32 source-owned data claims, no new dsd symbol errors.
  Final 16777216-byte ROM SHA256:
  `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
  Log: `build/enemy-switch-rombuild.log`. Stock control still has known symbol
  errors; global advisory metadata contains three unrelated differing records.
- tubuild verify: 12/12 MATCH, isolation and relocation destinations clean,
  complete output accounting and ascending emission order.
- Strict isolated link proof: 12/12 VERIFIED, zero blind/differing words.
- Independent full metadata comparison: 12 records / 376 bytes exact, including
  string tails and both complete vtables with preambles; no blind relocations.
  Executable check and evidence: `build/check_enemy_switch.py` and `enemy-switch-proof.json`.
- Port references: 408 resolve. Changed-scope declaration agreement passes with
  no new disagreements. No new Enemy declaration-baseline exception is needed.
- Enemy committed-range relocation: 12/12 VERIFIED, zero warnings/blockers.
- Scene affected-source sweep: 1824 VERIFIED, one BLIND-2 warning independently
  reproduced on unchanged base, zero blocking failures.
- Combined independent source review accepted partial reconstruction on
  `cb629b96be16ee228e8d555801ee29792943b31c` against original base
  `5fa32e37cbe517c15b01cd902cb15fb5d0ab8950`.
- Final bookkeeping-only commit receives fresh tracked gates and delta review.
- ES-SR-01: fixed stale paths/class identity in [notes/actor-leaf-provenance.md](../../../notes/actor-leaf-provenance.md).

## Bookkeeping integration and publication

The user instruction "commit and push lets continue" authorized applying the
prepared isolated bookkeeping changes and publishing this branch. Other tasks'
claims and worktrees are untouched.

- [attribution.json](../../../attribution.json): add twelve exact path#symbol credit overrides. Together with
  the Scene input, all thirty-three consolidated functions retain their credit.
- [config/converted-baseline.json](../../../config/converted-baseline.json): migrate five existing enemy member identities;
  preserve all eighteen previously migrated Scene identities.
- [symbols/actor_renames.tsv](../../../symbols/actor_renames.tsv): correct ten method rows to the cartridge class names,
  preserving each former name in the why column.
- [config/decl-agreement-baseline.json](../../../config/decl-agreement-baseline.json): move six already-banked Scene disagreements
  to the promoted path and remove twenty-one repaired/retired old entries. No
  new disagreement exception is added. The enemy change needs no new exception.

Original-base proposal checks passed: thirty-three credits intact, zero lost or
changed; converted ratchet baseline 3109/current 3464; all ledger rows consistent;
declaration scope 185 files has no new disagreements. These proposals are now
applied to the tracked files. The final exact-commit tracked results and review
are recorded separately under build/ before branch publication. The earlier
Scene handoff's pending permission language describes its historical checkpoint
and is superseded by this authorization and applied migration.

Default production enrollment and both TU sources are unchanged from the full
106/106 build and independent combined source review. Next action: run the final
tracked checks, obtain independent bookkeeping delta review, and push this branch.
No merge or current-main integration acceptance is claimed.
