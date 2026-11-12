# 64-bit size locks

Size locks that assume a 4-byte pointer are skipped on `SM64DS_HOST_64`.
An 8-byte file handle keeps its pointer as `T * DS_P32`, so the size assert stays.

Wrapped asserts: 53. `DS_P32` handles: 3.

## Wrapped

Each lock below is inside `#ifndef SM64DS_HOST_64`.

- `include/MgBanner.h`: `ElemFn` (pointer-to-member), `MgBanner`
- `include/WmWork.h`: `WmWork`
- `include/nitro/wm/wm.h`: `WMParentParam`, `WMScanParam`, `WMStatus`
- `include/nnsys/snd/player.h`: `NNSSndSeqPlayer`
- `include/private/arm9_a1fc0_file_share.h`: `FileShareNode`, `FileShareParentArea`, `FileShareChildArea`, `FileShareWork`
- `include/private/arm9_archive_table.h`: `ArchiveTableEntry`
- `include/private/fs_views.h`: `FSArchive`, `FSFindPathInfo`, `FSFile`
- `include/daObjYajirusi_c.h`: `daObjYajirusi_c` sizeof, plus the ten `offsetof` locks on the same host view (`actorID`, `mPosX`, `mCamSpacePosX`, `mAngleY`, `mModel`, `mMeshCollider`, `mClsnMat`, `mShadowModel`, `mShadowMat`, `mVariant`)
- `src/engine/fs/FS_CommandDefault.cpp`: `FSDirEntry`
- `src/engine/link/unit0203d7b8.cpp`: `FileShareChildArea::mRecvWork` offset (pointer tables sit in front of it)
- `src/game/actors/d_a_obj_ct_kaitendai.cpp`: `KaitendaiResources`
- `src/game/actors/d_a_obj_ewb_ice.cpp`: `EwbIceFileSlot`, `EwbIceClpsSlot`, `daObjEwbIce_State`
- `src/game/actors/d_a_obj_kurumajiku.cpp`: `daObjKurumajiku_c::Resources`
- `src/game/actors/d_a_obj_rc_carpet.cpp`: `FlyingCarpetResources`
- `src/game/actors/d_a_obj_simple_bg.cpp`: `SimpleBgAssets`
- `src/game/actors/d_a_owl.cpp`: `OwlState`
- `src/game/actors/d_a_snowman.cpp`: `daSnowman_c::State`
- `src/minigames/d_s_mg_roulette_methods.cpp`: `RouletteStrip`
- `src/ui/menus/unit020b2cdc.cpp`: `Ov007Cursor`
- `src/ui/menus/unit020be0dc.cpp`: `TrackingRecord`, `Ov007ParticleSprites`
- `src/ui/menus/unit020bed60.cpp`: `Ov007MeshScene`
- `src/ui/menus/unit020bf690.cpp`: `Ov007Fade`
- `src/unnamed/arm9/0201/func_0201a96c.cpp`: `WatchdogAlarm`
- `src/unnamed/arm9/0204/func_0204dab4.cpp`: `ParticleElement`, `TrackingRecord`
- `src/unnamed/ov007/func_ov007_020ade58.c`: `Ov007NodeSprites`
- `src/unnamed/ov007/func_ov007_020ae070.c`: `Ov007Work::selectedSlot`
- `src/unnamed/ov007/func_ov007_020ae2d0.cpp`: `Ov007Work::itemViews`
- `src/unnamed/ov007/func_ov007_020aefb4.cpp`: `Ov007Work::itemNodes`
- `src/unnamed/ov007/func_ov007_020b0358.c`: `Ov007Work::transitionMode`
- `src/unnamed/ov007/func_ov007_020b58e4.c`: `Ov007Work::transitionMode`
- `src/unnamed/ov007/func_ov007_020bcdb8.cpp`: `Ov007Work::display`

`Ov007Work`'s own sizeof lock was already under `#ifndef SM64DS_PLATFORM_PC`. The offset locks above were not, and pointers sit in front of those fields.

## DS_P32 handles

The slot is `u16` file id, `u8` refcount, padding, one file pointer. The size assert is unchanged.

- `include/SharedFilePtr.h`: `SharedFilePtr::filePtr`
- `src/game/actors/d_a_kuri_king.cpp`: `KuriKingAnimFile::filePtr` (same shape, no size assert of its own)
- `src/runtime/actor/d_path_lift_actor.cpp`: `PathStuff::filePtr` (same shape, no size assert of its own)

