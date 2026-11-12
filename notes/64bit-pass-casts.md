# 64-bit pass: address casts

(int)&expr / (s32)&expr became DS_IADDR. (u32)&expr, (unsigned)&expr, and (unsigned int)&expr became DS_UADDR. On the NDS build those macros are the original casts plus the parentheses the macro adds. ds_width.h is included after each file's existing includes. 	ypes.h was not added again: every DS_UADDR file already had u32 from 	ypes.h directly or through common.h / a Nitro header before ds_width.h.

194 sites, 75 files. No function was reverted. Matcher: 	ools/match.py logic, mwccarm 2004/b56, strict reloc destinations. Each TU was compiled once.

## Reverts

None. Two static initializers report a strict-reloc WRONG-DEST with zero instruction words differing, and the unmodified source reports the same destinations, so the cast swap did not cause them:

- __sinit_d_s_mg_memory2.cpp @ 0x021314e4 size 0x3bc (ov006). Bytes match. Three pre-existing wrong destinations at +0x364, +0x368, +0x370 (@1702/@1704/@1706). CardFlyAway itself matches.
- __sinit_KuppaScript.cpp @ 0x02073e6c size 0xf24 (arm9). Bytes match. One pre-existing wrong destination at +0xf20 (@576 0x0212347c != 0x0209b278). KS_PTR is the address split this initializer emits.

## Files and functions matched

Body changes, unless noted. Address and size are the symbol row.

