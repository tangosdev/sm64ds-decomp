# Rename Candidates

Read-only research notes for possible symbol renames. These are not verified symbol
imports and should not be applied directly to `config/**/symbols.txt` without call-site
review and a normal matching verification pass.

Confidence:
- High: behavior is clear, neighbors agree, and the candidate is probably stable.
- Medium: subsystem and behavior are clear, but exact wording may change.
- Low: useful lead only; needs call-site or disassembly confirmation.

## Strong Candidates

| Current | Candidate | Confidence | Evidence | Blockers |
|---|---|---:|---|---|
| `func_02010da4` | `Actor::ClampPos` | High | Clamps actor position fields at `+0x5c`, `+0x60`, and `+0x64` to `+-0x7d00000`; adjacent to `Actor::UpdatePosWithOnlySpeed` and `Actor::SetRanges`. | Confirm all call sites use an `Actor *`. |
| `func_02013944` | `SaveData::SetGlowingRabbitFoundFlag` | High | Calls `SaveData::NumGlowingRabbitsFound`, sets a bit in save flags, and sets another flag after at least 7 glowing rabbits. | Confirm exact game-facing wording for the final unlock flag. |
| `func_020373b8` | `WithMeshClsn::~WithMeshClsn` deleting destructor | High | Destructor shape: writes `_ZTV12WithMeshClsn`, destroys `RaycastLine` and `SphereClsn` members, calls base dtor, then `Memory::operator_delete2`. | Confirm whether this should be `D0`, `D1`, or `D2` in the symbol table. |
| `func_02017c24` | `SharedFilePtr::Unload` | Medium-high | Saves `self->fileID` to `LAST_USED_FILE_ID`, deallocates `self->data`, and clears the data pointer; adjacent to `SharedFilePtr::LoadFile` / `SharedFilePtr::Load`. | Confirm naming convention used by reference symbols for release/unload/free. |
| `func_02039218` | `Stage::ClearMeshColliderSlots` | Medium-high | Zeroes 0x18 entries in `data_020a0c80`; adjacent to `Stage::ResetMeshColliders` and mesh-collider update code. | Identify `data_020a0c80` type/name. |
| `func_020391f0` | `MeshColliderBase::Disable` or empty virtual hook | Medium | Empty function between `MeshColliderBase::Enable` and `Stage::ResetMeshColliders`; likely a no-op virtual or paired collision hook. | Needs vtable slot confirmation. |
| `func_02037464` | `RaycastGround::Reset` | Medium | Calls an initializer on `this+0x10`, writes `0x80000000` at `+0x44`, clears byte at `+0x48`; adjacent to `RaycastGround::SetObjAndPos`. | Confirm field meanings in `RaycastGround`. |
| `func_0202deac` | `LoadLevelOverlaysAndMarkLoaded` | Medium | Calls `LoadLevelOverlays(levelID)` and sets global `g0209f278 = 1`; adjacent to `UnloadLevelOverlays` / `LoadLevelOverlays`. | Find callers to identify exact semantic flag. |
| `func_02013c84` | `SaveData::CopyAndMaybeSaveFile` | Medium | Copies a `FileSaveData`; if `fileIndex < 0`, stores `charID` in `SAVE_DATA.unk328`, otherwise calls `SaveData::SaveFile`. | Exact role of `SAVE_DATA.unk328` unknown. |
| `func_02010844` | `Actor::CalcVertAngleToVecRelativeToAng` | Medium | Uses `atan2`, horizontal length, sine table, and an input angle to compute another angle; adjacent to actor coin/player search helpers. | Needs call-site naming and parameter semantics. |
| `func_02010e78` | `Actor::SetSpawnContext` | Medium | Stores several arguments into actor-spawn globals and clears one global; adjacent to `Actor::Spawn` and `Actor::Next`. | Confirm globals are spawn context rather than iteration context. |
| `func_02012120` | `Sound::PlayCharVoiceWithOffset` | Medium | Adds `data_02075250[p1]` to a sound ID and calls `Sound::PlayLong(..., 1, adjustedID, pos, p4)`; adjacent to `Sound::LoadGroupAndSetBank` and `Sound::PlayCharVoice`. | Confirm table meaning and parameter names. |
| `func_02012860` | `Sound::SetMusicParamsByFlags` | Medium | Applies three sound-player parameter calls based on flag bits; called by `func_020127ec`; adjacent to `Sound::Play2D` and music layer control. | Need names for callees `func_0204f82c`, `func_0204f7fc`, `func_0204f7cc`. |
| `func_02011f7c` | `Sound::SetPlayerVoiceGroup` | Medium | Between `Sound::ResetPlayerVoiceGroup` and `Sound::UnsetPlayerVoiceGroup`; copies/loads sound info and records `self` in a voice-group global. | Needs sound-info type names. |
| `func_02043810` | `ActorBase::GetHeapSizeOrNodeID` | Low-medium | Returns a field from the object at `this+0x14`; adjacent to ActorBase virtual allocation/destruction flow. | Exact field and virtual purpose unknown. |

