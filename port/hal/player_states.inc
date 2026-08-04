/* GENERATED from config/arm9/overlays/ov002/symbols.txt: DS state-fn
 * address -> host method, for every St_ file the slice hosts.
 * St_LevelEnter stays out: its entrance-anim table is junk under the
 * smoke's fake spawn context (real level boot will stage it). */
    case 0x020cc228: return ((Player *)self)->Player::St_Shell_Cleanup();
    case 0x020d2404: return ((Player *)self)->Player::St_Wait_Main();
    case 0x020d32bc: return ((Player *)self)->Player::St_Wait_Init();
    case 0x020d38c0: return ((Player *)self)->Player::St_Walk_Main();
    case 0x020d4270: return ((Player *)self)->Player::St_Walk_Init();
    case 0x020dcc28: return ((Player *)self)->Player::St_StomachSlide_Main();
    case 0x020dd104: return ((Player *)self)->Player::St_StomachSlide_Init();
    case 0x020e04ec: return ((Player *)self)->Player::St_Land_Main();
    case 0x020e088c: return ((Player *)self)->Player::St_Land_Init();
    case 0x020e2118: return ((Player *)self)->Player::St_Jump_Main();
    /* St_Jump_Init defines at C linkage in src (explicit char* this) */
    case 0x020e22c0: return _ZN6Player12St_Jump_InitEv((char *)self);
    /* Mario's airborne gravity fn, dispatched as a PMF from the
       per-character rows in data_ov002_0211073c (St_Jump_Main) */
    case 0x020e200c: func_ov002_020e200c((char *)self); return 1;
    /* Luigi's and Yoshi's rows of the same table (Wario shares Mario's) */
    case 0x020e1e70: func_ov002_020e1e70((char *)self); return 1;
    case 0x020e1c20: func_ov002_020e1c20((char *)self); return 1;
    case 0x020e1af8: return ((Player *)self)->Player::St_Fall_Main();
    case 0x020e1b80: return ((Player *)self)->Player::St_Fall_Init();
    case 0x020cb15c: return ((Player *)self)->Player::St_Headstand_Main();
    case 0x020cb2f0: return ((Player *)self)->Player::St_Headstand_Init();
    /* ST_CLIMB (record 0x021106dc) -- the tree/pole grab, and the freeze
       Tango hit on a bush. Init reaches its matched source through a
       shadow class and Main through a forwarding bridge; both are in
       port/unmatched/Player_St_Climb.cpp. */
    case 0x020cbd34: return port_player_st_climb_init((void *)self);
    case 0x020cb5bc: return port_player_st_climb_main((void *)self);
    case 0x020cb568: return ((Player *)self)->Player::St_Climb_Cleanup();
    case 0x020cf714: return ((Player *)self)->Player::St_OnWall_Main();
    case 0x020cfa90: return ((Player *)self)->Player::St_OnWall_Init();
    case 0x020d0ee0: return ((Player *)self)->Player::St_Crawl_Main();
    case 0x020d112c: return ((Player *)self)->Player::St_Crawl_Init();
    case 0x020d1354: return ((Player *)self)->Player::St_Crouch_Main();
    case 0x020d15e0: return ((Player *)self)->Player::St_Crouch_Init();
    case 0x020d4de8: return ((Player *)self)->Player::St_Bonk_Main();
    case 0x020d4f8c: return ((Player *)self)->Player::St_Bonk_Init();
    case 0x020dc1d0: return ((Player *)self)->Player::St_SlideKick_Main();
    case 0x020dc3e0: return ((Player *)self)->Player::St_SlideKick_Init();
    case 0x020dd3c4: return ((Player *)self)->Player::St_PunchKick_Main();
    case 0x020dd79c: return ((Player *)self)->Player::St_PunchKick_Init();
    case 0x020dd9e0: return ((Player *)self)->Player::St_GroundPound_Cleanup();
    case 0x020dd9f8: return ((Player *)self)->Player::St_GroundPound_Main();
    case 0x020dddf0: return ((Player *)self)->Player::St_GroundPound_Init();
    case 0x020e0aa8: return ((Player *)self)->Player::St_SmallLaunchUp_Main();
    case 0x020e0adc: return ((Player *)self)->Player::St_SmallLaunchUp_Init();
    case 0x020e0b50: return ((Player *)self)->Player::St_SlopeJump_Main();
    case 0x020e0c04: return ((Player *)self)->Player::St_SlopeJump_Init();
    case 0x020e1238: return ((Player *)self)->Player::St_LongJump_Main();
    case 0x020e127c: return ((Player *)self)->Player::St_LongJump_Init();
    case 0x020e13c0: return ((Player *)self)->Player::St_SideFlip_Main();
    case 0x020e1a10: return ((Player *)self)->Player::St_SideFlip_Init();
    /* gate-10 tier-2 wave: water, damage, death, carry and cutscene
       state families whose Init AND Main are both matched. */
    /* Balloon */
    case 0x020de45c: return ((Player *)self)->Player::St_Balloon_Main();
    case 0x020de8ac: return ((Player *)self)->Player::St_Balloon_Init();
    case 0x020de3b4: return ((Player *)self)->Player::St_Balloon_Cleanup();
    /* BowserEarthquake */
    case 0x020c5560: return ((Player *)self)->Player::St_BowserEarthquake_Main();
    case 0x020c568c: return ((Player *)self)->Player::St_BowserEarthquake_Init();
    /* BurnFire */
    case 0x020d53ac: return ((Player *)self)->Player::St_BurnFire_Main();
    case 0x020d5750: return ((Player *)self)->Player::St_BurnFire_Init();
    /* ButtSlide */
    case 0x020dc740: return ((Player *)self)->Player::St_ButtSlide_Main();
    case 0x020dca80: return ((Player *)self)->Player::St_ButtSlide_Init();
    /* CameraZoom */
    case 0x020cad10: return ((Player *)self)->Player::St_CameraZoom_Main();
    case 0x020caef4: return ((Player *)self)->Player::St_CameraZoom_Init();
    case 0x020cac38: return ((Player *)self)->Player::St_CameraZoom_Cleanup();
    /* Cannon */
    case 0x020def14: return ((Player *)self)->Player::St_Cannon_Main();
    case 0x020df208: return ((Player *)self)->Player::St_Cannon_Init();
    case 0x020deefc: return ((Player *)self)->Player::St_Cannon_Cleanup();
    /* CeilingGrate */
    case 0x020cf398: return ((Player *)self)->Player::St_CeilingGrate_Main();
    case 0x020cf6bc: return ((Player *)self)->Player::St_CeilingGrate_Init();
    /* CrazedCrate */
    case 0x020e0d48: return ((Player *)self)->Player::St_CrazedCrate_Main();
    case 0x020e0fac: return ((Player *)self)->Player::St_CrazedCrate_Init();
    case 0x020e0d28: return ((Player *)self)->Player::St_CrazedCrate_Cleanup();
    /* DebugFly */
    case 0x020ca7f4: return ((Player *)self)->Player::St_DebugFly_Main();
    case 0x020ca8d0: return ((Player *)self)->Player::St_DebugFly_Init();
    /* DizzyStars */
    case 0x020d828c: return ((Player *)self)->Player::St_DizzyStars_Main();
    case 0x020d82cc: return ((Player *)self)->Player::St_DizzyStars_Init();
    case 0x020d8274: return ((Player *)self)->Player::St_DizzyStars_Cleanup();
    /* Electrocute */
    case 0x020d5834: return ((Player *)self)->Player::St_Electrocute_Main();
    case 0x020d59b0: return ((Player *)self)->Player::St_Electrocute_Init();
    /* Fly */
    case 0x020dfdf0: return ((Player *)self)->Player::St_Fly_Main();
    case 0x020e027c: return ((Player *)self)->Player::St_Fly_Init();
    /* GrabBowserTail */
    case 0x020da438: return ((Player *)self)->Player::St_GrabBowserTail_Main();
    case 0x020da794: return ((Player *)self)->Player::St_GrabBowserTail_Init();
    case 0x020da41c: return ((Player *)self)->Player::St_GrabBowserTail_Cleanup();
    /* Grabbed */
    case 0x020db3bc: return ((Player *)self)->Player::St_Grabbed_Main();
    case 0x020db4d8: return ((Player *)self)->Player::St_Grabbed_Init();
    case 0x020db368: return ((Player *)self)->Player::St_Grabbed_Cleanup();
    /* HoldHeavy */
    case 0x020d16bc: return ((Player *)self)->Player::St_HoldHeavy_Main();
    case 0x020d19a8: return ((Player *)self)->Player::St_HoldHeavy_Init();
    /* HoldLight */
    case 0x020d1a1c: return ((Player *)self)->Player::St_HoldLight_Main();
    case 0x020d1ea0: return ((Player *)self)->Player::St_HoldLight_Init();
    case 0x020d19fc: return ((Player *)self)->Player::St_HoldLight_Cleanup();
    /* Hurt */
    case 0x020d966c: return ((Player *)self)->Player::St_Hurt_Main();
    case 0x020d9b68: return ((Player *)self)->Player::St_Hurt_Init();
    case 0x020d9654: return ((Player *)self)->Player::St_Hurt_Cleanup();
    /* HurtWater */
    case 0x020cc7d0: return ((Player *)self)->Player::St_HurtWater_Main();
    case 0x020cc910: return ((Player *)self)->Player::St_HurtWater_Init();
    /* InYoshiMouth */
    case 0x020d60b8: return ((Player *)self)->Player::St_InYoshiMouth_Main();
    case 0x020d62f8: return ((Player *)self)->Player::St_InYoshiMouth_Init();
    case 0x020d6084: return func_ov002_020d6084((char *)self);   /* St_InYoshiMouth_Cleanup slot */
    /* JumpQuicksand */
    case 0x020dedf0: return ((Player *)self)->Player::St_JumpQuicksand_Main();
    case 0x020deeac: return ((Player *)self)->Player::St_JumpQuicksand_Init();
    /* LedgeGrab */
    case 0x020d0824: return ((Player *)self)->Player::St_LedgeGrab_Main();
    case 0x020d08b0: return ((Player *)self)->Player::St_LedgeGrab_Init();
    /* MetalWaterGround */
    case 0x020ccccc: return ((Player *)self)->Player::St_MetalWaterGround_Main();
    case 0x020cd0d0: return ((Player *)self)->Player::St_MetalWaterGround_Init();
    /* MetalWaterWater */
    case 0x020cc9c0: return ((Player *)self)->Player::St_MetalWaterWater_Main();
    case 0x020ccc3c: return ((Player *)self)->Player::St_MetalWaterWater_Init();
    /* Null */
    case 0x020cac28: return ((Player *)self)->Player::St_Null_Main();
    case 0x020cac20: return ((Player *)self)->Player::St_Null_Cleanup();
    case 0x020cac30: return func_ov002_020cac30();   /* St_Null_Init slot */
    /* OpeningWakeUp */
    case 0x020c3dd0: return ((Player *)self)->Player::St_OpeningWakeUp_Main();
    case 0x020c3ed8: return ((Player *)self)->Player::St_OpeningWakeUp_Init();
    /* Owl */
    case 0x020df3ec: return ((Player *)self)->Player::St_Owl_Main();
    case 0x020df740: return ((Player *)self)->Player::St_Owl_Init();
    case 0x020df3c8: return ((Player *)self)->Player::St_Owl_Cleanup();
    /* Respawn */
    case 0x020c3f40: return ((Player *)self)->Player::St_Respawn_Main();
    case 0x020c4088: return ((Player *)self)->Player::St_Respawn_Init();
    /* Shell */
    case 0x020cc27c: return ((Player *)self)->Player::St_Shell_Main();
    case 0x020cc780: return ((Player *)self)->Player::St_Shell_Init();
    /* Spin */
    case 0x020e10ec: return ((Player *)self)->Player::St_Spin_Main();
    case 0x020e119c: return ((Player *)self)->Player::St_Spin_Init();
    case 0x020e10d4: return ((Player *)self)->Player::St_Spin_Cleanup();
    /* StuckInGround */
    case 0x020c52bc: return ((Player *)self)->Player::St_StuckInGround_Main();
    case 0x020c54e8: return ((Player *)self)->Player::St_StuckInGround_Init();
    /* Swallow */
    case 0x020d6474: return ((Player *)self)->Player::St_Swallow_Main();
    case 0x020d666c: return ((Player *)self)->Player::St_Swallow_Init();
    /* SweepKick */
    case 0x020dc48c: return ((Player *)self)->Player::St_SweepKick_Main();
    case 0x020dc510: return ((Player *)self)->Player::St_SweepKick_Init();
    /* Swim */
    case 0x020cd94c: return ((Player *)self)->Player::St_Swim_Main();
    case 0x020ce550: return ((Player *)self)->Player::St_Swim_Init();
    case 0x020cd1e4: return ((Player *)self)->Player::St_Swim_Cleanup();
    /* Talk */
    case 0x020c48e0: return ((Player *)self)->Player::St_Talk_Main();
    case 0x020c4bd8: return ((Player *)self)->Player::St_Talk_Init();
    case 0x020c4760: return ((Player *)self)->Player::St_Talk_Cleanup();
    /* Teleport */
    case 0x020c6b3c: return ((Player *)self)->Player::St_Teleport_Main();
    case 0x020c6dc4: return ((Player *)self)->Player::St_Teleport_Init();
    /* Throw */
    case 0x020dae6c: return ((Player *)self)->Player::St_Throw_Main();
    case 0x020dafd4: return ((Player *)self)->Player::St_Throw_Init();
    /* Thrown */
    case 0x020db0bc: return ((Player *)self)->Player::St_Thrown_Main();
    case 0x020db2e8: return ((Player *)self)->Player::St_Thrown_Init();
    case 0x020db0a4: return ((Player *)self)->Player::St_Thrown_Cleanup();
    /* TornadoSpin */
    case 0x020ddfdc: return ((Player *)self)->Player::St_TornadoSpin_Main();
    case 0x020de2a0: return ((Player *)self)->Player::St_TornadoSpin_Init();
    /* TurnAround */
    case 0x020d34bc: return ((Player *)self)->Player::St_TurnAround_Main();
    case 0x020d369c: return ((Player *)self)->Player::St_TurnAround_Init();
    /* Wait */
    case 0x020d2378: return ((Player *)self)->Player::St_Wait_Cleanup();
    /* WaitQuicksand */
    case 0x020d2184: return ((Player *)self)->Player::St_WaitQuicksand_Main();
    case 0x020d2224: return ((Player *)self)->Player::St_WaitQuicksand_Init();
    /* WallJump */
    case 0x020e1714: return ((Player *)self)->Player::St_WallJump_Main();
    case 0x020e17f8: return func_ov002_020e17f8((void *)self);   /* St_WallJump_Init slot */
    /* WallSlide */
    case 0x020d0d78: return ((Player *)self)->Player::St_WallSlide_Main();
    case 0x020d0e8c: return ((Player *)self)->Player::St_WallSlide_Init();
    /* WindCarry */
    case 0x020deab8: return ((Player *)self)->Player::St_WindCarry_Main();
    case 0x020dec0c: return ((Player *)self)->Player::St_WindCarry_Init();
    /* YoshiPower */
    case 0x020d7504: return ((Player *)self)->Player::St_YoshiPower_Main();
    case 0x020d7ed0: return ((Player *)self)->Player::St_YoshiPower_Init();
    case 0x020d736c: return ((Player *)self)->Player::St_YoshiPower_Cleanup();
    /* SHARED-BODY states: these five have no Main of their own. Their
       State objects (built by __sinit_ov002_021019d0) point the Main slot
       at a body another state already owns, so only the Init was missing:
         Dive           state 0x021105bc  Main = St_StomachSlide_Main
         SlideKickRec.  state 0x021101fc  Main = St_SideFlip_Main
         BackFlip       state 0x0211052c  Main = St_SideFlip_Main
         WaterJump      state 0x02110694  Main = St_SideFlip_Main
         HeadstandJump  state 0x02110724  Main = St_SideFlip_Main */
    /* NoControl: the cutscene/door/pipe state. Its Init dispatches a
       per-kind entry function through a sinit-built PMF table, so it runs
       from port/unmatched (hal_call_state_fn on the DS code word). */
    case 0x020c816c: return ((Player *)self)->Player::St_NoControl_Main();
    case 0x020c976c: return ((Player *)self)->Player::St_NoControl_Init();
    case 0x020c8128: return ((Player *)self)->Player::St_NoControl_Cleanup();
    /* DEATH states: every Main here funnels into KillPlayer ->
       StartExitFaderWipe, which derefs the fader-wipe array at
       0x0209f324. Stage::InitResources allocates that array and the port
       never boots a Stage, so hal/fader_wipes.cpp stages it instead. */
    /* DeadPit */
    case 0x020c5780: return ((Player *)self)->Player::St_DeadPit_Main();
    case 0x020c59c0: return ((Player *)self)->Player::St_DeadPit_Init();
    /* DeadHit */
    case 0x020c5e34: return ((Player *)self)->Player::St_DeadHit_Main();
    case 0x020c6010: return ((Player *)self)->Player::St_DeadHit_Init();
    /* Squish */
    case 0x020c662c: return ((Player *)self)->Player::St_Squish_Main();
    case 0x020c69a0: return ((Player *)self)->Player::St_Squish_Init();
    case 0x020c65f8: return ((Player *)self)->Player::St_Squish_Cleanup();
    /* BurnLava: its Main defines at C linkage in src (explicit char* this) */
    case 0x020d5038: return _ZN6Player16St_BurnLava_MainEv((char *)self);
    case 0x020d529c: return ((Player *)self)->Player::St_BurnLava_Init();
    case 0x020dd17c: return ((Player *)self)->Player::St_Dive_Init();
    case 0x020e162c: return ((Player *)self)->Player::St_WaterJump_Init();
    case 0x020e16a4: return ((Player *)self)->Player::St_HeadstandJump_Init();
    case 0x020e18a8: return ((Player *)self)->Player::St_BackFlip_Init();
    case 0x020e1990: return ((Player *)self)->Player::St_SlideKickRecover_Init();
    /* St_LevelEnter, the boot state (gate 14). Main is the ov002 function at
       0x020c6f3c -- the community symbol _ZN6Player18St_LevelEnter_MainEv
       names ov006's bytes at the same address -- and the seven entrance-step
       handlers it dispatches through data_ov002_0211075c are plain ov002
       functions rather than Player methods. */
    case 0x020c7838: return ((Player *)self)->Player::St_LevelEnter_Init();
    case 0x020c6f3c: return func_ov002_020c6f3c(self);
    case 0x020c6f34: return ((Player *)self)->Player::St_LevelEnter_Cleanup();
    case 0x020c75f0: func_ov002_020c75f0((char *)self); return 1;
    case 0x020c7350: func_ov002_020c7350((char *)self); return 1;
    case 0x020c71e0: func_ov002_020c71e0((char *)self); return 1;
    case 0x020c7194: func_ov002_020c7194((char *)self); return 1;
    case 0x020c72a4: func_ov002_020c72a4((void *)self); return 1;
    case 0x020c70ac: func_ov002_020c70ac((char *)self); return 1;
    case 0x020c6fe4: func_ov002_020c6fe4((char *)self); return 1;