- src/GetActorDebugName.c (1): GetActorDebugName 0x020233d4 0x1c
- src/func_0201a2f8.cpp (3): unc_0201a2f8 0x0201a2f8 0xec
- src/cstd3.cpp (3): unc_0206ccd8 0x0206ccd8 0x6c; cstd::__assert _ZN4cstd8__assertEPKciS1_i 0x0206cd44 0x58
- src/MBi_SetRecvBufferFromChild.c (2): MBi_SetRecvBufferFromChild 0x02065c2c 0x130
- src/Message3.cpp (5): Message::LoadTextVS _ZN7Message10LoadTextVSEv 0x020349e0 0x98
- src/MI_SendGXCommand.c (1, DS_UADDR): MI_SendGXCommand 0x01ffde98 0xa4 (itcm)
- src/KuppaScript.cpp (4, DS_UADDR in KS_PTR): initializer only; see the pre-existing sinit note above. No other function body uses the macro.
- src/unnamed/arm9/0201/func_0201a614.c (2): unc_0201a614 0x0201a614 0x80; unc_0201a694 0x0201a694 0xc0
- src/unnamed/arm9/0201/func_0201a754.c (2): unc_0201a754 0x0201a754 0x44
- src/unnamed/arm9/0201/func_0201a798.c (2): unc_0201a798 0x0201a798 0x44
- src/unnamed/arm9/0201/func_0201aa18.c (1): layout assert only. Include probe unc_0201aa18 0x0201aa18 0x38
- src/unnamed/arm9/0205/func_0205e3d4.c (4): unc_0205e3d4 0x0205e3d4 0x310
- src/unnamed/arm9/0206/func_02063718.c (4, DS_UADDR): unc_02063718 0x02063718 0x268
- src/unnamed/arm9/0206/func_0206c8b4.c (2): unc_0206c8b4 0x0206c8b4 0x58
- src/unnamed/arm9/0207/func_02071ccc.c (1): unc_02071ccc 0x02071ccc 0x70
- src/unnamed/ov007/func_ov007_020ae070.c, unc_ov007_020ae2d0.cpp, unc_ov007_020aefb4.cpp, unc_ov007_020b0358.c, unc_ov007_020b58e4.c, unc_ov007_020bcdb8.cpp: one offsetof assert each, under #ifndef SM64DS_HOST_64 so the host skips the 32-bit check. NDS still sees DS_IADDR. Probes matched: unc_ov007_020ae070 0x020ae070 0x260; unc_ov007_020ae2d0 0x020ae2d0 0x184; unc_ov007_020aefb4 0x020aefb4 0x130; unc_ov007_020b0358 0x020b0358 0x1c8; unc_ov007_020b58e4 0x020b58e4 0x30c; unc_ov007_020bcdb8 0x020bcdb8 0xb8
- src/engine/card/CARD_Rom.c (1, DS_UADDR): CARDi_IsInTcm is a static inline. Caller CARDi_TryReadCardDma 0x02060b64 0x148
- src/engine/ctrdg/CTRDG_Proc.cpp (1, DS_UADDR): CTRDGi_InitModuleInfo 0x0206a6d0 0x1bc
- src/engine/wm/WM_System.c (2, DS_UADDR): WM_SendCommand 0x02061558 0x104; WM_CheckStateEx 0x02061428 0xa8
- src/engine/collision/d_bg_ch_actr.cpp (6): dBgCh_Actr::UpdateExtraContinous _ZN10dBgCh_Actr20UpdateExtraContinousEv 0x020358ac 0xa6c
- src/engine/collision/d_bg_ch_sph_crr_query.cpp (2): dBgCh_Gnd::DetectClsn _ZN9dBgCh_Gnd10DetectClsnEv 0x02038f44 0x1fc; dBgCh_SphCrr::DetectClsnExceptSlot0 _ZN12dBgCh_SphCrr21DetectClsnExceptSlot0Ev 0x02038824 0x214
- src/engine/model/ModelComponents.cpp (1): ModelComponents::func_020453c0 _ZN15ModelComponents13func_020453c0EP8BMD_BoneP8BCA_Filei 0x020453c0 0xbc
- src/engine/model/d_ext_model.cpp (2): unc_0201628c 0x0201628c 0x18; unc_020162a4 0x020162a4 0x20
- src/engine/view/d_camera.cpp (5, two DS_UADDR): dCamera_c::func_0200c66c _ZN9dCamera_c13func_0200c66cEPvPiS1_S1_ 0x0200c66c 0x374; dCamera_c::SetMode _ZN9dCamera_c7SetModeEi 0x0200cb58 0x18; dCamera_c::GoBehindPlayer _ZN9dCamera_c14GoBehindPlayerEj 0x0200d304 0xf4; dCamera_c::Behavior _ZN9dCamera_c8BehaviorEv 0x0200debc 0x47c
- src/engine/link/unit0203d7b8.cpp (2): offsetof asserts only. __sinit_unit0203d7b8.cpp 0x02075150 0x4 matched (bytes unchanged).
- src/runtime/actor/d_actor.cpp (2): dActor_c destructor, all three emissions: _ZN8dActor_cD2Ev 0x020112c8 0x4c; _ZN8dActor_cD0Ev 0x02011314 0x60; _ZN8dActor_cD1Ev 0x02011374 0x4c
- src/runtime/actor/d_cap_enemy_arm9.cpp (1): dCapEnemy_c::GetCapState _ZN11dCapEnemy_c11GetCapStateEv 0x02005fa0 0xb4. The (long long)(int)& launder stayed a long long of DS_IADDR.
- src/runtime/actor/d_enemy_base.cpp (1): EnemyDeath::func_ov002_020ae64c _ZN11EnemyDeathC19func_ov002_020ae64cER10dBgCh_Actr 0x020ae64c 0x5c
- src/runtime/actor/d_path_lift_actor.cpp (2): dPathLiftActor_c::StateFall _ZN16dPathLiftActor_c9StateFallEv 0x020ef408 0x174
- src/game/actors/d_a_choropu.cpp (8): _ZN11daChoropu_c19func_ov080_02123924Ev 0x02123924 0x110; _ZN11daChoropu_c19func_ov080_02123c24Ev 0x02123c24 0x2a8; _ZN11daChoropu_c19func_ov080_02123eccEv 0x02123ecc 0x100; _ZN11daChoropu_c19func_ov080_02123fccEv 0x02123fcc 0xbc
- src/game/actors/d_a_coin.cpp (6): daCoin_c::InitResources _ZN8daCoin_c13InitResourcesEv 0x020b2508 0x52c. The comment that names the old launder was left alone.
- src/game/actors/d_a_gmch.cpp (1): daGmch_c::SetState _ZN8daGmch_c8SetStateEi 0x0212777c 0x1c
- src/game/actors/d_a_krb.cpp (13): _ZN7daKrb_c19func_ov084_021298d0Ev 0x021298d0 0x130; _ZN7daKrb_c19func_ov084_02129a00Ev 0x02129a00 0x29c; _ZN7daKrb_c19func_ov084_02129ed4Ev 0x02129ed4 0x6ac; daKrb_c::ActPause _ZN7daKrb_c8ActPauseEv 0x0212a6f8 0x7c; _ZN7daKrb_c19func_ov084_0212abd4Ev 0x0212abd4 0x3a0; daKrb_c::CleanupResources _ZN7daKrb_c16CleanupResourcesEv 0x0212b510 0xa8; daKrb_c::InitResources _ZN7daKrb_c13InitResourcesEv 0x0212bc30 0x390
- src/game/actors/d_a_kuri_king.cpp (4): daKuriKing_c::UpdateScaleChange _ZN12daKuriKing_c17UpdateScaleChangeEv 0x0211fa74 0x110; daKuriKing_c::UpdateSummonGoombas _ZN12daKuriKing_c19UpdateSummonGoombasEv 0x0211fd74 0x238; daKuriKing_c::UpdateIntroTalk _ZN12daKuriKing_c15UpdateIntroTalkEv 0x021201f0 0x1f0
- src/game/actors/d_a_nknk.cpp (2): daNknk_c::func_ov062_02118718 _ZN8daNknk_c19func_ov062_02118718Ev 0x02118718 0x2e8
- src/game/actors/d_a_obj_bc_switch.cpp (2): daObjBC_Switch_c::InitResources _ZN16daObjBC_Switch_c13InitResourcesEv 0x020f1468 0x110
- src/game/actors/d_a_obj_block_s.cpp (3): daObjBlockS_c::ResetToHome _ZN13daObjBlockS_c11ResetToHomeEv 0x02138ce0 0x128. Same long long launder as the cap enemy.
- src/game/actors/d_a_obj_fall_block.cpp (4): daObjFallBlock_c::Behavior _ZN16daObjFallBlock_c8BehaviorEv 0x0213a36c 0x428
- src/game/actors/d_a_obj_mario_cap.cpp (1): daObjMarioCap_c::Render _ZN15daObjMarioCap_c6RenderEv 0x020b83c4 0xe4
- src/game/actors/d_a_obj_sm_lift.cpp (1): daObjSm_Lift_c::Behavior _ZN14daObjSm_Lift_c8BehaviorEv 0x02111368 0x34c. Only the inner (int)& moved; the outer (unsigned) of that integer stayed.
- src/game/actors/d_a_red_bombhei.cpp (1, DS_UADDR): daRedBombhei_c::UpdateTalk _ZN14daRedBombhei_c10UpdateTalkEv 0x0212c508 0x394
- src/game/actors/d_a_sound_obj.cpp (7): data initializers of SoundObjStarter. __sinit_d_a_sound_obj.cpp 0x02107f88 0xc4
- src/game/actors/d_a_star.cpp (3): daStar_c::StateCollectTalk _ZN8daStar_c16StateCollectTalkEv 0x020e9d18 0x354; daStar_c::OnTurnIntoEgg _ZN8daStar_c13OnTurnIntoEggER7daPly_c 0x020e8edc 0xc
- src/game/actors/d_a_wanwan.cpp (1, DS_UADDR): daWanwan_c::CapChainToPivot _ZN10daWanwan_c15CapChainToPivotEv 0x0211236c 0x1a0
- src/game/actors/d_a_yegg.cpp (1): daYegg_c::SeekHoming _ZN8daYegg_c10SeekHomingEv 0x020ecd18 0x27c
- src/game/player/d_a_ply_methods.cpp (18, one DS_UADDR): one compile, all eight changed functions matched. unc_ov002_020db54c 0x020db54c 0xa8; daPly_c::func_ov002_020cc05c _ZN7daPly_c19func_ov002_020cc05cEt 0x020cc05c 0xe4; daPly_c::St_Hurt_Init _ZN7daPly_c12St_Hurt_InitEv 0x020d9b68 0xdc; daPly_c::St_Land_Init _ZN7daPly_c12St_Land_InitEv 0x020e088c 0x1d8; daPly_c::Render _ZN7daPly_c6RenderEv 0x020e3a08 0x3f8; daPly_c::func_ov002_020e4768 _ZN7daPly_c19func_ov002_020e4768Ev 0x020e4768 0x204; daPly_c::Behavior _ZN7daPly_c8BehaviorEv 0x020e4d24 0x868; daPly_c::func_ov002_020e640c _ZN7daPly_c19func_ov002_020e640cEv 0x020e640c 0x374
- src/game/stages/d_s_dsmt.cpp (4): dScDSMT_c::Behavior _ZN9dScDSMT_c8BehaviorEv 0x020cc2cc 0x190; dScDSMT_c::InitResources _ZN9dScDSMT_c13InitResourcesEv 0x020cc4c0 0x140
- src/game/stages/d_s_entry_methods.cpp (2): dScEntry_c::EnterQuit _ZN10dScEntry_c9EnterQuitEv 0x02117bc4 0x48
- src/game/stages/d_s_mb.cpp (2): LoadMultiBootResources 0x02034fbc 0xb0
- src/game/stages/d_s_stage_methods.cpp (3): LoadOrUnloadObjectOverlays _Z26LoadOrUnloadObjectOverlaysPFviEi 0x0202df70 0x118
- src/minigames/d_mg3d_heyho_obj_adapter.cpp (1): Heyho_SeparateAll 0x020c42bc 0x1f8. long long of DS_IADDR kept; dropping the widening still loses the 15 words the comment records.
- src/minigames/d_s_mg_amida.cpp (3, plus PIECE_OFF): dScMgAmida_c::ShuffleGoals _ZN12dScMgAmida_c12ShuffleGoalsEi 0x020d122c 0x224; dScMgAmida_c::SetupRound _ZN12dScMgAmida_c10SetupRoundEv 0x020d3ba0 0x98c; dScMgAmida_c::Behavior _ZN12dScMgAmida_c8BehaviorEv 0x020d4b7c 0x774
- src/minigames/d_s_mg_d3d_base.cpp (1): dScMgD3DBase_c::InitEngines _ZN14dScMgD3DBase_c11InitEnginesEv 0x020e7124 0x19c
- src/minigames/d_s_mg_memory2.cpp (1, MEMORY2_OFF): dScMgMemory2_c::CardFlyAway _ZN14dScMgMemory2_c11CardFlyAwayEi 0x020f5cb4 0x12c
- src/minigames/d_s_mg_pachinko.cpp (4, SHOT_BASE/BALL_BASE/SHOT_OFF/BALL_OFF): dScMgPachinko_c::Ball_AimAtTarget _ZN15dScMgPachinko_c16Ball_AimAtTargetEi 0x020fcb4c 0x240; dScMgPachinko_c::Ball_CheckHitByBobOmb _ZN15dScMgPachinko_c21Ball_CheckHitByBobOmbEi 0x020fdaf0 0x250; dScMgPachinko_c::BobOmb_StateAim _ZN15dScMgPachinko_c15BobOmb_StateAimEi 0x020fe394 0x3bc. __sinit_d_s_mg_pachinko.cpp 0x02131a38 0x298 also matched.
- src/minigames/d_s_mg_slots.cpp (1, SLOT3_OFF): Slot3_ReelDigits 0x0210ab08 0x88; dScMgSlot3_c::StartRound _ZN12dScMgSlot3_c10StartRoundEi 0x0210b314 0x334; dScMgSlot3_c::Render _ZN12dScMgSlot3_c6RenderEv 0x0210b648 0x668. __sinit_d_s_mg_slots.cpp 0x02132894 0xdc matched.
- src/minigames/d_s_mg_snowball.cpp (3, SNOW_OFF plus two direct casts): dScMgSnowball_c::DrawCourseColumn _ZN15dScMgSnowball_c16DrawCourseColumnEii 0x02126b4c 0x398; dScMgSnowball_c::Render _ZN15dScMgSnowball_c6RenderEv 0x02127d10 0x694. __sinit_d_s_mg_snowball.cpp 0x021333e0 0x18c matched.
- src/minigames/d_s_mg_trampoline2.cpp (3): dScMgTrampoline2_c::SpawnWave _ZN18dScMgTrampoline2_c9SpawnWaveEv 0x02123428 0x314; dScMgTrampoline2_c::StatePlay _ZN18dScMgTrampoline2_c9StatePlayEv 0x02123cb4 0x38c
- src/minigames/unit020bfec0.cpp (1): MascotWalker_StartNextPath 0x020c057c 0x160
- src/minigames/unit020cd744.cpp (3): TrmpLine_Create 0x020d0c38 0x3ac
- src/ui/hud/d_map.cpp (1): dMap_c::Behavior _ZN6dMap_c8BehaviorEv 0x020fa690 0xcfc
- src/ui/menus/unit020b8f50.cpp (2): PickerScene_UpdateReplay 0x020bbd24 0x13c