## Good Next Checks

- For high-confidence candidates, inspect all callers and verify the proposed receiver type.
- For destructor candidates, inspect vtables and nearby `D0`/`D1`/`D2` entries before choosing the mangled symbol.
- For sound candidates, identify the low-level callees around `0x0204f7cc` to `0x0204f82c`.
- For actor candidates, resolve globals near `data_0209b44c` through `data_0209b460`.

## Player State Leads

These are lower-confidence because Player state names depend heavily on the state table
and animation IDs. They are still useful breadcrumbs because most sit between already
named `Player::St_*` methods.

| Current | Candidate | Confidence | Evidence | Blockers |
|---|---|---:|---|---|
| `func_ov002_020ca78c` | `Player::TryEnterNoControlState10` | Medium | Checks current state against `data_ov002_0211022c`, validates fields `+0x6e3` and `+0x70b`, then calls `Player::SetNoControlState(0x10, -1, 1)`; adjacent to `Player::TryExitWhiteDoorWithStar` and `Player::St_DebugFly_Main`. | Decode no-control state ID `0x10` and state table name. |
| `func_ov002_020cac60` | `Player::TryEnterFinalBowserDoor` | Medium | If current level/area field is `0xd`, `NumStars() >= 0xe`, and camera angle checks pass, calls `LoadLevel(0x1f, 0, 1, 0, 0xe)`, stops color fader, and plays sound `0x1e`; adjacent to camera-zoom state methods. | Confirm destination level `0x1f` and star threshold context. |
| `func_ov002_020cae10` | `Player::St_CameraZoom_Init` helper | Medium | Computes camera-relative angle, clamps to `[-0x2aaa, 0x2aaa]`, stores fields `+0x764/+0x766`, and sets mode byte `+0x742 = 2`; adjacent to `Player::St_CameraZoom_Main` and `Player::St_CameraZoom_Init`. | Determine whether this is the init body or a helper used by it. |
| `func_ov002_020c56f0` | `Player::EnterDeadPitOrBowserEarthquakeState` | Low-medium | Requires `func_ov002_020d82f0`, stores arg at `+0x6e3`, then changes to one of two Player state objects depending on `func_ov002_020d91e0(..., 0x300, 1)`; adjacent to `Player::St_BowserEarthquake_Init` and `Player::St_DeadPit_Main`. | Need state-object names for `data_ov002_0211010c` and `data_ov002_021103c4`. |
| `func_ov002_020cd190` | `Player::MaybeApplyMetalWaterGroundVelocity` | Low-medium | If flag `+0x703` is set and vertical position is close enough to `data_0209f32c`, calls `func_ov002_020ce8bc(self, self->field_0x98)`; adjacent to `Player::St_MetalWaterGround_Init` and `Player::St_Swim_Cleanup`. | Need meaning of `func_ov002_020ce8bc` and field `+0x98`. |
| `func_ov002_020d3498` | `Player::TurnAround` | Medium | Negates field `+0x98`, adds `0x8000` to facing angle `+0x8e`, and copies it to `+0x94`; adjacent to `Player::St_Wait_Init` and `Player::St_TurnAround_Main`. | Confirm whether `+0x98` is forward speed or horizontal speed. |
| `func_ov002_020e0a64` | `Player::GetLandActionAfterAnim` | Low-medium | If not animation `0x54`, stores `8` at `+0x6a8`; returns `0x9b` if `+0x703` is set, otherwise table lookup by character/player field `+0x6e1`; adjacent to `Player::St_Land_Init` and `Player::St_SmallLaunchUp_Main`. | Need animation/state IDs and table `data_ov002_0210a5b4`. |
