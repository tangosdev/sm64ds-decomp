/* The registry's class rows, one line per hosted actor class.
 *
 * Included by hal/actor_registry.cpp into the table body. Each row is
 *
 *     { actorID, "NAME", <ROM SpawnInfo>, <host factory>, <vtable fill> }
 *
 * and each needs three things landed together: the SpawnInfo in
 * port/ov002_syms.txt (or the ov009 whole mount), the class's matched src
 * closure in slice_gate16.txt, and a vtable fill in hal/actor_classes.cpp
 * written to the vtable law.
 *
 * The factory column is the class's own matched <Class>_Spawn, which the ROM
 * stores in the SpawnInfo's +0 word; registering the class overwrites that
 * word with this host address and leaves the rest of Nintendo's record alone.
 */
extern "C" {
/* ---- TREE (286, ov002) x21 ---- */
extern unsigned char g_profile_TREE[];      /* ov002 0x0210abdc */
void *daTree_c_classInit(void);                     /* ov002 0x020ec32c */
void hal_fill_tree_vtable(void);
/* ---- AMBIENT_SOUND_EFFECTS (350, ov002) x5 ----
   The class whose methods and vtable the config names EnemySwitchTag; see the
   header of that section in hal/actor_classes.cpp for how the ROM settles it. */
extern unsigned char g_profile_SET_SE[];   /* ov002 0x0210b47c */
void *daSetSE_c_classInit(void);                  /* ov002 0x020f1b94 */
void hal_fill_ambient_sound_vtable(void);
/* ---- gate 211: ENEMY_SWITCH_TAG (357) and ENEMY_SPAWNER (358), ov002 ----
   Two more classes out of the ov002 spawn table. AMBIENT_SOUND_EFFECTS (350)
   above once carried EnemySwitchTag's config-named blocks (the #1048 shift);
   the three vtables are distinct addresses and the two fills below own their
   own host arrays. See port/slice_gate211.txt. */
extern unsigned char g_profile_ENEMY_SWITCH[];        /* ov002 0x0210b324, id 357 */
void *daESwitch_c_classInit(void);                       /* ov002 0x020f1954 */
void hal_fill_enemy_switch_tag_vtable(void);
extern unsigned char g_profile_ENEMY_CREATE[];          /* ov002 0x0210b340, id 358 */
void *daECreate_c_classInit(void);                         /* ov002 0x020f1924 */
void hal_fill_enemy_spawner_vtable(void);
/* ---- gate 212: QUESTION_SWITCH (26), ov002. 32-slot Platform table; see
   port/slice_gate212.txt and hal/actor_classes_question_switch.cpp. */
extern unsigned char g_profile_HATENA_SWITCH[];        /* ov002 0x02108e38, id 26 */
void *daObjHatenaSwitch_c_classInit(void);                       /* ov002 0x020b56d8 */
void hal_fill_question_switch_vtable(void);
/* ---- BLACK_BRICK_BLOCK (17, ov002) x1 ---- */
extern unsigned char g_profile_BLOCK_LL[];       /* ov002 0x02108a78 */
void *daObjBlockL_c_classInit_BLOCK_LL(void);                      /* ov002 0x020b409c */
void hal_fill_black_brick_block_vtable(void);
/* ---- BLUE_COIN_SWITCH (10, ov002) ---- */
extern unsigned char g_profile_BC_SWITCH[];        /* ov002 0x0210b248 */
void *daObjBC_Switch_c_classInit(void);                       /* ov002 0x020f15cc */
void hal_fill_blue_coin_switch_vtable(void);
/* ---- STAR_MARKER (180, ov002) ---- */
extern unsigned char g_profile_STARBASE[];            /* ov002 0x0210aa78 */
void *daStarBase_c_classInit(void);                           /* ov002 0x020ebe8c */
void hal_fill_star_marker_vtable(void);
/* ---- SIGN_POST (184, ov002) x5 ---- */
extern unsigned char g_profile_TATEFUDA[];              /* ov002 0x02109ad4 */
void *daObjTatefuda_c_classInit(void);                             /* ov002 0x020bc3c8 */
void hal_fill_sign_post_vtable(void);
/* ---- ONE_UP_MUSHROOM (276, ov002) x3 ---- */
extern unsigned char g_profile_ONEUPKINOKO[];         /* ov002 0x02108388 */
void *da1up_c_classInit_ONEUPKINOKO(void);                        /* ov002 0x020b0580 */
void hal_fill_one_up_mushroom_vtable(void);
/* ---- gate 17: the level overlay's own four ----
   The SpawnInfo records come from the PER-SYMBOL ov009 mount (ov009_syms.txt),
   not the whole one: the registry rewrites the factory word in place and the
   ActorBase constructor reads the two priorities back out of the same record,
   so it has to be storage the actor code and the registry agree on. */
extern unsigned char g_profile_SBIRD[];                  /* ov009 0x02113934 */
void *daSBird_c_classInit(void);                                 /* ov009 0x02111a30 */
void hal_fill_bird_vtable(void);
extern unsigned char g_profile_MC_WATER[];           /* ov009 0x021139f4 */
void *daObjMcWater_c_classInit(void);                          /* ov009 0x02111d8c */
void hal_fill_castle_water_vtable(void);
extern unsigned char g_profile_MC_METALNET[];              /* ov009 0x02113abc */
void *daObjMc_Metalnet_c_classInit(void);                             /* ov009 0x02112048 */
void hal_fill_metal_net_vtable(void);
extern unsigned char g_profile_MC_FLAG[];                  /* ov009 0x02113b7c */
void *daMcFlag_c_classInit(void);                                 /* ov009 0x021121f0 */
void hal_fill_flag_vtable(void);
/* ---- gate 143: level 11's own class, IceSlideManager (356, ov019) ----
   Cool Cool Mountain's slide. The SpawnInfo comes from the per-symbol ov019
   mount (ov019_syms.txt); the vtable fill is hal/actor_classes_ccm.cpp. */
extern unsigned char g_profile_SLIDER_MANAGER[];       /* ov019 0x021133a8 */
void *daSldMng_c_classInit(void);                      /* ov019 0x0211274c */
void hal_fill_ice_slide_manager_vtable(void);
/* ---- gate 190: level 10's own classes, ICE_SHEET (295) + POWER_STAR_CREATE
   (355), Cool Cool Mountain (ov018). Both SpawnInfo records come from the
   per-symbol ov018 mount (ov018_syms.txt); both vtable fills are in
   hal/actor_classes_ccm.cpp. */
extern unsigned char g_profile_ICE_BOARD[];              /* ov018 0x02113b10 */
void *daObjIceBoard_c_classInit(void);                             /* ov018 0x02112a44 */
void hal_fill_ice_sheet_vtable(void);
extern unsigned char g_profile_STAR_CREATE[];       /* ov018 0x02113a50 */
void *daSCre_c_classInit(void);                      /* ov018 0x0211278c */
void hal_fill_power_star_create_vtable(void);
/* ---- gate 190: ONE_UP_LOGO (331), ov002 (already mounted). Its SpawnInfo
   joins the existing per-symbol ov002 mount; the vtable fill is
   hal/actor_classes_ccm.cpp. */
extern unsigned char g_profile_OBJ_1UPLOGO[];             /* ov002 0x0210b188 */
void *daObj1UpLogo_c_classInit(void);                            /* ov002 0x020f1170 */
void hal_fill_one_up_logo_vtable(void);
/* ---- gate 190 fix round: SOUND_OBJECT (359), ov002. The positional sound
   emitter the free-flying POWER_STAR retries spawning every frame
   (func_ov002_020e7e24, Actor::SpawnSoundObj(6)); hosting PSC's fifth star
   made the retry stream visible. SpawnInfo joins the ov002 mount; the fill
   (and the PMF cell seat) is hal/actor_classes_ccm.cpp +
   port/unmatched/SoundObject_Behavior.cpp. */
extern unsigned char g_profile_SOUND_OBJ[];           /* ov002 0x0210c064 */
void *daSoundObj_c_classInit(void);                          /* ov002 0x020f972c */
void hal_fill_sound_object_vtable(void);
/* ---- gate 191: SKI_LIFT (63) + MOTHER_PENGUIN (257), ov018, extending the
   gate-190 per-symbol mount (ov018_syms.txt). SpawnInfo records join the
   same mount; both vtable fills are in hal/actor_classes_ccm.cpp. */
extern unsigned char g_profile_SM_LIFT[];               /* ov018 0x021138a8 */
void *daObjSm_Lift_c_classInit(void);                              /* ov018 0x02111818 */
void hal_fill_ski_lift_vtable(void);
extern unsigned char g_profile_PENGUIN_MOTHER[];         /* ov018 0x02113998 */
void *daPgMthr_c_classInit(void);                        /* ov018 0x0211267c */
void hal_fill_mother_penguin_vtable(void);
/* ---- gate 192: the ov081 trio -- ICE_BLOCK (18), MR_BLIZZARD (223),
   SPINDRIFT (312), a fresh per-symbol mount (ov081_syms.txt). SpawnInfo
   records come from that mount; all three vtable fills are in
   hal/actor_classes_ov081.cpp. */
extern unsigned char g_profile_HUWAHUWA[];             /* ov081 0x02128858 */
void *daHuwa_c_classInit(void);                            /* ov081 0x02124040 */
void hal_fill_spindrift_vtable(void);
extern unsigned char g_profile_SNOWMAN[];            /* ov081 0x021289cc */
void *daSnowman_c_classInit(void);                           /* ov081 0x02125ec0 */
void hal_fill_mr_blizzard_vtable(void);
extern unsigned char g_profile_ICE_BLOCK_LL[];              /* ov081 0x02128ca0 */
void *daObjIceBlock_c_classInit(void);                             /* ov081 0x021280a0 */
void hal_fill_ice_block_vtable(void);
/* ---- gate 193: ov072's BABY_PENGUIN (256) + the actual SNOWMAN (272,
   the unnamed daBgSnwmn_c block -- NOT SnowmanBody/SnowmanHead, see
   port/ov072_syms.txt), a fresh per-symbol mount (ov072_syms.txt).
   SpawnInfo records come from that mount; both vtable fills are in
   hal/actor_classes_ov072.cpp. daBgSnwmn_c's own SpawnInfo has no clean
   dsd name -- g_profile_BIG_SNOWMAN is what the ROM's own spawn table
   entry resolves to. */
extern unsigned char g_profile_BIG_SNOWMAN[];             /* ov072 0x02122954 */
void *daBgSnwmn_c_classInit(void);                        /* ov072 0x02120c00 */
void hal_fill_snowman_vtable(void);
extern unsigned char g_profile_PENGUIN_BABY[];           /* ov072 0x02122a6c */
void *daPgBby_c_classInit(void);                          /* ov072 0x02121fac */
void hal_fill_baby_penguin_vtable(void);
/* ---- gate 193 extension (lane w3-d): ov072's remaining two classes,
   SNOWMAN_HEAD (273) and SNOWMAN_BODY (274). Both are plain 31-slot
   Actors; dsd names both SpawnInfos and both vtables cleanly, unlike
   daBgSnwmn_c's. Fills in hal/actor_classes_ov072.cpp. */
extern unsigned char g_profile_BIG_SNOWMAN_HEAD[];           /* ov072 0x02122898 */
void *daBgSnmHed_c_classInit(void);                          /* ov072 0x021207d4 */
void hal_fill_snowman_head_vtable(void);
extern unsigned char g_profile_BIG_SNOWMAN_BODY[];           /* ov072 0x0212279c */
void *daBgSnmBdy_c_classInit(void);                          /* ov072 0x0211fedc */
void hal_fill_snowman_body_vtable(void);
/* ---- gate 194: HOOT_THE_OWL (234), ov094, a fresh per-symbol mount
   (ov094_syms.txt). SpawnInfo comes from that mount; the vtable fill is in
   hal/actor_classes_ov094.cpp. */
extern unsigned char g_profile_OWL[];            /* ov094 0x02136a34 */
void *daOwl_c_classInit(void);                           /* ov094 0x02136798 */
void hal_fill_hoot_the_owl_vtable(void);
/* ---- gate 199: SWITCH_PILLAR (34) + BASEMENT_WATER (35), ov012 (level 4),
   a fresh per-symbol mount (ov012_syms.txt) joining the existing whole
   mount. SpawnInfo records come from that mount; both vtable fills are in
   hal/actor_classes_ov012.cpp. */
extern unsigned char g_profile_C0_SWITCH[];          /* ov012 0x02112320 */
void *daObjC0_Switch_c_classInit(void);                         /* ov012 0x02111420 */
void hal_fill_switch_pillar_vtable(void);
extern unsigned char g_profile_C0_WATER[];         /* ov012 0x021123e4 */
void *daObjC0Water_c_classInit(void);                        /* ov012 0x02111730 */
void hal_fill_basement_water_vtable(void);
/* ---- gate 200: PUSH_BLOCK (306) + MUGEN_BGM (351), ov002 (already
   mounted; the SpawnInfo/vtable symbols joined ov002_syms.txt for this
   gate). id 306's records are dsd's "PowerFlower" names -- a naming-race
   decoy; daObjPowerUpItem_c_classInit installs _ZTV9PushBlock itself (no SpawnInfo
   +0x24 vtable word for this id). Both fills are in
   hal/actor_classes_ov002g200.cpp. */
extern unsigned char g_profile_POWER_UP_ITEM[];           /* ov002 0x021097a0, +4 reads 306 */
void *daObjPowerUpItem_c_classInit(void);                          /* ov002 0x020b9e0c */
void hal_fill_pushblock_vtable(void);
extern unsigned char g_profile_MUGEN_BGM[];              /* ov002 0x0210b560, +4 reads 351 */
void *daMugenBGM_c_classInit(void);                             /* ov002 0x020f1f40 */
void hal_fill_mugen_bgm_vtable(void);
/* ---- link100 SEAT3, gate 230: PUSHABLE_BLOCK (192), ov002 (already mounted;
   the SpawnInfo record joined ov002_syms.txt for this gate). A DIFFERENT class
   from the PUSH_BLOCK row above, which is id 306: this one's factory really is
   called daObjPushblock_c_classInit and its table is the unnamed _ZTV16daObjPushblock_c,
   RTTI daObjPushblock_c. Keyed on the id and the addresses, not the names. Its
   vtable is filled by hal/actor_base_tables_ov002.cpp (gate 226, nine own
   slots, no traps), and hal_fill_pushblock192_vtable there is the re-fill hook
   the savestate path needs. */
extern unsigned char g_profile_PUSHBLOCK[];             /* ov002 0x0210968c, +4 reads 192 */
void *daObjPushblock_c_classInit(void);                            /* ov002 0x020b910c */
void hal_fill_pushblock192_vtable(void);
/* ---- gate 202: CLOCK_HAND_SHORT (292) + CLOCK_HAND_LONG (293) +
   CLOCK_PENDULUM (294), ov013 (level 5), a fresh per-symbol mount
   (ov013_syms.txt) joining the existing whole level mount. Both hands share
   ONE vtable (HandLong_Spawn installs _ZTV22ClockPaintingHandShort too,
   reloc-verified; InitResources picks the hand off the actor id) so both
   hand rows use the same fill. All three fills are in
   hal/actor_classes_ov013.cpp. */
extern unsigned char g_profile_CLOCK_SHORT[]; /* ov013 0x021121dc, word[1] reads 292 */
void *daObjClock_c_classInit_CLOCK_SHORT(void);                /* ov013 0x0211163c */
extern unsigned char g_profile_CLOCK_LONG[];  /* ov013 0x021121c0, word[1] reads 293 */
void *daObjClock_c_classInit_CLOCK_LONG(void);                 /* ov013 0x02111674 */
void hal_fill_clock_hand_vtable(void);
extern unsigned char g_profile_CLOCK_HURIKO[];  /* ov013 0x02112104, word[1] reads 294 */
void *daObjClockHuriko_c_classInit(void);                 /* ov013 0x02111384 */
void hal_fill_clock_pendulum_vtable(void);
/* ---- run rel0215 wave 1 (lane cast-ov036): Rainbow Cruise's own seven-class
   platform cast, ov036 (level 28), a fresh per-symbol mount (ov036_syms.txt)
   joining the existing whole level mount. All seven SpawnInfo records come
   from that mount and all seven fills are in hal/actor_classes_ov036.cpp.
   THE ROWS ARE PAIRED BY ADDRESS, NOT BY NAME: config attaches this overlay's
   C++ method and _ZTV names ONE CLASS EARLY (the ov045 off-by-one), so
   SHIP_WING's own table is the array config calls _ZTV18RotatingPlatformRr,
   DONUT_BLOCK's is _ZTV8ShipWing, ARMED_ROTATING_PLATFORM's is
   _ZTV18daObjRc_Guruguru_c and TRICKY_TRIANGLES' is _ZTV16daObjRc_Dorifu_c. The
   Spawn and SpawnInfo names below are the ones config got RIGHT; the
   three-route derivation is in port/ov036_syms.txt. */
extern unsigned char g_profile_RC_BURANKO[];       /* ov036 0x02113a74, word[1] reads 127 */
void *daObjRcBuranko_c_classInit(void);                      /* ov036 0x02111414 */
void hal_fill_swinging_platform_vtable(void);
extern unsigned char g_profile_RC_KAITEN[];     /* ov036 0x02113b50, word[1] reads 129 */
void *daObjRc_Kaitendai_c_classInit(void);                    /* ov036 0x02111544 */
void hal_fill_rotating_platform_rr_vtable(void);
extern unsigned char g_profile_RC_HANE[];               /* ov036 0x02113c14, word[1] reads 125 */
void *daObjRc_Hane_c_classInit(void);                              /* ov036 0x02111904 */
void hal_fill_ship_wing_vtable(void);
extern unsigned char g_profile_RC_TIKUWA[];             /* ov036 0x02113cd4, word[1] reads 126 */
void *daObjRc_Tikuwa_c_classInit(void);                            /* ov036 0x02111cd8 */
void hal_fill_donut_block_vtable(void);
extern unsigned char g_profile_RC_GURUGURU[];  /* ov036 0x02113da8, word[1] reads 132 */
void *daObjRc_Guruguru_c_classInit(void);                 /* ov036 0x02111f5c */
void hal_fill_armed_rotating_platform_vtable(void);
extern unsigned char g_profile_RC_DORIFU[];        /* ov036 0x02113e6c, word[1] reads 123 */
void *daObjRc_Dorifu_c_classInit(void);                       /* ov036 0x021120c8 */
void hal_fill_tricky_triangles_vtable(void);
extern unsigned char g_profile_RC_CARPET[];           /* ov036 0x02113f78, word[1] reads 130 */
void *daObjRcCarpet_c_classInit(void);                          /* ov036 0x02112538 */
void hal_fill_flying_carpet_vtable(void);
/* ---- gate 18: ov085, the first ACTOR overlay ----
   Not the engine overlay and not the level's -- six classes several levels
   share, of which the castle grounds names two. The SpawnInfo records come
   from the per-symbol ov085 mount (ov085_syms.txt). */
extern unsigned char g_profile_MIP[];                /* ov085 0x021300d4 */
void *daMip_c_classInit(void);                               /* ov085 0x0212cc2c */
void hal_fill_rabbit_vtable(void);
extern unsigned char g_profile_OBJ_MIP_KEY[];             /* ov085 0x021301b4 */
void *daObj_Mip_Key_c_classInit(void);                            /* ov085 0x0212d4e8 */
void hal_fill_rabbit_key_vtable(void);
extern unsigned char g_profile_C_JUGEM[];             /* ov085 0x02130320 */
void *daC_Jugem_c_classInit(void);                            /* ov085 0x0212ed54 */
void hal_fill_lakitu_bro_vtable(void);
/* ---- run lvled, lane intro-cutscene: the new-file opening's cast ---- */
extern unsigned char g_profile_DEMO[];        /* ov002 0x0210ba60 */
void *daDemo_c_classInit(void);                       /* ov002 0x020f8808 */
void hal_fill_cutscene_object_vtable(void);
/* ---- gate 19: ov098's CANNON ---- */
extern unsigned char g_profile_CANNON[];                /* ov098 0x0213c67c */
void *daCnn_c_classInit(void);                               /* ov098 0x0213b43c */
void hal_fill_cannon_vtable(void);
/* ---- gate 20: ov002's last two ----
   Both SpawnInfo records come from the ov002 per-symbol mount that has
   carried Player/Tree/SignPost since gate 14. The class-method names are the
   neighbour's in both cases and the *_SpawnInfo names are the right ones --
   the ROM's RTTI settles it, see slice_gate20.txt. */
extern unsigned char g_profile_CH_SCENE[];                  /* ov002 0x02108690 */
void *daChScene_c_classInit(void);                                 /* ov002 0x020b0f24 */
void hal_fill_exit_vtable(void);
extern unsigned char g_profile_WATERFALL[];         /* ov002 0x0210947c */
void *daObjWaterfall_c_classInit(void);                        /* ov002 0x020b6ee8 */
void hal_fill_waterfall_mist_vtable(void);
/* ---- gate 21: ov100's BUTTERFLY and FISH ----
   The third actor overlay (ov100_syms.txt). Both class names check out
   against the ROM's own RTTI; the one shifted pair in ov100 is Door/StarDoor
   and that is gate 22's problem. */
extern unsigned char g_profile_BUTTERFLY[];             /* ov100 0x02147e78 */
void *daBtfly_c_classInit(void);                            /* ov100 0x02141ea4 */
void hal_fill_butterfly_vtable(void);
extern unsigned char g_profile_FISH[];                  /* ov100 0x02148498 */
void *daFish_c_classInit(void);                                 /* ov100 0x02146d38 */
void hal_fill_fish_vtable(void);
extern unsigned char g_profile_WANWAN2[];        /* ov100 0x02148020 */
void *daWanwan2_c_classInit(void);
void hal_fill_unchained_chomp_vtable(void);
/* ---- gate 22: ov100's DOOR ----
   The last class the castle grounds names. Its vtable is _ZTV4Door, ov100
   0x02148188, RTTI 8daDoor_c, exactly as config names it, and the _ZN4Door*
   family is its own. See the gate-22 block in hal/actor_classes.cpp for the
   three cartridge readings lane CRASH12 used to settle it. */
extern unsigned char g_profile_DOOR[];                  /* ov100 0x02148164 */
void *daDoor_c_classInit(void);                                 /* ov100 0x0214589c */
void hal_fill_door_vtable(void);
/* ---- gate 40: ov100's STAR_DOOR (354) ----
   NOT the DOOR above, and not a second name for it either. Its table is
   _ZTV12daStarGate_c, ov100 0x021483cc, a different address from the plain
   door's 0x02148188 with a different RTTI name, so it has its own six methods
   and its own vtable fill (hal_fill_star_door_vtable), distinct from the plain
   door's hal_fill_door_vtable. Its SpawnInfo is already mounted by
   ov100_syms.txt at 0x021483a8; the record's +4 halfword reads 354. See the
   head of slice_gate40.txt and the gate-40 section of hal/actor_classes.cpp. */
extern unsigned char g_profile_STAR_GATE[];              /* ov100 0x021483a8 */
void *daStarGate_c_classInit(void);                             /* ov100 0x021461d4 */
void hal_fill_star_door_vtable(void);
/* ---- gate 41: ov010's TRAP (36) and LIGHT_BEAM (37), CORRECTED by link100
   SPAWN2 gate 232. Level 2's own overlay, mounted per symbol
   (ov010_syms.txt). These are TWO classes with TWO tables, not one table
   under three names: the RTTI name string at each table's vtable-4 record
   says 0x02112ae4 is "14daObjC1_Trap_c" and 0x02112ba8 is
   "15daObjC1Hikari_c", and each factory's own literal pool loads its own
   one (daObjC1_Trap_c_classInit at 0x021119cc -> 0x02112ae4, daObjC1Hikari_c_classInit at
   0x02111e0c -> 0x02112ba8). So the fill that used to sit on the 36 row has
   moved to the 37 row, which is the one that really installs _ZTV4Trap, and
   36 gets its own table and its own six bodies from
   hal/actor_classes_ov010.cpp. Both records' +4 halfwords read 36 / 37. */
extern unsigned char g_profile_C1_TRAP[];                  /* ov010 0x02112ac0 */
void *daObjC1_Trap_c_classInit(void);                                 /* ov010 0x02111998 */
void hal_fill_trap_vtable(void);
void hal_fill_ov010_trap36_vtable(void);                /* _ZTV14daObjC1_Trap_c, 32 */
extern unsigned char g_profile_C1_HIKARI[];             /* ov010 0x02112b84 */
void *daObjC1Hikari_c_classInit(void);                            /* ov010 0x02111dd0 */
/* ---- gate 42: ov010's PEACH_PAINTING (38) ----
   The prior "blocked on ov052/RollingRock" reading was the shared-base illusion
   -- both mislabeled references stay inside ov010 (_ZN13PeachPainting20UpdateModelTransformEv and the
   ov010 BSS SharedFilePtr data_ov010_02112d64), closed by /alternatename in
   hal/actor_classes.cpp. Its SpawnInfo is mounted (ov010_syms.txt, +4 = 38). */
extern unsigned char g_profile_C1_PEACH[];        /* ov010 0x02112c44 */
void *daObjC1Peach_c_classInit(void);                       /* ov010 0x02112004 */
void hal_fill_peach_painting_vtable(void);
/* ---- gate 50: ov080's PAINTING (daPicGate_c, actor 307) ----
   A DIFFERENT class from PEACH_PAINTING, in a different overlay, with no
   cross-overlay spawn dependency. Its record's +4 halfword reads 307 and the
   +0 word is daPicGate_c_classInit (ov080 0x02126f8c), which the registry repoints to
   the host factory. Its vtable is the 31-slot Actor table dsd left unnamed at
   _ZTV8Painting; hal/actor_classes_painting.cpp declares that as the
   host array daPicGate_c_classInit installs, and hal_fill_painting_vtable fills it. */
extern unsigned char g_profile_PICTURE_GATE[];             /* ov080 0x02128290 */
void *daPicGate_c_classInit(void);                            /* ov080 0x02126f8c */
void hal_fill_painting_vtable(void);
/* ---- gate 23: ov102's QUESTION_BLOCK ----
   The fifth overlay, and the last id on the castle grounds' skip list that is
   not a 2D system. Its name checks out against the ROM's RTTI
   (daObjHatenaBlock_c) and its SpawnInfo's own +4 halfword. */
extern unsigned char g_profile_HATENA_BLOCK[];         /* ov102 0x0214e3e8 */
void *daObjHatenaBlock_c_classInit_HATENA_BLOCK(void);                        /* ov102 0x0214a8ec */
void hal_fill_question_block_vtable(void);
/* ---- gate 27: the BOTTOM SCREEN'S OWN ------------------------------------
   HUD (334), one of the two ids the castle-grounds census has been printing
   as skipped. Its SpawnInfo is ov002 0x0210c210, which config names
   g_profile_METER, and the ROM puts the CONSTRUCTOR in the record's +0
   word -- there is no separate Spawn function to repoint. Its vtable is
   eighteen slots, not twenty; see the table read out of the ROM at the head
   of hal/sub_actors.cpp. */
extern unsigned char g_profile_METER[];      /* ov002 0x0210c210 */
int *dMeter_c_classInit(void);                         /* ov002 0x020fe154 */
void hal_fill_hud_vtable(void);
/* ---- gate 28: the MINIMAP -------------------------------------------------
   The other id (335) the census printed as skipped, and the same shape as the
   HUD: SpawnInfo at ov002 0x0210c160 with the CONSTRUCTOR in the +0 word, and
   an eighteen-slot vtable. Its object is 600 bytes rather than the HUD's 124.
   It is also the actor that turns BG3-sub on -- InitResources ORs bit 3 into
   the layer mask hal/sub_screen.cpp publishes every frame. */
extern unsigned char g_profile_MAP[];  /* ov002 0x0210c160 */
int *dMap_c_classInit(void);                     /* ov002 0x020fb8bc */
void hal_fill_minimap_vtable(void);
/* ---- gate 33: BOB-OMB BATTLEFIELD's mechanisms, terrain and pickups -------
   Level 6, ov014. Every id here was read out of that level's own object
   tables and resolved through the EU ACTOR_SPAWN_TABLE at arm9 0x02090864;
   hal/actor_classes_bob_world.cpp carries the evidence and the vtable fills.
   Kept as one contiguous block so the merge with the enemy stream, which
   appends its own block, is a straight concatenation. */
extern unsigned char g_profile_COIN[];                  /* ov002 0x02108790 */
extern unsigned char g_profile_RED_COIN[];               /* ov002 0x021087ac */
extern unsigned char g_profile_BLUE_COIN[];              /* ov002 0x021087c8 */
void *daCoin_c_classInit_COIN(void);                                 /* ov002 0x020b2b48 */
void *daCoin_c_classInit_RED_COIN(void);                              /* ov002 0x020b2af0 */
void *daCoin_c_classInit_BLUE_COIN(void);                             /* ov002 0x020b2a98 */
void hal_fill_coin_vtable(void);
/* BRICK_BLOCK has no fill of its own: its factory installs
   _ZTV13BigBrickBlock, which gate 16 already fills for BLACK_BRICK_BLOCK. */
extern unsigned char g_profile_BLOCK_L[];            /* ov002 0x02108a24 */
void *daObjBlockL_c_classInit_BLOCK_L(void);                           /* ov002 0x020b4708 */
/* gate 48: BIG_BRICK_BLOCK (16) and FORTRESS_TOWER_WALL (46), Whomp's Fortress.
   Both factories install _ZTV13BigBrickBlock too -- no fill of their own, same
   as BRICK_BLOCK. */
extern unsigned char g_profile_DP_BLOCK_L[];         /* ov002 0x02108a40 */
void *daObjBlockL_c_classInit_DP_BLOCK_L(void);                        /* ov002 0x020b40fc */
extern unsigned char g_profile_BK_FUTA[];     /* ov002 0x02108a94 */
void *daObjBlockL_c_classInit_BK_FUTA(void);                    /* ov002 0x020b406c */
extern unsigned char g_profile_BLK_SKINOKO_TAG[];  /* ov002 0x02108ba0 */
void *daObjBlockItemTag_c_classInit_BLK_SKINOKO_TAG(void);
void hal_fill_mega_mushroom_block_tag_vtable(void);
/* gate 49: ONE_UP_MUSHROOM_BLOCK_TAG (321), the same _ZTV10BrickBlock tag body
   as MEGA_MUSHROOM_BLOCK_TAG -- no fill of its own. */
extern unsigned char g_profile_BLK_OKINOKO_TAG[]; /* ov002 0x02108bf4 */
void *daObjBlockItemTag_c_classInit_BLK_OKINOKO_TAG(void);                /* ov002 0x020b4670 */
extern unsigned char g_profile_CANNON_SHUTTER[];           /* ov002 0x02109d14 */
void *daObjCannonShutter_c_classInit(void);
void hal_fill_cannon_hatch_vtable(void);
extern unsigned char g_profile_OBJ_MARIO_CAP[];                   /* ov002 0x021095cc */
void *daObjMarioCap_c_classInit(void);
void hal_fill_cap_vtable(void);
extern unsigned char g_profile_HEART[];          /* ov002 0x02109c50 */
void *daObjHeart_c_classInit(void);
void hal_fill_healing_heart_vtable(void);
extern unsigned char g_profile_HANSWITCH[];     /* ov002 0x02109900 */
void *daObjSwitch_c_classInit_HANSWITCH(void);
void hal_fill_exclamation_switch_vtable(void);
extern unsigned char g_profile_SECRET_COIN[];       /* ov002 0x0210b00c */
void *daSCoin_c_classInit(void);
void hal_fill_invisible_secret_vtable(void);
/* gate 46: INVISIBLE_POLE (287), RTTI daBar_c. Host-storage vtable, filled by
   hand (the InvisibleSecret case). */
extern unsigned char g_profile_BAR[];         /* ov002 0x0210845c */
void *daBar_c_classInit(void);                        /* ov002 0x020b0710 */
void hal_fill_invisible_pole_vtable(void);
/* lane SEAT-CAMERATAG: CAMERA_TAG (333), RTTI daCamTag_c. Its table is the
   config's _ZTV10daCamTag_c (0x0210853c), host storage filled by hand. */
extern unsigned char g_profile_CAMERA_TAG[];             /* ov002 0x02108518 */
void *daCamTag_c_classInit(void);                            /* ov002 0x020b07c8 */
void hal_fill_camera_tag_vtable(void);
extern unsigned char g_profile_YAJIRUSI_L[];         /* ov098 0x0213c398 */
void *daObjYajirusi_c_classInit_YAJIRUSI_L(void);
extern unsigned char g_profile_YAJIRUSI_R[];        /* ov098 0x0213c3b4 */
void *daObjYajirusi_c_classInit_YAJIRUSI_R(void);
void hal_fill_arrow_sign_vtable(void);
extern unsigned char g_profile_WATERBOMB[];             /* ov098 0x0213c74c */
void *daWbm_c_classInit(void);
void hal_fill_water_bomb_vtable(void);
extern unsigned char g_profile_IRONBALL[];       /* ov100 0x02147f58 */
void *daIbl_c_classInit(void);
void hal_fill_rolling_iron_ball_vtable(void);
extern unsigned char g_profile_DOKAN[];              /* ov102 0x0214e134 */
void *daObjSimpleBg_c_classInit_DOKAN(void);
void hal_fill_warp_pipe_vtable(void);
extern unsigned char g_profile_WARPKUN[];                  /* ov002 0x0210ac98 */
void *daWarpkun_c_classInit(void);                                 /* ov002 0x020ec534 */
void hal_fill_warp_vtable(void);
/* ---- gate 32: BOB-OMB BATTLEFIELD'S LIVING CAST ---------------------------
   The first ENEMY-family classes the port hosts, which is why they are in a
   file of their own (hal/actor_classes_bob_enemy.cpp): their vtables are
   THIRTY-ONE slots where every class above is twenty, because Actor appends
   thirteen virtuals of its own past ActorBase's eighteen.
   The ids are the ROM's own, cross-checked three ways: symbols/overlay_actors.md
   (built from ACTOR_SPAWN_TABLE at arm9 0x02090864), each SpawnInfo's +4
   halfword, and each class's RTTI name. */
extern unsigned char g_profile_BOMBHEI[];        /* ov102 0x0214e534 */
void *daBmb_c_classInit(void);                       /* ov102 0x0214c6f8 */
void hal_fill_bob_omb_vtable(void);
extern unsigned char g_profile_SHELL[];    /* ov102 0x0214e62c, id 285 */
void *daShl_c_classInit(void);                   /* ov102 0x0214d6b4 */
void hal_fill_koopa_shell_vtable(void);
/* ---- gate 210 (lane SEAT-OV002-RUNTIME): run-time spawns from linked code ---- */
extern unsigned char g_profile_FPAKUN_BALL[];      /* ov002 0x0210bf70, id 254 */
void *daFPknBall_c_classInit(void);                     /* ov002 0x020f9304 */
void hal_fill_fireball_vtable(void);
extern unsigned char g_profile_FIRERING[]; /* ov060 0x0211ab30, id 281 */
void *daFRing_c_classInit(void);             /* ov060 0x021191f4 */
void hal_fill_bowser_shockwaves_vtable(void);
extern unsigned char g_profile_OBJ_ABUKU[];        /* ov002 0x02108940, id 291 */
void *daObjAbuku_c_classInit(void);                       /* ov002 0x020b3568 */
void hal_fill_bubble_vtable(void);
extern unsigned char g_profile_YOSHI_EGG[];      /* ov002 0x0210ad90, id 9 */
void *daYegg_c_classInit(void);                     /* ov002 0x020ee3d8 */
void hal_fill_yoshi_egg_vtable(void);
extern unsigned char g_profile_KURIBO[];        /* ov084 0x021308ec */
void *daKrb_c_classInit_KURIBO(void);                       /* ov084 0x0212c0b0 */
void hal_fill_goomba_vtable(void);
extern unsigned char g_profile_RED_BOMBHEI[];   /* ov084 0x02130a14 */
void *daRedBombhei_c_classInit(void);                  /* ov084 0x0212d200 */
void hal_fill_bob_omb_buddy_vtable(void);
/* ov014 is Bob-omb Battlefield's OWN level overlay, mounted twice (whole for
   the loaders, per symbol for these two classes' tables) -- the gate-17 shape
   applied to a second level. */
extern unsigned char g_profile_WANWAN[];      /* ov014 0x02114750 */
void *daWanwan_c_classInit(void);                     /* ov014 0x02112d1c */
void hal_fill_chain_chomp_vtable(void);
extern unsigned char g_profile_WANWAN_SHUTTER[]; /* ov014 0x0211488c */
void *daObjWanwanShutter_c_classInit(void);                /* ov014 0x0211307c */
void hal_fill_chain_chomp_fence_vtable(void);
/* gate 82: SHUTTER_BOB (40), the same ov014 mount as the chain chomp. A
   Platform subclass (RTTI daObjBSwdoor_c); SpawnInfo and data_ov014_021145c4
   already come from ov014_syms.txt, so this is a HAL-only addition. */
extern unsigned char g_profile_SWITCHDOOR[];      /* ov014 0x021145e4 */
void *daObjBSwdoor_c_classInit(void);                     /* host copy, unmatched/ */
void hal_fill_shutter_bob_vtable(void);
/* gate 83: SEESAW_BOB (39), the fresh ov095 mount. A Platform subclass (RTTI
   daObjSeesaw_c); SpawnInfo and the three file tables come from ov095_syms.txt,
   the sinits from hal/actor_overlays.cpp. */
extern unsigned char g_profile_BOMB_SEESAW[];       /* ov095 0x02137484 */
void *daObjSeesaw_c_classInit_BOMB_SEESAW(void);                      /* ov095 0x02135f94 */
void hal_fill_seesaw_bob_vtable(void);
extern unsigned char g_profile_RACE_NOKO[];   /* ov062 0x0211db78 */
void *daRNk_c_classInit(void);                  /* ov062 0x0211aee0 */
void hal_fill_koopa_the_quick_vtable(void);
/* Actor 27, the post the chomp is chained to. Its CLASS is daObjPile_c, not
   the Stump the config's _ZTV5Stump names -- see the section header in
   hal/actor_classes_bob_enemy.cpp. The SpawnInfo and factory names are the
   right ones: that record's +4 halfword reads 27. */
extern unsigned char g_profile_PILE[];           /* ov091 0x02135298 */
void *daObjPile_c_classInit(void);                          /* ov091 0x02133938 */
void hal_fill_stump_vtable(void);
extern unsigned char g_profile_RACE_FLAG[];       /* ov062 0x0211dc30 */
void *daRFlag_c_classInit(void);                      /* ov062 0x0211b208 */
void hal_fill_koopa_flag_vtable(void);
/* ov078 is KING_BOB_OMB's whole overlay, and level 6 is the only level of the
   fifty-two that loads it. */
extern unsigned char g_profile_BOMBKING[];      /* ov078 0x02126e28 */
void *daBombking_c_classInit(void);                     /* ov078 0x021265fc */
void hal_fill_king_bob_omb_vtable(void);
/* ---- gates 60-63: Whomp's Fortress' mechanical platforms (ov015) ----------
   Wired by the vtable each factory installs, not the class name the config
   carries (the ov015 name shift; see hal/actor_classes_wf.cpp). SpawnInfo
   records from the ov015 per-symbol mount (ov015_syms.txt); each +4 halfword
   is the id below. */
extern unsigned char g_profile_BK_LIFT01[];       /* ov015 0x0211462c, id 52 */
void *daObjBk_Lift_c_classInit(void);                      /* ov015 0x0211290c */
void hal_fill_tower_step_vtable(void);
extern unsigned char g_profile_BK_ROTEBAR[];  /* ov015 0x021146f0, id 51 */
void *daObjBk_Rotebar_c_classInit(void);                 /* ov015 0x02112ba0 */
void hal_fill_rotating_bridge_vtable(void);
extern unsigned char g_profile_BK_BILLBOARD[];   /* ov015 0x0211433c, id 42 */
void *daObjBkBillboard_c_classInit(void);                  /* ov015 0x021112dc */
void hal_fill_pole_billboard_vtable(void);
extern unsigned char g_profile_BK_BOTAOSI[];  /* ov015 0x021143fc, id 44 */
void *daObjBk_Botaosi_c_classInit(void);                 /* ov015 0x02111b68 */
void hal_fill_knock_down_plank_vtable(void);
/* MOVING_BAR_BIG (53) and MOVING_BAR_SMALL (54) share vtable 0x0211458c (the
   _ZN14KnockDownPlank* methods); both factories install it. KnockDownPlank::
   Behavior forms its per-frame pointer-to-member over a FORWARD-DECLARED struct,
   so a host copy (port/unmatched/KnockDownPlank_Behavior.cpp) reads the table as
   plain ints; the same file seats both {fn, delta} tables. See the note in
   hal/actor_classes_wf.cpp. */
extern unsigned char g_profile_BK_DOSSUNBAR_L[];    /* ov015 0x02114568, id 53 */
void *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L(void);                   /* ov015 0x02112230 */
extern unsigned char g_profile_BK_DOSSUNBAR_S[];  /* ov015 0x0211454c, id 54 */
void *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S(void);                 /* ov015 0x02112260 */
void hal_fill_moving_bar_vtable(void);            /* shared vtable 0x0211458c */
extern unsigned char g_profile_BK_UKISIMA[]; /* ov015 0x021147c4, id 50 */
/* its factory is a host wrapper: the raw daObjBk_Ukisima_c_classInit stores the
   VT1 placeholder rather than a named vtable, so the wrapper reseats the vptr
   onto the host table (hal/actor_classes_wf.cpp). */
void *port_factory_rotating_platform_wf(void);
void hal_fill_rotating_platform_wf_vtable(void);
/* ---- gates 70-74: Whomp's Fortress' level-7 enemies and platforms ----------
   Two ov084 piranhas and three ov091 platform/Platform-enemy classes, wired by
   the vtable each factory installs (the ov015 name shift twice more; see
   hal/actor_classes_l7.cpp). ov084 and ov091 are already mounted (gate 32); the
   SpawnInfo records come from ov084_syms.txt / ov091_syms.txt and each +4
   halfword is the id below. */
/* gate 72: THWOMP (161). Its factory leaves the vptr on the VT1 placeholder, so
   its factory is a host wrapper that reseats it, the RotatingPlatformWf case. */
extern unsigned char g_profile_DOSUN[];          /* ov091 0x02135150, id 161 */
void *port_factory_thwomp(void);
void hal_fill_thwomp_vtable(void);
/* gate 74: SLIDING_PLATFORM_WF (55). Clean vtable install, no wrapper. */
extern unsigned char g_profile_BK_TRANSBAR[]; /* ov091 0x02135008, id 55 */
void *daObjSimpleLift_c_classInit_BK_TRANSBAR(void);
void hal_fill_sliding_platform_wf_vtable(void);
/* gate 73: ROTATING_UP_DOWN_PLATFORM (29). The ov015 name shift: its factory
   installs the RotatingUpDownPlatformUtm table, so it runs Utm's methods. */
extern unsigned char g_profile_UPDOWN_LIFT[]; /* ov091 0x02134bf8, id 29 */
void *daObjRotateUpdownLift_c_classInit_UPDOWN_LIFT(void);
void hal_fill_rotating_up_down_platform_vtable(void);
/* run rel0215 wave 3 (lane w3-f2): the OTHER ov091 lift class, the one gate 73
   does NOT reach. ARROW_PATH_LIFT (157) and SQUARE_METAL_NET_LIFT (144) both
   install _ZTV22RotatingUpDownPlatform (ov091 0x02134ec0, RTTI 13daLinelift2_c,
   32 slots), which is a DIFFERENT table from gate 73's Utm one at 0x02134c5c --
   two ids, one class, one fill. The nine-record PMF state seat that serves both
   this class and FWOOSH is in the same file. See hal/actor_classes_ov091.cpp
   and port/slice_w3f2.txt. */
extern unsigned char g_profile_KM3_LIFT[];      /* ov091 0x02134e80, id 157 */
extern unsigned char g_profile_KM2_SUSUMU[]; /* ov091 0x02134e9c, id 144 */
void *daLinelift2_c_classInit_KM3_LIFT(void);                     /* ov091 0x021323cc */
void *daLinelift2_c_classInit_KM2_SUSUMU(void);                /* ov091 0x02132394 */
void hal_fill_rotating_up_down_platform_lift_vtable(void);  /* 32 slots */
/* run rel0215 wave 3 (lane w3-f2): FWOOSH (231), Stump / 10daHyuhyu_c, ov091
   0x021353ac, 31 slots, an Enemy subclass. NOT actor 27's table -- the config
   name _ZTV5Stump sits on FWOOSH's table and actor 27's is the unnamed
   0x021352bc the registry already fills, which port/ov091_syms.txt derived and
   this lane re-checked from the bytes. */
extern unsigned char g_profile_HYUHYU[];             /* ov091 0x02135388, id 231 */
void *daHyuhyu_c_classInit(void);                            /* ov091 0x021344a0 */
void hal_fill_fwoosh_vtable(void);                   /* 31 slots */
/* gate 71: FIRE_PIRANHA_PLANT (253). The ov015 name shift: its factory installs
   the FirePiranhaPlantBig table, so it runs that class. No state seat. */
extern unsigned char g_profile_PAKUN2[];  /* ov084 0x02130acc, id 253 */
void *daFPkn_c_classInit_PAKUN2(void);
void hal_fill_fire_piranha_plant_vtable(void);
/* gate 70: PIRANHA_PLANT (250). Clean vtable install; its nine-state PMF
   Behavior is a host copy (port/unmatched/PiranhaPlant_States.cpp). */
extern unsigned char g_profile_PAKUN[];      /* ov084 0x02130c04, id 250 */
void *daPkn_c_classInit(void);
void hal_fill_piranha_plant_vtable(void);
/* FALL_BLOCK_WF (45), vtable 0x021148dc (_ZTV20daObjBk_Fall_Block_c). The inherited
   ov098-chain blocker was a phantom: the Behavior _ZN16daObjFallBlock_c8BehaviorEv is a plain
   switch machine whose same-id walk is null-safe. Every slot is matched src; a
   plain 32-slot fill and a slice reach it. See hal/actor_classes_wf.cpp. */
extern unsigned char g_profile_BK_DOWN_B[];     /* ov015 0x021148b8, id 45 */
void *daObjBk_Fall_Block_c_classInit(void);                    /* ov015 0x02112dd0 */
void hal_fill_fall_block_wf_vtable(void);
/* ---- gates 64-69: Whomp's Fortress' enemy cast (ov079) --------------------
   Six classes, ids read from the arm9 ACTOR_SPAWN_TABLE (the ROM's numbers, not
   the task's shorthand) and each SpawnInfo's +4 halfword. WHOMP/WHOMP_KING
   share _ZTV5Whomp and FORTRESS_WALL/FORTRESS_WALL_BREAKABLE share
   _ZTV12FortressWall, so only the first of each pair carries the vtable fill;
   the second row's factory installs the same table (fill 0). SpawnInfo records
   from the ov079 per-symbol mount (ov079_syms.txt). See
   hal/actor_classes_wf_enemy.cpp. */
extern unsigned char g_profile_BATAN[];           /* ov079 0x02127c5c, id 164 */
void *daBtn_c_classInit_BATAN(void);                          /* ov079 0x021265e8 */
void hal_fill_whomp_vtable_c(void);
extern unsigned char g_profile_BATANKING[];       /* ov079 0x02127c40, id 165 */
void *daBtn_c_classInit_BATANKING(void);                      /* ov079 0x02126588 */
extern unsigned char g_profile_KILLER[];      /* ov079 0x02127ec4, id 222 */
void *daKlr_c_classInit(void);                     /* ov079 0x02126d64 */
void hal_fill_bullet_bill_vtable_c(void);
/* BULLET (233), Snufit's ov002 projectile: ov002 mounted, HAL + one-PMF host copy */
extern unsigned char g_profile_PROPELLER_HEYHO_FIRE[];          /* ov002 0x0210d630, +4 reads 233 */
void *daPropeller_Heyho_Fire_c_classInit(void);                         /* ov002 0x020fefcc, installs _ZTV6Bullet */
void hal_fill_bullet_vtable(void);
extern unsigned char g_profile_BK_KILLER_DAI[];     /* ov079 0x02127f94, id 43 */
void *daObjBkKillerdai_c_classInit(void);                    /* ov079 0x021271b4 */
void hal_fill_bill_blaster_vtable_c(void);
extern unsigned char g_profile_BK_KABE01[];    /* ov079 0x0212808c, id 47 */
void *daObjBk_Kabe_c_classInit_BK_KABE01(void);                   /* ov079 0x0212757c */
void hal_fill_fortress_wall_vtable_c(void);
extern unsigned char g_profile_BK_KABE00[]; /* ov079 0x02128070, id 48 */
void *daObjBk_Kabe_c_classInit_BK_KABE00(void);          /* ov079 0x021275ac */
/* ---- gate 89: POWER_STAR (178, ov002) -------------------------------------
   An Enemy subclass whose vtable _ZTV9PowerStar (ov002 0x0210ab3c, 31 slots,
   RTTI daStar_c) daStar_c_classInit_STAR installs directly. SpawnInfo record from the
   ov002 per-symbol mount (added to ov002_syms.txt); the record's +4 halfword
   reads 178. Its Behavior dispatches a fourteen-entry pointer-to-member state
   table, hosted in port/unmatched/PowerStar_States.cpp. See
   hal/actor_classes_star.cpp. */
extern unsigned char g_profile_STAR[];       /* ov002 0x0210aa40, id 178 */
void *daStar_c_classInit_STAR(void);                      /* ov002 0x020ebf30 */
void hal_fill_power_star_vtable(void);
/* ---- gate 90: STAR_CAMERA (177, ov002) ------------------------------------
   A bare Actor: StarCamera_Spawn installs NO vtable of its own, so it dispatches
   the base Actor vtable _ZTV5Actor (data_0208e3a4). hal_fill_actor_base_vtable
   fills that shared table -- provably safe, it is the only class that leaves it
   installed. SpawnInfo from the ov002 mount; the record's +4 halfword reads
   177. See hal/actor_classes_star.cpp. */
extern unsigned char g_profile_STAR_CAMERA[];      /* ov002 0x0210aa94, id 177 */
void *StarCamera_Spawn(void);                     /* ov002 0x020ebe5c */
void hal_fill_actor_base_vtable(void);
/* ---- gate 102: FORTRESS_TOWER (49, ov102) ---------------------------------
   Whomp's Fortress' tower, a Platform subclass (RTTI daObjSimpleBg_c). Its
   factory daObjSimpleBg_c_classInit_BK_TOWER installs _ZTV13FortressTower, the SAME table
   WARP_PIPE (id 298) installs and hal_fill_warp_pipe_vtable already fills, so
   this row carries no fill of its own. All five methods are already compiled
   (gate 33's WARP_PIPE slice) and g_profile_BK_TOWER is already mounted
   (ov102_syms.txt); the record's +4 halfword reads 49. See slice_gate102.txt
   and the WARP_PIPE section in hal/actor_classes_bob_world.cpp. */
extern unsigned char g_profile_BK_TOWER[];   /* ov102 0x0214e118, id 49 */
void *daObjSimpleBg_c_classInit_BK_TOWER(void);                  /* ov102 0x02148f2c */
/* ---- gate 103: STAR_SWITCH (12, ov002) ------------------------------------
   Whomp's Fortress' star switch, a Platform subclass (RTTI daObjSwitch_c). Its
   factory daObjSwitch_c_classInit_STAR_SWITCH installs _ZTV10StarSwitch, the SAME table
   EXCLAMATION_SWITCH (id 11) installs and hal_fill_exclamation_switch_vtable
   already fills (including the five-state PMF seat), so this row carries no
   fill of its own. All five methods are already compiled (gate 33's
   EXCLAMATION_SWITCH slice). This gate mounts g_profile_STAR_SWITCH (added to
   ov002_syms.txt); the record's +4 halfword reads 12. See slice_gate103.txt
   and the EXCLAMATION_SWITCH section in hal/actor_classes_bob_world.cpp. */
extern unsigned char g_profile_STAR_SWITCH[];      /* ov002 0x0210991c, id 12 */
void *daObjSwitch_c_classInit_STAR_SWITCH(void);                     /* ov002 0x020baaac */
/* ---- gate 128: EXCLAMATION_BLOCK (21, ov102) ------------------------------
   Jolly Roger Bay's ship sublevel (level 9) prints id 0x15 (21) skipped. Its
   factory daObjHatenaBlock_c_classInit_ITEM_BLOCK installs _ZTV13QuestionBlock, the SAME table
   QUESTION_BLOCK (id 20) installs and hal_fill_question_block_vtable already
   fills, so this row carries no fill of its own -- the FORTRESS_TOWER shape.
   All of QuestionBlock's methods are already compiled and its host vtable is
   already filled (gate 23), and g_profile_ITEM_BLOCK is already mounted
   (ov102_syms.txt); the record's +4 halfword reads 21. See slice_gate128.txt
   and the QUESTION_BLOCK section in hal/actor_classes.cpp. */
extern unsigned char g_profile_ITEM_BLOCK[]; /* ov102 0x0214e404, id 21 */
void *daObjHatenaBlock_c_classInit_ITEM_BLOCK(void);                /* ov102 0x0214a8ac */
/* ---- gate 121: STATIC_ROCK (61, ov102) ------------------------------------
   Jolly Roger Bay's rocks (level 8's biggest skipped class, x33), a Platform
   subclass. Its factory daObjSimpleBg_c_classInit_KI_IWA installs _ZTV13FortressTower, the SAME
   table FORTRESS_TOWER (id 49, gate 102) and WARP_PIPE (id 298, gate 33)
   install and hal_fill_warp_pipe_vtable already fills, so this row carries no
   fill of its own. All five methods are already compiled (gate 33's WARP_PIPE
   slice) and g_profile_KI_IWA is already mounted (ov102_syms.txt); the
   record's +4 halfword reads 61, matching ACTOR_SPAWN_TABLE[61]. See
   slice_gate121.txt and the WARP_PIPE section in hal/actor_classes_bob_world.cpp. */
extern unsigned char g_profile_KI_IWA[];      /* ov102 0x0214e0e0, id 61 */
void *daObjSimpleBg_c_classInit_KI_IWA(void);                     /* ov102 0x02148f8c */
/* ---- gate 122: BRICK_BLOCK_SWITCH_ACTIVATED (19, ov002) -------------------
   Jolly Roger Bay's switch-activated brick (x8). A Platform subclass whose
   factory installs _ZTV13BigBrickBlock, the SAME table BRICK_BLOCK (15),
   BIG_BRICK_BLOCK (16) and FORTRESS_TOWER_WALL (46) install and
   hal_fill_black_brick_block_vtable already fills (gate 16). No fill of its own.
   Its SpawnInfo is mounted in ov002_syms.txt; +4 reads 19, matching
   ACTOR_SPAWN_TABLE[19]. See slice_gate122.txt. */
extern unsigned char g_profile_SW_BLOCK_L[]; /* ov002 0x02108a5c */
void *daObjBlockL_c_classInit_SW_BLOCK_L(void);                /* ov002 0x020b40cc */
/* ---- gate 123: GREEN_SHELL_BLOCK_TAG (323, ov002) ------------------------
   Jolly Roger Bay's green-shell block tag (x1). A tag body sharing
   _ZTV10BrickBlock, the SAME table ONE_UP_MUSHROOM_BLOCK_TAG (321) and
   MEGA_MUSHROOM_BLOCK_TAG (322) install and hal_fill_brick_block_tag_vtable
   already fills (gate 49). No fill of its own. Its SpawnInfo is mounted in
   ov002_syms.txt; +4 reads 323, matching ACTOR_SPAWN_TABLE[323]. See
   slice_gate123.txt. */
extern unsigned char g_profile_BLK_GNSHELL_TAG[];        /* ov002 0x02108bbc */
void *daObjBlockItemTag_c_classInit_BLK_GNSHELL_TAG(void);                       /* ov002 0x020b4610 */
/* ---- gate 149: HAUNTED_CHAIR (326), Big Boo's Haunt (ov020) ----
   ov020 is level 12's own overlay, mounted whole and (gate 149) per symbol so
   the class reaches its SpawnInfo record and its model SharedFilePtr by name.
   The fill and factory are in hal/actor_classes_bbh.cpp. The level's other three
   furniture classes (BOOKEND 327, BOOK_SHOT_SPAWNER 328, BOOK_SHOT 325) stay
   skipped: BookShot's return-to-shelf behaviour calls an undecompiled ov063
   function, so they are documented blockers rather than a stubbed boot. */
extern unsigned char g_profile_CHAIR[];      /* ov020 0x021149f0 */
void *daChair_c_classInit(void);                     /* ov020 0x02113494 */
void hal_fill_haunted_chair_vtable(void);
/* ---- gate 172: CRATE (194), level 13's first fresh candidate (ov098) ----
   The breakable wooden box, daObjBlockS_c. ov098 was already mounted per
   symbol for the WaterBomb/FallBlockWf/ExclamationSwitch gates, so the
   SpawnInfo, the state-table statics and the file tables are all in place;
   the fill is hal/actor_classes_hmc.cpp, the closure slice_gate172.txt, and
   the state-table seat port/unmatched/Crate_StateDispatch.cpp. */
extern unsigned char g_profile_BLOCK_S[];             /* ov098 0x0213c510 */
void *daObjBlockS_c_classInit(void);                            /* ov098 0x02139f04 */
void hal_fill_crate_vtable(void);
/* ---- gate 173: UP_DOWN_LIFT_HMC (33), Hazy Maze Cave's second fresh class
   (ov095) ---- The up-and-down platform lift, daUdlift_c. ov095 was mounted
   per symbol at gate 83 (SEESAW_BOB); this gate adds g_profile_UDLIFT,
   its two variant-indexed file tables and its CLPS table to that mount
   (ov095_syms.txt). The fill is hal/actor_classes_updownlift.cpp, the closure
   slice_gate173.txt, the Behavior host copy + state seat
   port/unmatched/UpDownLiftBbh_Behavior.cpp. */
extern unsigned char g_profile_UDLIFT[];     /* ov095 0x021375e8, +4 reads 33 */
void *daUdlift_c_classInit_UDLIFT(void);                    /* ov095 0x021367cc (matched src) */
void hal_fill_up_down_lift_hmc_vtable(void);
/* ---- gate 174: MONTY_MOLE (310), first of ov080's enemy cast ----
   The burrowing mole, daChoropu_c. ov080 was mounted per symbol at gate 50
   (PAINTING) and already carries the SpawnInfo, state statics and
   SharedFilePtrs. The fill is hal/actor_classes_montymole.cpp, the closure
   slice_gate174.txt, the Behavior host copy + state seat
   port/unmatched/MontyMole_StateDispatch.cpp. */
extern unsigned char g_profile_CHOROPU[];         /* ov080 0x02127fec, +4 reads 310 */
void *daChoropu_c_classInit(void);                        /* ov080 0x021249e0 (matched src) */
void hal_fill_monty_mole_vtable(void);
/* ---- gate 208: MONTY_MOLE_ROCK (311), the rock the mole throws ----
   daChoro_Rock_c, ov080's second enemy class. The mole's throw state spawns
   0x137 and the port declined it, so the ROM-faithful thrower stored through
   the NULL. The whole cast is matched: no host copy, no state seat, no thunk.
   The fill is hal/actor_classes_montymolerock.cpp, the closure
   slice_gate208.txt, which also carries the 31-slot vtspan derivation and the
   slot-9 adjudication. */
extern unsigned char g_profile_CHORO_ROCK[];     /* ov080 0x02128008, +4 reads 311 */
void *daChoro_Rock_c_classInit(void);                    /* ov080 0x02124998 (matched src) */
void hal_fill_monty_mole_rock_vtable(void);
/* ---- gate 206: DIAMOND_LIFT (135), ov043's first seated class ----
   daObjKm1_Ukishima_c, six instances on level 35 (Bowser in the Dark World).
   ov043 is whole-mounted already and joins the per-symbol mounts here, the
   ov012/ov013/ov045 dual-mount shape. 32-slot Platform table, every vtspan
   route agreeing; the fill and the mount bring-up are
   hal/actor_classes_ov043.cpp, the closure slice_gate206.txt, and the four
   re-verified traps (G0/G1, the destructor's different G0, the two
   ov047-spelled cells, the VT0/VT1 placeholders) are derived there. Ids 134
   and 137 stay skipped with the reasons written up. */
extern unsigned char g_profile_KM1_UKISHIMA[];       /* ov043 0x02112294, +4 reads 135 */
void *daObjKm1_Ukishima_c_classInit(void);                      /* ov043 0x021113cc (matched src) */
void hal_fill_diamond_lift_vtable(void);
/* ---- lane SEAT-BDW: two of ov043's remaining three classes, ids pinned by
   Spawn-store, not by name (the naming shift, port/ov043_syms.txt). 136 and
   134 are fully matched and seated. link100 SPAWN2 adds the THIRD, id 137
   RICKSHAW_BDW: the "inferred stub with no match" reading that kept it out is
   retired by lane STUBADJ's rulings (0x021114c4 slot 0, 0x021114b0 slot 3 and
   0x0211144c slot 17 are each REAL_DECOMP), and slot 16's 0x021113fc never
   carried the marker at all. The NULL-deref the old note describes was the
   DECLINE path a trapped InitResources took, not the ROM body. See
   hal/actor_classes_ov043.cpp. */
extern unsigned char g_profile_KM1_KURUMA[]; /* ov043 0x02112438, +4 reads 136 */
extern unsigned char g_profile_KM1_DORIFU[];           /* ov043 0x021124fc, +4 reads 134 */
extern unsigned char g_profile_KM1_KURUMAJIKU[];         /* ov043 0x02112368, +4 reads 137 */
void *daObjKm1_Kuruma_c_classInit(void);                /* ov043 0x021115f4 (matched src, returns int*) */
void *daObjKm1_Dorifu_c_classInit(void);                          /* ov043 0x0211176c (matched src) */
void *daObjKm1_Kurumajiku_c_classInit(void);                        /* ov043 0x021114dc (matched src) */
void hal_fill_rickshaw_platform_bdw_vtable(void);
void hal_fill_stairs_bdw_vtable(void);
void hal_fill_rickshaw_bdw_vtable(void);              /* _ZTV21daObjKm1_Kurumajiku_c, 32 */
/* ---- gate 175: BLUE_FLAME (317) + RED_FLAME (316), the cave's flames ----
   One class (daObjFire_c, ov002), two matched factories installing
   _ZTV9BlueFlame; RED_FLAME's row is fill=0 (the GreenShellBlockTag shape).
   Level 13 names RED_FLAME x17. The fill and the Player::Burn face are
   hal/actor_classes_flame.cpp; the closure slice_gate175.txt; no state
   table, no seat. */
extern unsigned char g_profile_OBJ_BLUE_FIRE[];         /* ov002 0x02108f14, +4 reads 317 */
void *daObjFire_c_classInit_OBJ_BLUE_FIRE(void);                        /* ov002 0x020b59a8 (matched src) */
void hal_fill_blue_flame_vtable(void);
extern unsigned char g_profile_OBJ_RED_FIRE[];          /* ov002 0x02108ef8, +4 reads 316 */
void *daObjFire_c_classInit_OBJ_RED_FIRE(void);                         /* ov002 0x020b59e0 (matched src) */
/* ---- gate 176: SCUTTLEBUG (255), ov071's first mount ----
   The spider daSpd_c. Fresh per-symbol ov071 mount (ov071_syms.txt); the fill
   is hal/actor_classes_scuttlebug.cpp, the closure slice_gate176.txt, the two
   state dispatchers host-copied + the record seat in
   port/unmatched/Scuttlebug_StateDispatch.cpp. */
extern unsigned char g_profile_SPIDER[];        /* ov071 0x02122c08, +4 reads 255 */
void *daSpd_c_classInit(void);                       /* ov071 0x02120618 (matched src) */
void hal_fill_scuttlebug_vtable(void);
/* ---- run linkw wave 7, lane w7b: THE MR. I FAMILY, ov071's second cast ----
   MR_I (262) and BIG_MR_I (263) are ONE class behind ONE vtable -- daEykn_c_classInit_EYEKUN
   and daEykn_c_classInit_EYEKUN_BOSS are the same 0x50-byte function twice, both `new(0x218)`
   storing _ZTV3MrI, and MrI::InitResources tells the two apart by the actor's
   own id (0x106 -> scale 0x1000, 0x107 -> 0x2000). So 263 is a FREE SHARE of
   262's fill and carries a null filler in its row, the RED_FLAME shape.
   MR_I_PROJECTILE (264) is the shot MrI's state 1 fires through
   Actor::Spawn(0x108); it needs its own vtable and row or every shot would
   meet the pre-spawn gate as an unregistered id.
   ov071 was already mounted (gate 176); the fill is hal/actor_classes_ov071.cpp,
   the closure slice_w7b.txt, the two per-frame state MAINs + the two PMF
   dispatchers + the record seat + the slot-5 Render in port/unmatched/MrI_*.cpp.
   Ids re-derived from ACTOR_SPAWN_TABLE + each record's +4 halfword, NOT from
   port/ov071_syms.txt's header comment, which mis-attributes them to 198/199. */
extern unsigned char g_profile_EYEKUN[];               /* ov071 0x02122cf0, +4 reads 262 */
extern unsigned char g_profile_EYEKUN_BOSS[];            /* ov071 0x02122d0c, +4 reads 263 */
extern unsigned char g_profile_EYEKUN_BEAM[];    /* ov071 0x02122dc4, +4 reads 264 */
void *daEykn_c_classInit_EYEKUN(void);                              /* ov071 0x02121a1c (matched src) */
void *daEykn_c_classInit_EYEKUN_BOSS(void);                           /* ov071 0x021219cc (matched src) */
void *daEyBm_c_classInit(void);                   /* ov071 0x02121f9c (matched src) */
void hal_fill_mri_vtable(void);
void hal_fill_mri_projectile_vtable(void);
/* ---- gate 177: ov064's first mount -- BULLY (215), BIG_BULLY (216),
   ROTATING_FIREBAR (81), Lethal Lava Land's no-PMF trio. The bullies'
   factories leave the vptr on the BASE table (VT1), so the registry rows
   carry port_factory_* wrappers that reseat onto the host arrays -- the
   Thwomp treatment, here mandatory. Fills hal/actor_classes_ov064.cpp,
   closure slice_gate177.txt. */
extern unsigned char g_profile_DONKETU[];            /* ov064 0x0211b818, +4 reads 215 */
extern unsigned char g_profile_BOSS_DONKETU[];         /* ov064 0x0211b920, +4 reads 216 */
extern unsigned char g_profile_FL_KOMA_U[];  /* ov064 0x0211bdec, +4 reads 81  */
void *port_factory_bully(void);            /* wrapper: daDonketu_c_classInit + reseat vptr */
void *port_factory_big_bully(void);        /* wrapper: daBDonketu_c_classInit + reseat   */
void *daObjFl_KomaU_c_classInit(void);         /* installs _ZTV15RotatingFirebar itself */
void hal_fill_bully_vtable(void);
void hal_fill_big_bully_vtable(void);
void hal_fill_rotating_firebar_vtable(void);

/* ---- gate 178: ov064's second mount -- METAL_NET_LIFT (69, daObjFl_Amilift_c)
   and LAVA_BUBBLE (214). id 69's factory installs the Amilift Platform table by
   its RTTI name; both factories leave the REAL table (no reseat wrapper). Fills
   hal/actor_classes_ov064_gate178.cpp, closure slice_gate178.txt. */
extern unsigned char g_profile_FL_AMILIFT[];  /* ov064 0x0211bc44, +4 reads 69  */
extern unsigned char g_profile_BUBBLE[];    /* ov064 0x0211bec8, +4 reads 214 */
void *daObjFl_Amilift_c_classInit(void);   /* installs _ZTV17daObjFl_Amilift_c itself */
void *daBbl_c_classInit(void);     /* installs _ZTV10LavaBubble itself */
void hal_fill_metal_net_lift_vtable(void);
void hal_fill_lava_bubble_vtable(void);

/* ---- gate 179: ov064's BowserPuzzle pair (Lethal Lava Land). The config
   names cross the classes: id 78's factory installs _ZTV19BowserPuzzleManager
   (the 828-byte piece class), id 79 is a 216-byte coordinator shell on the
   mounted table _ZTV19BowserPuzzleManager. Both factories install their REAL table;
   no reseat wrappers. */
extern unsigned char g_profile_FL_PUZZLE[];    /* ov064 0x0211c160, +4 reads 78 */
extern unsigned char g_profile_FL_COIN[];  /* ov064 0x0211c17c, +4 reads 79 */
void *daObjFl_Puzzle_c_classInit(void);     /* installs _ZTV19BowserPuzzleManager */
void *port_factory_bowser_puzzle_shell(void); /* Manager_Spawn + reseat onto the host shell table */
void hal_fill_bowser_puzzle_pair_vtables(void);
void hal_fill_bowser_puzzle_shell_vtable(void);

/* ---- gate 182: ov062's KOOPA (203) and CHUCKYA (190), Bob-omb Battlefield's
   last two NPCs. ov062 already mounted (gate 32); HAL-only gate. Both
   factories install their OWN real table; no reseat wrappers. KOOPA's closure
   reaches one unmatched particle helper (func_ov062_02117724) -- stubbed
   name-and-abort in unmatched/KoopaChuckya_StateDispatch.cpp. */
extern unsigned char g_profile_HOLHEI[];   /* ov062 0x0211d9bc, +4 reads 190 */
extern unsigned char g_profile_NOKONOKO[];     /* ov062 0x0211da74, +4 reads 203 */
void *daHolhei_c_classInit(void);   /* installs _ZTV7Chuckya itself */
void *daNknk_c_classInit_NOKONOKO(void);     /* installs _ZTV5Koopa itself */
/* KLEPTO (239), the cap-stealing bird: ov062 already mounted (gate 32), HAL-only
   seat. Two-PMF-dispatcher class; see hal/actor_classes_koopa_chuckya.cpp. */
extern unsigned char g_profile_JANGO[];    /* ov062 0x0211dd38, +4 reads 239 */
void *daJango_c_classInit(void);                   /* ov062 0x0211ce80, installs _ZTV6Klepto */
void hal_fill_klepto_vtable(void);
void hal_fill_chuckya_vtable(void);
void hal_fill_koopa_vtable(void);

/* ---- gate 188: ov016's six MOVER classes (Jolly Roger Bay). Per-symbol mount
   ov016_syms.txt; fills hal/actor_classes_jrb.cpp; closure slice_gate188.txt.
   Four host vtables (Unagi 31-slot Enemy, ShipUp 32 + RockPillar 32 + FloatOn-
   Water 37, all Platform). ShipDown SHARES _ZTV6ShipUp and daObjKi_Fune_c_classInit_KI_FUNE_UP's body;
   SlidingBox SHARES _ZTV23FloatOnWaterPlatformJrb and installs it directly.
   daObjKi_Ita_c_classInit's LAST vptr write is VT1 (the shared placeholder),
   so it takes a reseat wrapper (the bully treatment); the other five factories
   install their real table (or a shared real table) and need none.
   THE CONFIG CLASS NAMES ARE A DECOY (the gate-178 Amilift pattern), read from
   ov016's relocs: id 60 (FloatOnWaterPlatformJrb) installs _ZTV13daObjKi_Ita_c,
   a 32-slot daObjKi_Ita_c BASE table with arm9 Platform defaults for
   Behavior/Cleanup/Render and _ZN13daObjKi_Ita_c13InitResourcesEv/ef4/f44 for Init/D1/D0 -- so
   it is the SIMPLE base with no slot-5 render and no coupling. id 313
   (SlidingBox) installs _ZTV23FloatOnWaterPlatformJrb, the 37-slot DERIVED table
   whose real _ZN23FloatOnWaterPlatformJrb* methods carry BOTH the slot-5 Render
   collision AND the COUPLED Behavior (case 0 does FindWithActorID(0x39=SHIP_UP)
   and self-destructs if the ship is absent), so id 313 only survives with
   SHIP_UP hosted -- which this gate does. Both factories install their real
   table as the last vptr write; neither needs a reseat wrapper. */
extern unsigned char g_profile_MORAY[];                    /* ov016 0x021148e0, +4 reads 242 */
extern unsigned char g_profile_KI_FUNE[];                 /* ov016 0x021149fc, +4 reads 56  */
extern unsigned char g_profile_KI_FUNE_UP[];                   /* ov016 0x02114a18, +4 reads 57  */
extern unsigned char g_profile_KI_HASIRA[];               /* ov016 0x02114adc, +4 reads 58  */
extern unsigned char g_profile_KI_ITA[];  /* ov016 0x02114ba8, +4 reads 60  */
extern unsigned char g_profile_SLIDE_BOX[];               /* ov016 0x02114c68, +4 reads 313 */
void *daMoray_c_classInit(void);                    /* installs _ZTV5Unagi itself */
void *daObjKi_Fune_c_classInit_KI_FUNE_UP(void);                   /* installs _ZTV6ShipUp (shared by ShipDown) */
void *daObjKi_Fune_c_classInit_KI_FUNE(void);                 /* same body as daObjKi_Fune_c_classInit_KI_FUNE_UP */
void *daObjKi_Hasira_c_classInit(void);               /* installs _ZTV10RockPillar (RockPillar table) */
void *port_factory_float_on_water_jrb(void);/* id 60: daObjKi_Ita_c_classInit writes the VT1 placeholder as its last
                                               vptr write, so this wrapper reseats p[0] onto the host _ZTV13daObjKi_Ita_c table */
void *daSlide_Box_c_classInit(void);               /* id 313: installs _ZTV23FloatOnWaterPlatformJrb (derived) directly */
void hal_fill_unagi_vtable(void);
void hal_fill_ship_up_vtable(void);
void hal_fill_rock_pillar_vtable(void);
void hal_fill_float_on_water_jrb_vtable(void);   /* fills _ZTV13daObjKi_Ita_c for id 60 */
void hal_fill_sliding_box_vtable(void);          /* fills _ZTV23FloatOnWaterPlatformJrb for id 313 */
/* ---- run linkw wave 4: ov045's platform cast (Bowser in the Fire Sea) ----
   Six classes, twenty instances on level 37. ov045 was whole-mounted in wave 3
   and joins the per-symbol mounts for this (port/ov045_syms.txt), so every
   SpawnInfo below is real ROM storage the registry and the class code share.
   THE CLASS NAMES HERE ARE THE SPAWN NAMES, NOT THE METHOD OR _ZTV NAMES: dsd
   ends each mwcc class block at its Spawn and then labels the block as if it
   started there, so the bodies and vtable labels sit one class late. Five
   independent reads settle every row -- the arm9 spawn table, the raw SpawnInfo
   id halfword, the Spawn address in the record, each table's slot 0, and each
   Spawn's own vtable store. Full derivation in hal/actor_classes_ov045.cpp.
   Three factories are wrappers: their matched Spawn's LAST vptr write is the
   shared-header VT1 placeholder (a zeroed host array), so the wrapper reseats
   p[0] onto the class's host table, the port_factory_rotating_platform_wf
   recipe. The other three store a real name and are registered directly. */
extern unsigned char g_profile_KM2_AGARU[];    /* ov045 0x02112cd0, +4 reads 140 */
void *daObjKm2_Agaru_c_classInit(void);                   /* ov045 0x021114dc */
extern unsigned char g_profile_KM2_AMI_BOU[];           /* ov045 0x02112d98, +4 reads 142 */
void *daObjKm2_Ami_Bou_c_classInit(void);                          /* ov045 0x02111808 */
extern unsigned char g_profile_KM2_NOBIRU[];  /* ov045 0x02112e5c, +4 reads 148 */
void *daObjKm2_Nobiru_c_classInit(void);                 /* ov045 0x02111ad4 */
extern unsigned char g_profile_KM2_UKISHIMA[];   /* ov045 0x02112f2c, +4 reads 145 */
void *port_factory_floating_floor_bfs(void);         /* VT1 wrapper over ov045 0x02111bf4 */
extern unsigned char g_profile_KM2_KUZURE[];       /* ov045 0x021130d0, +4 reads 139 */
void *port_factory_fall_block_bfs(void);             /* VT1 wrapper over ov045 0x02111e24 */
/* wave 5 (lane w5-c): the sixth class, released by the hosted sqrt unit */
extern unsigned char g_profile_KM2_GURA[]; /* ov045 0x02112ffc, +4 reads 141 */
void *port_factory_tilting_platform_bfs(void);       /* VT1 wrapper over ov045 0x02111d0c */
void hal_fill_fire_sea_elevator_vtable(void);        /* fills _ZTV15FireSeaElevator, 32 slots */
void hal_fill_pole_lift_vtable(void);                /* fills _ZTV15FireSeaElevator, 32 */
void hal_fill_extending_platform_vtable(void);       /* fills _ZTV8PoleLift, 31 */
void hal_fill_floating_floor_bfs_vtable(void);       /* fills _ZTV19daObjKm2_Ukishima_c, 32 */
void hal_fill_fall_block_bfs_vtable(void);           /* fills _ZTV21daObjKm2_Fall_Block_c, 32 */
void hal_fill_tilting_platform_bfs_vtable(void);     /* fills _ZTV15daObjKm2_Gura_c, 32 */

/* ---- run linkw wave 5 (lane w5-e): the Bowser fight arena's floor ----
   ov060 joins the per-symbol mounts (port/ov060_syms.txt, which carries the
   whole seven-class cast map, the SpikeBomb/SkyPlatform dsd label shift and
   the width pins). ONE class registers this wave: BOWSER_FIRE_SEA_ARENA
   (166, daKpa2Bg_c), level 38's walkable floor. The other six are measured
   and banked -- Bowser's two raw state-pair tables and SkyPlatform's bss
   Entry dispatch need StateDispatch seats first, and Bowser without
   BowserTail is a null child spawn (hal/actor_classes_ov060.cpp's header). */
extern unsigned char g_profile_KOOPA2BG[]; /* ov060 0x0211a88c, +4 reads 166 */
void *daKpa2Bg_c_classInit(void);                /* ov060 0x02117cdc */
void hal_fill_bowser_fire_sea_arena_vtable(void);    /* fills _ZTV18BowserFireSeaArena, 32 */
/* ---- run linkw wave 6 (lane w6-A): THE FIGHT ITSELF ----
   The other five ov060 classes wave 5 measured and banked, plus ov089's two.
   Every one of the seven factories stores its table by a REAL name, so all
   seven register directly -- no VT1 wrappers anywhere in this cast.
   THE COUPLING IS MEASURED, NOT STYLISTIC: Bowser::InitResources spawns
   0x118 (280) and 0x116 (278) and writes through both results with no null
   check, and his defeat path (func_ov060_021135fc) spawns 0x11a (282) and
   writes through that one too. 279 alone, or 279+278+280 without 282, is a
   null-pointer store, not a smaller step. The state tables all seven read are
   seated in port/unmatched/Ov060_StateDispatch.cpp. */
extern unsigned char g_profile_KIRAI[];          /* ov060 0x0211aa68, +4 reads 284 */
void *daKirai_c_classInit(void);                         /* ov060 0x02118cbc */
extern unsigned char g_profile_KOOPA[];             /* ov060 0x0211a5f4, +4 reads 279 */
void *daKpa_c_classInit(void);                            /* ov060 0x02116428 */
extern unsigned char g_profile_KOOPATAIL[];         /* ov060 0x0211a610, +4 reads 278 */
void *daKpaTail_c_classInit(void);                        /* ov060 0x021163f0 */
extern unsigned char g_profile_KOOPAFIRE[];         /* ov060 0x0211a7d0, +4 reads 280 */
void *daKpaFire_c_classInit(void);                        /* ov060 0x02117938 */
extern unsigned char g_profile_KOOPA3BG[];  /* ov060 0x0211a964, +4 reads 167 */
void *daKpa3Bg_c_classInit(void);                 /* ov060 0x02118408 */
extern unsigned char g_profile_OBJ_KEY[];                /* ov089 0x02132b68, +4 reads 282 */
void *daObjKey_c_classInit_OBJ_KEY(void);                               /* ov089 0x02132828 */
extern unsigned char g_profile_LAST_STAR[];           /* ov089 0x02132b84, +4 reads 283 */
void *daObjKey_c_classInit_LAST_STAR(void);                          /* ov089 0x021327d0 */
void hal_fill_spike_bomb_vtable(void);          /* fills _ZTV17BowserSkyPlatform, 31 */
void hal_fill_bowser_vtable(void);              /* fills _ZTV6Bowser, 31 */
void hal_fill_bowser_tail_vtable(void);         /* fills _ZTV10BowserTail, 31 */
void hal_fill_bowser_fire_vtable(void);         /* fills _ZTV10BowserFire, 31 */
void hal_fill_bowser_sky_platform_vtable(void); /* fills _ZTV17BowserSkyPlatform, 32 */
void hal_fill_key_vtable(void);                 /* fills _ZTV3Key, 31 (both ids) */
/* ---- run linkw wave 5 (lane w5-c): ov070's enemy pack + the two engine-side
   census finds for level 37. SpawnInfo records from the fresh per-symbol
   ov070 mount (port/ov070_syms.txt; ids re-read from the raw overlay image,
   NO naming shift in this overlay -- see hal/actor_classes_ov070.cpp) and,
   for 196, from the existing ov002 mount. All four factories store their own
   host table by real name, so all four register directly; every fill is in
   hal/actor_classes_ov070.cpp.

   FLYGUY (232) IS ON NOW (lane w3-d). This note used to end "FlyGuy (232)
   stays OFF: no bootable level spawns it", which was true when wave 5 wrote
   it. Level 19 (Snowman's Land) places FlyGuy and level 19 is mounted: the
   baseline census on cons cc10acd4e reads "[spawn] actor 0xe8 not
   registered, skipped", and 0xe8 is 232. port/ov070_syms.txt had already
   done the hard half -- the vtable left out as host storage and all twelve
   PMF sources rewritten with host addresses -- and predicted the rest
   ("a future FlyGuy row then needs only a row and a fill"). It needed one
   thing more than that: g_profile_PROPELLER_HEYHO was not mounted, and is now. */
extern unsigned char g_profile_PROPELLER_HEYHO[];             /* ov070 0x02123144, +4 reads 232 */
void *daPropeller_Heyho_c_classInit(void);                            /* ov070 0x02120520 */
extern unsigned char g_profile_BIRIKYU[];                /* ov070 0x02123254, +4 reads 266 */
void *daBrq_c_classInit(void);                               /* ov070 0x021210ac */
extern unsigned char g_profile_KERONPA[];         /* ov070 0x0212334c, +4 reads 270 */
void *daKrpa_c_classInit(void);                        /* ov070 0x02121af8 */
extern unsigned char g_profile_KERONPA_FIRE[];     /* ov070 0x02123424, +4 reads 271 */
void *daKpFr_c_classInit(void);                    /* ov070 0x021221fc */
extern unsigned char g_profile_LAVA[]; /* ov002 0x021093bc, +4 reads 196 */
void *daObjLava_c_classInit(void);                /* ov002 0x020b6dd8 */
void hal_fill_fly_guy_vtable(void);                   /* fills _ZTV6FlyGuy, 31 */
void hal_fill_amp_vtable(void);                      /* fills _ZTV3Amp, 31 */
void hal_fill_flame_chomp_vtable(void);              /* fills _ZTV10FlameChomp, 31 */
void hal_fill_flame_chomp_fire_vtable(void);         /* fills _ZTV14FlameChompFire, 31 */
void hal_fill_popping_lava_bubbles_vtable(void);     /* fills _ZTV11daObjLava_c, 31 */
/* ---- run linkw wave 5 (lane w5-B): Hazy Maze Cave's cast ----
   ov065's HMC subset + ov021's resident trio, both freshly per-symbol
   mounted (port/ov065_syms.txt, port/ov021_syms.txt). THE CAST MAP WAS
   RE-DERIVED, five reads per class, and the names are NOT shifted in either
   overlay -- the full derivation and every table width is in
   hal/actor_classes_ov065.cpp. Six factories store their own table by a
   real name and register directly; daObjCvShutter_c_classInit is the ov045 factory-F
   shape (base first, own second, own spelled VT1) and gets the wrapper. */
extern unsigned char g_profile_YUREI_MUCHO[];        /* ov065 0x0211cb80, +4 reads 236 */
void *daYurei_Mucho_c_classInit(void);                       /* ov065 0x02116f48 */
extern unsigned char g_profile_BASABASA[];         /* ov065 0x0211cc7c, +4 reads 237 */
void *daBasabasa_c_classInit(void);                        /* ov065 0x02117ee8 */
extern unsigned char g_profile_DOSSY[];        /* ov065 0x0211cd84, +4 reads 168 */
void *daDossy_c_classInit(void);                       /* ov065 0x02119634 */
extern unsigned char g_profile_DOSSY_CAP[];     /* ov065 0x0211cda0, +4 reads 169 */
void *daDossyCap_c_classInit(void);                    /* ov065 0x021195ec */
extern unsigned char g_profile_CV_NEWS_LIFT[];  /* ov021 0x02114768, +4 reads 68 */
void *daObjCvNewsLift_c_classInit(void);                 /* ov021 0x02112168 */
extern unsigned char g_profile_GORO_ROCK[];   /* ov021 0x02114824, +4 reads 221 */
void *daGrock_c_classInit(void);                  /* ov021 0x02112d64 */
extern unsigned char g_profile_CV_SHUTTER[];    /* ov021 0x021148f0, +4 reads 67 */
void *port_factory_shutter_hmc(void);           /* VT1 wrapper over ov021 0x02112ed4 */
void hal_fill_snufit_vtable(void);              /* fills _ZTV6Snufit, 31 */
void hal_fill_swoop_vtable(void);               /* fills _ZTV5Swoop, 31 */
void hal_fill_dorrie_vtable(void);              /* fills _ZTV6Dorrie, 31 */
void hal_fill_dorrie_cap_vtable(void);          /* fills _ZTV9DorrieCap, 31+2 */
void hal_fill_work_elevator_vtable(void);       /* fills _ZTV12WorkElevator, 32 */
void hal_fill_rolling_rock_vtable(void);        /* fills _ZTV11RollingRock, 31 */
void hal_fill_shutter_hmc_vtable(void);         /* fills _ZTV16daObjCvShutter_c, 32 */
/* ---- run rel0215 wave 3 (lane w3-a2): ov065's TICK TOCK CLOCK cluster ----
   ELEVEN ids over SEVEN classes and seven 32-slot Platform tables. Every row
   below was fixed FOUR independent ways and the derivation is in
   port/ov065_syms.txt's wave-3 section: (1) the arm9 spawn table at
   0x02090864 + id*4 in extracted/arm9_dec.bin BASED AT 0x02004000; (2) each
   record's own +4 id halfword out of extracted/overlays/overlay_0065.bin;
   (3) each factory's literal pool, for which table it stores; (4) each D0's
   pool, for which table it restores. The registry's own boot check is a fifth
   -- it compares each row's id against the record's +4 halfword and says so.

   READ THE ID, NEVER THE PREFIX. dsd's BODY names are shifted from 0x0211d3b4
   on and its SpawnInfo names are not, so daObjCtKaitendai_c_classInit_CT_MECHA06 is id 114's
   FACTORY while _ZN15TtcRotatingGear* are ids 116/117's BODIES. The fill names
   below are spelled for the RTTI class (daObjCt*), which is the only spelling
   right on both halves.

   Ids 110 and 113 have no dsd SpawnInfo name and no dsd _ZTV name: dsd
   labelled both their tables plain data, so their records are reached by
   address and their factories are func_ov065_*. They are ov065's own classes
   all the same -- the RTTI strings at each table[-1] read 16daObjCtMecha03_c
   and 16daObjCtMecha05_c out of ov065's own image, and ovdata.py's cross pass
   independently found nine __si_class_type_info records in this overlay where
   the old header accounted for seven. */
extern unsigned char g_profile_CT_MECHA01[];      /* 0x0211cfe8, +4 = 108 */
void *daObjCtRotateBlock_c_classInit_CT_MECHA01(void);                     /* 0x02119efc */
extern unsigned char g_profile_CT_MECHA02[];     /* 0x0211d004, +4 = 109 */
void *daObjCtRotateBlock_c_classInit_CT_MECHA02(void);                    /* 0x02119ebc */
extern unsigned char g_profile_CT_MECHA03[];            /* 0x0211d0c8, +4 = 110 */
void *daObjCtMecha03_c_classInit(void);                       /* 0x0211a45c */
extern unsigned char g_profile_CT_MECHA04L[]; /* 0x0211d1c8, +4 = 111 */
void *daObjCtMecha04_c_classInit_CT_MECHA04L(void);                /* 0x0211aae0 */
extern unsigned char g_profile_CT_MECHA04S[]; /* 0x0211d1ac, +4 = 112 */
void *daObjCtMecha04_c_classInit_CT_MECHA04S(void);                /* 0x0211ab20 */
extern unsigned char g_profile_CT_MECHA05[];        /* 0x0211d290, +4 = 113 */
void *daObjCtMecha05_c_classInit(void);                       /* 0x0211b328 */
extern unsigned char g_profile_CT_MECHA06[];      /* 0x0211d374, +4 = 114 */
void *daObjCtKaitendai_c_classInit_CT_MECHA06(void);                     /* 0x0211b7b8 */
extern unsigned char g_profile_CT_MECHA07[];  /* 0x0211d390, +4 = 115 */
void *daObjCtKaitendai_c_classInit_CT_MECHA07(void);                 /* 0x0211b780 */
extern unsigned char g_profile_CT_MECHA08A[];       /* 0x0211d454, +4 = 116 */
void *daObjCtMecha08_c_classInit_CT_MECHA08A(void);                      /* 0x0211bbac */
extern unsigned char g_profile_CT_MECHA08B[];       /* 0x0211d470, +4 = 117 */
void *daObjCtMecha08_c_classInit_CT_MECHA08B(void);                      /* 0x0211bb7c */
extern unsigned char g_profile_CT_MECHA09[];       /* 0x0211d544, +4 = 118 */
void *daObjCtMecha09_c_classInit(void);                      /* 0x0211c040 */
void hal_fill_ct_rotate_block_vtable(void);  /* 0x0211d028, 32, ids 108/109 */
void hal_fill_ct_mecha03_vtable(void);       /* 0x0211d0ec, 32, id  110     */
void hal_fill_ct_mecha04_vtable(void);       /* 0x0211d1ec, 32, ids 111/112 */
void hal_fill_ct_mecha05_vtable(void);       /* 0x0211d2b4, 32, id  113     */
void hal_fill_ct_kaitendai_vtable(void);     /* 0x0211d3b4, 32, ids 114/115 */
void hal_fill_ct_mecha08_vtable(void);       /* 0x0211d494, 32, ids 116/117 */
void hal_fill_ct_mecha09_vtable(void);       /* 0x0211d568, 32, id  118     */
/* ---- run linkw wave 5 (lane w5-A): ov063, Big Boo's Haunt's own cast ----
   Ten ids, six tables, first ov063 mount (port/ov063_syms.txt). The id map
   was re-derived from the raw image + the arm9 spawn table and this config
   has NO ov045-style naming shift -- every SpawnInfo's word[0] is its own
   name's Spawn and its +4 halfword the id (derivation in
   hal/actor_classes_ov063.cpp). BigBoo shares Boo's table (its own matched
   Spawn stores &_ZTV7daTrs_c); the four furniture ids share MansionSteps'
   (one daTrsTrap_c class, four records, mode off the id at +0xc). Only
   FallBlockBbh needs a factory wrapper: its Spawn's last vptr write is the
   shared-header VT1 placeholder, the port_factory_fall_block_bfs shape.
   MansionSteps' InitResources SPAWNS ids 65 and 213 at runtime and writes
   through the results unchecked, so 65's own row and BOOK_SWITCH 213 (ov020,
   the straggler block) must stay registered with 158. */
extern unsigned char g_profile_TERESA[];            /* ov063 0x0211e770, +4 reads 209 */
void *daTrs_c_classInit_TERESA(void);                           /* ov063 0x0211c590 */
extern unsigned char g_profile_BOSS_TERESA[];         /* ov063 0x0211e78c, +4 reads 210 */
void *daTrs_c_classInit_BOSS_TERESA(void);                        /* ov063 0x0211c520 */
extern unsigned char g_profile_ICON_TERESA[];     /* ov063 0x0211e7c4, +4 reads 211 */
void *daTrsIcon_c_classInit(void);                    /* ov063 0x0211c4a0 */
extern unsigned char g_profile_T_BASKET[];        /* ov063 0x0211e7a8, +4 reads 212 */
void *daTBasket_c_classInit(void);                       /* ov063 0x0211c4d0 */
extern unsigned char g_profile_KAIDAN[];   /* ov063 0x0211ea10, +4 reads 65 */
void *daTrsTrap_c_classInit_KAIDAN(void);                  /* ov063 0x0211d360 */
extern unsigned char g_profile_BOOKSHELF[];      /* ov063 0x0211ea2c, +4 reads 158 */
void *daTrsTrap_c_classInit_BOOKSHELF(void);                     /* ov063 0x0211d320 */
extern unsigned char g_profile_MERRYGOROUND[];   /* ov063 0x0211ea48, +4 reads 159 */
void *daTrsTrap_c_classInit_MERRYGOROUND(void);                  /* ov063 0x0211d2e0 */
extern unsigned char g_profile_TERESAPIT[];       /* ov063 0x0211ea64, +4 reads 160 */
void *daTrsTrap_c_classInit_TERESAPIT(void);                      /* ov063 0x0211d2a0 */
extern unsigned char g_profile_TH_DOWN_B[];   /* ov063 0x0211eb34, +4 reads 66 */
void *port_factory_fall_block_bbh(void);         /* VT1 wrapper over ov063 0x0211d47c */
extern unsigned char g_profile_PIANO[];       /* ov063 0x0211ed10, +4 reads 249 */
void *daPiano_c_classInit(void);                      /* ov063 0x0211e128 */
void hal_fill_boo_vtable(void);                  /* fills _ZTV7daTrs_c, 31 (209+210) */
void hal_fill_big_boo_icon_vtable(void);         /* fills _ZTV10BigBooIcon, 31 */
void hal_fill_boo_cage_vtable(void);             /* fills _ZTV7BooCage, 31 */
void hal_fill_mansion_steps_vtable(void);        /* fills _ZTV12MansionSteps, 31 (x4 ids) */
void hal_fill_fall_block_bbh_vtable(void);       /* fills _ZTV12FallBlockBbh, 32 */
void hal_fill_mad_piano_vtable(void);            /* fills _ZTV8MadPiano, 32 */
/* ---- w5-A stragglers: the ov020 BOOK CAST + UP_DOWN_LIFT_BBH (ov095) ----
   Four book ids on two tables (all four records config-`ambiguous`, each
   settled by its own bytes; the 213 record and factory are UNNAMED in
   config -- g_profile_BOOK_SWITCH / daBook_c_classInit_BOOK_SWITCH). 213 is what
   Bookshelf's InitResources spawns 3x unchecked. UpDownLiftBbh (32) is the
   third id of gate 173's daUdlift_c trio and reuses its fill wholesale. */
extern unsigned char g_profile_KILLER_BOOK[];        /* ov020 0x02114860, +4 reads 327 */
void *daBook_c_classInit_KILLER_BOOK(void);                       /* ov020 0x02112880 */
extern unsigned char g_profile_SHOOT_BOOK[];       /* ov020 0x021148b4, +4 reads 325 */
void *daBook_c_classInit_SHOOT_BOOK(void);                      /* ov020 0x021128dc */
extern unsigned char g_profile_BOOK_SWITCH[];      /* BookSwitch record, +4 reads 213 */
void *daBook_c_classInit_BOOK_SWITCH(void);                 /* BookSwitch factory */
extern unsigned char g_profile_BOOK_GENERATOR[]; /* ov020 0x0211487c, +4 reads 328 */
void *daBookGen_c_classInit(void);               /* ov020 0x02112850 */
void hal_fill_book_shot_vtable(void);            /* fills _ZTV8BookShot, 31 (327/325/213) */
void hal_fill_book_shot_spawner_vtable(void);    /* fills _ZTV15BookShotSpawner, 31 */
extern unsigned char g_profile_UDLIFT_TERESA[];  /* ov095 0x021375cc, +4 reads 32 */
void *daUdlift_c_classInit_UDLIFT_TERESA(void);                 /* ov095 0x021367fc */
extern unsigned char g_profile_BATTA_BLOCK[];    /* ov080 0x02128174, +4 reads 193 */
void *daBttBk_c_classInit(void);                   /* ov080 0x021253b4 */
void hal_fill_crazed_crate_vtable(void);         /* fills _ZTV11CrazedCrate, 31 */
extern unsigned char g_profile_CASKET[];         /* ov071 0x02122eb0, +4 reads 64 */
void *daObjCasket_c_classInit(void);                        /* ov071 0x02122670 */
void hal_fill_coffin_vtable(void);               /* fills _ZTV6Coffin, 32 */
/* ---- run linkw wave 6 (lane w6-f): FLAMETHROWER (318, ov095) ----
   daObjFlamethrower_c, the wall jet of fire. The one class on the board that
   skips on FOUR mounted levels at once (13 x3, 14 x5, 15 x3, 37 x3). Lane w5-C
   left it BLOCKED because Behavior has no matched TU; this gate hosts that one
   method as a per-instruction host copy
   (port/unmatched/Flamethrower_Behavior.c) and slices the other four
   (slice_w6f.txt). The fill is hal/actor_classes_flamethrower.cpp, 31 slots,
   with the reloc-pinned slot map in its banner. */
extern unsigned char g_profile_OBJ_FLAMETHROWER[];   /* ov095 0x021376cc, +4 reads 318 */
void *daObjFlamethrower_c_classInit(void);                  /* ov095 0x02136ed4 */
void hal_fill_flamethrower_vtable(void);         /* fills _ZTV12Flamethrower, 31 */
/* ---- run linkw wave 6 (lane w6-D): PATH_LIFT (31), ov100's leaf over
   ov002's PathLiftActor_c. Both routes agree: arm9 spawn table
   0x02090864 + 31*4 = 0x020908e0 holds 0x02148558, and that record read RAW
   out of extracted/overlays/overlay_0100.bin is
   {0x02147328, 0x012b001f, ...} -- word[0] is daObjPathLift_c_classInit inside ov100
   and word[1]'s low half is 31. The full hierarchy, both 33-slot widths and
   the ov006 naming trap are in hal/actor_classes_ov100pl.cpp. ---- */
extern unsigned char g_profile_PATH_LIFT[];       /* ov100 0x02148558, +4 reads 31 */
void *daObjPathLift_c_classInit(void);                      /* ov100 0x02147328 */
void hal_fill_path_lift_vtable(void);            /* fills data_ov100_0214857c, 33 */
/* ---- run linkw wave 7 (lane w7-c): ORANGE_BALL_BILLBOARD (301), the ONE
   class either new boss arena needed. Its overlay is the LEVEL overlay, not an
   actor overlay: arm9 spawn table 0x02090864 + 301*4 = 0x02090d18 holds
   0x021115e0, and that record read RAW out of extracted/overlays/
   overlay_0044.bin (ov044 is compressed:true, so the dsd export is the wrong
   image) is {0x021112dc, 0x00be012d, ...} -- word[0] is
   daObjKb1Billboard_c_classInit inside ov044's own load window and word[1]'s low
   half is 301. RTTI at vtable[-1] names the class daObjKb1Billboard_c (kb1 =
   koopa boss 1); the width pin, the held-out D1 and the cross-overlay
   SharedFilePtr spelling are in hal/actor_classes_ov044.cpp. x16 on level 36,
   placed on no other level. ---- */
extern unsigned char g_profile_KB1_BILLBOARD[]; /* ov044 0x021115e0, +4 reads 301 */
void *daObjKb1Billboard_c_classInit(void);                /* ov044 0x021112dc */
void hal_fill_orange_ball_billboard_vtable(void);     /* _ZTV19daObjKb1Billboard_c, 31 */
/* ---- run linkw wave 8 (lane w8-packs): ov022's LETHAL LAVA LAND cast, five
   of the eleven ids level 14's own level overlay owns. Every id resolved by
   BOTH routes -- the SpawnInfo's word[0] spawnFunc lands inside ov022 and its
   word[1] low half is the id the arm9 table slot came from -- and the class
   identity taken from the RTTI typeinfo at vtable[-1], not from dsd's labels
   (two of the four blocks carry the ov045-style one-class name shift). The
   derivation, the widths and the four blocked classes are in
   hal/actor_classes_ov022.cpp and port/ov022_syms.txt.
   FLOATING_FLOOR_LLL_BIG shares SMALL's table and fill (the BOO/BIG_BOO
   shape) and both take a VT1 factory wrapper; the other three store their own
   table by a real name as the final write. ---- */
extern unsigned char g_profile_FL_RING[];    /* ov022 0x02113cf4, +4 reads 71 */
void *daObjFl_Ring_c_classInit(void);                   /* ov022 0x02111578 */
extern unsigned char g_profile_FL_BLOCK[]; /* ov022 0x02113e88, +4 = 74 */
void *daObjFl_Block_c_classInit(void);           /* ov022 0x02111950 */
extern unsigned char g_profile_FL_UKIYUKA[];   /* ov022 0x021140ec, +4 = 75 */
void *port_factory_floating_floor_lll_big(void);   /* VT1 wrapper, ov022 0x021120f4 */
extern unsigned char g_profile_FL_UKIYUKA_L[]; /* ov022 0x02114108, +4 = 76 */
void *port_factory_floating_floor_lll_small(void); /* VT1 wrapper, ov022 0x021120b8 */
extern unsigned char g_profile_FL_UKI_KI[];      /* ov022 0x021141cc, +4 reads 82 */
void *daObjFl_UkiKi_c_classInit(void);                     /* ov022 0x02112350 */
void hal_fill_volcano_ring_vtable(void);         /* fills _ZTV11VolcanoRing, 32 */
void hal_fill_float_on_lava_platform_vtable(void); /* _ZTV19FloatOnLavaPlatform, 32 */
void hal_fill_floating_floor_lll_vtable(void);   /* _ZTV21FloatingFloorLllSmall, 32
                                                    (75 + 76) */
void hal_fill_lava_plank_vtable(void);           /* _ZTV9LavaPlank, 32 */
/* ---- run linkw wave 9 (lane w9-reg): TILTING_PLATFORM_LLL (72, ov064) ----
   ACTOR_SPAWN_TABLE 0x02090984 -> 0x0211bd08; the raw record there (out of
   extracted/overlays/overlay_0064.bin, NOT the compressed dsd export) has
   word[0] = daObjFl_Gura_c_classInit inside ov064 and its +4 halfword reads 72,
   so both attribution routes agree. Table 0x0211bd2c, 32 slots pinned twice
   (reloc run and next-symbol landing both end at 0x0211bdac); RTTI at
   vtable[-1] names it daObjFl_Gura_c, gura = the tilting one, the same root as
   ov045's TiltingPlatformBfs. x14, the biggest class in the wave-8 census.
   The factory column is a WRAPPER because the Spawn's final vptr store is the
   zeroed VT1 placeholder -- see hal/actor_classes_ov064_w9.cpp. ---- */
extern unsigned char g_profile_FL_GURA[];  /* ov064 0x0211bd08, +4 reads 72 */
void *port_factory_tilting_platform_lll(void);        /* wraps ov064 0x021180fc */
void hal_fill_tilting_platform_lll_vtable(void);      /* _ZTV14daObjFl_Gura_c, 32 */
/* ---- run linkw wave 9 (lane w9-reg): three ov084 SHARED-TABLE siblings ----
   Sweeping every spawn-table entry against ov084's raw image shows ids 200/201/
   202 all storing 0x02130948 and ids 251/252/253 all storing 0x02130b28. Both
   tables are ALREADY host arrays that an already-registered id fills
   (_ZTV6Goomba[31] in hal/actor_classes_bob_enemy.cpp, _ZTV19FirePiranhaPlantBig
   [31] in hal/actor_classes_l7.cpp), and all three SpawnInfo records are already
   mounted in port/ov084_syms.txt at the record's real 0x1c. So these three rows
   reuse the existing fills and add nothing but their own factories.
   Each Spawn stores its table by a REAL name -- no wrapper -- and each is
   shape-identical to the sibling already registered beside it. ---- */
extern unsigned char g_profile_KURIBO_L[];   /* ov084 0x02130924, +4 reads 202 */
void *daKrb_c_classInit_KURIBO_L(void);                  /* ov084 0x0212bff8, 1144 B, CapEnemy */
extern unsigned char g_profile_FIREPAKUN[];   /* ov084 0x02130ae8, +4 = 251 */
void *daFPkn_c_classInit_FIREPAKUN(void);                  /* 556 B, Enemy */
extern unsigned char g_profile_FIREPAKUN_S[]; /* ov084 0x02130b04, +4 = 252 */
void *daFPkn_c_classInit_FIREPAKUN_S(void);                /* 556 B, Enemy */
/* ---- run linkw wave 9 (lane w9-reg): three ov091 SLIDING PLATFORM siblings --
   SEVEN ids store the table 0x02135080 (55, 124, 138, 146, 147, 154, 155) and
   id 55 is already registered, so _ZTV17SlidingPlatformWf[32] in
   hal/actor_classes_l7.cpp is already filled before any of these spawn. All
   three SpawnInfo records are already mounted in port/ov091_syms.txt at 0x1c,
   and all three Spawns are the same 816-byte Platform shape as the registered
   daObjSimpleLift_c_classInit_BK_TRANSBAR with a single real-name vptr store -- no wrapper. ---- */
extern unsigned char g_profile_KM1_DERU[];    /* ov091 0x02134f60, +4 = 138 */
void *daObjSimpleLift_c_classInit_KM1_DERU(void);                   /* ov091 0x02132908 */
extern unsigned char g_profile_KM3_DERU01[]; /* ov091 0x02134f7c, +4 = 154 */
void *daObjSimpleLift_c_classInit_KM3_DERU01(void);                /* ov091 0x021328d8 */
extern unsigned char g_profile_KM3_DERU02[]; /* ov091 0x02134f98, +4 = 155 */
void *daObjSimpleLift_c_classInit_KM3_DERU02(void);                /* ov091 0x021328a8 */
/* ---- run linkw wave 9 (lane w9-reg): three ov095 SEESAW siblings -- SEVEN ids
   store the table 0x021374fc (28, 39, 128, 133, 143, 149, 150) and id 39
   (SEESAW_BOB) is already registered, so _ZTV9SeesawBob[32] in
   hal/actor_classes_bob_world.cpp is already filled before these spawn. The
   three records join the mount at 0x1c, the stride port/ov095_syms.txt already
   measured for every record in the overlay. All three Spawns are the same
   808-byte Platform shape as daObjSeesaw_c_classInit_BOMB_SEESAW with a single real-name vptr
   store -- no wrapper. ---- */
extern unsigned char g_profile_KM1_SEESAW[];        /* ov095 0x021373f8, +4 = 133 */
void *daObjSeesaw_c_classInit_KM1_SEESAW(void);                       /* ov095 0x02135f64 */
extern unsigned char g_profile_KM3_SEESAW[];    /* ov095 0x02137430, +4 = 149 */
void *daObjSeesaw_c_classInit_KM3_SEESAW(void);                   /* ov095 0x02135f04 */
extern unsigned char g_profile_KM3_YOKOSEESAW[];   /* ov095 0x0213744c, +4 = 150 */
void *daObjSeesaw_c_classInit_KM3_YOKOSEESAW(void);                  /* ov095 0x02135ed4 */
/* ---- run linkw wave 10 (lane w10-shared): ov031's SLIDE DECORATION QUAD.
   FOUR ids on ONE ROM table (0x02111984, _ZTV18daObjHsBillboard_c ==
   _ZTV25SlideDecorationSilverStar), x3 each on level 23. The class reads its
   variant out of its own actor id -- InitResources switches on the +0xc id
   halfword over 0x12e..0x131 and indexes one four-entry SharedFilePtr table --
   so ONE fill serves all four rows, and it is idempotent, so the order the
   registry installs them in does not matter. See hal/actor_classes_ov031_w10.cpp
   and port/slice_w10a.txt. ---- */
extern unsigned char g_profile_HS_MOON[];   /* ov031 0x0211190c, +4 = 302 */
void *daObjHsBillboard_c_classInit_HS_MOON(void);                  /* ov031 0x021113ec */
extern unsigned char g_profile_HS_STAR[];   /* ov031 0x02111928, +4 = 303 */
void *daObjHsBillboard_c_classInit_HS_STAR(void);                  /* ov031 0x021113b4 */
extern unsigned char g_profile_HS_Y_STAR[]; /* ov031 0x02111944, +4 = 304 */
void *daObjHsBillboard_c_classInit_HS_Y_STAR(void);                /* ov031 0x0211137c */
extern unsigned char g_profile_HS_B_STAR[];   /* ov031 0x02111960, +4 = 305 */
void *daObjHsBillboard_c_classInit_HS_B_STAR(void);                  /* ov031 0x02111344 */
void hal_fill_slide_decoration_vtable(void);     /* _ZTV18daObjHsBillboard_c, 31 */
/* ---- run linkw wave 10 (lane w10-shared): MEGA_MUSHROOM (277, ov002) x1 on
   level 22. Its factory stores _ZTV13OneUpMushroom -- the SAME ROM table
   0x021083c8 the already-registered ONE_UP_MUSHROOM (276) installs -- so it
   needs no fill of its own and the row below names 276's deliberately. One
   record in port/ov002_syms.txt, one TU in port/slice_w10b.txt, no vtable and
   no new mount. ---- */
extern unsigned char g_profile_SCALEUP_KINOKO[];   /* ov002 0x021083a4, +4 = 277 */
void *da1up_c_classInit_SCALEUP_KINOKO(void);                  /* ov002 0x020b0530 */
/* ---- run linkw wave 11 (lane w11): NINETEEN ids on ten tables that a
   REGISTERED sibling already fills. The widened sweep -- all 512 spawn-table
   ids resolved to (owner overlay, the table their own Spawn stores), not the
   ~70 that appear in a level census -- is what makes these visible: each one's
   overlay is already mounted and its table is already a host array in
   walk_window.map, so the whole registration is a record, a factory and a row.
   Thirteen of the nineteen already had their record mounted; the other six
   gained one line each in port/ov095_syms.txt / port/ov002_syms.txt. Every
   reused fill was read for a done-guard first -- see port/slice_w11.txt, which
   carries both attribution routes for all nineteen, the ten tables with the
   sibling that fills each, and the guard evidence per fill. ---- */
/* ov102, table 0x0214e1d8 _ZTV13FortressTower, filled by id 298 */
extern unsigned char g_profile_KI_HASIRA_DAI[];       /* ov102 0x0214e0fc, +4 = 59 */
void *daObjSimpleBg_c_classInit_KI_HASIRA_DAI(void);                      /* ov102 0x02148f5c, 48 B */
extern unsigned char g_profile_MC_DODAI[];           /* ov102 0x0214e150, +4 = 340 */
void *daObjSimpleBg_c_classInit_MC_DODAI(void);                          /* ov102 0x02148ecc, 48 B */
extern unsigned char g_profile_MC_HAZAD[];        /* ov102 0x0214e16c, +4 = 341 */
void *daObjSimpleBg_c_classInit_MC_HAZAD(void);                       /* ov102 0x02148e9c, 48 B */
/* ov102, table 0x0214e47c _ZTV13QuestionBlock, filled by id 20 */
extern unsigned char g_profile_VS_ITEM_BLOCK[]; /* ov102 0x0214e3cc, +4 = 22 */
void *daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK(void);                /* ov102 0x0214a92c, 64 B */
extern unsigned char g_profile_CAP_BLOCK_M[];      /* ov102 0x0214e420, +4 = 23 */
void *daObjHatenaBlock_c_classInit_CAP_BLOCK_M(void);                     /* ov102 0x0214a86c, 64 B */
extern unsigned char g_profile_CAP_BLOCK_W[];      /* ov102 0x0214e458, +4 = 24 */
void *daObjHatenaBlock_c_classInit_CAP_BLOCK_W(void);                     /* ov102 0x0214a7ec, 64 B */
extern unsigned char g_profile_CAP_BLOCK_L[];      /* ov102 0x0214e43c, +4 = 25 */
void *daObjHatenaBlock_c_classInit_CAP_BLOCK_L(void);                     /* ov102 0x0214a82c, 64 B */
/* ov091, table 0x02135080 _ZTV17SlidingPlatformWf, filled by 55/138/154/155 */
extern unsigned char g_profile_KM2_RIFUT01[];    /* ov091 0x02134fd0, +4 = 147 */
void *daObjSimpleLift_c_classInit_KM2_RIFUT01(void);                   /* ov091 0x02132848 */
extern unsigned char g_profile_KM2_RIFUT02[]; /* ov091 0x02134fec, +4 = 146 */
void *daObjSimpleLift_c_classInit_KM2_RIFUT02(void);                /* ov091 0x02132818 */
extern unsigned char g_profile_RC_RIFT01[];           /* ov091 0x02134fb4, +4 = 124 */
void *daObjSimpleLift_c_classInit_RC_RIFT01(void);                          /* ov091 0x02132878 */
/* ov091, table 0x02134c5c _ZTV23daObjRotateUpdownLift_c, filled by id 29
   (the host array is spelled _ZTV25RotatingUpDownPlatformUtm and aliased) */
extern unsigned char g_profile_HS_UPDOWN_LIFT[];   /* ov091 0x02134c14, +4 = 30 */
void *daObjRotateUpdownLift_c_classInit_HS_UPDOWN_LIFT(void);                  /* ov091 0x02131ba4, 56 B */
/* ov084 0x02130948 _ZTV6Goomba (200/202) and ov062 0x0211dab4 _ZTV5Koopa (203) */
extern unsigned char g_profile_KURIBO_S[];        /* ov084 0x02130908, +4 = 201 */
void *daKrb_c_classInit_KURIBO_S(void);                       /* ov084 0x0212c054, 92 B */
extern unsigned char g_profile_NOKONOKO_S[];         /* ov062 0x0211da90, +4 = 204 */
void *daNknk_c_classInit_NOKONOKO_S(void);                        /* ov062 0x021196bc, 80 B */
/* ov095, table 0x021374fc _ZTV9SeesawBob, filled by 39/133/149/150 */
extern unsigned char g_profile_KM2_YOKOSEESAW[];          /* ov095 0x02137414, +4 = 143 */
void *daObjSeesaw_c_classInit_KM2_YOKOSEESAW(void);                         /* ov095 0x02135f34, 48 B */
extern unsigned char g_profile_SEESAW[];          /* ov095 0x021373dc, +4 = 28 */
void *daObjSeesaw_c_classInit_SEESAW(void);                         /* ov095 0x02135fc4, 48 B */
extern unsigned char g_profile_RC_SEESAW[];           /* ov095 0x02137468, +4 = 128 */
void *daObjSeesaw_c_classInit_RC_SEESAW(void);                          /* ov095 0x02135ea4, 48 B */
/* ov095, table 0x02137628 _ZTV13UpDownLiftBbh, filled by 32/33 */
extern unsigned char g_profile_RC_RIFT02[];       /* ov095 0x02137604, +4 = 131 */
void *daUdlift_c_classInit_RC_RIFT02(void);                      /* ov095 0x0213679c, 48 B */
/* ov002, 0x0210ab3c _ZTV8daStar_c (178) and 0x02108c18 _ZTV10BrickBlock (322) */
extern unsigned char g_profile_SILVER_STAR[];         /* ov002 0x0210aa5c, +4 = 179 */
void *daStar_c_classInit_SILVER_STAR(void);                        /* ov002 0x020ebed4, 92 B */
extern unsigned char g_profile_BLK_SLVSTAR_TAG[]; /* ov002 0x02108bd8, +4 = 324 */
void *daObjBlockItemTag_c_classInit_BLK_SLVSTAR_TAG(void);                /* ov002 0x020b45e0, 48 B */
/* ---- run linkw wave 12 (lane w12): ov073, the whole cast of level 49 ----
   170/171/172 are ONE C++ class under three ids -- the three Spawns are the
   same 0x30-byte body and all three store the same table, 0x021231e8, so 171
   and 172 take the `0` fill column and row 170 carries the one fill (the
   RED_FLAME / BIG_MR_I idiom). CHIEF_CHILLY's factory is a host one: his
   matched Spawn is an ARM r0 ride-through onto Enemy::C2. Derivations in
   port/ov073_syms.txt and hal/actor_classes_ov073.cpp. */
extern unsigned char g_profile_EWB_ICE_A[];           /* ov073 0x02123168, +4 = 170 */
void *daObjEwbIce_c_classInit_EWB_ICE_A(void);                          /* ov073 0x02122844, 48 B */
extern unsigned char g_profile_EWB_ICE_B[];          /* ov073 0x02123184, +4 = 171 */
void *daObjEwbIce_c_classInit_EWB_ICE_B(void);                         /* ov073 0x02122814, 48 B */
extern unsigned char g_profile_EWB_ICE_C[];        /* ov073 0x021231a0, +4 = 172 */
void *daObjEwbIce_c_classInit_EWB_ICE_C(void);                       /* ov073 0x021227e4, 48 B */
void hal_fill_ccc_arena_vtable(void);
extern unsigned char g_profile_KING_DONKETU[];        /* ov073 0x0212306c, +4 = 218 */
void *port_factory_chief_chilly(void);               /* ov073 0x02121ec8, 200 B */
void hal_fill_chief_chilly_vtable(void);
/* ---- run linkw wave 12 (lane w12): ov052's two platform classes, level 44 ----
   Both Spawns name their receiver and store a real table name, so both
   register directly. Derivations in port/ov052_syms.txt and
   hal/actor_classes_ov052.cpp; id 174's dsd block is unnamed, so its record
   and factory carry address names. */
extern unsigned char g_profile_EMM_LOG[];          /* ov052 0x021124fc, +4 = 174 */
void *daObjEmmLog_c_classInit(void);                     /* ov052 0x02111410, 48 B */
void hal_fill_bobbing_log_emm_vtable(void);
extern unsigned char g_profile_EMM_YUKA[];     /* ov052 0x021125cc, +4 = 175 */
void *daObjEmmYuka_c_classInit(void);                    /* ov052 0x02111830, 56 B */
void hal_fill_square_path_lift_vtable(void);
/* ---- run linkw wave 12 (lane w12): CLAM (315), ov064 ----
   Nine instances, five on level 8 and four on level 18. Its Spawn names its
   receiver and stores _ZTV4Clam by a real name, so it registers directly.
   TREASURE_CHEST (13), the sibling this wave's ranking put beside it, was
   BLOCKED on an unmatched state body (hal/actor_classes_ov064_w12.cpp). Run
   rel0215 wave 3 (lane w3-c) seated it by transcribing that body; its row is
   at the end of this file. */
extern unsigned char g_profile_OBJ_SHELL[];               /* ov064 0x0211c5a4, +4 = 315 */
void *daObjShell_c_classInit(void);                              /* ov064 0x0211ad70, 64 B */
void hal_fill_clam_vtable(void);
/* ---- run linkw wave 15 (lane w15): BIG_MOVING_ICE_BLOCK (173), ov056 ----
   Nine instances on level 48 ("Wario's key course"), which is nine of that
   level's fourteen skipped instances; the other five are MR_BLIZZARD, refused
   on purpose. Its Spawn names its receiver and stores _ZTV17BigMovingIceBlock
   by a real name as its final write, so it registers directly with no factory
   wrapper. See hal/actor_classes_ov056.cpp and port/ov056_syms.txt. */
extern unsigned char g_profile_EWM_ICE_BLOCK[];  /* ov056 0x02113344, +4 = 173 */
void *daObjEwmIceBlock_c_classInit(void);                 /* ov056 0x02111594, 56 B */
void hal_fill_big_moving_ice_block_vtable(void);
/* ---- run linkw wave 15 (lane w15): the MEGA MUSHROOM TAG pair, ov002 ----
   ONE ROM class under TWO ids on ONE table (0x02108cf4), one instance of each
   on levels 22 and 24. The class branches on its own actor id, so both rows
   share the closure AND the fill; the fill is stores only and safe to call
   twice. See hal/actor_classes_ov002_w15.cpp and port/slice_w15b.txt. */
extern unsigned char g_profile_KINOKO_CREATE_TAG[]; /* ov002 0x02108cd0, +4 = 319 */
void *daObjKinokoTag_c_classInit_KINOKO_CREATE_TAG(void);                /* ov002 0x020b4a00, 56 B */
extern unsigned char g_profile_KINOKO_TAG[];       /* ov002 0x02108cb4, +4 = 320 */
void *daObjKinokoTag_c_classInit_KINOKO_TAG(void);                      /* ov002 0x020b4a38, 56 B */
void hal_fill_mega_mushroom_tag_vtable(void);
/* ---- run linkw wave 15 (lane w15): ov025's two pyramid classes ----
   Five of level 17's eleven skipped instances. Both Spawns name their receiver
   and store their table by a real name as the final write, so both register
   directly with no factory wrapper. See hal/actor_classes_ov025.cpp and
   port/ov025_syms.txt. */
extern unsigned char g_profile_DP_BROCK[];        /* ov025 0x021138f0, +4 = 87 */
void *daObjDpBrock_c_classInit(void);                       /* ov025 0x021120ac, 56 B */
void hal_fill_pyramid_step_vtable(void);
extern unsigned char g_profile_DP_LIFT[];        /* ov025 0x021139b0, +4 = 88 */
void *daDpLift_c_classInit(void);                       /* ov025 0x021125f0, 100 B */
void hal_fill_pyramid_lift_vtable(void);
/* run linkw wave 16 (lane w16-unblock): FALL_BLOCK_LLL (83, ov022). The record
   and the file table were already mounted by lane w8-packs; what held the id
   back was the inferred-stub guard, and all three marker-carrying bodies ruled
   REAL DECOMP against the ROM. The factory is the port_factory_fall_block_bfs
   shape -- daObjFl_Fall_Block_c_classInit.c spells its own table VT1 and the intermediate
   base by the class name, so the wrapper forces the vptr afterward and neither
   spelling has to be right. See port/slice_w16a.txt. */
extern unsigned char g_profile_FL_KUZURE[];       /* ov022 0x021142a0, +4 = 83 */
void *port_factory_fall_block_lll(void);             /* VT1 wrapper over ov022 0x0211245c */
void hal_fill_fall_block_lll_vtable(void);
/* run linkw wave 17 (lane w17): SEAWEED (296, ov002), RTTI 13daObjWakame_c --
   x8 on level 18 and x4 on level 8, the largest single-id population left on
   the cast board. Its record needed one mount line (0x1c, NOT the 0x24 the dsd
   next-symbol delta reads -- the two extra words are the table's own Itanium
   prologue), its slot 17 needed a per-source -DVT0 because the shared VT0
   placeholder is a live 20-word dummy, and its slot 9 is the ModelAnim slot-5
   collision hosted in hal/actor_classes_ov002_w17.cpp. daObjWakame_c_classInit's last
   vptr write is the host array itself, so there is no factory wrapper. See
   port/slice_w17a.txt. Note dsd's _ZTV7Seaweed is a DIFFERENT class
   (12daObjHeart_c, HEALING_HEART) and is untouched by this row. */
extern unsigned char g_profile_WAKAME[];            /* ov002 0x02109b94, +4 = 296 */
void *daObjWakame_c_classInit(void);                           /* ov002 0x020bc5a8, 56 B */
void hal_fill_seaweed_vtable(void);
/* run link60 lane A1: ov085's last two screened classes, WALL_SIGN (183,
   13daObjKanban_c) x9 and TOAD (185, 11daKinopio_c) x10 across levels 2, 4, 5
   and 50 -- all four already mounted, all four already booting. Both records
   are already in port/ov085_syms.txt at the pinned 0x1c (gate 18 mounted the
   overlay's whole data span per symbol), and all six of its sinits are already
   called, so these two rows cost code and nothing else. The fills, the two
   widths (32 and 31, and why the typeinfo route's 54 for Toad is the ov026
   pair-table trap), the two-names-one-table aliases and Toad's hosted slot 16
   are in hal/actor_classes_ov085.cpp; the slice is port/slice_gate205.txt.
   Neither class's slot 6 is decompiled: both Behaviors are HOST
   REIMPLEMENTATIONS in port/unmatched/Ov085_Behaviors.cpp, written from the
   ROM and queued as crack targets. */
extern unsigned char g_profile_KANBAN[];           /* ov085 0x021303dc, +4 = 183 */
void *daObjKanban_c_classInit(void);                          /* ov085 0x0212f244, 56 B */
void hal_fill_wall_sign_vtable(void);                /* _ZTV8WallSign, 32 */
extern unsigned char g_profile_KINOPIO[];               /* ov085 0x0212fe6c, +4 = 185 */
void *daKinopio_c_classInit(void);                              /* ov085 0x02129cd0, 72 B */
void hal_fill_toad_vtable(void);                     /* _ZTV4Toad, 31 */
/* ---- run rel0215 lane cast-ov077: the ov077 SHARED ENEMY PACK, a fresh
   per-symbol mount (port/ov077_syms.txt). All three SpawnInfo records come
   from that mount; all three fills are in hal/actor_classes_ov077.cpp, and
   each fill calls port_ov077_states_seat() before it writes a slot. Every id
   below is confirmed by BOTH attribution routes (the arm9 spawn-table
   relocation AND the record's own +4 halfword) -- required here because ov077
   is the fourth occupant of the 0x02123740 window and one of the three
   relocations reads `module:overlays(77,80)`. */
extern unsigned char g_profile_POPOI[];            /* ov077 0x02127a74, +4 = 238 */
void *daPopoi_c_classInit(void);                           /* ov077 0x021271d4, 92 B */
void hal_fill_heave_ho_vtable(void);                 /* _ZTV7HeaveHo, 31 */
extern unsigned char g_profile_TOGEZO[];              /* ov077 0x02127960, +4 = 260 */
void *daTgz_c_classInit(void);                             /* ov077 0x021261f4, 88 B */
void hal_fill_spiny_vtable(void);                    /* _ZTV5Spiny, 31 */
extern unsigned char g_profile_JUGEM[];             /* ov077 0x02127848, +4 = 265 */
void *daJgm_c_classInit(void);                            /* ov077 0x02124b04, 96 B */
void hal_fill_lakitu_vtable(void);                   /* _ZTV6Lakitu, 31 */
/* ---- run rel0215 wave 1 (lane cast-ov030): ov030's TALL TALL MOUNTAIN cast.
   FOUR ids, THREE classes -- UKIKI_THIEF (267) and UKIKI_STAR (268) are ONE
   class (7daMky_c) under two spawn entry points, so they share one fill. All
   four SpawnInfo records come from the fresh per-symbol ov030 mount
   (port/ov030_syms.txt); the three vtable fills are in
   hal/actor_classes_ov030.cpp; the slice is port/slice_ov030cast.txt.
   THE CONFIG'S _ZTV LABELS AND METHOD NAMES ARE SHIFTED ONE CLASS in this
   overlay (the ov012/ov072 dsd class-identity shape, third instance) and
   everything is seated BY ADDRESS -- ov030_syms.txt's header carries the ROM
   evidence. Nothing in config/ or src/ is renamed by that lane. */
extern unsigned char g_profile_HM_MARUTA[];      /* ov030 0x02115a24, +4 low = 102 */
void *daObjHmMaruta_c_classInit(void);                     /* ov030 0x0211164c, 60 B */
void hal_fill_rollinglogttm_vtable(void);            /* _ZTV15daObjHmMaruta_c, 32 */
extern unsigned char g_profile_HM_BASKET[];          /* ov030 0x02115950, +4 low = 103 */
void *daObjHmBskt_c_classInit(void);                         /* ov030 0x02111524, 56 B */
void hal_fill_ukikicage_vtable(void);                /* _ZTV13daObjHmBskt_c, 32 */
extern unsigned char g_profile_MONKEY_THIEF[];         /* ov030 0x02115b90, +4 low = 267 */
void *daMky_c_classInit_MONKEY_THIEF(void);                        /* ov030 0x02114638, 88 B */
extern unsigned char g_profile_MONKEY_STAR[];          /* ov030 0x02115bac, +4 low = 268 */
void *daMky_c_classInit_MONKEY_STAR(void);                         /* ov030 0x021145e0, 88 B */
void hal_fill_ukiki_vtable(void);                    /* _ZTV13RollingLogTtm == _ZTV7daMky_c, 31 */
/* ---- run rel0215 wave 1 (lane cast-ov026): ov026's WHOLE cast, level 18
   (Dire Dire Docks). Five classes; the SpawnInfo records come from the
   PER-SYMBOL ov026 mount (port/ov026_syms.txt), not the whole one, for the
   gate-17 reason -- the registry rewrites the factory word in place and the
   ActorBase constructor reads the two list priorities back out of the same
   record. Widths, the two pointer-to-member source tables that over-read the
   typeinfo route, and the dsd class-identity swap (every `_ZN9Submarine*` TU
   is the WHIRLPOOL's) are in hal/actor_classes_ov026.cpp's header; the slice
   is port/slice_ov026cast.txt. Both attribution routes were re-run on all five
   ids: each SpawnInfo's word[0] lands inside ov026's own image AND the arm9
   spawn table slot at 0x02090864 + id*4 points back at the record. ---- */
extern unsigned char g_profile_WL_POLELIFT[];          /* ov026 0x02113abc, +4 = 89  */
void *daObjWlPolelift_c_classInit(void);                     /* ov026 0x02111678, 80 B     */
void hal_fill_ov026_polelift_vtable(void);           /* _ZTV17daObjWlPolelift_c, 31 */
extern unsigned char g_profile_WL_SUBMARINE[];          /* ov026 0x02113c48, +4 = 90  */
void *daObjWlSubmarine_c_classInit(void);                         /* ov026 0x02111a70, 48 B     */
void hal_fill_ov026_submarine_vtable(void);          /* _ZTV18daObjWlSubmarine_c, 32 */
extern unsigned char g_profile_WL_KOOPA_SHUTTER[];      /* ov026 0x02113b80, +4 = 91  */
void *daObjWlKoopaShutter_c_classInit(void);                     /* ov026 0x02111888, 48 B     */
void hal_fill_ov026_bowser_shutter_vtable(void);     /* _ZTV21daObjWlKoopaShutter_c, 32 */
extern unsigned char g_profile_WATER_TATUMAKI[];          /* ov026 0x02113d30, +4 = 246 */
void *daWater_Tatumaki_c_classInit(void);                         /* ov026 0x021121bc, 64 B     */
void hal_fill_ov026_whirlpool_vtable(void);          /* _ZTV18daWater_Tatumaki_c, 31 */
extern unsigned char g_profile_WATER_SUIKOMI[];       /* ov026 0x02113e00, +4 = 247 */
void *daWater_Suikomi_c_classInit(void);                      /* ov026 0x02112450, 64 B     */
void hal_fill_ov026_water_suction_vtable(void);      /* _ZTV12WaterSuction, 31     */
/* run rel0215 wave 1 (lane cast-sweep1): OV035's OWN CAST, level 27's Tick
   Tock Clock furniture -- nineteen of that level's 57 skipped instances, the
   largest block on it. FOUR ids, TWO 32-slot Platform tables: 119, 121 and 122
   share 0x02112b00 and every method body with it, 120 owns 0x02112bcc alone.
   dsd's two vtable labels are shifted one class -- it put _ZTV17RotatingClockHand
   on SPINNING_PLATFORM's table -- so the rows below are wired by ADDRESS and
   the dsd spellings survive only as the host array names. The four factories'
   installs, the RTTI names, both attribution routes, the nine T3 width pins and
   the two live shared-window alias races are derived in port/ov035_syms.txt's
   second header; the fills and the 32-slot Platform close are in
   hal/actor_classes_ov035.cpp; the slice is port/slice_sweep1_ov035.txt.
   id 121 has no dsd SpawnInfo name: 0x02112ac0 is ov010's g_profile_C1_TRAP under
   the shared window, and ov035's own record there is reached by address. */
extern unsigned char g_profile_CT_MECHA10[];  /* ov035 0x02112aa4, +4 = 119 */
void *daObjCtMecha10_c_classInit_CT_MECHA10(void);                 /* ov035 0x021116bc, 48 B */
extern unsigned char g_profile_CT_MECHA11[];   /* ov035 0x02112ba8, +4 = 120 */
void *daObjCtMecha11_c_classInit(void);                  /* ov035 0x02111b98, 56 B */
extern unsigned char g_profile_CT_MECHA12L[];          /* ov035 0x02112ac0, +4 = 121 */
void *daObjCtMecha10_c_classInit_CT_MECHA12L(void);                     /* ov035 0x0211168c, 48 B */
extern unsigned char g_profile_CT_MECHA12S[];   /* ov035 0x02112adc, +4 = 122 */
void *daObjCtMecha10_c_classInit_CT_MECHA12S(void);                  /* ov035 0x0211165c, 48 B */
void hal_fill_rotating_cog_vtable(void);             /* 0x02112b00, 32 slots */
void hal_fill_spinning_platform_vtable(void);        /* 0x02112bcc, 32 slots */
/* run rel0215 wave 1 (lane cast-sweep1): OV024's OWN CAST, level 16's pyramid
   pair -- five of Shifting Sand Land's nineteen skipped instances. TWO tables
   of TWO DIFFERENT LENGTHS: PYRAMID_TOP is Platform-derived and its table
   closes on the slot-31 Platform::Kill (32), PYRAMID_TAG is a plain Actor and
   its table has no slot 31 at all (31). dsd's landings under-read both, at 4
   and 25. The widths, both attribution routes, the four T3 pins and the one
   held-out //cpp destructor are derived in port/ov024_syms.txt and
   port/slice_sweep1_ov024.txt; the fills are hal/actor_classes_ov024.cpp.
   PYRAMID_TAG finds PYRAMID_TOP by actor id and destroys itself if it is
   absent, so the two rows land together or neither is worth anything. */
extern unsigned char g_profile_DL_PYRAMID[];         /* ov024 0x02113804, +4 = 85 */
void *daObjDlPyramid_c_classInit(void);                        /* ov024 0x02111874, 56 B */
void hal_fill_pyramid_top_vtable(void);              /* _ZTV10PyramidTop, 32 */
extern unsigned char g_profile_DL_PYRAMID_DUMMY[];         /* ov024 0x02113820, +4 = 86 */
void *daObjDlPyramidDummy_c_classInit(void);                        /* ov024 0x0211183c, 56 B */
void hal_fill_pyramid_tag_vtable(void);              /* _ZTV10PyramidTag, 31 */
/* run rel0215 wave 1 (lane cast-sweep1): OV033's OWN CAST, Tiny-Huge Island's
   small side -- two of level 25's three skipped instances. dsd names ONE
   vtable in this overlay and it is on the WRONG class: _ZTV9TinyWater
   (0x02112440, RTTI 14daObjTtWater_c) is TINY_WATER's, and TINY_COVER's own
   table is the unnamed _ZTV9TinyCover (RTTI 13daObjTtFuta_c, futa = lid).
   The factory installs, the RTTI strings and the spawn records all three agree,
   so the rows below go by ADDRESS and every _ZN9TinyCover* body in src/ serves
   id 105. Both tables are 32-slot Platform; dsd's landings read 26 and THREE.
   Id 104 carries SEVEN own slots, not six -- slot 21 is a real OnGroundPounded
   override and pounding the lid is the class's whole purpose. Derivation in
   port/ov033_syms.txt, fills in hal/actor_classes_ov033.cpp, slice in
   port/slice_sweep1_ov033.txt. */
extern unsigned char g_profile_TT_FUTA[];          /* ov033 0x02112358, +4 = 104 */
void *daObjTtFuta_c_classInit(void);                         /* ov033 0x021113a4, 48 B */
void hal_fill_tiny_cover_vtable(void);               /* _ZTV9TinyCover, 32 */
extern unsigned char g_profile_TT_WATER[];          /* ov033 0x0211241c, +4 = 105 */
void *daObjTtWater_c_classInit(void);                         /* ov033 0x02111690, 56 B */
void hal_fill_tiny_water_vtable(void);               /* _ZTV9TinyWater, 32 */
/* run rel0215 wave 1 (lane cast-sweep1): OV023's ONE CLASS, the volcano
   squasher -- and it is the WHOLE of level 15's skip list, so this row takes
   that level to zero skipped classes. Its table is 32-slot Platform; the dsd
   next-symbol landing OVER-reads it by seven slots because the next symbol is
   the .bss base. Both attribution routes, the three T3 pins, the three alias
   races its InitResources and destructors carry (two spelled with ov026 and
   ov029 names for ov023's own addresses) and the one cross-overlay CLPS block
   that genuinely lives in ov064 are derived in port/ov023_syms.txt; the fill
   is hal/actor_classes_ov023.cpp and the slice is
   port/slice_sweep1_ov023.txt. */
extern unsigned char g_profile_FM_BATTAN[];           /* ov023 0x02111fc0, +4 = 84 */
void *daObjFm_Battan_c_classInit(void);                          /* ov023 0x02111728, 56 B */
void hal_fill_squasher_vtable(void);                 /* _ZTV16daObjFm_Battan_c, 32 */
/* run rel0215 wave 1 (lane cast-sweep1): OV017's ONE CLASS, the sunken ship's
   water surface -- one of level 9's two skipped instances, and a blocker
   hal/level_boot.cpp's level-9 block wrote down and deferred. Its table is
   32-slot Platform and dsd's landing reads TEN. dsd's two names for it agree
   with the RTTI ("14daObjKsWater_c"), so unlike ov033 and ov035 there is no
   class-identity shift here. What there IS is three alias races in one
   function, one of them live on this tip: the collision SharedFilePtr is
   spelled &_ZN15daObjKm2_Gura_cD0Ev, an ov045 FUNCTION that is in the map
   today. All three are pinned to the ROM's own literal pool and routed by
   per-source -D. Derivation in port/ov017_syms.txt, fill in
   hal/actor_classes_ov017.cpp, slice in port/slice_sweep1_ov017.txt.
   NOTE its Behavior gates on TREASURE_CHEST (id 13, ov064). While that class
   was unregistered there was no id-13 actor to find, the "all chests opened"
   test was vacuously true and the water drained from frame one -- visible, not
   fatal, and owned by the OTHER class's seat. Run rel0215 wave 3 (lane w3-c)
   registered id 13, so the gate is a real test again. */
extern unsigned char g_profile_KS_MIZU[];          /* ov017 0x02111bd4, +4 = 62 */
void *daObjKsWater_c_classInit(void);                         /* ov017 0x02111480, 56 B */
void hal_fill_ship_water_vtable(void);               /* _ZTV9ShipWater, 32 */
/* run rel0215 wave 1 (lane cast-sweep1): OV058's ONE CLASS, the Rec Room's
   cupboard -- and it is the WHOLE of level 50's skip list, so this row takes
   that level to zero skipped. A 0x21c-byte Actor holding an array of five
   MovingCylinderClsnWithPos at +0xd4; it loads no file and ov058 has no static
   initialiser at all. Its table is 31-slot ACTOR, not 32-slot Platform (the
   word at slot 31 is a literal zero, not a relocation) -- the ov024
   PYRAMID_TAG shape -- and dsd's next-symbol landing reads TWO. Slot 9 is the
   BASE Render; the class has none of its own. No alias race, no resolver
   placeholder and no class-identity shift: the only overlay in this lane that
   needed no per-source -D. Derivation in port/ov058_syms.txt, fill in
   hal/actor_classes_ov058.cpp, slice in port/slice_sweep1_ov058.txt. */
extern unsigned char g_profile_PL_CLOSET[];    /* ov058 0x02111a4c, +4 = 182 */
void *daObjCloset_c_classInit(void);                   /* ov058 0x02111688, 92 B */
void hal_fill_rec_room_cupboard_vtable(void);        /* _ZTV15RecRoomCupboard, 31 */
/* ---- run rel0215 wave 2 (lane cast-ov090): ov090, THE WATER ENEMY PACK.
   Four ids, four classes, all Enemy-derived and all 31-slot ACTOR tables (the
   semantic tail pins every one at slot 30 Actor::OnAimedAtWithEggReturnVec;
   Shark's dsd next-symbol landing reads 37 and the extra six words are zero
   pad). All four SpawnInfo records come from the fresh per-symbol ov090 mount
   (port/ov090_syms.txt); the fills and the sixteen-record PMF state seat are
   in hal/actor_classes_ov090.cpp; the slice is port/slice_ov090.txt.
   NO CLASS-IDENTITY SHIFT in this overlay -- each table's own vtable[-1]
   typeinfo record reads through to the RTTI name dsd gives that same address,
   and each table's slot 0 is that class's own InitResources. Levels 18 and 21
   both name this pack: 18 places SHARK, CHEEP_CHEEP and SKEETER, 21 places
   SKEETER. MANTA_RAY is placed on neither and is seated for the pack's sake.
   The dsd `ambiguous` mark on all four records is an ov006 dual-dump artifact,
   settled from ov006's own raw bytes in port/ov090_syms.txt. */
extern unsigned char g_profile_MENBO[];            /* ov090 0x02134144, +4 = 230 */
void *daMenbo_c_classInit(void);                           /* ov090 0x02132654, 72 B */
void hal_fill_skeeter_vtable(void);                  /* _ZTV7Skeeter, 31 */
extern unsigned char g_profile_MANTA[];           /* ov090 0x02134218, +4 = 226 */
void *daManta_c_classInit(void);                          /* ov090 0x02132fe8, 76 B */
void hal_fill_manta_ray_vtable(void);                /* _ZTV8MantaRay, 31 */
extern unsigned char g_profile_PUKUPUKU[];         /* ov090 0x02134300, +4 = 227 */
void *daPukupuku_c_classInit(void);                        /* ov090 0x02133634, 72 B */
void hal_fill_cheep_cheep_vtable(void);              /* _ZTV10CheepCheep, 31 */
extern unsigned char g_profile_SHARK[];              /* ov090 0x021343c8, +4 = 225 */
void *daShark_c_classInit(void);                             /* ov090 0x02133ca0, 72 B */
void hal_fill_shark_vtable(void);                    /* _ZTV5Shark, 31 */
/* run rel0215 wave 2 (lane cast-ov066): OV066's ONE CLASS, EYEROK -- the SSL
   pyramid boss, and the whole of ov066. The ROM calls it 10daIwante_c (iwa +
   te = rock hand) and its base 10dBgActor_c = Platform, so its table is
   32-slot PLATFORM, not 31-slot Actor: slot 31 is a relocation to ov002's
   Platform::Kill, which the base typeinfo and daIwante_c_classInit's PlatformC2Ev call
   confirm two more ways. ONE CLASS, NOT THREE -- .data carries exactly one
   __si_class_type_info, and the two hands and the head are STATES of a
   NINETEEN-cell pointer-to-member machine (38 source pairs). Level 17 alone
   loads it. Derivation in port/ov066_syms.txt, fill in
   hal/actor_classes_ov066.cpp, slice in port/slice_ov066.txt -- which also
   itemises the SIX /alternatename directives this mount defeats. */
extern unsigned char g_profile_IWANTE[];             /* ov066 0x0211ad40, +4 = 176 */
void *daIwante_c_classInit(void);                            /* ov066 0x0211a370, 168 B */
void hal_fill_eyerok_vtable(void);                   /* _ZTV6Eyerok, 32 */

/* ---- run rel0215 wave 2 (lane cast-ov027): Snowman's Land's own cast, FIVE
   ids on FOUR vtables, level 19. The task sheet named four classes; route 2
   (the SpawnInfo's own word[1] idhalf plus word[0] landing in ov027's .text)
   found a fifth, id 258, whose record and factory config leaves unnamed and
   which owns sixteen of the overlay's 53 functions. Derivation in
   port/ov027_syms.txt, fill in hal/actor_classes_ov027.cpp, slice in
   port/slice_ov027cast.txt.
   IDS 92 AND 93 ARE ONE CLASS: daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOT and daObjSlIceBlock_c_classInit_SL_ICEBLOCK
   are the same twelve instructions and both store 0x02113824, and the class
   branches on its own actor id inside Init/Cleanup/Render. Two records, two
   rows, ONE table and ONE fill -- so hal_fill_sliding_ice_vtable appears
   twice below, which is deliberate and idempotent.
   DA_PG_DFDR's name is the ROM's own RTTI string (10daPgDfdr_c) rather than an
   English one: config names neither its record nor its factory, nothing in the
   tree carries an English name for 258, and inventing one would be a guess
   printed in every census. ---- */
extern unsigned char g_profile_SL_ICEBLOCK[];  /* ov027 0x021137e4, +4 = 92 */
extern unsigned char g_profile_SL_ICEBLOCK_SHOT[];         /* ov027 0x02113800, +4 = 93 */
void *daObjSlIceBlock_c_classInit_SL_ICEBLOCK(void);                 /* ov027 0x02111594, 48 B */
void *daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOT(void);                        /* ov027 0x02111564, 48 B */
void hal_fill_sliding_ice_vtable(void);              /* _ZTV10SlidingIce, 32 */
extern unsigned char g_profile_ICE_DONKETU[];         /* ov027 0x021138d8, +4 = 217 */
void *daIDonketu_c_classInit(void);                        /* ov027 0x0211186c, 92 B */
void hal_fill_chill_bully_vtable(void);              /* data_ov027_02113930, 37 */
extern unsigned char g_profile_PENGUIN_DEFENDER[];          /* ov027 0x02113a00, +4 = 258 */
void *daPgDfdr_c_classInit(void);                     /* ov027 0x0211207c, 72 B */
void hal_fill_da_pg_dfdr_vtable(void);               /* _ZTV10daPgDfdr_c, 32 */
extern unsigned char g_profile_SNOWMAN_BREATH[];      /* ov027 0x02113b2c, +4 = 275 */
void *daSnmBth_c_classInit(void);                     /* ov027 0x02112ab4, 96 B */
void hal_fill_snowman_breath_vtable(void);           /* _ZTV13SnowmanBreath, 31 */
/* ---- run rel0215 wave 2 (lane cast-lvl16pair, sub-job 1 of 2): ov092's ONE
   class, TOX_BOX, level 16's crushing block. The SpawnInfo record comes from
   the fresh per-symbol ov092 mount (port/ov092_syms.txt); the fill, the
   nine-state pointer-to-member seat and the two method faces are in
   hal/actor_classes_ov092.cpp; the slice is port/slice_ov092.txt. The id is
   confirmed by BOTH attribution routes -- the arm9 spawn-table relocation at
   0x02090d38 AND the record's own +4 halfword, which reads 0x0135 = 309 -- and
   for once the module column is SINGULAR (`module:overlay(92)`, and dsd marks
   zero ov092 symbols ambiguous), so this row needed no adjudication. The table
   is a 32-slot PLATFORM close, not the 31-slot Actor close: slot 31 is
   0x020ee55c _ZN10dBgActor_c4KillEv and daOnms_c_classInit calls _ZN10dBgActor_cC2Ev. The
   mount is brought up from hal_fill_tox_box_vtable's own port_ov92_bringup(),
   the ov017/ov024/ov035 self-contained shape, so this lane adds NOTHING to
   hal/actor_overlays.cpp. */
extern unsigned char g_profile_ONIMASU[];             /* ov092 0x021322ac, +4 low = 309 */
void *daOnms_c_classInit(void);                            /* ov092 0x02132018, 92 B */
void hal_fill_tox_box_vtable(void);                  /* _ZTV6ToxBox, 32 */
/* ---- run rel0215 wave 2 (lane cast-lvl16pair, sub-job 2 of 2): ov096's cast,
   the rest of level 16's. THREE IDS, TWO CLASSES -- 240 and 241 are ONE class
   (9daSanbo_c) under two spawn entry points, so they share one fill. Proved
   three ways rather than assumed from the name: src/actors/daSanbo_c.cpp and
   src/actors/daSanbo_c.cpp are identical apart from their own names, both
   factories' pools relocate to the SAME table 0x021379d8 (0x02136d5c and
   0x02136dac), and PokeySegment's own record's typeinfo word points at POKEY's
   __si_class_type_info. The ov030 UKIKI_THIEF / UKIKI_STAR shape.
   All three SpawnInfo records come from the fresh per-symbol ov096 mount
   (port/ov096_syms.txt); the two vtable fills, the six-state PMF seat and the
   seven method faces are hal/actor_classes_ov096.cpp; the slice is
   port/slice_ov096.txt.
   ALL THREE IDS ARE CONTESTED AND ALL THREE ARE ADJUDICATED. dsd marks every
   one `ambiguous`, every arm9 relocation reads `module:overlays(6,96)`, and
   ov006's mount already hosts all three addresses today -- so both attribution
   routes were mandatory here and both agree on all three. The raw bytes of
   both claimants are in port/ov096_syms.txt: ov096 has a real prologue in
   word[0] and the reaching table index in word[1] at every one, ov006 has GX
   display-list words at all three.
   BOTH TABLES ARE 31 SLOTS, the plain Actor close -- not ov092's 32-slot
   Platform close in the other half of this lane. Tornado's next-symbol landing
   reads 36 and LIES: slots 31..35 are five zero pad words to .bss's align-32
   boundary and the reloc run stops at 30. */
extern unsigned char g_profile_SANBO[];              /* ov096 0x02137998, +4 low = 240 */
void *daSanbo_c_classInit_SANBO(void);                             /* ov096 0x02136d60, 80 B */
extern unsigned char g_profile_SANBO_BODY[];       /* ov096 0x021379b4, +4 low = 241 */
void *daSanbo_c_classInit_SANBO_BODY(void);                      /* ov096 0x02136d10, 80 B */
void hal_fill_pokey_vtable(void);                    /* _ZTV5Pokey == _ZTV9daSanbo_c, 31 */
extern unsigned char g_profile_TORNADO[];            /* ov096 0x02137a6c, +4 low = 308 */
void *daTor_c_classInit(void);                           /* ov096 0x021376bc, 80 B */
void hal_fill_tornado_vtable(void);                  /* _ZTV7Tornado == _ZTV7daTor_c, 31 */

/* ---- run rel0215 wave 2 (lane w2-ov074): the ov074 GOOMBOSS PACK, a fresh
   per-symbol mount (port/ov074_syms.txt). TWO IDS AND ONE FILL:
   src/d_a_kuri_king_kuriking.cpp and src/d_a_kuri_king_kuriking_vanish.cpp are the same 0xd4
   body twice -- both allocate 0x610, both store _ZTV8Goomboss into p[0] and
   both run the identical member-construction sequence -- and ov074 holds
   exactly one vtable and one typeinfo record, whose name string reads
   "12daKuriKing_c". So both rows point at hal_fill_goomboss_vtable.
   Both SpawnInfo records come from the mount, and both ids were confirmed on
   BOTH routes (the arm9 spawn-table relocation at 0x02090864 + id*4 AND the
   record's own +4 halfword) -- required here because ov074 is the fourth
   occupant of the 0x0211f000 window and eleven other spawn-table slots
   relocate into its span. ---- */
extern unsigned char g_profile_KURIKING[];           /* ov074 0x02122e78, +4 = 198 */
void *daKuriKing_c_classInit_KURIKING(void);                          /* ov074 0x02122838, 212 B */
extern unsigned char g_profile_KURIKING_VANISH[];    /* ov074 0x02122e94, +4 = 199 */
void *daKuriKing_c_classInit_KURIKING_VANISH(void);                   /* ov074 0x02122764, 212 B */
void hal_fill_goomboss_vtable(void);                 /* _ZTV8Goomboss, 31 */
/* run rel0215 wave 2 (lane cast-sweep2): OV032'S OWN CAST, Tiny-Huge Island's
   BIG side -- four of level 24's seven skipped instances. THE CAMPAIGN PLAN
   LISTS TWO CLASSES HERE AND THE ROM HAS THREE: id 228 (RTTI 12daBakubaku_c,
   the big fish; tools/actor_names.py reads it as BUBBA) carries twenty-four of
   the overlay's forty-one TUs and dsd named neither its factory nor its table.
   dsd names ONE vtable ADDRESS in this overlay and puts TWO names on it, both
   the WATER class's: _ZTV9HugeCover and _ZTV14daObjTdWater_c are both
   0x021139a4 (RTTI 14daObjTdWater_c). HUGE_COVER's own table is the unnamed
   _ZTV13daObjTdFuta_c (RTTI 13daObjTdFuta_c, futa = lid) and BUBBA's is the
   unnamed _ZTV12daBakubaku_c. The factory installs, the RTTI strings and the
   spawn records all three agree, so the rows below go by ADDRESS and every
   _ZN9HugeCover* body in src/ serves id 107. Widths 32 / 32 / 31 -- BUBBA is
   Enemy-derived and ends on slot 30 with no Platform::Kill -- and dsd's
   next-symbol landings read FOUR, THREE and TWO. BUBBA carries EIGHT own
   slots: 12 OnPendingDestroy is an empty override and 29 OnAimedAtWithEgg
   returns 0xa0000. Derivation in port/ov032_syms.txt, fills in
   hal/actor_classes_ov032.cpp, slice in port/slice_sweep2_ov032.txt. */
extern unsigned char g_profile_TD_FUTA[];          /* ov032 0x021138bc, +4 = 106 */
void *daObjTdFuta_c_classInit(void);                         /* ov032 0x02112668, 48 B */
void hal_fill_huge_cover_vtable(void);               /* _ZTV13daObjTdFuta_c, 32 */
extern unsigned char g_profile_TD_WATER[];          /* ov032 0x02113980, +4 = 107 */
void *daObjTdWater_c_classInit(void);                         /* ov032 0x021128b8, 56 B */
void hal_fill_huge_water_vtable(void);               /* _ZTV9HugeCover, 32 */
extern unsigned char g_profile_BAKUBAKU[];          /* ov032 0x02113800, +4 = 228 */
void *daBakubaku_c_classInit(void);                     /* ov032 0x0211244c, 92 B */
void hal_fill_bubba_vtable(void);                    /* _ZTV12daBakubaku_c, 31 */
/* run rel0215 wave 2 (lane cast-sweep2): OV034'S ONE CLASS, Tiny-Huge Island's
   CAVE -- and it is the WHOLE of level 26's skip list, so this row takes that
   level to zero skipped. Unlike ov032 and ov033 there is NO class-identity
   shift: _ZTV7Wiggler (0x021144bc) is on the right class, its RTTI reads
   "12daHanachan_c" (Hanachan is the ROM's word for the Wiggler), and dsd's
   inferred 31-slot width is right -- the state table starts exactly where the
   vtable ends. A 2280-byte Enemy built out of FIVE of everything (five
   ModelAnims at +0x110, five MaterialChangers, five TextureSequences, two
   five-element cylinder arrays), with an eleven-entry pointer-to-member state
   table at data_ov034_02114538 whose dsd landing reads ONE entry of eleven.
   ONE SLOT IS UNHOSTED: _ZN7Wiggler8BehaviorEv has no matched body anywhere in
   the tree and is faced loudly, so the Wiggler renders but holds state
   DEMOWAIT. Derivation in port/ov034_syms.txt, fill in
   hal/actor_classes_ov034.cpp, slice in port/slice_sweep2_ov034.txt. */
extern unsigned char g_profile_HANACHAN[];            /* ov034 0x02114498, +4 = 248 */
void *daHanachan_c_classInit(void);                  /* ov034 0x021136a4, 380 B */
void hal_fill_wiggler_vtable(void);                  /* _ZTV7Wiggler, 31 */
/* The factory is the matched src/d_a_hanachan.c (run linkfull lane SMALLS2),
   the word the ROM's own profile holds (ov034 relocs.txt from:0x02114498).
   The ChiefChilly r0 ride-through that held it out is gone from the synced
   src, which passes the receiver to _ZN12dEnemyBase_cC2Ev; the hand-spelled
   port_factory_wiggler that stood in for it is retired. */
/* run rel0215 wave 2 (lane cast-sweep2): OV047'S OWN CAST, Bowser in the Sky --
   seven of level 39's nine skipped instances. THE CAMPAIGN PLAN LISTS THREE
   CLASSES HERE AND THE ROM HAS FOUR: id 156 (RTTI 20daObjKm3_Kaitendai_c, the
   rotating stand) is a fourth, and tools/actor_names.py already reads it as
   ROTATING_PLATFORM_BS. dsd names TWO vtables in this overlay and each is
   attached ONE TABLE LATE in .data address order, so _ZTV20daObjKm3_Kaitendai_c
   (0x0211237c) is id 156's and _ZTV17daObjKm3_Dorifu_c (0x0211254c) is id
   153's, while ids 152 and 151 own the two tables dsd left as bare data
   symbols. The factory installs, the RTTI at vtable[-1] and the spawn records
   all three agree, so the rows below go by ADDRESS and every _ZN10RickshawBs*
   body serves id 156 while every _ZN18RickshawPlatformBs* body serves id 153.
   All four tables are 32-slot Platform with the same four own slots (0, 3, 16,
   17); 6 and 9 are inherited from an ov002 intermediate base. dsd's landings
   read 0x80 on one and SHORT on the other three. id 151 is not placed on level
   39 and is registered because both attribution routes name it. Derivation in
   port/ov047_syms.txt, fills in hal/actor_classes_ov047.cpp, slice in
   port/slice_sweep2_ov047.txt. */
extern unsigned char g_profile_KM3_KURUMA[]; /* ov047 0x02112428, +4 = 151 */
void *daObjKm3_Kuruma_c_classInit(void);                /* ov047 0x021114d4, 60 B */
void hal_fill_rickshaw_platform_bs_vtable(void);     /* _ZTV17daObjKm3_Kuruma_c, 32 */
extern unsigned char g_profile_KM3_KURUMAJIKU[];         /* ov047 0x0211227c, +4 = 152 */
void *daObjKm3_Kurumajiku_c_classInit(void);                        /* ov047 0x02111280, 60 B */
void hal_fill_rickshaw_bs_vtable(void);              /* _ZTV21daObjKm3_Kurumajiku_c, 32 */
extern unsigned char g_profile_KM3_DORIFU[];           /* ov047 0x021124ec, +4 = 153 */
void *daObjKm3_Dorifu_c_classInit(void);                          /* ov047 0x0211164c, 144 B */
void hal_fill_stairs_bs_vtable(void);                /* _ZTV17daObjKm3_Dorifu_c, 32 */
extern unsigned char g_profile_KM3_KAITENDAI[];          /* ov047 0x02112358, +4 = 156 */
void *daObjKm3_Kaitendai_c_classInit(void);                     /* ov047 0x021113bc, 60 B */
void hal_fill_rotating_platform_bs_vtable(void);     /* _ZTV20daObjKm3_Kaitendai_c, 32 */
/* run rel0215 wave 2 (lane cast-sweep2): OV055'S ONE CLASS, Luigi's key arena
   -- and it is the WHOLE of level 47's skip list, so this row takes that level
   to zero skipped. NO identity shift: _ZTV11MirrorLuigi (0x02111ae0) is on the
   right class, its RTTI reads "9daLuigi_c" and its base typeinfo is ACTOR's,
   which is why the table is 31 slots with a literal zero where a Platform
   keeps Kill (the ov024 PYRAMID_TAG shape). dsd's landing reads FIVE. SEVEN
   own slots: 12 OnPendingDestroy is a four-byte override. A 524-byte Actor
   with a ModelAnim at +0xd4, a Model at +0x138, a ShadowModel at +0x188 and a
   ONE-CELL pointer-to-member state machine at +0x1d8 whose tick mirrors the
   player's X and Y angle -- the smallest instance of that machine in the tree.
   Derivation in port/ov055_syms.txt, fill in hal/actor_classes_ov055.cpp,
   slice in port/slice_sweep2_ov055.txt. */
extern unsigned char g_profile_LUIGI[];        /* ov055 0x02111abc, +4 = 195 */
void *daLuigi_c_classInit(void);                       /* ov055 0x02111860, 116 B */
void hal_fill_mirror_luigi_vtable(void);             /* _ZTV11MirrorLuigi, 31 */
/* run rel0215 wave 3 (lane w3-c): OV064'S LAST THREE SPAWN-TABLE CLASSES, the
   three port/ov064_syms.txt's gate-177 NOT-RUN paragraph named and deferred.
   After these rows ov064's cast is closed: fifteen spawn-table slots relocate
   into this overlay's window, twelve of them are ov064's own records and all
   twelve are registered. (Slots 6, 7 and 346 read `module:overlays(64,75)` and
   are ov075's -- two of the three records are past ov064's image end at
   0x0211c660 and the third reads as the interior of Clam's vtable.)

   NO IDENTITY SHIFT ON TWO OF THE THREE: _ZTV9WaterRing (0x0211c420) and
   _ZTV13TreasureChest (0x0211c50c) are each on the right class, each factory
   installs it, and the RTTI agrees -- "14daWater_Ring_c" and "11daObjTbox_c".
   Both are 31 slots: an Enemy and an Actor, no Platform::Kill tail, so slot 30
   (OnAimedAtWithEggReturnVec) closes each. Both reloc runs overrun -- 32 and 34
   -- into the class's own pointer-to-member SOURCE pairs, which is the trap
   hal/actor_classes_ov064_w12.cpp measured on this exact pair of neighbours.

   JET_STREAM IS THE SHIFT, and gate 179 already carries its consequences.
   dsd's ov064 BowserPuzzle labels are rotated by one class:
   _ZTV17BowserPuzzlePiece (0x0211c334) is JET STREAM'S table (RTTI
   "18daWater_Hakidasi_c"), daWater_Hakidasi_c_classInit is the only unhosted thing that
   spelled it, and gate 179 filled it with the seven bodies dsd named
   _ZN17BowserPuzzlePiece* -- which are JetStream's own. hal_fill_jet_stream_-
   vtable is the correctly-named entry point onto that same fill, not a second
   one. Derivations in port/ov064_syms.txt's wave-3 block, fills in
   hal/actor_classes_ov064_w3c.cpp, slice in port/slice_w3c.txt. */
extern unsigned char g_profile_WATER_HAKIDASI[];      /* ov064 0x0211c310, +4 = 245 */
void *daWater_Hakidasi_c_classInit(void);                     /* ov064 0x02119a18, 64 B */
void hal_fill_jet_stream_vtable(void);           /* gate 179's array, 31 */
extern unsigned char g_profile_WATER_RING[];      /* ov064 0x0211c3fc, +4 = 244 */
void *daWater_Ring_c_classInit(void);                     /* ov064 0x0211a1b0, 80 B */
void hal_fill_water_ring_vtable(void);           /* _ZTV9WaterRing, 31 */
extern unsigned char g_profile_TREASURE_BOX[];  /* ov064 0x0211c4e8, +4 = 13 */
void *daObjTbox_c_classInit(void);                 /* ov064 0x0211a8f0, 64 B */
void hal_fill_treasure_chest_vtable(void);       /* _ZTV13TreasureChest, 31 */
/* run rel0215 wave 3 (lane w3-f): ov085's SIXTH AND LAST class, PRINCESS_PEACH
   (186, 9daPeach_c). Identity taken both ways rather than inherited: the record
   at 0x0212ff9c holds factory 0x0212a684 and the +4 halfword 186, and the arm9
   spawn table slot at 0x02090864 + 186*4 = 0x02090b4c holds that same record.
   The mount and all six sinits have been in place since gate 18, so this row
   costs code and nothing else -- the same shape gate 205's two rows had.

   PLACED ON NO LEVEL OF THE FIFTY-TWO, and that was measured on this lane's own
   baseline rather than assumed: the pre-spawn census over every id in
   port_level_table[] declines 0xe7, 0x90 and 0x9d and never 0xba. She is a
   CUTSCENE actor; a level walk does not exercise her. Registered for the SPINY
   reason four blocks down -- she is the overlay's last class, her seat is as
   derived as the other five, and an unregistered factory is a ROM-faithful body
   the port declines for no reason. The width (31), her own slot 12, the hosted
   slot 6 and the hosted slot 16 are in hal/actor_classes_ov085.cpp; the slice is
   port/slice_w3f.txt. */
extern unsigned char g_profile_PEACH_PRINCESS[];      /* ov085 0x0212ff9c, +4 = 186 */
void *daPeach_c_classInit(void);                     /* ov085 0x0212a684, 80 B */
void hal_fill_princess_peach_vtable(void);           /* _ZTV13PrincessPeach, 31 */
/* run rel0215 wave 3 (lane w3-b): THE TWO CLASSES GATE 192 LEFT BEHIND when
   it mounted ov081 for ICE_BLOCK / MR_BLIZZARD / SPINDRIFT. Nothing about
   that mount is re-done -- ov081 is already on PORT_ACTOR_OVERLAYS and both
   fills go in the same hal/actor_classes_ov081.cpp. Both ids are confirmed
   BOTH ROUTES: arm9 ACTOR_SPAWN_TABLE 0x02090864 + id*4 relocates to the
   record below (module:overlay(81), not an ambiguous pair), and each
   record's +0 word is ov081's own factory with its +4 halfword reading the
   id back. Both tables are the plain 31-slot Actor shape, re-derived with
   port/tools/vtspan.py on this tree; the derivation and the two held-out
   TUs are in port/slice_w3b.txt. */
extern unsigned char g_profile_SNOWBALL[];           /* ov081 0x02128a98, +4 = 224 */
void *daSnowball_c_classInit(void);                          /* ov081 0x021264b4, 80 B */
void hal_fill_snowball_vtable(void);                 /* _ZTV8Snowball, 31 */
extern unsigned char g_profile_GAMAGUCHI[];           /* ov081 0x02128be0, +4 = 261 */
void *daGmch_c_classInit(void);                          /* ov081 0x02127adc, 88 B */
void hal_fill_moneybag_vtable(void);                 /* _ZTV8Moneybag, 31 */
/* run rel0215 wave 3 (lane w3-e): THE REST OF ov022, Lethal Lava Land's cast.
   Wave 8 seated five ids and wave 16 a sixth; these five close the overlay and
   take level 14's skip list to zero. 73/77/80 were held back on the
   inferred-stub ratchet, and all fourteen marker-carrying bodies now rule
   REAL_DECOMP on a tools/match.py rebuild at 2004/b56 with --strict-relocs
   (port/tools/inferred_stub_adjudicated.txt); 70 was held back on three ov080
   bodies that slice_ov030cast.txt has since sliced for the same ROM class in
   ov030. 243 is on no placement row and is registered because VolcanoRing's
   Behavior spawns it at run time. Derivation in port/slice_w3e_ov022.txt, fill
   in hal/actor_classes_ov022_w3e.cpp. */
extern unsigned char g_profile_FL_KOMA_D[]; /* ov022 0x02113dc4, +4 = 80 */
void *daObjFl_Koma_D_c_classInit(void);                /* ov022 0x02111688, 60 B */
void hal_fill_rotating_platform_lll_vtable(void);     /* 0x02113de8, 32 */
extern unsigned char g_profile_FL_LONDON[];          /* ov022 0x02113f4c, +4 = 73 */
void *daObjFl_London_c_classInit(void);                         /* ov022 0x02111c7c, 48 B */
void hal_fill_lava_bridge_vtable(void);               /* 0x02113f70, 32 */
extern unsigned char g_profile_FL_SEESAW[];          /* ov022 0x02114010, +4 = 77 */
void *daObjFl_Seesaw_c_classInit(void);                         /* ov022 0x02111f3c, 48 B */
void hal_fill_lava_seesaw_vtable(void);               /* 0x02114034, 32 */
extern unsigned char g_profile_FL_MARUTA[];       /* ov022 0x02114380, +4 = 70 */
void *daObjFlMaruta_c_classInit(void);                      /* ov022 0x021125a4, 60 B */
void hal_fill_rolling_log_lll_vtable(void);           /* _ZTV13RollingLogLll, 32 */
extern unsigned char g_profile_OBJ_VOLCANO_CANNON[];         /* ov022 0x02114458, +4 = 243 */
void *daObj_volcanoCannon_c_classInit(void);                        /* ov022 0x02112918, 56 B */
void hal_fill_volcano_fire_vtable(void);              /* _ZTV13RollingLogLll, 31 */
/* run rel0215 wave 3 (lane w3-e): ov025's OTHER TWO IDS, level 17's pyramid
   interior -- and the last of that overlay's four-id cast. 162 is GRINDEL by
   the game's own id table (symbols/actor_renames_report.txt, confirmed a
   second way by daDkk_c_classInit's 928-byte allocation matching that file's
   alloc=0x3a0) and derives from ov091's Thwomp base, taking its slots 3 and 9
   from bodies already linked for THWOMP. 163 is SPINDEL, and that report
   resolves it to ov021 -- wrongly: at 0x0211373c ov021's record passes route 1
   and FAILS route 2 (w1 low halfword 0), while ov025's passes both. Neither
   record is named <Class>_SpawnInfo by dsd for 163, so its row goes by the
   mounted address. Derivation in port/slice_w3e_ov025.txt, fill in
   hal/actor_classes_ov025_w3e.cpp. */
extern unsigned char g_profile_DONKAKU[];             /* ov025 0x0211382c, +4 = 162 */
void *daDkk_c_classInit(void);                            /* ov025 0x02111cf4, 76 B */
void hal_fill_grindel_vtable(void);                   /* 0x02113850, 32 */
extern unsigned char g_profile_DONGURU[];           /* ov025 0x0211373c, +4 = 163 */
void *daDgr_c_classInit(void);                      /* ov025 0x02111898, 48 B */
void hal_fill_spindel_vtable(void);                   /* 0x02113760, 32 */
/* run rel0215 wave 3 (lane w3-e): RACING_PENGUIN (259, ov019), the other class
   in level 11's own overlay. It is on NO mounted level's object list and
   nothing spawns it at run time -- level 11's baseline is 84 spawned, ZERO
   skipped -- so this row is LINKAGE and changes no census. It is registered
   because both attribution routes name it and its closure is complete; an
   unseated class in a mounted overlay is a trap waiting for the first roster
   that reaches it. 31 slots, an Actor and not a Platform. Derivation in
   port/slice_w3e_ov019.txt, fill in hal/actor_classes_ov019_w3e.cpp. */
extern unsigned char g_profile_PENGUIN_RACER[];       /* ov019 0x021132ec, +4 = 259 */
void *daPgRcer_c_classInit(void);                      /* ov019 0x021125bc, 96 B */
void hal_fill_racing_penguin_vtable(void);            /* _ZTV13RacingPenguin, 31 */
/* lane VSPOPUP: NUMBER (330) and WING_FEATHER (345), both ov002 -- the two ids
   every live VS match declines. Neither is placed by a level's object table;
   both are spawned at RUN TIME (Actor::SpawnNumber, already in slice_gate10,
   and the wing cap), which is why no boot census ever asked for them. Number's
   methods and table are the ones the config spells _ZN15InvisibleSecret* /
   _ZTV15InvisibleSecret -- the name shift hal/actor_classes_bob_world.cpp
   documented when it seated the REAL 329. Derivation in port/slice_vspopup.txt,
   fills in hal/actor_classes_vspopup.cpp. */
extern unsigned char g_profile_OBJ_NUMBER[];              /* ov002 0x0210b0c8, +4 = 330 */
void *daObjNumber_c_classInit(void);                             /* ov002 0x020f0d90, 64 B */
void hal_fill_number_vtable(void);                    /* _ZTV15InvisibleSecret, 31 */
extern unsigned char g_profile_FEATHER[];         /* ov002 0x02108884, +4 = 345 */
void *daFeather_c_classInit(void);                        /* ov002 0x020b3248, 80 B */
void hal_fill_wing_feather_vtable(void);              /* _ZTV11WingFeather, 31 */
/* ov029: level 21's own overlay (water_city, Dire Dire Docks city). Eight
   actors, ids 94..101, every one placed by the level-21 boot census. The live
   tables are keyed by the last vtable store in each _Spawn (the dsd _ZTV names
   are shifted by one; see hal/actor_classes_ov029.cpp). Six are 32-slot
   Platforms, WaterDiamond's is a 31-slot Actor. Derivation in
   port/ov029_syms.txt / port/slice_ov029.txt, fills in
   hal/actor_classes_ov029.cpp. */
extern unsigned char g_profile_WC_UKISIMA[];              /* ov029 0x02113ff4, +4 = 94  */
void *daObjWc_Obj07_c_classInit(void);                            /* ov029 0x02112168 */
void hal_fill_rotating_platform_wdw_vtable(void);                 /* _ZTV15daObjWc_Obj07_c 0x02114018, 32 */
extern unsigned char g_profile_WC_OBJ01[];    /* ov029 0x02113c08, +4 = 95  */
void *daObjWcObj01_c_classInit(void);                  /* ov029 0x02111340 */
void hal_fill_float_on_water_platform_wdw_square_vtable(void);    /* _ZTV14daObjWcObj01_c, 32 */
extern unsigned char g_profile_WC_OBJ02[];                        /* ov029 0x02113ccc, +4 = 96  */
void *daObjWc_Obj02_c_classInit(void);                                      /* ov029 0x021116f8 */
void hal_fill_arrow_lift_vtable(void);                            /* _ZTV15daObjWc_Obj02_c 0x02113cf0, 32 */
extern unsigned char g_profile_WC_OBJ03[];                     /* ov029 0x02113d90, +4 = 97  */
void *daObjWc_Obj03_c_classInit(void);                                  /* ov029 0x02111a84 */
void hal_fill_water_diamond_vtable(void);                        /* _ZTV9ArrowLift 0x02113db4, 31 (Actor) */
extern unsigned char g_profile_WC_OBJ04[];             /* ov029 0x0211417c, +4 = 98  */
void *daObjWc_Obj04_c_classInit(void);                          /* ov029 0x02112964 */
void hal_fill_switch_activated_plank_vtable(void);               /* _ZTV20SwitchActivatedPlank 0x021141a0, 32 */
/* ---- link100 SPAWN2: CAGE_LIFT (99), ov029, level 21 x2. The note that
   stood here -- five of six lifecycle bodies guess-marked, so no faithful
   seat -- is retired by lane STUBADJ's rulings: all five are REAL_DECOMP in
   port/tools/inferred_stub_adjudicated.txt, each a match.py 2004/b56
   strict-reloc MATCH against the ROM with 0 WRONG-DEST. The SpawnInfo record
   was already mounted (g_profile_WC_OBJ05:0x1c, port/ov029_syms.txt); what
   was missing was the factory TU, the host table and this row. */
extern unsigned char g_profile_WC_OBJ05[];                         /* ov029 0x02113e50, +4 = 99  */
void *daObjWc_Obj05_c_classInit(void);                                      /* ov029 0x02111e74 */
void hal_fill_cage_lift_vtable(void);                            /* _ZTV15daObjWc_Obj05_c, 32 */
extern unsigned char g_profile_WC_OBJ06[]; /* ov029 0x02113f20, +4 = 100 */
void *daObjWcObj06_c_classInit(void);              /* ov029 0x02112044 */
void hal_fill_float_on_water_platform_wdw_rectangle_vtable(void);/* _ZTV14daObjWcObj06_c, 32 */
extern unsigned char g_profile_WC_MIZU[];                        /* ov029 0x021140b8, +4 = 101 */
void *daObjWc_Mizu_c_classInit(void);                                     /* ov029 0x021125f8 */
void hal_fill_wdw_water_vtable(void);                            /* _ZTV19RotatingPlatformWdw 0x021140dc, 32 */
/* ---- link100 OV ---- */
/* Two overlay actor classes the registry had never carried. Both
   resolved BY ADDRESS off ACTOR_SPAWN_TABLE; the full derivation, the
   ov002 one-class name shift and the ov039 double name are in the header
   of hal/actor_classes_ov_link100.cpp, the slice is
   port/slice_gate217.txt and the ov039 mount is port/ov039_syms.txt.
   The externs sit here rather than at the very end of the file because
   everything past the #define below is one line-continued macro. */
/* id 348, ov002 0x021085f8, RTTI 10daChRoom_c: the room-change trigger
   volume of a castle door. Placed by the DOOR sub-loader (entry 0 of
   data_ov002_0210cb88 is 348), one each on levels 17, 18 and 21. */
extern unsigned char g_profile_CH_ROOM[];   /* ov002 0x021085d4, +4 = 348 */
void *daChRoom_c_classInit(void);                  /* ov002 0x020b0980 */
void hal_fill_change_room_vtable(void);         /* _ZTV10daChRoom_c 0x021085f8, 31 */
/* id 314, ov039 0x02111858, RTTI 11daObjKumo_c: Tiny-Huge Island's sky
   clouds. Level 31 places seventeen; no other level places any. */
extern unsigned char g_profile_OBJ_KUMO[];         /* ov039 0x02111834, +4 = 314 */
void *daObjKumo_c_classInit(void);                        /* ov039 0x0211137c */
void hal_fill_cloud_vtable(void);               /* _ZTV5Cloud 0x02111858, 31 */
}