Headers, offsetof asserts only, no function body:

- include/MWCPlusLib.h (2)
- include/Ov007Camera.h (2)
- include/Ov007Picker.h (1)
- include/Ov007Settings.h (1)
- include/private/arm9_action_walk.h (1)
- include/private/arm9_wbt.h (1)
- include/private/ov075_entry_mesh.h (3)
- include/private/throw_context.h (1)

No (int)pointer argument of a cache flush, overlay loader, or sound call turned up apart from the &OVERLAY_* loads above, which are in the & census.

## Leftovers

Not edited. Another pass owns these files. Counts are (int)& / (u32)& still in code, not comments.

- src/cstd4.cpp — 5, (int)&out into unc_0206de14
- src/engine/model/Model4.cpp — (int)&file and (int)&((Matrix4x3 *)0)[a + b]
- src/engine/model/dExtAnmTexPat_c_UpdateFileOffsets.cpp — (int)&file
- src/engine/os/OS_Printf.c — (u32)&format
- src/engine/os/OS_Exception.cpp — (u32)&data_020a63ac
- src/nds_printf.c — (u32)&fmt

src/nds_vprintf.c, src/engine/heap/, src/engine/cp15/, src/engine/mi/MI_Memory.c, src/OS_InitContext.c, and the other owned OS files had no & cast of this form. Comments that still spell the old cast (d_a_coin.cpp, d_a_nknk.cpp, d_a_star.cpp, d_s_dsmt.cpp, d_a_mip.cpp, d_a_trs_trap.cpp, include/daCoin_c.h) were not rewritten.