`SharedFilePtr` subclasses do not redeclare the pointer. Their size-8 asserts stay, because the base slot stays 4 bytes: `DonketuModelFilePtr`, `DonketuAnimFileHandle`, `DkkModelFilePtr`, `DkkClsnFileHandler`, `MkyModelFilePtr`, `MkyAnimFilePtr`, `FallBlockModelFilePtr`, `FallBlockClsnFileHandle`, `UkisimaModelFilePtr`, `UkisimaClsnFileHandle`, `BSwdoorModelFilePtr`, `BSwdoorClsnFilePtr`, `KaitendaiModelFile`, `KaitendaiCollisionFilePtr`, `KomaUModelFilePtr`, `KomaUClsnFileHandle`, `MarutaModelFilePtr`, `MarutaClsnFileHandle`, `HsBillboardModelFilePtr`, `KiFuneModelFilePtr`, `KiFuneClsnFileHandle`, `Km1UkishimaModelFilePtr`, `Km1UkishimaClsnFilePtr`, `Km2FallModelFilePtr`, `Km2FallClsnFileHandle`, `Km2GuraModelFilePtr`, `Km2GuraClsnFilePtr`, `DorifuModelFilePtr`, `DorifuClsnFileHandle`, `Km3KaitendaiModelFilePtr`, `Km3KaitendaiClsnFilePtr`, `Km3KurumaModelFilePtr`, `Km3KurumaClsnFilePtr`, `Km3KurumajikuModelFilePtr`, `Km3KurumajikuClsnFilePtr`, `RcBurankoModelFilePtr`, `RcBurankoClsnFilePtr`, `RcHaneModelFilePtr`, `TikuwaModelFilePtr`, `TikuwaCollisionFilePtr`, `SimpleBgModelFilePtr`, `SimpleBgClsnFilePtr`, `WakameModelFilePtr`, `WakameAnimationFileHandle`, `WanwanShutterModelFilePtr`, `WanwanShutterClsnFilePtr`, `OwlModelFilePtr`, `daOwlAnimationFilePtr`, `PgBbyModelFile`, `PgBbyAnimationFile`, `daSnowmanModelFilePtr`, `daSnowmanAnimationFilePtr`, `Jump3DMarioAnimFilePtr`, `Jump3DModelFilePtr`, `Trmpln2ModelFilePtr`, `Trmpln2MarioAnimFilePtr`, `KrbAnimationFileHandle`.

`FileShareParentArea::mNodes` stays unwrapped. The bytes in front of it are `unsigned char mReplies[0x10][0xc]`, so the offset is still `0xc0`.

## Left, integers only

No pointer, pointer-to-member, or virtual function. Asserts stay.

- `src/engine/heap/Heap.cpp`: `VecOff`
- `src/game/actors/d_a_krb.cpp`: `SilverStarView` (byte head plus a `u32`)
- `src/game/actors/d_a_mky.cpp`: `MkyOff`
- `src/game/actors/d_a_obj_km1_dorifu.cpp`: `Km1DorifuClpsBlock`
- `src/game/actors/d_a_obj_km3_kaitendai.cpp`: `Km3KaitendaiClpsBlock`
- `src/game/actors/d_a_obj_km3_kuruma.cpp`: `Km3KurumaClpsBlock`
- `src/game/actors/d_a_obj_km3_kurumajiku.cpp`: `Km3KurumajikuClpsBlock`
- `src/game/actors/d_a_owl.cpp`: `Matrix4x3`, `V6`
- `src/game/actors/d_a_sound_obj.cpp`: `SoundObjRow`
- `src/game/actors/d_a_trs_trap.cpp`: `BookSwitchView` (byte head plus a `u32`)
- `src/game/player/d_a_ply_methods.cpp`: `PlayerFileBone`
- `src/game/stages/d_s_entry_entry.cpp`: `BMDMaterialColors`
- `src/minigames/d_mg_jump3d_mario.cpp`: `Jump3DLayoutSlot`
- `src/minigames/d_mg_trmpln2_mario.cpp`: `TrmpGoal`
- `src/minigames/d_s_mg_amida.cpp`: `AmidaPattern`
- `src/minigames/d_s_mg_hanachan_methods.cpp`: `HanaStarPopup`
- `src/minigames/d_s_mg_slots.cpp`: `SlotBetIcon` (icon bytes, not a compiler vptr)
- `src/ui/menus/unit020b797c.cpp`: `Ov007MessageWindow`
- `src/ui/menus/unit020be0dc.cpp`: `GXOamAffine`
- `src/unnamed/ov007/func_ov007_020c9688.c`: `Ov007CellAnimHeader`, `Ov007CellAnimFrame`, `Ov007OamRecord`
- `src/unnamed/ov007/func_ov007_020c99d8.c`: `Ov007CharBlock`
- `include/CaptureEffect.h`: `CaptureEffectWork`
- `include/dScMgTrampoline_c.h`: `dScMgTrampoline_DoorMark`
- `include/MessageBank.h`: `MessageBankHeader`, `MessageInfoSection`, `MessageEntry`, `MessageDataSection`
- `include/MgMenuRow.h`: `MgEntry`
- `include/Ov007MenuState.h`: `Ov007MenuState`
- `include/Ov007Touch.h`: `Ov007Touch`
- `include/SaveBlock.h`: `FileSaveData`, `MinigameSaveData`
- `include/WmWork.h`: `WmStartScanCallback`, `WmParentRecord`
- `include/math/Quaternion.h`: `Quaternion`
- `include/nitro/wm/wm.h`: `WMBssDesc`, `WMMpRecvBuf`, `WMMpRecvData`, `WMMpRecvHeader`
- `include/private/arm9_a1fc0_file_share.h`: `FileShareMsg`
- `include/private/controller_input.h`: `ControllerInput`
- `include/private/gx_state.h`: `GXVRamCnt`
- `include/private/mb_views.h`: `MBUserInfo`
- `include/private/particle_resource_header.h`: `ParticleResourceHeader`
- `include/private/player_input_record.h`: `PlayerInputRecord`
- `include/private/rom_header_view.h`: `RomHeaderView`
- `include/private/wm_views.h`: `WMDataSet`, `WMDataSharingInfo`

Locks already inside `#ifndef SM64DS_PLATFORM_PC` were not wrapped again.
`include/nitro/fs/overlay.h` and `src/engine/snd/SND_Bank.cpp` were not edited.