#define PORT_ACTOR_CLASS_ROWS                                                 \
    {286, "TREE", g_profile_TREE, daTree_c_classInit, hal_fill_tree_vtable},          \
    {350, "AMBIENT_SOUND", g_profile_SET_SE,                     \
     daSetSE_c_classInit, hal_fill_ambient_sound_vtable},               \
    /* ---- gate 211: ENEMY_SWITCH_TAG (357), ENEMY_SPAWNER (358), ov002 ---- */ \
    {357, "ENEMY_SWITCH_TAG", g_profile_ENEMY_SWITCH, daESwitch_c_classInit, \
     hal_fill_enemy_switch_tag_vtable},                                       \
    {358, "ENEMY_SPAWNER", g_profile_ENEMY_CREATE, daECreate_c_classInit,        \
     hal_fill_enemy_spawner_vtable},                                          \
    /* ---- gate 212: QUESTION_SWITCH (26), ov002 ---- */                     \
    {26, "QUESTION_SWITCH", g_profile_HATENA_SWITCH, daObjHatenaSwitch_c_classInit,   \
     hal_fill_question_switch_vtable},                                        \
    {17, "BLACK_BRICK_BLOCK", g_profile_BLOCK_LL,                      \
     daObjBlockL_c_classInit_BLOCK_LL, hal_fill_black_brick_block_vtable},               \
    /* ---- gate 78: BLUE_COIN_SWITCH (10), ov002 ---- */                      \
    {10, "BLUE_COIN_SWITCH", g_profile_BC_SWITCH, daObjBC_Switch_c_classInit,   \
     hal_fill_blue_coin_switch_vtable},                                       \
    /* ---- gate 79: STAR_MARKER (180), ov002 ---- */                          \
    {180, "STAR_MARKER", g_profile_STARBASE, daStarBase_c_classInit,              \
     hal_fill_star_marker_vtable},                                            \
    {184, "SIGN_POST", g_profile_TATEFUDA, daObjTatefuda_c_classInit,                    \
     hal_fill_sign_post_vtable},                                              \
    {276, "ONE_UP_MUSHROOM", g_profile_ONEUPKINOKO, da1up_c_classInit_ONEUPKINOKO,    \
     hal_fill_one_up_mushroom_vtable},                                        \
    {338, "CASTLE_WATER", g_profile_MC_WATER, daObjMcWater_c_classInit,           \
     hal_fill_castle_water_vtable},                                           \
    {339, "METAL_NET", g_profile_MC_METALNET, daObjMc_Metalnet_c_classInit,                    \
     hal_fill_metal_net_vtable},                                              \
    {342, "FLAG", g_profile_MC_FLAG, daMcFlag_c_classInit, hal_fill_flag_vtable},          \
    {343, "BIRD", g_profile_SBIRD, daSBird_c_classInit, hal_fill_bird_vtable},          \
    {187, "RABBIT", g_profile_MIP, daMip_c_classInit, hal_fill_rabbit_vtable},  \
    {229, "RABBIT_KEY", g_profile_OBJ_MIP_KEY, daObj_Mip_Key_c_classInit,                 \
     hal_fill_rabbit_key_vtable},                                            \
    {235, "LAKITU_BRO", g_profile_C_JUGEM, daC_Jugem_c_classInit,                 \
     hal_fill_lakitu_bro_vtable},                                             \
    {207, "CANNON", g_profile_CANNON, daCnn_c_classInit, hal_fill_cannon_vtable}, \
    {349, "EXIT", g_profile_CH_SCENE, daChScene_c_classInit, hal_fill_exit_vtable},         \
    {197, "WATERFALL_MIST", g_profile_WATERFALL, daObjWaterfall_c_classInit,    \
     hal_fill_waterfall_mist_vtable},                                        \
    {336, "BUTTERFLY", g_profile_BUTTERFLY, daBtfly_c_classInit,                 \
     hal_fill_butterfly_vtable},                                             \
    {337, "UNCHAINED_CHOMP", g_profile_WANWAN2, daWanwan2_c_classInit, \
     hal_fill_unchained_chomp_vtable},                                       \
    {344, "FISH", g_profile_FISH, daFish_c_classInit, hal_fill_fish_vtable},             {353, "DOOR", g_profile_DOOR, daDoor_c_classInit, hal_fill_door_vtable},             {20, "QUESTION_BLOCK", g_profile_HATENA_BLOCK, daObjHatenaBlock_c_classInit_HATENA_BLOCK,          hal_fill_question_block_vtable},         \
    {334, "HUD", g_profile_METER, port_factory_hud, hal_fill_hud_vtable},   \
    {335, "MINIMAP", g_profile_MAP, port_factory_minimap,             \
     hal_fill_minimap_vtable},                                                 \
    /* ---- gate 33: Bob-omb Battlefield, one contiguous block ---- */         \
    {288, "COIN", g_profile_COIN, daCoin_c_classInit_COIN, hal_fill_coin_vtable},           \
    {289, "RED_COIN", g_profile_RED_COIN, daCoin_c_classInit_RED_COIN, hal_fill_coin_vtable}, \
    {290, "BLUE_COIN", g_profile_BLUE_COIN, daCoin_c_classInit_BLUE_COIN,                     \
     hal_fill_coin_vtable},                                                    \
    {15, "BRICK_BLOCK", g_profile_BLOCK_L, daObjBlockL_c_classInit_BLOCK_L, 0},            \
    /* gate 48: two more of the same class body (installs _ZTV13BigBrickBlock,  \
       filled at gate 16), for Whomp's Fortress. No fill of their own, like     \
       BRICK_BLOCK. */                                                          \
    {16, "BIG_BRICK_BLOCK", g_profile_DP_BLOCK_L, daObjBlockL_c_classInit_DP_BLOCK_L, 0},   \
    {46, "FORTRESS_TOWER_WALL", g_profile_BK_FUTA,                    \
     daObjBlockL_c_classInit_BK_FUTA, 0},                                              \
    {322, "MEGA_MUSHROOM_BLOCK_TAG", g_profile_BLK_SKINOKO_TAG,           \
     daObjBlockItemTag_c_classInit_BLK_SKINOKO_TAG, hal_fill_mega_mushroom_block_tag_vtable},     \
    /* gate 49: same tag body (installs _ZTV10BrickBlock, filled above), for   \
       Whomp's Fortress. No fill of its own. */                               \
    {321, "ONE_UP_MUSHROOM_BLOCK_TAG", g_profile_BLK_OKINOKO_TAG,        \
     daObjBlockItemTag_c_classInit_BLK_OKINOKO_TAG, 0},                                         \
    {14, "CANNON_HATCH", g_profile_CANNON_SHUTTER, daObjCannonShutter_c_classInit,             \
     hal_fill_cannon_hatch_vtable},                                            \
    {297, "HEALING_HEART", g_profile_HEART, daObjHeart_c_classInit,      \
     hal_fill_healing_heart_vtable},                                        \
    /* CAP (269), EXCLAMATION_SWITCH (11) and ROLLING_IRON_BALL (220) are     \
       registered at gate 51 (their PMF state tables are now seated). WATER_  \
       BOMB (208) is registered at gate 55 -- see the gate-55 note in         \
       actor_classes_bob_world.cpp. */                                        \
    {208, "WATER_BOMB", g_profile_WATERBOMB, daWbm_c_classInit,                 \
     hal_fill_water_bomb_vtable},                                             \
    {329, "INVISIBLE_SECRET", g_profile_SECRET_COIN, daSCoin_c_classInit,\
     hal_fill_invisible_secret_vtable},                                        \
    /* gate 46: the moving bar (RTTI daBar_c), Whomp's Fortress */             \
    {287, "INVISIBLE_POLE", g_profile_BAR, daBar_c_classInit,      \
     hal_fill_invisible_pole_vtable},                                          \
    /* lane SEAT-CAMERATAG: the camera tag (RTTI daCamTag_c), placed by no    \
       shipped level; SM64DS_SPAWN_ACTOR=333 is its edge */                   \
    {333, "CAMERA_TAG", g_profile_CAMERA_TAG, daCamTag_c_classInit,                 \
     hal_fill_camera_tag_vtable},                                             \
    {299, "ARROW_SIGN_LEFT", g_profile_YAJIRUSI_L, daObjYajirusi_c_classInit_YAJIRUSI_L,     \
     hal_fill_arrow_sign_vtable},                                              \
    {300, "ARROW_SIGN_RIGHT", g_profile_YAJIRUSI_R, daObjYajirusi_c_classInit_YAJIRUSI_R,  \
     hal_fill_arrow_sign_vtable},                                              \
    {347, "WARP", g_profile_WARPKUN, daWarpkun_c_classInit, hal_fill_warp_vtable},           \
    {298, "WARP_PIPE", g_profile_DOKAN, daObjSimpleBg_c_classInit_DOKAN,                     \
     hal_fill_warp_pipe_vtable},                                           \
    /* ---- gate 32, Bob-omb Battlefield's cast (append-only block) ---- */    \
    {206, "BOB_OMB", g_profile_BOMBHEI, daBmb_c_classInit, hal_fill_bob_omb_vtable},  \
    {285, "KOOPA_SHELL", g_profile_SHELL, daShl_c_classInit,               \
     hal_fill_koopa_shell_vtable},                                             \
    {200, "GOOMBA", g_profile_KURIBO, daKrb_c_classInit_KURIBO, hal_fill_goomba_vtable},    \
    {181, "BOB_OMB_BUDDY", g_profile_RED_BOMBHEI, daRedBombhei_c_classInit,            \
     hal_fill_bob_omb_buddy_vtable},                                           \
    {219, "CHAIN_CHOMP", g_profile_WANWAN, daWanwan_c_classInit,               \
     hal_fill_chain_chomp_vtable},                                             \
    {41, "CHAIN_CHOMP_FENCE", g_profile_WANWAN_SHUTTER, daObjWanwanShutter_c_classInit,\
     hal_fill_chain_chomp_fence_vtable},                                       \
    {27, "STUMP", g_profile_PILE, daObjPile_c_classInit, hal_fill_stump_vtable},        \
    {188, "KOOPA_THE_QUICK", g_profile_RACE_NOKO, daRNk_c_classInit,     \
     hal_fill_koopa_the_quick_vtable},                                         \
    {205, "KOOPA_FLAG", g_profile_RACE_FLAG, daRFlag_c_classInit,                  \
     hal_fill_koopa_flag_vtable},                                              \
    {189, "KING_BOB_OMB", g_profile_BOMBKING, daBombking_c_classInit,              \
     hal_fill_king_bob_omb_vtable},                                            \
    /* ---- gates 40-44: levels 1 and 2 (append-only block) ---- */            \
    {354, "STAR_DOOR", g_profile_STAR_GATE, daStarGate_c_classInit,                     \
     hal_fill_star_door_vtable},                                              \
    {36, "TRAP", g_profile_C1_TRAP, daObjC1_Trap_c_classInit,                                \
     hal_fill_ov010_trap36_vtable},                                         \
    /* GATE 232 (link100 SPAWN2) MOVED THE FILL HERE. _ZTV4Trap 0x02112ba8 \
       is LIGHT_BEAM's own table -- its RTTI string reads                   \
       "15daObjC1Hikari_c" and daObjC1Hikari_c_classInit's pool word 0x02111e0c loads \
       it -- so the row that installs it is this one, not TRAP's. TRAP has  \
       its own table 0x02112ae4 and its own fill now. */                    \
    {37, "LIGHT_BEAM", g_profile_C1_HIKARI, daObjC1Hikari_c_classInit,                \
     hal_fill_trap_vtable},                                                 \
    /* ---- gate 50: ov080's PAINTING (append-only block) ---- */              \
    {307, "PAINTING", g_profile_PICTURE_GATE, daPicGate_c_classInit,                      \
     hal_fill_painting_vtable},                                               \
    /* ---- gate 51: CAP (269), the metal/vanish/wing cap on the ground ---- */\
    {269, "CAP", g_profile_OBJ_MARIO_CAP, daObjMarioCap_c_classInit, hal_fill_cap_vtable},               \
    /* ---- gate 51: EXCLAMATION_SWITCH (11), the floor switch ---- */         \
    {11, "EXCLAMATION_SWITCH", g_profile_HANSWITCH,                    \
     daObjSwitch_c_classInit_HANSWITCH, hal_fill_exclamation_switch_vtable},             \
    /* ---- gate 51: ROLLING_IRON_BALL (220), Whomp's Fortress ---- */         \
    {220, "ROLLING_IRON_BALL", g_profile_IRONBALL, daIbl_c_classInit,\
     hal_fill_rolling_iron_ball_vtable},                                       \
    /* ---- gate 60: TOWER_STEP (52), Whomp's Fortress (ov015) ---- */         \
    {52, "TOWER_STEP", g_profile_BK_LIFT01, daObjBk_Lift_c_classInit,                    \
     hal_fill_tower_step_vtable},                                              \
    /* ---- gate 61: ROTATING_BRIDGE (51), Whomp's Fortress (ov015) ---- */    \
    {51, "ROTATING_BRIDGE", g_profile_BK_ROTEBAR, daObjBk_Rotebar_c_classInit,    \
     hal_fill_rotating_bridge_vtable},                                         \
    /* ---- gate 62: POLE_BILLBOARD (42), KNOCK_DOWN_PLANK (44) ---- */         \
    {42, "POLE_BILLBOARD", g_profile_BK_BILLBOARD, daObjBkBillboard_c_classInit,       \
     hal_fill_pole_billboard_vtable},                                          \
    {44, "KNOCK_DOWN_PLANK", g_profile_BK_BOTAOSI, daObjBk_Botaosi_c_classInit,   \
     hal_fill_knock_down_plank_vtable},                                        \
    /* ---- gate 63: ROTATING_PLATFORM_WF (50), Whomp's Fortress (ov015) ---- */\
    {50, "ROTATING_PLATFORM_WF", g_profile_BK_UKISIMA,                 \
     port_factory_rotating_platform_wf, hal_fill_rotating_platform_wf_vtable}, \
    /* ---- gate 76: MOVING_BAR_BIG (53), MOVING_BAR_SMALL (54) ---- */         \
    /* Both install vtable 0x0211458c (the _ZN14KnockDownPlank* methods) and    \
       run KnockDownPlank's lifecycle; the shared fill is idempotent so the     \
       second row re-runs it harmlessly. */                                    \
    {53, "MOVING_BAR_BIG", g_profile_BK_DOSSUNBAR_L, daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L,         \
     hal_fill_moving_bar_vtable},                                              \
    {54, "MOVING_BAR_SMALL", g_profile_BK_DOSSUNBAR_S, daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S,   \
     hal_fill_moving_bar_vtable},                                              \
    /* ---- gate 77: FALL_BLOCK_WF (45), Whomp's Fortress (ov015/ov098) ---- */ \
    {45, "FALL_BLOCK_WF", g_profile_BK_DOWN_B, daObjBk_Fall_Block_c_classInit,            \
     hal_fill_fall_block_wf_vtable},                                          \
    /* ---- gate 42: ov010's PEACH_PAINTING (38) ---- */                       \
    {38, "PEACH_PAINTING", g_profile_C1_PEACH, daObjC1Peach_c_classInit,       \
     hal_fill_peach_painting_vtable},                                          \
    /* ---- gates 70-74: Whomp's Fortress' level-7 cast (append-only) ---- */   \
    /* gate 72: THWOMP (161), ov091 */                                         \
    {161, "THWOMP", g_profile_DOSUN, port_factory_thwomp,                     \
     hal_fill_thwomp_vtable},                                                  \
    /* gate 74: SLIDING_PLATFORM_WF (55), ov091 */                             \
    {55, "SLIDING_PLATFORM_WF", g_profile_BK_TRANSBAR,                   \
     daObjSimpleLift_c_classInit_BK_TRANSBAR, hal_fill_sliding_platform_wf_vtable},           \
    /* gate 73: ROTATING_UP_DOWN_PLATFORM (29), ov091 (Utm name shift) */      \
    {29, "ROTATING_UP_DOWN_PLATFORM", g_profile_UPDOWN_LIFT,        \
     daObjRotateUpdownLift_c_classInit_UPDOWN_LIFT, hal_fill_rotating_up_down_platform_vtable}, \
    /* gate 71: FIRE_PIRANHA_PLANT (253), ov084 (FirePiranhaPlantBig shift) */ \
    {253, "FIRE_PIRANHA_PLANT", g_profile_PAKUN2,                    \
     daFPkn_c_classInit_PAKUN2, hal_fill_fire_piranha_plant_vtable},             \
    /* gate 70: PIRANHA_PLANT (250), ov084 */                                  \
    {250, "PIRANHA_PLANT", g_profile_PAKUN, daPkn_c_classInit,         \
     hal_fill_piranha_plant_vtable}, \
    /* ---- gate 82: SHUTTER_BOB (40), Bob-omb Battlefield (ov014) ---- */ \
    {40, "SHUTTER_BOB", g_profile_SWITCHDOOR, daObjBSwdoor_c_classInit, \
     hal_fill_shutter_bob_vtable}, \
    /* ---- gate 83: SEESAW_BOB (39), Bob-omb Battlefield (ov095) ---- */ \
    {39, "SEESAW_BOB", g_profile_BOMB_SEESAW, daObjSeesaw_c_classInit_BOMB_SEESAW, \
     hal_fill_seesaw_bob_vtable}, \
    /* ---- gate 89: POWER_STAR (178), ov002 ---- */ \
    {178, "POWER_STAR", g_profile_STAR, daStar_c_classInit_STAR, \
     hal_fill_power_star_vtable}, \
    /* ---- gate 90: STAR_CAMERA (177), ov002 (bare Actor, base vtable) ---- */ \
    {177, "STAR_CAMERA", g_profile_STAR_CAMERA, StarCamera_Spawn, \
     hal_fill_actor_base_vtable}, \
    /* ---- gates 64-69: Whomp's Fortress' enemy cast (ov079), now live ---- */ \
    {164, "WHOMP", g_profile_BATAN, daBtn_c_classInit_BATAN, hal_fill_whomp_vtable_c}, \
    {165, "WHOMP_KING", g_profile_BATANKING, daBtn_c_classInit_BATANKING, 0}, \
    {222, "BULLET_BILL", g_profile_KILLER, daKlr_c_classInit, \
     hal_fill_bullet_bill_vtable_c}, \
    /* ---- gate BULLET: ov002's Snufit-fired projectile ---- */ \
    {233, "BULLET", g_profile_PROPELLER_HEYHO_FIRE, daPropeller_Heyho_Fire_c_classInit, hal_fill_bullet_vtable}, \
    {43, "BILL_BLASTER", g_profile_BK_KILLER_DAI, daObjBkKillerdai_c_classInit, \
     hal_fill_bill_blaster_vtable_c}, \
    {47, "FORTRESS_WALL", g_profile_BK_KABE01, daObjBk_Kabe_c_classInit_BK_KABE01, \
     hal_fill_fortress_wall_vtable_c}, \
    {48, "FORTRESS_WALL_BREAKABLE", g_profile_BK_KABE00, \
     daObjBk_Kabe_c_classInit_BK_KABE00, 0}, \
    /* ---- gate 102: FORTRESS_TOWER (49), Whomp's Fortress (ov102) ---- */ \
    /* Installs _ZTV13FortressTower, filled by the WARP_PIPE row above; no \
       fill of its own, the BIG_BRICK_BLOCK / LIGHT_BEAM shape. */ \
    {49, "FORTRESS_TOWER", g_profile_BK_TOWER, daObjSimpleBg_c_classInit_BK_TOWER, 0}, \
    /* ---- gate 103: STAR_SWITCH (12), Whomp's Fortress (ov002) ---- */ \
    /* Installs _ZTV10StarSwitch, filled by the EXCLAMATION_SWITCH row (with \
       its PMF state seat); no fill of its own. */ \
    {12, "STAR_SWITCH", g_profile_STAR_SWITCH, daObjSwitch_c_classInit_STAR_SWITCH, 0}, \
    /* ---- gate 128: EXCLAMATION_BLOCK (21), Jolly Roger Bay ship (ov102) ---- */ \
    /* Installs _ZTV13QuestionBlock, filled by the QUESTION_BLOCK row above; no \
       fill of its own, the FORTRESS_TOWER / STAR_SWITCH shape. */ \
    {21, "EXCLAMATION_BLOCK", g_profile_ITEM_BLOCK, daObjHatenaBlock_c_classInit_ITEM_BLOCK, 0}, \
    /* ---- gate 121: STATIC_ROCK (61), Jolly Roger Bay (ov102) ---- */ \
    /* Installs _ZTV13FortressTower, filled by the WARP_PIPE row above; no \
       fill of its own, the FORTRESS_TOWER shape. */ \
    {61, "STATIC_ROCK", g_profile_KI_IWA, daObjSimpleBg_c_classInit_KI_IWA, 0}, \
    /* ---- gate 122: BRICK_BLOCK_SWITCH_ACTIVATED (19), Jolly Roger Bay ---- */ \
    /* Installs _ZTV13BigBrickBlock, filled at gate 16; no fill of its own, the \
       BIG_BRICK_BLOCK shape. */ \
    {19, "BRICK_BLOCK_SWITCH_ACTIVATED", g_profile_SW_BLOCK_L, \
     daObjBlockL_c_classInit_SW_BLOCK_L, 0}, \
    /* ---- gate 123: GREEN_SHELL_BLOCK_TAG (323), Jolly Roger Bay ---- */ \
    /* Installs _ZTV10BrickBlock, filled at gate 49; no fill of its own, the \
       ONE_UP_MUSHROOM_BLOCK_TAG shape. */ \
    {323, "GREEN_SHELL_BLOCK_TAG", g_profile_BLK_GNSHELL_TAG, \
     daObjBlockItemTag_c_classInit_BLK_GNSHELL_TAG, 0}, \
     /* ---- gate 143: IceSlideManager (356), Cool Cool Mountain slide (ov019) ---- */ \
     /* Level 11's own overlay class; SpawnInfo from the per-symbol ov019 mount, \
        vtable filled by hal/actor_classes_ccm.cpp. */ \
     {356, "ICE_SLIDE_MANAGER", g_profile_SLIDER_MANAGER, daSldMng_c_classInit, \
      hal_fill_ice_slide_manager_vtable}, \
     /* ---- gate 190: CCM slice A -- ICE_SHEET + POWER_STAR_CREATE (ov018), \
        ONE_UP_LOGO (ov002) ---- */ \
     {295, "ICE_SHEET", g_profile_ICE_BOARD, daObjIceBoard_c_classInit, \
      hal_fill_ice_sheet_vtable}, \
     {355, "POWER_STAR_CREATE", g_profile_STAR_CREATE, daSCre_c_classInit, \
      hal_fill_power_star_create_vtable}, \
     {331, "ONE_UP_LOGO", g_profile_OBJ_1UPLOGO, daObj1UpLogo_c_classInit, \
      hal_fill_one_up_logo_vtable}, \
     {359, "SOUND_OBJECT", g_profile_SOUND_OBJ, daSoundObj_c_classInit, \
      hal_fill_sound_object_vtable}, \
     /* ---- gate 191: SKI_LIFT (63) + MOTHER_PENGUIN (257), ov018 ---- */ \
     {63, "SKI_LIFT", g_profile_SM_LIFT, daObjSm_Lift_c_classInit, \
      hal_fill_ski_lift_vtable}, \
     {257, "MOTHER_PENGUIN", g_profile_PENGUIN_MOTHER, daPgMthr_c_classInit, \
      hal_fill_mother_penguin_vtable}, \
     /* ---- run lvled, lane intro-cutscene: the OPENING's whole cast ---- */ \
     /* CUTSCENE_OBJECT (0x160) is every visible thing in the new-file opening: \
        the Peach letter (kuppa cmd 0x17), the Lakitu flight (0x12), Peach \
        (0x13), the three warp pipes (0x14/0x15/0x16) and the cloud (0x19). \
        One class, spawned repeatedly by the script with a different model each \
        time. Fully matched and offered nowhere until port/slice_intro.txt, so \
        the opening used to log "[spawn-declined] actor 0x160 not registered" \
        and play to an empty stage. Seat in hal/actor_classes_intro.cpp. */ \
     {0x160, "CUTSCENE_OBJECT", g_profile_DEMO, daDemo_c_classInit, \
      hal_fill_cutscene_object_vtable}, \
     /* ---- gate 192: the ov081 trio -- ICE_BLOCK (18), MR_BLIZZARD (223),
        SPINDRIFT (312) ---- */ \
     /* MR_BLIZZARD (223) WAS unregistered here for six weeks, and is not any
        more. Two exclusion reasons were recorded and both are closed.
        (1) The ORIGINAL one, a func_0204488c "unvalidated raw-DMA source"
        fear, was resolved by the dma verdict: Part.fc/f8 are rebased at load
        and the synchronous forward is safe.
        (2) The SECOND one (registration attempted 2026-08-10) was written up
        as "a codegen-sensitive callee-saved register violation": the register
        holding `this` was seen coming back from one of Behavior's callees
        skewed to this+0x150 (mWithMeshClsn), after which Behavior's tail
        store of the 0x1000 anim speed at +0x368 landed 0x4b8 past the base,
        on the next instance's render-list node. The list of suspect callees
        was Actor::UpdatePos, Enemy::UpdateWMClsn, func_ov081_021254d8,
        func_ov081_021243cc and CylinderClsn::Clear/Update.
        THAT DIAGNOSIS WAS WRONG IN ITS MECHANISM AND RIGHT IN ITS SYMPTOM.
        No callee violated anything. The seam was the class's own POINTER-TO-
        MEMBER STATE DISPATCH, the family pmf_guard was later built for. The
        ROM's own tick dispatch (MrBlizzard::Behavior 0x0212588c, at
        0x02125a98) reads `ldr r1,[r4,#0x3f8] / ldr r0,[r1,#8] / add r3,r1,#8 /
        ldr r1,[r3,#4] / add r0,r4,r1,asr #1 / blx r1`: the receiver is
        computed into r0, the FIRST ARGUMENT REGISTER, and nothing is placed
        on the stack. MSVC compiles the matched source's equivalent as a
        __thiscall dispatch with the receiver in ecx and an empty stack, while
        the ten seated cells are flat f(self) bodies that read the receiver
        from [esp+4]. Each cell therefore ran on whatever stale word was
        there; one of those words was a spilled &this->mWithMeshClsn, which is
        exactly this+0x150, and 0x150 + 0x368 = 0x4b8, the reported stomp.
        Nothing "came back skewed": the callee was handed the wrong receiver
        going in.
        The cure already exists and predates this registration. The state
        installer func_ov081_02125488 and Behavior's own tick dispatch are
        host-copied in port/unmatched/MrBlizzard_StateDispatch.cpp and
        port/unmatched/MrBlizzard_Behavior.cpp, which call the cell as a plain
        function pointer through the seat's own PortMrBlizzardPair layout, so
        the receiver goes where the flat bodies read it. With those two in the
        image the class boots and ticks clean: levels 10, 19 and 48 carry 3, 4
        and 5 of them, 0 skipped and 0 quarantined over 600-frame idle and
        moving runs, three runs each. Registered here on that measurement. */ \
     {223, "MR_BLIZZARD", g_profile_SNOWMAN, daSnowman_c_classInit, \
      hal_fill_mr_blizzard_vtable}, \
     {18, "ICE_BLOCK", g_profile_ICE_BLOCK_LL, daObjIceBlock_c_classInit, \
      hal_fill_ice_block_vtable}, \
     {312, "SPINDRIFT", g_profile_HUWAHUWA, daHuwa_c_classInit, \
      hal_fill_spindrift_vtable}, \
     /* ---- gate 193: the actual SNOWMAN (272) + BABY_PENGUIN (256), ov072 ---- */ \
     /* BABY_PENGUIN's original exclusion ("corrupted-IP jump deep in the
        shared actor-Process pipeline") is ROOT-CAUSED and FIXED
        (babypenguin-crash lane): it was never pipeline infra -- the
        matched _ZN11BabyPenguin6RenderEv.cpp dispatches ModelAnim slot 5
        through a local six-virtual ROM-order shadow, which the host
        _ZTV9ModelAnim numbers as Virtual18 (the documented ModelAnim
        slot-5 collision, the Whomp/Butterfly/Fish case); Virtual18's
        two-stack-arg trampoline skewed esp by 4 on return and the caller
        ret'd into its own spilled `self` (measured: EIP == the actor
        pointer, DEP execute-violation, first frame the near instance
        drew -- the pipeline frame the dump named was the victim, not the
        culprit). Fix: the Render TU is host-copied in
        port/unmatched/ModelAnim_Renders.cpp and dropped from
        slice_gate193.txt; full forensics in hal/actor_classes_ov072.cpp's
        header and the commit message. */ \
     {272, "SNOWMAN", g_profile_BIG_SNOWMAN, daBgSnwmn_c_classInit, \
      hal_fill_snowman_vtable}, \
     {256, "BABY_PENGUIN", g_profile_PENGUIN_BABY, daPgBby_c_classInit, \
      hal_fill_baby_penguin_vtable}, \
     /* ---- gate 193 extension (lane w3-d): the other two ov072 classes.
        This note used to say SnowmanHead (273) and SnowmanBody (274) stay
        unregistered. They do not any more, and ov072 is now hosted whole.
        SNOWMAN_HEAD is the one with a measured before/after: level 10's
        baseline census on cons cc10acd4e reads "[spawn] actor 0x111 not
        registered, skipped", and daBgSnwmn_c::InitResources spawns exactly
        that id, unconditionally, until star 5 is collected -- so every Cool
        Cool Mountain boot the port had ever done declined it.
        SNOWMAN_BODY is registered on the same ROM evidence (identity by both
        routes, 31-slot table read out of the overlay image) but is on NO
        census in this lane's proof set and nothing in src/ spawns 0x112, so
        its seat is correct-and-unexercised rather than demonstrated. The two
        are not equally proved and are deliberately not written as if they
        were. Both classes' PMF state cells are seated with host addresses
        before their fills return; see hal/actor_classes_ov072.cpp. */ \
     {273, "SNOWMAN_HEAD", g_profile_BIG_SNOWMAN_HEAD, daBgSnmHed_c_classInit, \
      hal_fill_snowman_head_vtable}, \
     {274, "SNOWMAN_BODY", g_profile_BIG_SNOWMAN_BODY, daBgSnmBdy_c_classInit, \
      hal_fill_snowman_body_vtable}, \
     /* ---- gate 194: HOOT_THE_OWL (234), ov094 ---- */ \
     {234, "HOOT_THE_OWL", g_profile_OWL, daOwl_c_classInit, \
      hal_fill_hoot_the_owl_vtable}, \
     /* ---- gate 199: SWITCH_PILLAR (34) + BASEMENT_WATER (35), ov012 ---- */ \
     {34, "SWITCH_PILLAR", g_profile_C0_SWITCH, daObjC0_Switch_c_classInit, \
      hal_fill_switch_pillar_vtable}, \
     {35, "BASEMENT_WATER", g_profile_C0_WATER, daObjC0Water_c_classInit, \
      hal_fill_basement_water_vtable}, \
     /* ---- gate 200: PUSH_BLOCK (306, the "PowerFlower" decoy) + MUGEN_BGM
        (351), ov002 ---- */ \
     {306, "PUSH_BLOCK", g_profile_POWER_UP_ITEM, daObjPowerUpItem_c_classInit, \
      hal_fill_pushblock_vtable}, \
     {351, "MUGEN_BGM", g_profile_MUGEN_BGM, daMugenBGM_c_classInit, \
      hal_fill_mugen_bgm_vtable}, \
     /* ---- gate 202: the 2F clock-painting furniture trio, ov013 ---- */ \
     {292, "CLOCK_HAND_SHORT", g_profile_CLOCK_SHORT, \
      daObjClock_c_classInit_CLOCK_SHORT, hal_fill_clock_hand_vtable}, \
     {293, "CLOCK_HAND_LONG", g_profile_CLOCK_LONG, \
      daObjClock_c_classInit_CLOCK_LONG, hal_fill_clock_hand_vtable}, \
     {294, "CLOCK_PENDULUM", g_profile_CLOCK_HURIKO, \
      daObjClockHuriko_c_classInit, hal_fill_clock_pendulum_vtable}, \
    /* ---- gate 149: HAUNTED_CHAIR (326), Big Boo's Haunt (ov020, level 12) ---- */ \
    {326, "HAUNTED_CHAIR", g_profile_CHAIR, daChair_c_classInit, \
     hal_fill_haunted_chair_vtable}, \
    /* ---- gate 172: CRATE (194), Hazy Maze Cave's first fresh class (ov098) ---- */ \
    {194, "CRATE", g_profile_BLOCK_S, daObjBlockS_c_classInit, hal_fill_crate_vtable}, \
    /* ---- gate 173: UP_DOWN_LIFT_HMC (33), Hazy Maze Cave (ov095) ---- */ \
    {33, "UP_DOWN_LIFT_HMC", g_profile_UDLIFT, daUdlift_c_classInit_UDLIFT, \
     hal_fill_up_down_lift_hmc_vtable}, \
    /* ---- gate 174: MONTY_MOLE (310), ov080's enemy cast ---- */ \
    {310, "MONTY_MOLE", g_profile_CHOROPU, daChoropu_c_classInit, \
     hal_fill_monty_mole_vtable}, \
    /* ---- gate 208: MONTY_MOLE_ROCK (311), the rock the mole throws ---- */ \
    {311, "MONTY_MOLE_ROCK", g_profile_CHORO_ROCK, daChoro_Rock_c_classInit, \
     hal_fill_monty_mole_rock_vtable}, \
    /* ---- gate 206: DIAMOND_LIFT (135), ov043, level 35's six lifts ---- */ \
    {135, "DIAMOND_LIFT", g_profile_KM1_UKISHIMA, daObjKm1_Ukishima_c_classInit, \
     hal_fill_diamond_lift_vtable}, \
    /* ---- lane SEAT-BDW: two of ov043's remaining Bowser-in-the-Dark-World \
       classes. RICKSHAW_PLATFORM_BDW (136) is RICKSHAW_BDW's runtime child \
       (not level-placed; census via SM64DS_SPAWN_ACTOR=136), STAIRS_BDW (134, \
       L35 x1) the self-assembling staircase; both fully matched. link100 \
       SPAWN2 adds RICKSHAW_BDW (137, L35 x1), the axle those four children \
       hang off: its three marker-carrying bodies are ruled REAL_DECOMP and \
       its fourth never carried a marker. ---- */ \
    {136, "RICKSHAW_PLATFORM_BDW", g_profile_KM1_KURUMA, \
     daObjKm1_Kuruma_c_classInit, hal_fill_rickshaw_platform_bdw_vtable}, \
    {134, "STAIRS_BDW", g_profile_KM1_DORIFU, daObjKm1_Dorifu_c_classInit, \
     hal_fill_stairs_bdw_vtable}, \
    {137, "RICKSHAW_BDW", g_profile_KM1_KURUMAJIKU, daObjKm1_Kurumajiku_c_classInit, \
     hal_fill_rickshaw_bdw_vtable}, \
    /* ---- gate 175: the flames (ov002) -- BLUE_FLAME carries the fill, \
       RED_FLAME reuses it, the GreenShellBlockTag shape ---- */ \
    {317, "BLUE_FLAME", g_profile_OBJ_BLUE_FIRE, daObjFire_c_classInit_OBJ_BLUE_FIRE, \
     hal_fill_blue_flame_vtable}, \
    {316, "RED_FLAME", g_profile_OBJ_RED_FIRE, daObjFire_c_classInit_OBJ_RED_FIRE, 0}, \
    /* ---- gate 176: SCUTTLEBUG (255), ov071's first mount ---- */ \
    {255, "SCUTTLEBUG", g_profile_SPIDER, daSpd_c_classInit, \
     hal_fill_scuttlebug_vtable}, \
    /* ---- run linkw wave 7, lane w7b: the MR. I family (ov071) -- MR_I \
       carries the fill, BIG_MR_I reuses it (one class, one vtable, the \
       RED_FLAME shape), MR_I_PROJECTILE has its own ---- */ \
    {262, "MR_I", g_profile_EYEKUN, daEykn_c_classInit_EYEKUN, hal_fill_mri_vtable}, \
    {263, "BIG_MR_I", g_profile_EYEKUN_BOSS, daEykn_c_classInit_EYEKUN_BOSS, 0}, \
    {264, "MR_I_PROJECTILE", g_profile_EYEKUN_BEAM, daEyBm_c_classInit, \
     hal_fill_mri_projectile_vtable}, \
    /* ---- gate 177: ov064's Bully family + firebar (Lethal Lava Land) ---- */ \
    {215, "BULLY", g_profile_DONKETU, port_factory_bully, hal_fill_bully_vtable}, \
    {216, "BIG_BULLY", g_profile_BOSS_DONKETU, port_factory_big_bully, \
     hal_fill_big_bully_vtable}, \
    {81, "ROTATING_FIREBAR", g_profile_FL_KOMA_U, daObjFl_KomaU_c_classInit, \
     hal_fill_rotating_firebar_vtable},     /* ---- gate 178: ov064's METAL_NET_LIFT + LAVA_BUBBLE (Lethal Lava Land) ---- */     {69, "METAL_NET_LIFT", g_profile_FL_AMILIFT, daObjFl_Amilift_c_classInit,      hal_fill_metal_net_lift_vtable},     {214, "LAVA_BUBBLE", g_profile_BUBBLE, daBbl_c_classInit,      hal_fill_lava_bubble_vtable},     /* ---- gate 179: ov064's BowserPuzzle pair (Lethal Lava Land) ---- */     {78, "BOWSER_PUZZLE_PIECE", g_profile_FL_PUZZLE,      daObjFl_Puzzle_c_classInit, hal_fill_bowser_puzzle_pair_vtables},     {79, "BOWSER_PUZZLE_MANAGER", g_profile_FL_COIN,      port_factory_bowser_puzzle_shell, hal_fill_bowser_puzzle_shell_vtable},     /* ---- gate 182: ov062's KOOPA + CHUCKYA (Bob-omb Battlefield) ---- */     {190, "CHUCKYA", g_profile_HOLHEI, daHolhei_c_classInit, hal_fill_chuckya_vtable}, \
    /* KOOPA (203) restored: his model fileptrs (data_ov062_0211cee0 -> ov002)
       now rebase through ovdata.py's per-target cross pass, so SF4 boots. */ \
    {203, "KOOPA", g_profile_NOKONOKO, daNknk_c_classInit_NOKONOKO, hal_fill_koopa_vtable}, \
    /* ---- gate KLEPTO: ov062's cap-stealing bird (Shifting Sand Land et al.) ---- */ \
    {239, "KLEPTO", g_profile_JANGO, daJango_c_classInit, hal_fill_klepto_vtable}, \
    /* ---- gate 188: ov016's six MOVER classes (Jolly Roger Bay) ---- */ \
    {242, "UNAGI", g_profile_MORAY, daMoray_c_classInit, hal_fill_unagi_vtable}, \
    {57, "SHIP_UP", g_profile_KI_FUNE_UP, daObjKi_Fune_c_classInit_KI_FUNE_UP, hal_fill_ship_up_vtable}, \
    {56, "SHIP_DOWN", g_profile_KI_FUNE, daObjKi_Fune_c_classInit_KI_FUNE, hal_fill_ship_up_vtable}, \
    {58, "ROCK_PILLAR", g_profile_KI_HASIRA, daObjKi_Hasira_c_classInit, \
     hal_fill_rock_pillar_vtable}, \
    {60, "FLOAT_ON_WATER_PLATFORM_JRB", g_profile_KI_ITA, \
     port_factory_float_on_water_jrb, hal_fill_float_on_water_jrb_vtable}, \
    {313, "SLIDING_BOX", g_profile_SLIDE_BOX, daSlide_Box_c_classInit, \
     hal_fill_sliding_box_vtable}, \
    /* ---- run linkw wave 4: ov045's platform cast, five of six (level 37);
       wave 5 (lane w5-c) released the sixth, 141, on the hosted sqrt unit --
       see hal/actor_classes_ov045.cpp ---- */ \
    {140, "FIRE_SEA_ELEVATOR", g_profile_KM2_AGARU, \
     daObjKm2_Agaru_c_classInit, hal_fill_fire_sea_elevator_vtable}, \
    {142, "POLE_LIFT", g_profile_KM2_AMI_BOU, daObjKm2_Ami_Bou_c_classInit, \
     hal_fill_pole_lift_vtable}, \
    {148, "EXTENDING_PLATFORM", g_profile_KM2_NOBIRU, \
     daObjKm2_Nobiru_c_classInit, hal_fill_extending_platform_vtable}, \
    {145, "FLOATING_FLOOR_BFS", g_profile_KM2_UKISHIMA, \
     port_factory_floating_floor_bfs, hal_fill_floating_floor_bfs_vtable}, \
    {139, "FALL_BLOCK_BFS", g_profile_KM2_KUZURE, \
     port_factory_fall_block_bfs, hal_fill_fall_block_bfs_vtable}, \
    {141, "TILTING_PLATFORM_BFS", g_profile_KM2_GURA, \
     port_factory_tilting_platform_bfs, hal_fill_tilting_platform_bfs_vtable}, \
    /* ---- run linkw wave 5: the Bowser fight arena's floor (level 38);
       the other six ov060 classes are measured but not hosted, see
       hal/actor_classes_ov060.cpp ---- */ \
    {166, "BOWSER_FIRE_SEA_ARENA", g_profile_KOOPA2BG, \
     daKpa2Bg_c_classInit, hal_fill_bowser_fire_sea_arena_vtable}, \
    /* ---- run linkw wave 6 (lane w6-A): the rest of the Bowser fight.
       284 is the level's eight placed mines; 279 is the boss and 278/280 are
       the two children his init spawns with unchecked writes through the
       results; 167 is level 40's floor (not placed in 38); 282/283 are
       ov089's boss reward, which the arena already has resident through the
       selector row, and 282 is spawned-and-written-through on the defeat
       path. LAST_STAR rides KEY's fill -- ov089 defines one vtable and both
       factories store it (the BLUE_FLAME/RED_FLAME shape). ---- */ \
    {284, "SPIKE_BOMB", g_profile_KIRAI, daKirai_c_classInit, \
     hal_fill_spike_bomb_vtable}, \
    {279, "BOWSER", g_profile_KOOPA, daKpa_c_classInit, hal_fill_bowser_vtable}, \
    {278, "BOWSER_TAIL", g_profile_KOOPATAIL, daKpaTail_c_classInit, \
     hal_fill_bowser_tail_vtable}, \
    {280, "BOWSER_FIRE", g_profile_KOOPAFIRE, daKpaFire_c_classInit, \
     hal_fill_bowser_fire_vtable}, \
    {167, "BOWSER_SKY_PLATFORM", g_profile_KOOPA3BG, \
     daKpa3Bg_c_classInit, hal_fill_bowser_sky_platform_vtable}, \
    {282, "KEY", g_profile_OBJ_KEY, daObjKey_c_classInit_OBJ_KEY, hal_fill_key_vtable}, \
    {283, "LAST_STAR", g_profile_LAST_STAR, daObjKey_c_classInit_LAST_STAR, 0}, \
    /* ---- run linkw wave 5 (lane w5-c): the rest of level 37's roaming cast.
       FLAME_CHOMP_FIRE is not level-placed; the chomp spawns it live. ---- */ \
    /* FLY_GUY was the fourth ov070 class wave 5 left unregistered; level 19
       places it and its baseline census skipped 0xe8. Its own slots run past
       the usual seven -- 18, 19 and 29 are its own (at 18 only Amp takes the
       shared default; FlameChomp 0x021211bc and FlameChompFire 0x02121bdc
       carry their own, as this branch's T5 row cites for precedent) -- and
       slot 9 is a host copy for the ModelAnim
       slot-5 collision, the same one Amp and FlameChomp already take. See
       hal/actor_classes_ov070.cpp. */ \
    {232, "FLY_GUY", g_profile_PROPELLER_HEYHO, daPropeller_Heyho_c_classInit, \
     hal_fill_fly_guy_vtable}, \
    {266, "AMP", g_profile_BIRIKYU, daBrq_c_classInit, hal_fill_amp_vtable}, \
    {270, "FLAME_CHOMP", g_profile_KERONPA, daKrpa_c_classInit, \
     hal_fill_flame_chomp_vtable}, \
    {271, "FLAME_CHOMP_FIRE", g_profile_KERONPA_FIRE, daKpFr_c_classInit, \
     hal_fill_flame_chomp_fire_vtable}, \
    {196, "POPPING_LAVA_BUBBLES", g_profile_LAVA, \
     daObjLava_c_classInit, hal_fill_popping_lava_bubbles_vtable}, \
    /* ---- run linkw wave 5 (lane w5-B): Hazy Maze Cave's cast (level 13);
       DORRIE_CAP is never in a level's object list -- Dorrie spawns id 169
       at runtime, and the registry row is what that spawn finds ---- */ \
    {236, "SNUFIT", g_profile_YUREI_MUCHO, daYurei_Mucho_c_classInit, hal_fill_snufit_vtable}, \
    {237, "SWOOP", g_profile_BASABASA, daBasabasa_c_classInit, hal_fill_swoop_vtable}, \
    {168, "DORRIE", g_profile_DOSSY, daDossy_c_classInit, hal_fill_dorrie_vtable}, \
    {169, "DORRIE_CAP", g_profile_DOSSY_CAP, daDossyCap_c_classInit, \
     hal_fill_dorrie_cap_vtable}, \
    {68, "WORK_ELEVATOR", g_profile_CV_NEWS_LIFT, daObjCvNewsLift_c_classInit, \
     hal_fill_work_elevator_vtable}, \
    {221, "ROLLING_ROCK", g_profile_GORO_ROCK, daGrock_c_classInit, \
     hal_fill_rolling_rock_vtable}, \
    {67, "SHUTTER_HMC", g_profile_CV_SHUTTER, port_factory_shutter_hmc, \
     hal_fill_shutter_hmc_vtable}, \
    /* ---- run rel0215 wave 3 (lane w3-a2): ov065's Tick Tock Clock cluster, \
       level 27's own cast. ELEVEN ids, SEVEN tables; the pairs share one \
       array and one idempotent fill, the ov035 ROTATING_COG shape. All eleven \
       are registered even though only seven appear in level 27's object table \
       -- the ov019 rule is that a class's static initialiser runs only if its \
       class is registered, and the seven Ttc sinits are owned across all \
       eleven ids. TTC_MOVING_BEAM (118) is registered and its Behavior is a \
       LOUD FACE: the ROM's body at 0x0211bd8c is in no src/ file and no \
       delink block, so the class quarantines on its first behaviour frame. \
       See battery.py's LEVEL_SKIPS row for 27, which is the GOOMBOSS shape \
       and retires itself the day that body is matched. ---- */ \
    {108, "TTC_ROTATING_CUBE", g_profile_CT_MECHA01, \
     daObjCtRotateBlock_c_classInit_CT_MECHA01, hal_fill_ct_rotate_block_vtable}, \
    {109, "TTC_ROTATING_PRISM", g_profile_CT_MECHA02, \
     daObjCtRotateBlock_c_classInit_CT_MECHA02, hal_fill_ct_rotate_block_vtable}, \
    {110, "TTC_PENDULUM", g_profile_CT_MECHA03, \
     daObjCtMecha03_c_classInit, hal_fill_ct_mecha03_vtable}, \
    {111, "TTC_CONVEYOR_BELT_LARGE", g_profile_CT_MECHA04L, \
     daObjCtMecha04_c_classInit_CT_MECHA04L, hal_fill_ct_mecha04_vtable}, \
    {112, "TTC_CONVEYOR_BELT_SMALL", g_profile_CT_MECHA04S, \
     daObjCtMecha04_c_classInit_CT_MECHA04S, hal_fill_ct_mecha04_vtable}, \
    {113, "TTC_MOVING_BAR", g_profile_CT_MECHA05, \
     daObjCtMecha05_c_classInit, hal_fill_ct_mecha05_vtable}, \
    {114, "TTC_ROTATING_GEAR", g_profile_CT_MECHA06, \
     daObjCtKaitendai_c_classInit_CT_MECHA06, hal_fill_ct_kaitendai_vtable}, \
    {115, "TTC_ROTATING_TRIANGLE", g_profile_CT_MECHA07, \
     daObjCtKaitendai_c_classInit_CT_MECHA07, hal_fill_ct_kaitendai_vtable}, \
    {116, "TTC_MOVING_CUBE_A", g_profile_CT_MECHA08A, \
     daObjCtMecha08_c_classInit_CT_MECHA08A, hal_fill_ct_mecha08_vtable}, \
    {117, "TTC_MOVING_CUBE_B", g_profile_CT_MECHA08B, \
     daObjCtMecha08_c_classInit_CT_MECHA08B, hal_fill_ct_mecha08_vtable}, \
    {118, "TTC_MOVING_BEAM", g_profile_CT_MECHA09, \
     daObjCtMecha09_c_classInit, hal_fill_ct_mecha09_vtable}, \
    /* ---- run linkw wave 5 (lane w5-A): ov063, Big Boo's Haunt's cast ---- */ \
    {209, "BOO", g_profile_TERESA, daTrs_c_classInit_TERESA, hal_fill_boo_vtable}, \
    {210, "BIG_BOO", g_profile_BOSS_TERESA, daTrs_c_classInit_BOSS_TERESA, hal_fill_boo_vtable}, \
    {211, "BIG_BOO_ICON", g_profile_ICON_TERESA, daTrsIcon_c_classInit, \
     hal_fill_big_boo_icon_vtable}, \
    {212, "BOO_CAGE", g_profile_T_BASKET, daTBasket_c_classInit, \
     hal_fill_boo_cage_vtable}, \
    {65, "MANSION_STEPS", g_profile_KAIDAN, daTrsTrap_c_classInit_KAIDAN, \
     hal_fill_mansion_steps_vtable}, \
    {158, "BOOKSHELF", g_profile_BOOKSHELF, daTrsTrap_c_classInit_BOOKSHELF, \
     hal_fill_mansion_steps_vtable}, \
    {159, "MERRY_GO_ROUND", g_profile_MERRYGOROUND, daTrsTrap_c_classInit_MERRYGOROUND, \
     hal_fill_mansion_steps_vtable}, \
    {160, "TRAP_DOOR", g_profile_TERESAPIT, daTrsTrap_c_classInit_TERESAPIT, \
     hal_fill_mansion_steps_vtable}, \
    {66, "FALL_BLOCK_BBH", g_profile_TH_DOWN_B, \
     port_factory_fall_block_bbh, hal_fill_fall_block_bbh_vtable}, \
    {249, "MAD_PIANO", g_profile_PIANO, daPiano_c_classInit, \
     hal_fill_mad_piano_vtable}, \
    /* ---- w5-A stragglers: the book cast (ov020) + the ov095 lift ---- */ \
    {327, "BOOKEND", g_profile_KILLER_BOOK, daBook_c_classInit_KILLER_BOOK, \
     hal_fill_book_shot_vtable}, \
    {325, "BOOK_SHOT", g_profile_SHOOT_BOOK, daBook_c_classInit_SHOOT_BOOK, \
     hal_fill_book_shot_vtable}, \
    {213, "BOOK_SWITCH", g_profile_BOOK_SWITCH, daBook_c_classInit_BOOK_SWITCH, \
     hal_fill_book_shot_vtable}, \
    {328, "BOOK_SHOT_SPAWNER", g_profile_BOOK_GENERATOR, \
     daBookGen_c_classInit, hal_fill_book_shot_spawner_vtable}, \
    {32, "UP_DOWN_LIFT_BBH", g_profile_UDLIFT_TERESA, daUdlift_c_classInit_UDLIFT_TERESA, \
     hal_fill_up_down_lift_hmc_vtable}, \
    {193, "CRAZED_CRATE", g_profile_BATTA_BLOCK, daBttBk_c_classInit, \
     hal_fill_crazed_crate_vtable}, \
    {64, "COFFIN", g_profile_CASKET, daObjCasket_c_classInit, hal_fill_coffin_vtable}, \
    /* ---- run linkw wave 6 (lane w6-f): the ov095 flamethrower ---- */ \
    {318, "FLAMETHROWER", g_profile_OBJ_FLAMETHROWER, daObjFlamethrower_c_classInit, \
     hal_fill_flamethrower_vtable}, \
    /* ---- run linkw wave 6 (lane w6-D): PATH_LIFT, the first three-level
       hierarchy this port hosts that is SPLIT ACROSS OVERLAYS
       (daObjPathLift_c in ov100 over PathLiftActor_c in ov002 over
       Platform). Both 33-slot tables, the inherited slot-32 AfterClsn and
       the shared WAIT/PATH/FALL machine are in
       hal/actor_classes_ov100pl.cpp. daObjPathLift_c_classInit's FINAL vptr write is
       its own table by a real name, so no factory wrapper. Placed on
       levels 13 (x1) and 15 (x2). ---- */ \
    {31, "PATH_LIFT", g_profile_PATH_LIFT, daObjPathLift_c_classInit, \
     hal_fill_path_lift_vtable}, \
    /* ---- run linkw wave 7 (lane w7-c): the Dark World arena's own class.
       Spawn stores its table by a real name, so no factory wrapper. ---- */ \
    {301, "ORANGE_BALL_BILLBOARD", g_profile_KB1_BILLBOARD, \
     daObjKb1Billboard_c_classInit, hal_fill_orange_ball_billboard_vtable}, \
    /* ---- run linkw wave 8 (lane w8-packs): ov022, Lethal Lava Land's own
       level overlay. Five ids, four tables; 75 rides 76's fill. Level 14's
       baseline census had 70/71/72/73/74/75/76/77/80/82/83/262 skipped, and
       these five are the ov022 classes whose slot bodies are all real
       decompilations -- 73/77/80/83 are held out because sixteen of their
       slot bodies are `recovered from vtable slot identity` guesses and
       port/tools/inferred_stub_guard.py freezes the seated-guess set at a
       ratchet that may only shrink. ---- */ \
    {71, "VOLCANO_RING", g_profile_FL_RING, daObjFl_Ring_c_classInit, \
     hal_fill_volcano_ring_vtable}, \
    {74, "FLOAT_ON_LAVA_PLATFORM", g_profile_FL_BLOCK, \
     daObjFl_Block_c_classInit, hal_fill_float_on_lava_platform_vtable}, \
    {76, "FLOATING_FLOOR_LLL_SMALL", g_profile_FL_UKIYUKA_L, \
     port_factory_floating_floor_lll_small, hal_fill_floating_floor_lll_vtable}, \
    {75, "FLOATING_FLOOR_LLL_BIG", g_profile_FL_UKIYUKA, \
     port_factory_floating_floor_lll_big, hal_fill_floating_floor_lll_vtable}, \
    {82, "LAVA_PLANK", g_profile_FL_UKI_KI, daObjFl_UkiKi_c_classInit, \
     hal_fill_lava_plank_vtable}, \
    /* ---- run linkw wave 9 (lane w9-reg): TILTING_PLATFORM_LLL, the largest \
       single class in the wave-8 skip census -- FOURTEEN instances, x10 on \
       level 44 (ex_m_map) and x4 on level 48 (ex_w_map). ov064 is already \
       mounted (gates 177/178), so this is registration with no new mount. \
       The dsd block shift spells its bodies _ZN12MetalNetLift* and its table \
       _ZTV14daObjFl_Gura_c; the class is id 72, not the gate-178 id 69, and \
       hal/actor_classes_ov064_gate178.cpp's header says so itself. Its Spawn \
       stores base-then-own and the own store is the zeroed VT1 placeholder, so \
       it needs a FACTORY WRAPPER; the disassembly that settles the store order \
       is in hal/actor_classes_ov064_w9.cpp. ---- */ \
    {72, "TILTING_PLATFORM_LLL", g_profile_FL_GURA, \
     port_factory_tilting_platform_lll, hal_fill_tilting_platform_lll_vtable}, \
    /* ---- run linkw wave 9 (lane w9-reg): the three ov084 shared-table \
       siblings. GOOMBA_LARGE x9 on level 24; FIRE_PIRANHA_PLANT_BIG x5 on \
       level 24; FIRE_PIRANHA_PLANT_SMALL x2 on level 39. Each reuses the fill \
       its already-registered sibling installs on the SAME ROM table, so the \
       fill column is deliberately the sibling's. ---- */ \
    {202, "GOOMBA_LARGE", g_profile_KURIBO_L, daKrb_c_classInit_KURIBO_L, \
     hal_fill_goomba_vtable}, \
    {251, "FIRE_PIRANHA_PLANT_BIG", g_profile_FIREPAKUN, \
     daFPkn_c_classInit_FIREPAKUN, hal_fill_fire_piranha_plant_vtable}, \
    {252, "FIRE_PIRANHA_PLANT_SMALL", g_profile_FIREPAKUN_S, \
     daFPkn_c_classInit_FIREPAKUN_S, hal_fill_fire_piranha_plant_vtable}, \
    /* ---- run linkw wave 9 (lane w9-reg): the three ov091 sliding-platform \
       siblings. x1 on level 35, x1 and x2 on level 39. Each reuses the fill \
       the registered id 55 installs on the SAME ROM table. ---- */ \
    {138, "SLIDING_PLATFORM_BDW", g_profile_KM1_DERU, \
     daObjSimpleLift_c_classInit_KM1_DERU, hal_fill_sliding_platform_wf_vtable}, \
    {154, "SLIDING_PLATFORM_BS_WIDE", g_profile_KM3_DERU01, \
     daObjSimpleLift_c_classInit_KM3_DERU01, hal_fill_sliding_platform_wf_vtable}, \
    {155, "SLIDING_PLATFORM_BS_LONG", g_profile_KM3_DERU02, \
     daObjSimpleLift_c_classInit_KM3_DERU02, hal_fill_sliding_platform_wf_vtable}, \
    /* ---- run linkw wave 9 (lane w9-reg): the three ov095 seesaw siblings. \
       x2 on level 35, x1 and x1 on level 39. Each reuses the fill the \
       registered id 39 installs on the SAME ROM table. ---- */ \
    {133, "SEESAW_BDW", g_profile_KM1_SEESAW, daObjSeesaw_c_classInit_KM1_SEESAW, \
     hal_fill_seesaw_bob_vtable}, \
    {149, "SEESAW_BS_PLANK", g_profile_KM3_SEESAW, daObjSeesaw_c_classInit_KM3_SEESAW, \
     hal_fill_seesaw_bob_vtable}, \
    {150, "SEESAW_BS_ZIGZAG", g_profile_KM3_YOKOSEESAW, daObjSeesaw_c_classInit_KM3_YOKOSEESAW, \
     hal_fill_seesaw_bob_vtable}, \
    /* ---- run linkw wave 10 (lane w10-shared): ov031's SLIDE DECORATION \
       QUAD. The lane's tool resolves all 512 spawn-table entries to the table \
       each factory stores; for ov031 it returns ONE table for FOUR ids, and \
       the class picks its model out of its own actor id (InitResources \
       switches on the +0xc halfword over 0x12e/0x12f/0x130/0x131 and indexes \
       data_ov031_02111424). So all four rows name the SAME fill deliberately. \
       x3 each on level 23, whose entire baseline skip set was these twelve \
       instances of these four classes. ---- */ \
    {302, "SLIDE_DECORATION_SILVER_STAR", g_profile_HS_MOON, \
     daObjHsBillboard_c_classInit_HS_MOON, hal_fill_slide_decoration_vtable},      \
    {303, "SLIDE_DECORATION_YELLOW_STAR", g_profile_HS_STAR, \
     daObjHsBillboard_c_classInit_HS_STAR, hal_fill_slide_decoration_vtable},      \
    {304, "SLIDE_DECORATION_ORANGE_SMILEY",                                   \
     g_profile_HS_Y_STAR,                                   \
     daObjHsBillboard_c_classInit_HS_Y_STAR, hal_fill_slide_decoration_vtable},    \
    {305, "SLIDE_DECORATION_BLUE_SMILEY", g_profile_HS_B_STAR, \
     daObjHsBillboard_c_classInit_HS_B_STAR, hal_fill_slide_decoration_vtable}, \
    /* ---- run linkw wave 10 (lane w10-shared): MEGA_MUSHROOM (277, \
       ov002) x1 on level 22. The lane's tool puts ids 276 and 277 on ONE \
       table, 0x021083c8, and 276 is already registered -- so the array is \
       already filled before this class spawns and the whole registration is \
       one record and one factory TU. \
 \
       THE FILL COLUMN IS 0, THE LIGHT_BEAM CONVENTION, AND IT IS NOT \
       COSMETIC. hal_fill_one_up_mushroom_vtable opens with \
       port_one_up_mushroom_types_seat(), a VERIFY-THEN-REWRITE seat over \
       __sinit_ov002_02100adc's fourteen mushroom-type PMFs: it aborts unless \
       each word still holds the ROM address it was compiled from, then \
       replaces it with the host body. Naming that fill on a second row calls \
       it a second time, and the second call finds the host address the first \
       one wrote -- `FATAL: mushroom type 0: the sinit left 004807a0, the \
       ROM's own table says 020aff10 -- WRONG BYTES`, measured on this tree \
       before the column was changed. A sibling's fill may only be reused \
       from another row when it is IDEMPOTENT; one carrying a seat is not. \
       ---- */ \
    {277, "MEGA_MUSHROOM", g_profile_SCALEUP_KINOKO, da1up_c_classInit_SCALEUP_KINOKO, 0}, \
    /* ---- run linkw wave 11 (lane w11): the widened sweep's harvest. Nineteen \
       ids whose table is ALREADY filled by a registered sibling and whose overlay \
       is ALREADY mounted, so each row costs one factory TU and nothing else. \
       Eleven placed instances across levels 8 and 37 come off the skip lists with \
       them; the thirteen ids that no mounted level places today are taken for the \
       linkage and so the lane that mounts their level gets them for free. \
 \
       EVERY FILL NAMED BELOW IS A SIBLING'S, AND EVERY ONE WAS CHECKED FOR A \
       DONE-GUARD BEFORE IT WAS NAMED. Four are idempotent by measurement -- the \
       registry already calls them 2 or 4 times on every one of the 35 mounted \
       levels. The other six were read to the bottom of their call graph: they are \
       stores plus a shared helper that is also stores, and the seats they reach \
       (hal_fill_platform_vtable, port_enemy_death_states_seat, \
       port_goomba_states_seat, port_power_star_states_seat) all open with \
       `static int done; if (done) return;`. port_power_star_states_seat is the \
       sharp one: it carries the same verify-then-abort that made wave 10 give \
       MEGA_MUSHROOM a 0 column, and it is safe ONLY because of that guard. ---- */ \
    {59, "ROCK_TRIANGLE", g_profile_KI_HASIRA_DAI, daObjSimpleBg_c_classInit_KI_HASIRA_DAI, \
     hal_fill_warp_pipe_vtable}, \
    {340, "CG_STAIRS", g_profile_MC_DODAI, daObjSimpleBg_c_classInit_MC_DODAI, \
     hal_fill_warp_pipe_vtable}, \
    {341, "ROPE_BARRIER", g_profile_MC_HAZAD, daObjSimpleBg_c_classInit_MC_HAZAD, \
     hal_fill_warp_pipe_vtable}, \
    {22, "EXCLAMATION_BLOCK_VS", g_profile_VS_ITEM_BLOCK, \
     daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK, hal_fill_question_block_vtable}, \
    {23, "CAP_BLOCK_MARIO", g_profile_CAP_BLOCK_M, daObjHatenaBlock_c_classInit_CAP_BLOCK_M, \
     hal_fill_question_block_vtable}, \
    {24, "CAP_BLOCK_WARIO", g_profile_CAP_BLOCK_W, daObjHatenaBlock_c_classInit_CAP_BLOCK_W, \
     hal_fill_question_block_vtable}, \
    {25, "CAP_BLOCK_LUIGI", g_profile_CAP_BLOCK_L, daObjHatenaBlock_c_classInit_CAP_BLOCK_L, \
     hal_fill_question_block_vtable}, \
    {147, "SLIDING_PLATFORM_BFS_SQUARE", g_profile_KM2_RIFUT01, \
     daObjSimpleLift_c_classInit_KM2_RIFUT01, hal_fill_sliding_platform_wf_vtable}, \
    {146, "SLIDING_PLATFORM_BFS_RECTANGLE", g_profile_KM2_RIFUT02, \
     daObjSimpleLift_c_classInit_KM2_RIFUT02, hal_fill_sliding_platform_wf_vtable}, \
    {124, "SLIDING_PLATFORM_RR", g_profile_RC_RIFT01, \
     daObjSimpleLift_c_classInit_RC_RIFT01, hal_fill_sliding_platform_wf_vtable}, \
    {30, "ROTATING_UP_DOWN_PLATFORM_UTM", g_profile_HS_UPDOWN_LIFT, \
     daObjRotateUpdownLift_c_classInit_HS_UPDOWN_LIFT, hal_fill_rotating_up_down_platform_vtable}, \
    {201, "GOOMBA_SMALL", g_profile_KURIBO_S, daKrb_c_classInit_KURIBO_S, \
     hal_fill_goomba_vtable}, \
    {204, "KOOPA_SMALL", g_profile_NOKONOKO_S, daNknk_c_classInit_NOKONOKO_S, \
     hal_fill_koopa_vtable}, \
    {143, "SEESAW_BFS", g_profile_KM2_YOKOSEESAW, daObjSeesaw_c_classInit_KM2_YOKOSEESAW, \
     hal_fill_seesaw_bob_vtable}, \
    {28, "SEESAW_UTM", g_profile_SEESAW, daObjSeesaw_c_classInit_SEESAW, \
     hal_fill_seesaw_bob_vtable}, \
    {128, "SEESAW_RR", g_profile_RC_SEESAW, daObjSeesaw_c_classInit_RC_SEESAW, \
     hal_fill_seesaw_bob_vtable}, \
    {131, "UP_DOWN_LIFT_RR", g_profile_RC_RIFT02, daUdlift_c_classInit_RC_RIFT02, \
     hal_fill_up_down_lift_hmc_vtable}, \
    {179, "SILVER_STAR", g_profile_SILVER_STAR, daStar_c_classInit_SILVER_STAR, \
     hal_fill_power_star_vtable}, \
    {324, "SILVER_STAR_BLOCK_TAG", g_profile_BLK_SLVSTAR_TAG, \
     daObjBlockItemTag_c_classInit_BLK_SLVSTAR_TAG, hal_fill_mega_mushroom_block_tag_vtable}, \
    /* ---- run linkw wave 12 (lane w12): ov073, level 49's whole cast ---- */ \
    {170, "CCC_ARENA", g_profile_EWB_ICE_A, daObjEwbIce_c_classInit_EWB_ICE_A, \
     hal_fill_ccc_arena_vtable}, \
    {171, "CCC_BIG_ICE", g_profile_EWB_ICE_B, daObjEwbIce_c_classInit_EWB_ICE_B, 0}, \
    {172, "CCC_SMALL_ICE", g_profile_EWB_ICE_C, daObjEwbIce_c_classInit_EWB_ICE_C, 0}, \
    {218, "CHIEF_CHILLY", g_profile_KING_DONKETU, port_factory_chief_chilly, \
     hal_fill_chief_chilly_vtable}, \
    /* ---- run linkw wave 12 (lane w12): ov052, level 44's platform pair ---- */ \
    {174, "BOBBING_LOG_EMM", g_profile_EMM_LOG, daObjEmmLog_c_classInit, \
     hal_fill_bobbing_log_emm_vtable}, \
    {175, "SQUARE_PATH_LIFT", g_profile_EMM_YUKA, daObjEmmYuka_c_classInit, \
     hal_fill_square_path_lift_vtable}, \
    /* ---- run linkw wave 12 (lane w12): CLAM (315), ov064 ---- */ \
    {315, "CLAM", g_profile_OBJ_SHELL, daObjShell_c_classInit, hal_fill_clam_vtable}, \
    /* ---- run linkw wave 15 (lane w15): ov056, level 48's ice block ---- */ \
    {173, "BIG_MOVING_ICE_BLOCK", g_profile_EWM_ICE_BLOCK, \
     daObjEwmIceBlock_c_classInit, hal_fill_big_moving_ice_block_vtable}, \
    /* ---- run linkw wave 15 (lane w15): ov002's mega mushroom tag pair ---- */ \
    {319, "MEGA_MUSHROOM_CREATE_TAG", g_profile_KINOKO_CREATE_TAG, \
     daObjKinokoTag_c_classInit_KINOKO_CREATE_TAG, hal_fill_mega_mushroom_tag_vtable}, \
    {320, "MEGA_MUSHROOM_TAG", g_profile_KINOKO_TAG, \
     daObjKinokoTag_c_classInit_KINOKO_TAG, hal_fill_mega_mushroom_tag_vtable}, \
    /* ---- run linkw wave 15 (lane w15): ov025, level 17's pyramid pair ---- */ \
    {87, "PYRAMID_STEP", g_profile_DP_BROCK, daObjDpBrock_c_classInit, \
     hal_fill_pyramid_step_vtable}, \
    {88, "PYRAMID_LIFT", g_profile_DP_LIFT, daDpLift_c_classInit, \
     hal_fill_pyramid_lift_vtable}, \
    /* ---- run linkw wave 16 (lane w16-unblock): ov022's fall block, the id \
       the inferred-stub guard held back; x9 on level 14 ---- */ \
    {83, "FALL_BLOCK_LLL", g_profile_FL_KUZURE, port_factory_fall_block_lll, \
     hal_fill_fall_block_lll_vtable}, \
    /* ---- run linkw wave 17 (lane w17): ov002's SEAWEED, x8 on level 18 and \
       x4 on level 24 ---- */ \
    {296, "SEAWEED", g_profile_WAKAME, daObjWakame_c_classInit, \
     hal_fill_seaweed_vtable}, \
    /* ---- run link60 (lane A1): ov085's WALL_SIGN and TOAD, nineteen placed \
       instances across levels 2, 4, 5 and 50 ---- */ \
    {183, "WALL_SIGN", g_profile_KANBAN, daObjKanban_c_classInit, \
     hal_fill_wall_sign_vtable}, \
    {185, "TOAD", g_profile_KINOPIO, daKinopio_c_classInit, hal_fill_toad_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-ov077): the ov077 shared enemy pack. \
       Placed instances measured from this tree's own baseline census, not \
       quoted: HEAVE_HO x3 on level 21, x1 on 27, x3 on 42; LAKITU x1 on 24, \
       x2 on 28, x2 on 43, x1 on 44. SPINY is placed on NO level of the \
       fifty-two -- it is registered because it is the overlay's third class \
       and its seat is as derived as the other two, but nothing in a level \
       walk exercises it (see port/slice_ov077.txt). ---- */ \
    {238, "HEAVE_HO", g_profile_POPOI, daPopoi_c_classInit, \
     hal_fill_heave_ho_vtable}, \
    {260, "SPINY", g_profile_TOGEZO, daTgz_c_classInit, hal_fill_spiny_vtable}, \
    {265, "LAKITU", g_profile_JUGEM, daJgm_c_classInit, hal_fill_lakitu_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-ov030): ov030's Tall Tall Mountain \
       cast. Four ids, three classes: 267 and 268 are one class under two \
       spawn entry points and share hal_fill_ukiki_vtable. On level 22 only \
       102 and 267 are placed (x1 each); 103 and 268 have no placed instance \
       there and are registered for the spawn path rather than for a census \
       count ---- */ \
    {102, "ROLLING_LOG_TTM", g_profile_HM_MARUTA, daObjHmMaruta_c_classInit, \
     hal_fill_rollinglogttm_vtable}, \
    {103, "UKIKI_CAGE", g_profile_HM_BASKET, daObjHmBskt_c_classInit, \
     hal_fill_ukikicage_vtable}, \
    {267, "UKIKI_THIEF", g_profile_MONKEY_THIEF, daMky_c_classInit_MONKEY_THIEF, \
     hal_fill_ukiki_vtable}, \
    {268, "UKIKI_STAR", g_profile_MONKEY_STAR, daMky_c_classInit_MONKEY_STAR, \
     hal_fill_ukiki_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-ov026): ov026's whole cast, level 18 \
       (Dire Dire Docks). Ten placed instances on 18 across the first four; \
       WATER_SUCTION is not placed on 18 and proves by no-regression. ---- */ \
    {89, "POLE_LIFT_DDD", g_profile_WL_POLELIFT, daObjWlPolelift_c_classInit, \
     hal_fill_ov026_polelift_vtable}, \
    {90, "SUBMARINE", g_profile_WL_SUBMARINE, daObjWlSubmarine_c_classInit, \
     hal_fill_ov026_submarine_vtable}, \
    {91, "BOWSER_SHUTTER", g_profile_WL_KOOPA_SHUTTER, daObjWlKoopaShutter_c_classInit, \
     hal_fill_ov026_bowser_shutter_vtable}, \
    {246, "WHIRLPOOL", g_profile_WATER_TATUMAKI, daWater_Tatumaki_c_classInit, \
     hal_fill_ov026_whirlpool_vtable}, \
    {247, "WATER_SUCTION", g_profile_WATER_SUIKOMI, daWater_Suikomi_c_classInit, \
     hal_fill_ov026_water_suction_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov035, level 27's own cast. \
       Three ids take the SAME fill on purpose -- one table, one idempotent \
       fill, the ov013 CLOCK_HAND_SHORT/LONG shape. ---- */ \
    {119, "ROTATING_CLOCK_HAND", g_profile_CT_MECHA10, \
     daObjCtMecha10_c_classInit_CT_MECHA10, hal_fill_rotating_cog_vtable}, \
    {120, "SPINNING_PLATFORM", g_profile_CT_MECHA11, \
     daObjCtMecha11_c_classInit, hal_fill_spinning_platform_vtable}, \
    {121, "ROTATING_COG_BIG", g_profile_CT_MECHA12L, \
     daObjCtMecha10_c_classInit_CT_MECHA12L, hal_fill_rotating_cog_vtable}, \
    {122, "ROTATING_COG_SMALL", g_profile_CT_MECHA12S, \
     daObjCtMecha10_c_classInit_CT_MECHA12S, hal_fill_rotating_cog_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov024, level 16's pyramid \
       pair. Two tables, two different widths -- 32 and 31. ---- */ \
    {85, "PYRAMID_TOP", g_profile_DL_PYRAMID, daObjDlPyramid_c_classInit, \
     hal_fill_pyramid_top_vtable}, \
    {86, "PYRAMID_TAG", g_profile_DL_PYRAMID_DUMMY, daObjDlPyramidDummy_c_classInit, \
     hal_fill_pyramid_tag_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov033, Tiny-Huge Island's \
       small side. The dsd vtable label is on the water, not the cover. ---- */ \
    {104, "TINY_COVER", g_profile_TT_FUTA, daObjTtFuta_c_classInit, \
     hal_fill_tiny_cover_vtable}, \
    {105, "TINY_WATER", g_profile_TT_WATER, daObjTtWater_c_classInit, \
     hal_fill_tiny_water_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov023, the volcano \
       squasher. Level 15's entire skip list, one row. ---- */ \
    {84, "SQUASHER", g_profile_FM_BATTAN, daObjFm_Battan_c_classInit, \
     hal_fill_squasher_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov017, the sunken ship's \
       water. The blocker level_boot.cpp's level-9 block deferred. ---- */ \
    {62, "SHIP_WATER", g_profile_KS_MIZU, daObjKsWater_c_classInit, \
     hal_fill_ship_water_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov058, the Rec Room's \
       cupboard. Level 50's entire skip list, one row. ---- */ \
    {182, "REC_ROOM_CUPBOARD", g_profile_PL_CLOSET, \
     daObjCloset_c_classInit, hal_fill_rec_room_cupboard_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-ov036): ov036's whole Rainbow \
       Cruise cast, forty-nine placed instances on level 28 (the level's \
       entire own skip list bar ov077's three). Rows in ROM block order, \
       which is also the order the seven vtables sit in .data. PAIRED BY \
       ADDRESS -- see port/ov036_syms.txt for why the fill named after one \
       class serves the id of another ---- */ \
    {127, "SWINGING_PLATFORM", g_profile_RC_BURANKO, \
     daObjRcBuranko_c_classInit, hal_fill_swinging_platform_vtable}, \
    {129, "ROTATING_PLATFORM_RR", g_profile_RC_KAITEN, \
     daObjRc_Kaitendai_c_classInit, hal_fill_rotating_platform_rr_vtable}, \
    {125, "SHIP_WING", g_profile_RC_HANE, daObjRc_Hane_c_classInit, \
     hal_fill_ship_wing_vtable}, \
    {126, "DONUT_BLOCK", g_profile_RC_TIKUWA, daObjRc_Tikuwa_c_classInit, \
     hal_fill_donut_block_vtable}, \
    {132, "ARMED_ROTATING_PLATFORM", g_profile_RC_GURUGURU, \
     daObjRc_Guruguru_c_classInit, hal_fill_armed_rotating_platform_vtable}, \
    {123, "TRICKY_TRIANGLES", g_profile_RC_DORIFU, \
     daObjRc_Dorifu_c_classInit, hal_fill_tricky_triangles_vtable}, \
    {130, "FLYING_CARPET", g_profile_RC_CARPET, daObjRcCarpet_c_classInit, \
     hal_fill_flying_carpet_vtable}, \
    /* ---- run rel0215 wave 2 (lane cast-ov090): ov090's whole four-class \
       WATER ENEMY PACK, the pack levels 18 and 21 both name. Rows in ROM \
       block order, which is also the order the four vtables sit in .data. \
       Both attribution routes agree on every id and NOTHING is shifted -- \
       see port/ov090_syms.txt for the derivation and for why dsd marks all \
       four SpawnInfo records `ambiguous` (ov006 dual-dump, not a contest). \
       Placed instances: level 18 has SHARK, CHEEP_CHEEP and SKEETER; level \
       21 has SKEETER. MANTA_RAY is placed on neither of the two proof \
       levels and is registered for the spawn path rather than for a census \
       count ---- */ \
    {230, "SKEETER", g_profile_MENBO, daMenbo_c_classInit, \
     hal_fill_skeeter_vtable}, \
    {226, "MANTA_RAY", g_profile_MANTA, daManta_c_classInit, \
     hal_fill_manta_ray_vtable}, \
    {227, "CHEEP_CHEEP", g_profile_PUKUPUKU, daPukupuku_c_classInit, \
     hal_fill_cheep_cheep_vtable}, \
    {225, "SHARK", g_profile_SHARK, daShark_c_classInit, hal_fill_shark_vtable}, \
    /* ---- run rel0215 wave 2 (lane cast-ov066): ov066's ONE class, the SSL \
       pyramid boss. Level 17's census names id 0xb0 = 176 skipped today. \
       Both attribution routes agree and the ov060 claim on the same address \
       is settled from the raw bytes -- see port/ov066_syms.txt ---- */ \
    {176, "EYEROK", g_profile_IWANTE, daIwante_c_classInit, hal_fill_eyerok_vtable}, \
    /* ---- run rel0215 wave 2 (lane cast-ov027): ov027's whole Snowman's Land \
       cast, four placed instances on level 19 (the level's entire own skip \
       list; the other seven skipped instances are ov081's 223/261 and \
       ov070/073/074's 232, named lanes elsewhere). Rows in ROM block order, \
       which is also the order the four vtables sit in .data. FIVE IDS, FOUR \
       FILLS: 92 and 93 are one class and share hal_fill_sliding_ice_vtable. \
       See port/ov027_syms.txt for both attribution routes on every id ---- */ \
    {92, "SLIDING_ICE_SPAWNER", g_profile_SL_ICEBLOCK, \
     daObjSlIceBlock_c_classInit_SL_ICEBLOCK, hal_fill_sliding_ice_vtable}, \
    {93, "SLIDING_ICE", g_profile_SL_ICEBLOCK_SHOT, daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOT, \
     hal_fill_sliding_ice_vtable}, \
    {217, "CHILL_BULLY", g_profile_ICE_DONKETU, daIDonketu_c_classInit, \
     hal_fill_chill_bully_vtable}, \
    {258, "DA_PG_DFDR", g_profile_PENGUIN_DEFENDER, daPgDfdr_c_classInit, \
     hal_fill_da_pg_dfdr_vtable}, \
    {275, "SNOWMAN_BREATH", g_profile_SNOWMAN_BREATH, daSnmBth_c_classInit, \
     hal_fill_snowman_breath_vtable}, \
    /* ---- run rel0215 wave 2 (lane cast-lvl16pair, sub-job 1 of 2): ov092's \
       one class. Placed instances measured from this tree's own baseline \
       census, not quoted: TOX_BOX x3 on level 16 and on NO other level of \
       the fifty-two -- id 309 appears in exactly one level's skip list. ---- */ \
    {309, "TOX_BOX", g_profile_ONIMASU, daOnms_c_classInit, hal_fill_tox_box_vtable}, \
    /* ---- run rel0215 wave 2 (lane cast-lvl16pair, sub-job 2 of 2): ov096, \
       the rest of level 16's cast. Placed instances measured from this tree's \
       own baseline census, not quoted: POKEY x4 and TORNADO x3 on level 16, \
       and neither appears in any other level's skip list. POKEY_SEGMENT (241) \
       is placed on NO level of the fifty-two -- POKEY spawns its own segments \
       -- and is registered because it is a real spawn entry point into the \
       same class, sharing POKEY's fill. ---- */ \
    {240, "POKEY", g_profile_SANBO, daSanbo_c_classInit_SANBO, hal_fill_pokey_vtable}, \
    {241, "POKEY_SEGMENT", g_profile_SANBO_BODY, daSanbo_c_classInit_SANBO_BODY, \
     hal_fill_pokey_vtable}, \
    {308, "TORNADO", g_profile_TORNADO, daTor_c_classInit, hal_fill_tornado_vtable}, \
    /* ---- run rel0215 wave 2 (lane w2-ov074): the ov074 GOOMBOSS pack. \
       GOOMBOSS is the whole of level 45's skip list, one placed instance; \
       EXPLOSION_GOOMBA is placed on no level of the fifty-two and is \
       spawned by the fight, so the boot walk registers it and does not \
       exercise it (see port/slice_ov074.txt). ONE FILL for both rows: the \
       two factories are the same body and store the same table. ---- */ \
    {198, "GOOMBOSS", g_profile_KURIKING, daKuriKing_c_classInit_KURIKING, \
     hal_fill_goomboss_vtable}, \
    {199, "EXPLOSION_GOOMBA", g_profile_KURIKING_VANISH, \
     daKuriKing_c_classInit_KURIKING_VANISH, hal_fill_goomboss_vtable}, \
    /* ---- run rel0215 wave 2 (lane cast-sweep2): ov032, Tiny-Huge Island's \
       BIG side. dsd's one vtable label is on the water twice; the cover's \
       table and BUBBA's are unnamed, so the rows go by address. BUBBA (228) \
       is a third class the campaign plan's inventory does not list. ---- */ \
    {106, "HUGE_COVER", g_profile_TD_FUTA, daObjTdFuta_c_classInit, \
     hal_fill_huge_cover_vtable}, \
    {107, "HUGE_WATER", g_profile_TD_WATER, daObjTdWater_c_classInit, \
     hal_fill_huge_water_vtable}, \
    {228, "BUBBA", g_profile_BAKUBAKU, daBakubaku_c_classInit, \
     hal_fill_bubba_vtable}, \
    /* ---- run rel0215 wave 2 (lane cast-sweep2): ov034, Tiny-Huge Island's \
       CAVE. Level 26's whole skip list in one row. ---- */ \
    {248, "WIGGLER", g_profile_HANACHAN, daHanachan_c_classInit, \
     hal_fill_wiggler_vtable}, \
    /* ---- run rel0215 wave 2 (lane cast-sweep2): ov047, Bowser in the Sky. \
       dsd's two vtable names are each attached ONE TABLE LATE, so the rows go \
       by address; id 156 is a fourth class the campaign plan's inventory does \
       not list. id 151 is not placed on level 39 and is registered because \
       both attribution routes name it. ---- */ \
    {151, "RICKSHAW_PLATFORM_BS", g_profile_KM3_KURUMA, \
     daObjKm3_Kuruma_c_classInit, hal_fill_rickshaw_platform_bs_vtable}, \
    {152, "RICKSHAW_BS", g_profile_KM3_KURUMAJIKU, daObjKm3_Kurumajiku_c_classInit, \
     hal_fill_rickshaw_bs_vtable}, \
    {153, "STAIRS_BS", g_profile_KM3_DORIFU, daObjKm3_Dorifu_c_classInit, \
     hal_fill_stairs_bs_vtable}, \
    {156, "ROTATING_PLATFORM_BS", g_profile_KM3_KAITENDAI, \
     daObjKm3_Kaitendai_c_classInit, hal_fill_rotating_platform_bs_vtable}, \
    /* ---- run rel0215 wave 2 (lane cast-sweep2): ov055, Luigi's key arena. \
       Level 47's whole skip list in one row. ---- */ \
    {195, "MIRROR_LUIGI", g_profile_LUIGI, daLuigi_c_classInit, \
     hal_fill_mirror_luigi_vtable}, \
    /* ---- run rel0215 wave 3 (lane w3-c): ov064's last three. JET_STREAM and \
       TREASURE_CHEST are level 8's and 18's remaining ov064 skips (and 13 is \
       level 9's ONLY skip, which also retires the ShipWater divergence \
       hal/actor_classes_ov017.cpp disclosed). WATER_RING is on no level's \
       spawn list and is registered because both attribution routes name it. \
       ---- */ \
    {245, "JET_STREAM", g_profile_WATER_HAKIDASI, daWater_Hakidasi_c_classInit, \
     hal_fill_jet_stream_vtable}, \
    {244, "WATER_RING", g_profile_WATER_RING, daWater_Ring_c_classInit, \
     hal_fill_water_ring_vtable}, \
    {13, "TREASURE_CHEST", g_profile_TREASURE_BOX, daObjTbox_c_classInit, \
     hal_fill_treasure_chest_vtable}, \
    /* ---- run rel0215 wave 3 (lane w3-f): ov085's SIXTH AND LAST class. \
       PLACED ON NO LEVEL of the fifty-two, measured on this lane's own \
       baseline census over every id in port_level_table[] -- it declines \
       0xe7, 0x90 and 0x9d and never 0xba. She is a CUTSCENE actor, so a \
       level walk does not exercise her; registered for the SPINY reason, \
       and because this row leaves ov085 with no unregistered factory \
       standing behind its own mount. \
       AND SHE HAS AN EXERCISER, so "no level dispatches this fill" is closed \
       rather than open. The seat lane reported the gap; the review found the \
       run and it reproduces: \
         SM64DS_LEVEL=50 SM64DS_SPAWN_ACTOR=186 SM64DS_FAULTS_FATAL=1 \
         SM64DS_WINDOW_SELFTEST=600 \
       gives rc 0, census 27 -> 28 (12 classes, 0 skipped) with `+ 186 x1 \
       PRINCESS_PEACH`, the watch probe still reading `186 x1 @(945,600,-1750)` \
       at frame 540, and the instance still on the teardown census at 600. No \
       fault, no quarantine, no UNHOSTED line. \
       WHICH SLOTS THAT ACTUALLY DISPATCHES, measured rather than argued. Add \
       SM64DS_ACTOR_PROBE=1 to the same run and it prints \
         [actor] PRINCESS_PEACH model 30037280 file 300D9238 \
                 transforms 300D8E48 mat.t (118,75,-219) scene \
       -- so slot 9 runs, and its non-null file and transforms say slot 0 \
       finished the load rather than bailing. Slot 6 is the hosted Behavior, \
       and it runs every frame: it tail-calls the MAIN pointer-to-member \
       dispatcher, which reads the ten-pair seat, so a wrong PMF stride or an \
       unseated DS address would fault on the first frame under FAULTS_FATAL \
       instead of surviving 600. Evidence: \
       runs/rel0215/out/w3-f/exerciser_186_level50.txt and \
       ..._actorprobe.txt. Do not re-open this as an unexercised seat. ---- */ \
    {186, "PRINCESS_PEACH", g_profile_PEACH_PRINCESS, daPeach_c_classInit, \
     hal_fill_princess_peach_vtable}, \
    /* ---- run rel0215 wave 3 (lane w3-b): ov081's last two classes, the \
       partial-module finish of the overlay gate 192 mounted. MR_BLIZZARD \
       (223) is STILL not here and this lane does not touch that refusal: \
       the register-skew blocker in the gate-192 comment above is measured \
       and its instrument has not been run. ---- */ \
    {224, "SNOWBALL", g_profile_SNOWBALL, daSnowball_c_classInit, \
     hal_fill_snowball_vtable}, \
    {261, "MONEYBAG", g_profile_GAMAGUCHI, daGmch_c_classInit, \
     hal_fill_moneybag_vtable}, \
    /* ---- run rel0215 wave 3 (lane w3-e): ov022's LAST FIVE IDS. 70/73/77/80 \
       are the whole of level 14's baseline skip list (5 instances, 4 classes); \
       243 is placed nowhere and is spawned at run time by VolcanoRing. All \
       five Spawns are registered directly -- 80's and 70's write their own \
       table through a per-source -DVT1 rather than a port_factory wrapper. --- */ \
    {80, "ROTATING_PLATFORM_LLL", g_profile_FL_KOMA_D, \
     daObjFl_Koma_D_c_classInit, hal_fill_rotating_platform_lll_vtable}, \
    {73, "LAVA_BRIDGE", g_profile_FL_LONDON, daObjFl_London_c_classInit, \
     hal_fill_lava_bridge_vtable}, \
    {77, "LAVA_SEESAW", g_profile_FL_SEESAW, daObjFl_Seesaw_c_classInit, \
     hal_fill_lava_seesaw_vtable}, \
    {70, "ROLLING_LOG_LLL", g_profile_FL_MARUTA, daObjFlMaruta_c_classInit, \
     hal_fill_rolling_log_lll_vtable}, \
    {243, "VOLCANO_FIRE", g_profile_OBJ_VOLCANO_CANNON, daObj_volcanoCannon_c_classInit, \
     hal_fill_volcano_fire_vtable}, \
    /* ---- run rel0215 wave 3 (lane w3-e): ov025's OTHER TWO IDS, the rest of \
       level 17's own skip list. 163's row goes by ADDRESS because dsd names no \
       SpawnInfo for it; 162's factory writes its own table through a \
       per-source -DVT1 and registers directly. ---- */ \
    {162, "GRINDEL", g_profile_DONKAKU, daDkk_c_classInit, \
     hal_fill_grindel_vtable}, \
    {163, "SPINDEL", g_profile_DONGURU, daDgr_c_classInit, \
     hal_fill_spindel_vtable}, \
    /* ---- run rel0215 wave 3 (lane w3-e): RACING_PENGUIN (259, ov019). Not on \
       any mounted level's object list and not spawned at run time, so this row \
       is linkage and moves no census -- see the note above the externs. ---- */ \
    {259, "RACING_PENGUIN", g_profile_PENGUIN_RACER, daPgRcer_c_classInit, \
     hal_fill_racing_penguin_vtable}, \
    /* ---- run rel0215 wave 3 (lane w3-f2): ov091's three declined ids. \
       ARROW_PATH_LIFT (157) and SQUARE_METAL_NET_LIFT (144) SHARE ONE CLASS \
       and therefore ONE FILL -- both 0x38-byte factories install ov091 \
       0x02134ec0, RTTI 13daLinelift2_c, and the variant they differ by is \
       read at run time out of each instance's own parameter word. Both \
       attribution routes agree on all three ids and nothing is shifted; the \
       derivation is hal/actor_classes_ov091.cpp's header. Placed instances: \
       level 39 has ARROW_PATH_LIFT, level 37 SQUARE_METAL_NET_LIFT and level \
       22 FWOOSH, and each was a declined line on that level before this. ---- */ \
    {157, "ARROW_PATH_LIFT", g_profile_KM3_LIFT, daLinelift2_c_classInit_KM3_LIFT, \
     hal_fill_rotating_up_down_platform_lift_vtable}, \
    {144, "SQUARE_METAL_NET_LIFT", g_profile_KM2_SUSUMU, \
     daLinelift2_c_classInit_KM2_SUSUMU, hal_fill_rotating_up_down_platform_lift_vtable}, \
    {231, "FWOOSH", g_profile_HYUHYU, daHyuhyu_c_classInit, hal_fill_fwoosh_vtable}, \
    /* ---- lane VSPOPUP: the two ids every live VS match declines ---- */ \
    /* NUMBER is the floating score popup: Actor::SpawnNumber (arm9 0x02010044, \
       live in the build since gate 10) asks for 0x14a every time a player \
       takes a star, banks red coins, or reads a count, and until now the gate \
       turned it away. WING_FEATHER is the wing-cap feather. Both are run-time \
       spawns, on no level's object table, so both rows move a boot census by \
       zero and only change what a PLAYED frame can build. */ \
    {330, "NUMBER", g_profile_OBJ_NUMBER, daObjNumber_c_classInit, hal_fill_number_vtable}, \
    {345, "WING_FEATHER", g_profile_FEATHER, daFeather_c_classInit, \
     hal_fill_wing_feather_vtable}, \
    /* ---- ov029: level 21's own eight actors (water_city, Dire Dire Docks \
       city). All eight are on the level-21 object table -- the boot census \
       skips ids 94..101 unregistered today, so every row lands a live spawn. \
       Fills in hal/actor_classes_ov029.cpp, keyed by the last _Spawn store. */ \
    {94,  "ROTATING_PLATFORM_WDW", g_profile_WC_UKISIMA, \
     daObjWc_Obj07_c_classInit, hal_fill_rotating_platform_wdw_vtable}, \
    {95,  "FLOAT_ON_WATER_PLATFORM_WDW_SQUARE", g_profile_WC_OBJ01, \
     daObjWcObj01_c_classInit, hal_fill_float_on_water_platform_wdw_square_vtable}, \
    {96,  "ARROW_LIFT", g_profile_WC_OBJ02, daObjWc_Obj02_c_classInit, \
     hal_fill_arrow_lift_vtable}, \
    {97,  "WATER_DIAMOND", g_profile_WC_OBJ03, daObjWc_Obj03_c_classInit, \
     hal_fill_water_diamond_vtable}, \
    {98,  "SWITCH_ACTIVATED_PLANK", g_profile_WC_OBJ04, \
     daObjWc_Obj04_c_classInit, hal_fill_switch_activated_plank_vtable}, \
    /* ---- link100 SPAWN2: CAGE_LIFT (99), the WDW cage the note above kept \
       out until its five bodies were ruled REAL_DECOMP. Level 21 x2. ---- */ \
    {99,  "CAGE_LIFT", g_profile_WC_OBJ05, daObjWc_Obj05_c_classInit, \
     hal_fill_cage_lift_vtable}, \
    {100, "FLOAT_ON_WATER_PLATFORM_WDW_RECTANGLE", g_profile_WC_OBJ06, \
     daObjWcObj06_c_classInit, hal_fill_float_on_water_platform_wdw_rectangle_vtable}, \
    {101, "WDW_WATER", g_profile_WC_MIZU, daObjWc_Mizu_c_classInit, \
     hal_fill_wdw_water_vtable}, \
    /* ---- gate 210 (lane SEAT-OV002-RUNTIME): four run-time spawns asked for \
       by code already in the link and turned away until now. None is on any \
       level's object table, so a boot census moves by zero; a played frame \
       (a fire piranha's spit, Bowser's stomp, an underwater chest, Yoshi's \
       egg) is what each row changes. hal/actor_classes_runtime_spawns.cpp. */ \
    {254, "FIREBALL", g_profile_FPAKUN_BALL, daFPknBall_c_classInit, \
     hal_fill_fireball_vtable}, \
    {281, "BOWSER_SHOCKWAVES", g_profile_FIRERING, daFRing_c_classInit, \
     hal_fill_bowser_shockwaves_vtable}, \
    {291, "BUBBLE", g_profile_OBJ_ABUKU, daObjAbuku_c_classInit, hal_fill_bubble_vtable}, \
    {9, "YOSHI_EGG", g_profile_YOSHI_EGG, daYegg_c_classInit, \
     hal_fill_yoshi_egg_vtable}, \
    /* ---- link100 OV ---- */ \
    {348, "CHANGE_ROOM", g_profile_CH_ROOM, daChRoom_c_classInit, \
     hal_fill_change_room_vtable}, \
    {314, "CLOUD", g_profile_OBJ_KUMO, daObjKumo_c_classInit, hal_fill_cloud_vtable}, \
    /* ---- link100 SEAT3, gate 230 ---- */ \
    {192, "PUSHABLE_BLOCK", g_profile_PUSHBLOCK, daObjPushblock_c_classInit, \
     hal_fill_pushblock192_vtable},
