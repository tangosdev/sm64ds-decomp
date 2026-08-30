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
extern unsigned char Tree_SpawnInfo[];      /* ov002 0x0210abdc */
void *Tree_Spawn(void);                     /* ov002 0x020ec32c */
void hal_fill_tree_vtable(void);
/* ---- AMBIENT_SOUND_EFFECTS (350, ov002) x5 ----
   The class whose methods and vtable the config names EnemySwitchTag; see the
   header of that section in hal/actor_classes.cpp for how the ROM settles it. */
extern unsigned char AmbientSoundEffects_SpawnInfo[];   /* ov002 0x0210b47c */
void *AmbientSoundEffects_Spawn(void);                  /* ov002 0x020f1b94 */
void hal_fill_ambient_sound_vtable(void);
/* ---- BLACK_BRICK_BLOCK (17, ov002) x1 ---- */
extern unsigned char BlackBrickBlock_SpawnInfo[];       /* ov002 0x02108a78 */
void *BlackBrickBlock_Spawn(void);                      /* ov002 0x020b409c */
void hal_fill_black_brick_block_vtable(void);
/* ---- BLUE_COIN_SWITCH (10, ov002) ---- */
extern unsigned char BlueCoinSwitch_SpawnInfo[];        /* ov002 0x0210b248 */
void *BlueCoinSwitch_Spawn(void);                       /* ov002 0x020f15cc */
void hal_fill_blue_coin_switch_vtable(void);
/* ---- STAR_MARKER (180, ov002) ---- */
extern unsigned char StarMarker_SpawnInfo[];            /* ov002 0x0210aa78 */
void *StarMarker_Spawn(void);                           /* ov002 0x020ebe8c */
void hal_fill_star_marker_vtable(void);
/* ---- SIGN_POST (184, ov002) x5 ---- */
extern unsigned char SignPost_SpawnInfo[];              /* ov002 0x02109ad4 */
void *SignPost_Spawn(void);                             /* ov002 0x020bc3c8 */
void hal_fill_sign_post_vtable(void);
/* ---- ONE_UP_MUSHROOM (276, ov002) x3 ---- */
extern unsigned char OneUpMushroom_SpawnInfo[];         /* ov002 0x02108388 */
void *OneUpMushroom_Spawn(void);                        /* ov002 0x020b0580 */
void hal_fill_one_up_mushroom_vtable(void);
/* ---- gate 17: the level overlay's own four ----
   The SpawnInfo records come from the PER-SYMBOL ov009 mount (ov009_syms.txt),
   not the whole one: the registry rewrites the factory word in place and the
   ActorBase constructor reads the two priorities back out of the same record,
   so it has to be storage the actor code and the registry agree on. */
extern unsigned char Bird_SpawnInfo[];                  /* ov009 0x02113934 */
void *Bird_Spawn(void);                                 /* ov009 0x02111a30 */
void hal_fill_bird_vtable(void);
extern unsigned char CastleWater_SpawnInfo[];           /* ov009 0x021139f4 */
void *CastleWater_Spawn(void);                          /* ov009 0x02111d8c */
void hal_fill_castle_water_vtable(void);
extern unsigned char MetalNet_SpawnInfo[];              /* ov009 0x02113abc */
void *MetalNet_Spawn(void);                             /* ov009 0x02112048 */
void hal_fill_metal_net_vtable(void);
extern unsigned char Flag_SpawnInfo[];                  /* ov009 0x02113b7c */
void *Flag_Spawn(void);                                 /* ov009 0x021121f0 */
void hal_fill_flag_vtable(void);
/* ---- gate 143: level 11's own class, IceSlideManager (356, ov019) ----
   Cool Cool Mountain's slide. The SpawnInfo comes from the per-symbol ov019
   mount (ov019_syms.txt); the vtable fill is hal/actor_classes_ccm.cpp. */
extern unsigned char IceSlideManager_SpawnInfo[];       /* ov019 0x021133a8 */
void *IceSlideManager_Spawn(void);                      /* ov019 0x0211274c */
void hal_fill_ice_slide_manager_vtable(void);
/* ---- gate 190: level 10's own classes, ICE_SHEET (295) + POWER_STAR_CREATE
   (355), Cool Cool Mountain (ov018). Both SpawnInfo records come from the
   per-symbol ov018 mount (ov018_syms.txt); both vtable fills are in
   hal/actor_classes_ccm.cpp. */
extern unsigned char IceSheet_SpawnInfo[];              /* ov018 0x02113b10 */
void *IceSheet_Spawn(void);                             /* ov018 0x02112a44 */
void hal_fill_ice_sheet_vtable(void);
extern unsigned char PowerStarCreate_SpawnInfo[];       /* ov018 0x02113a50 */
void *PowerStarCreate_Spawn(void);                      /* ov018 0x0211278c */
void hal_fill_power_star_create_vtable(void);
/* ---- gate 190: ONE_UP_LOGO (331), ov002 (already mounted). Its SpawnInfo
   joins the existing per-symbol ov002 mount; the vtable fill is
   hal/actor_classes_ccm.cpp. */
extern unsigned char OneUpLogo_SpawnInfo[];             /* ov002 0x0210b188 */
void *OneUpLogo_Spawn(void);                            /* ov002 0x020f1170 */
void hal_fill_one_up_logo_vtable(void);
/* ---- gate 190 fix round: SOUND_OBJECT (359), ov002. The positional sound
   emitter the free-flying POWER_STAR retries spawning every frame
   (func_ov002_020e7e24, Actor::SpawnSoundObj(6)); hosting PSC's fifth star
   made the retry stream visible. SpawnInfo joins the ov002 mount; the fill
   (and the PMF cell seat) is hal/actor_classes_ccm.cpp +
   port/unmatched/SoundObject_Behavior.cpp. */
extern unsigned char SoundObject_SpawnInfo[];           /* ov002 0x0210c064 */
void *SoundObject_Spawn(void);                          /* ov002 0x020f972c */
void hal_fill_sound_object_vtable(void);
/* ---- gate 191: SKI_LIFT (63) + MOTHER_PENGUIN (257), ov018, extending the
   gate-190 per-symbol mount (ov018_syms.txt). SpawnInfo records join the
   same mount; both vtable fills are in hal/actor_classes_ccm.cpp. */
extern unsigned char SkiLift_SpawnInfo[];               /* ov018 0x021138a8 */
void *SkiLift_Spawn(void);                              /* ov018 0x02111818 */
void hal_fill_ski_lift_vtable(void);
extern unsigned char MotherPenguin_SpawnInfo[];         /* ov018 0x02113998 */
void *MotherPenguin_Spawn(void);                        /* ov018 0x0211267c */
void hal_fill_mother_penguin_vtable(void);
/* ---- gate 192: the ov081 trio -- ICE_BLOCK (18), MR_BLIZZARD (223),
   SPINDRIFT (312), a fresh per-symbol mount (ov081_syms.txt). SpawnInfo
   records come from that mount; all three vtable fills are in
   hal/actor_classes_ov081.cpp. */
extern unsigned char Spindrift_SpawnInfo[];             /* ov081 0x02128858 */
void *Spindrift_Spawn(void);                            /* ov081 0x02124040 */
void hal_fill_spindrift_vtable(void);
extern unsigned char MrBlizzard_SpawnInfo[];            /* ov081 0x021289cc */
void *MrBlizzard_Spawn(void);                           /* ov081 0x02125ec0 */
void hal_fill_mr_blizzard_vtable(void);
extern unsigned char IceBlock_SpawnInfo[];              /* ov081 0x02128ca0 */
void *IceBlock_Spawn(void);                             /* ov081 0x021280a0 */
void hal_fill_ice_block_vtable(void);
/* ---- gate 193: ov072's BABY_PENGUIN (256) + the actual SNOWMAN (272,
   the unnamed daBgSnwmn_c block -- NOT SnowmanBody/SnowmanHead, see
   port/ov072_syms.txt), a fresh per-symbol mount (ov072_syms.txt).
   SpawnInfo records come from that mount; both vtable fills are in
   hal/actor_classes_ov072.cpp. daBgSnwmn_c's own SpawnInfo has no clean
   dsd name -- data_ov072_02122954 is what the ROM's own spawn table
   entry resolves to. */
extern unsigned char data_ov072_02122954[];             /* ov072 0x02122954 */
void *func_ov072_02120c00(void);                        /* ov072 0x02120c00 */
void hal_fill_snowman_vtable(void);
extern unsigned char BabyPenguin_SpawnInfo[];           /* ov072 0x02122a6c */
void *BabyPenguin_Spawn(void);                          /* ov072 0x02121fac */
void hal_fill_baby_penguin_vtable(void);
/* ---- gate 194: HOOT_THE_OWL (234), ov094, a fresh per-symbol mount
   (ov094_syms.txt). SpawnInfo comes from that mount; the vtable fill is in
   hal/actor_classes_ov094.cpp. */
extern unsigned char HootTheOwl_SpawnInfo[];            /* ov094 0x02136a34 */
void *HootTheOwl_Spawn(void);                           /* ov094 0x02136798 */
void hal_fill_hoot_the_owl_vtable(void);
/* ---- gate 199: SWITCH_PILLAR (34) + BASEMENT_WATER (35), ov012 (level 4),
   a fresh per-symbol mount (ov012_syms.txt) joining the existing whole
   mount. SpawnInfo records come from that mount; both vtable fills are in
   hal/actor_classes_ov012.cpp. */
extern unsigned char SwitchPillar_SpawnInfo[];          /* ov012 0x02112320 */
void *SwitchPillar_Spawn(void);                         /* ov012 0x02111420 */
void hal_fill_switch_pillar_vtable(void);
extern unsigned char BasementWater_SpawnInfo[];         /* ov012 0x021123e4 */
void *BasementWater_Spawn(void);                        /* ov012 0x02111730 */
void hal_fill_basement_water_vtable(void);
/* ---- gate 200: PUSH_BLOCK (306) + MUGEN_BGM (351), ov002 (already
   mounted; the SpawnInfo/vtable symbols joined ov002_syms.txt for this
   gate). id 306's records are dsd's "PowerFlower" names -- a naming-race
   decoy; PowerFlower_Spawn installs _ZTV9PushBlock itself (no SpawnInfo
   +0x24 vtable word for this id). Both fills are in
   hal/actor_classes_ov002g200.cpp. */
extern unsigned char PowerFlower_SpawnInfo[];           /* ov002 0x021097a0, +4 reads 306 */
void *PowerFlower_Spawn(void);                          /* ov002 0x020b9e0c */
void hal_fill_pushblock_vtable(void);
extern unsigned char MugenBgm_SpawnInfo[];              /* ov002 0x0210b560, +4 reads 351 */
void *MugenBgm_Spawn(void);                             /* ov002 0x020f1f40 */
void hal_fill_mugen_bgm_vtable(void);
/* ---- gate 202: CLOCK_HAND_SHORT (292) + CLOCK_HAND_LONG (293) +
   CLOCK_PENDULUM (294), ov013 (level 5), a fresh per-symbol mount
   (ov013_syms.txt) joining the existing whole level mount. Both hands share
   ONE vtable (HandLong_Spawn installs _ZTV22ClockPaintingHandShort too,
   reloc-verified; InitResources picks the hand off the actor id) so both
   hand rows use the same fill. All three fills are in
   hal/actor_classes_ov013.cpp. */
extern unsigned char ClockPaintingHandShort_SpawnInfo[]; /* ov013 0x021121dc, word[1] reads 292 */
void *ClockPaintingHandShort_Spawn(void);                /* ov013 0x0211163c */
extern unsigned char ClockPaintingHandLong_SpawnInfo[];  /* ov013 0x021121c0, word[1] reads 293 */
void *ClockPaintingHandLong_Spawn(void);                 /* ov013 0x02111674 */
void hal_fill_clock_hand_vtable(void);
extern unsigned char ClockPaintingPendulum_SpawnInfo[];  /* ov013 0x02112104, word[1] reads 294 */
void *ClockPaintingPendulum_Spawn(void);                 /* ov013 0x02111384 */
void hal_fill_clock_pendulum_vtable(void);
/* ---- gate 18: ov085, the first ACTOR overlay ----
   Not the engine overlay and not the level's -- six classes several levels
   share, of which the castle grounds names two. The SpawnInfo records come
   from the per-symbol ov085 mount (ov085_syms.txt). */
extern unsigned char Rabbit_SpawnInfo[];                /* ov085 0x021300d4 */
void *Rabbit_Spawn(void);                               /* ov085 0x0212cc2c */
void hal_fill_rabbit_vtable(void);
extern unsigned char RabbitKey_SpawnInfo[];             /* ov085 0x021301b4 */
void *RabbitKey_Spawn(void);                            /* ov085 0x0212d4e8 */
void hal_fill_rabbit_key_vtable(void);
extern unsigned char LakituBro_SpawnInfo[];             /* ov085 0x02130320 */
void *LakituBro_Spawn(void);                            /* ov085 0x0212ed54 */
void hal_fill_lakitu_bro_vtable(void);
/* ---- run lvled, lane intro-cutscene: the new-file opening's cast ---- */
extern unsigned char CutsceneObject_SpawnInfo[];        /* ov002 0x0210ba60 */
void *CutsceneObject_Spawn(void);                       /* ov002 0x020f8808 */
void hal_fill_cutscene_object_vtable(void);
/* ---- gate 19: ov098's CANNON ---- */
extern unsigned char Cannon_SpawnInfo[];                /* ov098 0x0213c67c */
void *Cannon_Spawn(void);                               /* ov098 0x0213b43c */
void hal_fill_cannon_vtable(void);
/* ---- gate 20: ov002's last two ----
   Both SpawnInfo records come from the ov002 per-symbol mount that has
   carried Player/Tree/SignPost since gate 14. The class-method names are the
   neighbour's in both cases and the *_SpawnInfo names are the right ones --
   the ROM's RTTI settles it, see slice_gate20.txt. */
extern unsigned char Exit_SpawnInfo[];                  /* ov002 0x02108690 */
void *Exit_Spawn(void);                                 /* ov002 0x020b0f24 */
void hal_fill_exit_vtable(void);
extern unsigned char WaterfallMist_SpawnInfo[];         /* ov002 0x0210947c */
void *WaterfallMist_Spawn(void);                        /* ov002 0x020b6ee8 */
void hal_fill_waterfall_mist_vtable(void);
/* ---- gate 21: ov100's BUTTERFLY and FISH ----
   The third actor overlay (ov100_syms.txt). Both class names check out
   against the ROM's own RTTI; the one shifted pair in ov100 is Door/StarDoor
   and that is gate 22's problem. */
extern unsigned char Butterfly_SpawnInfo[];             /* ov100 0x02147e78 */
void *Butterfly_Spawn(void);                            /* ov100 0x02141ea4 */
void hal_fill_butterfly_vtable(void);
extern unsigned char Fish_SpawnInfo[];                  /* ov100 0x02148498 */
void *Fish_Spawn(void);                                 /* ov100 0x02146d38 */
void hal_fill_fish_vtable(void);
extern unsigned char UnchainedChomp_SpawnInfo[];        /* ov100 0x02148020 */
void *UnchainedChomp_Spawn(void);
void hal_fill_unchained_chomp_vtable(void);
/* ---- gate 22: ov100's DOOR ----
   The last class the castle grounds names. Its vtable is the one config left
   as data_ov100_02148188 while giving `_ZTV4Door` to the star door; the ROM's
   RTTI settles it, see slice_gate22.txt. */
extern unsigned char Door_SpawnInfo[];                  /* ov100 0x02148164 */
void *Door_Spawn(void);                                 /* ov100 0x0214589c */
void hal_fill_door_vtable(void);
/* ---- gate 40: ov100's STAR_DOOR (354) ----
   NOT the DOOR above. Config swapped the two doors' names: the _ZN4Door*
   family is really the STAR door (RTTI daStarGate_c), so it has its own six
   methods and its own vtable fill (hal_fill_star_door_vtable), distinct from
   the real door's hal_fill_door_vtable. Its SpawnInfo is already mounted by
   ov100_syms.txt at 0x021483a8; the record's +4 halfword reads 354. See the
   head of slice_gate40.txt and the gate-40 section of hal/actor_classes.cpp. */
extern unsigned char StarDoor_SpawnInfo[];              /* ov100 0x021483a8 */
void *StarDoor_Spawn(void);                             /* ov100 0x021461d4 */
void hal_fill_star_door_vtable(void);
/* ---- gate 41: ov010's TRAP (36) and LIGHT_BEAM (37) ----
   Level 2's own overlay, mounted per symbol (ov010_syms.txt). LIGHT_BEAM has
   no fill of its own: its factory installs _ZTV4Trap, which the TRAP row
   fills. Both records' +4 halfwords read 36 / 37. */
extern unsigned char Trap_SpawnInfo[];                  /* ov010 0x02112ac0 */
void *Trap_Spawn(void);                                 /* ov010 0x02111998 */
void hal_fill_trap_vtable(void);
extern unsigned char LightBeam_SpawnInfo[];             /* ov010 0x02112b84 */
void *LightBeam_Spawn(void);                            /* ov010 0x02111dd0 */
/* ---- gate 42: ov010's PEACH_PAINTING (38) ----
   The prior "blocked on ov052/RollingRock" reading was the shared-base illusion
   -- both mislabeled references stay inside ov010 (func_ov010_02111e84 and the
   ov010 BSS SharedFilePtr data_ov010_02112d64), closed by /alternatename in
   hal/actor_classes.cpp. Its SpawnInfo is mounted (ov010_syms.txt, +4 = 38). */
extern unsigned char PeachPainting_SpawnInfo[];        /* ov010 0x02112c44 */
void *PeachPainting_Spawn(void);                       /* ov010 0x02112004 */
void hal_fill_peach_painting_vtable(void);
/* ---- gate 50: ov080's PAINTING (daPicGate_c, actor 307) ----
   A DIFFERENT class from PEACH_PAINTING, in a different overlay, with no
   cross-overlay spawn dependency. Its record's +4 halfword reads 307 and the
   +0 word is Painting_Spawn (ov080 0x02126f8c), which the registry repoints to
   the host factory. Its vtable is the 31-slot Actor table dsd left unnamed at
   data_ov080_021282b4; hal/actor_classes_painting.cpp declares that as the
   host array Painting_Spawn installs, and hal_fill_painting_vtable fills it. */
extern unsigned char Painting_SpawnInfo[];             /* ov080 0x02128290 */
void *Painting_Spawn(void);                            /* ov080 0x02126f8c */
void hal_fill_painting_vtable(void);
/* ---- gate 23: ov102's QUESTION_BLOCK ----
   The fifth overlay, and the last id on the castle grounds' skip list that is
   not a 2D system. Its name checks out against the ROM's RTTI
   (daObjHatenaBlock_c) and its SpawnInfo's own +4 halfword. */
extern unsigned char QuestionBlock_SpawnInfo[];         /* ov102 0x0214e3e8 */
void *QuestionBlock_Spawn(void);                        /* ov102 0x0214a8ec */
void hal_fill_question_block_vtable(void);
/* ---- gate 27: the BOTTOM SCREEN'S OWN ------------------------------------
   HUD (334), one of the two ids the castle-grounds census has been printing
   as skipped. Its SpawnInfo is ov002 0x0210c210, which config names
   _ZN3HUD9spawnDataE, and the ROM puts the CONSTRUCTOR in the record's +0
   word -- there is no separate Spawn function to repoint. Its vtable is
   eighteen slots, not twenty; see the table read out of the ROM at the head
   of hal/sub_actors.cpp. */
extern unsigned char _ZN3HUD9spawnDataE[];      /* ov002 0x0210c210 */
int *_ZN3HUDC1Ev(void);                         /* ov002 0x020fe154 */
void hal_fill_hud_vtable(void);
/* ---- gate 28: the MINIMAP -------------------------------------------------
   The other id (335) the census printed as skipped, and the same shape as the
   HUD: SpawnInfo at ov002 0x0210c160 with the CONSTRUCTOR in the +0 word, and
   an eighteen-slot vtable. Its object is 600 bytes rather than the HUD's 124.
   It is also the actor that turns BG3-sub on -- InitResources ORs bit 3 into
   the layer mask hal/sub_screen.cpp publishes every frame. */
extern unsigned char _ZN7Minimap9spawnDataE[];  /* ov002 0x0210c160 */
int *_ZN7MinimapC1Ev(void);                     /* ov002 0x020fb8bc */
void hal_fill_minimap_vtable(void);
/* ---- gate 33: BOB-OMB BATTLEFIELD's mechanisms, terrain and pickups -------
   Level 6, ov014. Every id here was read out of that level's own object
   tables and resolved through the EU ACTOR_SPAWN_TABLE at arm9 0x02090864;
   hal/actor_classes_bob_world.cpp carries the evidence and the vtable fills.
   Kept as one contiguous block so the merge with the enemy stream, which
   appends its own block, is a straight concatenation. */
extern unsigned char Coin_SpawnInfo[];                  /* ov002 0x02108790 */
extern unsigned char RedCoin_SpawnInfo[];               /* ov002 0x021087ac */
extern unsigned char BlueCoin_SpawnInfo[];              /* ov002 0x021087c8 */
void *Coin_Spawn(void);                                 /* ov002 0x020b2b48 */
void *RedCoin_Spawn(void);                              /* ov002 0x020b2af0 */
void *BlueCoin_Spawn(void);                             /* ov002 0x020b2a98 */
void hal_fill_coin_vtable(void);
/* BRICK_BLOCK has no fill of its own: its factory installs
   _ZTV13BigBrickBlock, which gate 16 already fills for BLACK_BRICK_BLOCK. */
extern unsigned char BrickBlock_SpawnInfo[];            /* ov002 0x02108a24 */
void *BrickBlock_Spawn(void);                           /* ov002 0x020b4708 */
/* gate 48: BIG_BRICK_BLOCK (16) and FORTRESS_TOWER_WALL (46), Whomp's Fortress.
   Both factories install _ZTV13BigBrickBlock too -- no fill of their own, same
   as BRICK_BLOCK. */
extern unsigned char BigBrickBlock_SpawnInfo[];         /* ov002 0x02108a40 */
void *BigBrickBlock_Spawn(void);                        /* ov002 0x020b40fc */
extern unsigned char FortressTowerWall_SpawnInfo[];     /* ov002 0x02108a94 */
void *FortressTowerWall_Spawn(void);                    /* ov002 0x020b406c */
extern unsigned char MegaMushroomBlockTag_SpawnInfo[];  /* ov002 0x02108ba0 */
void *MegaMushroomBlockTag_Spawn(void);
void hal_fill_mega_mushroom_block_tag_vtable(void);
/* gate 49: ONE_UP_MUSHROOM_BLOCK_TAG (321), the same _ZTV10BrickBlock tag body
   as MEGA_MUSHROOM_BLOCK_TAG -- no fill of its own. */
extern unsigned char OneUpMushroomBlockTag_SpawnInfo[]; /* ov002 0x02108bf4 */
void *OneUpMushroomBlockTag_Spawn(void);                /* ov002 0x020b4670 */
extern unsigned char CannonHatch_SpawnInfo[];           /* ov002 0x02109d14 */
void *CannonHatch_Spawn(void);
void hal_fill_cannon_hatch_vtable(void);
extern unsigned char Cap_SpawnInfo[];                   /* ov002 0x021095cc */
void *Cap_Spawn(void);
void hal_fill_cap_vtable(void);
extern unsigned char HealingHeart_SpawnInfo[];          /* ov002 0x02109c50 */
void *HealingHeart_Spawn(void);
void hal_fill_healing_heart_vtable(void);
extern unsigned char ExclamationSwitch_SpawnInfo[];     /* ov002 0x02109900 */
void *ExclamationSwitch_Spawn(void);
void hal_fill_exclamation_switch_vtable(void);
extern unsigned char InvisibleSecret_SpawnInfo[];       /* ov002 0x0210b00c */
void *InvisibleSecret_Spawn(void);
void hal_fill_invisible_secret_vtable(void);
/* gate 46: INVISIBLE_POLE (287), RTTI daBar_c. Host-storage vtable, filled by
   hand (the InvisibleSecret case). */
extern unsigned char InvisiblePole_SpawnInfo[];         /* ov002 0x0210845c */
void *InvisiblePole_Spawn(void);                        /* ov002 0x020b0710 */
void hal_fill_invisible_pole_vtable(void);
extern unsigned char ArrowSignLeft_SpawnInfo[];         /* ov098 0x0213c398 */
void *ArrowSignLeft_Spawn(void);
extern unsigned char ArrowSignRight_SpawnInfo[];        /* ov098 0x0213c3b4 */
void *ArrowSignRight_Spawn(void);
void hal_fill_arrow_sign_vtable(void);
extern unsigned char WaterBomb_SpawnInfo[];             /* ov098 0x0213c74c */
void *WaterBomb_Spawn(void);
void hal_fill_water_bomb_vtable(void);
extern unsigned char RollingIronBall_SpawnInfo[];       /* ov100 0x02147f58 */
void *RollingIronBall_Spawn(void);
void hal_fill_rolling_iron_ball_vtable(void);
extern unsigned char WarpPipe_SpawnInfo[];              /* ov102 0x0214e134 */
void *WarpPipe_Spawn(void);
void hal_fill_warp_pipe_vtable(void);
extern unsigned char Warp_SpawnInfo[];                  /* ov002 0x0210ac98 */
void *Warp_Spawn(void);                                 /* ov002 0x020ec534 */
void hal_fill_warp_vtable(void);
/* ---- gate 32: BOB-OMB BATTLEFIELD'S LIVING CAST ---------------------------
   The first ENEMY-family classes the port hosts, which is why they are in a
   file of their own (hal/actor_classes_bob_enemy.cpp): their vtables are
   THIRTY-ONE slots where every class above is twenty, because Actor appends
   thirteen virtuals of its own past ActorBase's eighteen.
   The ids are the ROM's own, cross-checked three ways: symbols/overlay_actors.md
   (built from ACTOR_SPAWN_TABLE at arm9 0x02090864), each SpawnInfo's +4
   halfword, and each class's RTTI name. */
extern unsigned char BobOmb_SpawnInfo[];        /* ov102 0x0214e534 */
void *BobOmb_Spawn(void);                       /* ov102 0x0214c6f8 */
void hal_fill_bob_omb_vtable(void);
extern unsigned char Goomba_SpawnInfo[];        /* ov084 0x021308ec */
void *Goomba_Spawn(void);                       /* ov084 0x0212c0b0 */
void hal_fill_goomba_vtable(void);
extern unsigned char BobOmbBuddy_SpawnInfo[];   /* ov084 0x02130a14 */
void *BobOmbBuddy_Spawn(void);                  /* ov084 0x0212d200 */
void hal_fill_bob_omb_buddy_vtable(void);
/* ov014 is Bob-omb Battlefield's OWN level overlay, mounted twice (whole for
   the loaders, per symbol for these two classes' tables) -- the gate-17 shape
   applied to a second level. */
extern unsigned char ChainChomp_SpawnInfo[];      /* ov014 0x02114750 */
void *ChainChomp_Spawn(void);                     /* ov014 0x02112d1c */
void hal_fill_chain_chomp_vtable(void);
extern unsigned char ChainChompFence_SpawnInfo[]; /* ov014 0x0211488c */
void *ChainChompFence_Spawn(void);                /* ov014 0x0211307c */
void hal_fill_chain_chomp_fence_vtable(void);
/* gate 82: SHUTTER_BOB (40), the same ov014 mount as the chain chomp. A
   Platform subclass (RTTI daObjBSwdoor_c); SpawnInfo and data_ov014_021145c4
   already come from ov014_syms.txt, so this is a HAL-only addition. */
extern unsigned char ShutterBob_SpawnInfo[];      /* ov014 0x021145e4 */
void *ShutterBob_Spawn(void);                     /* host copy, unmatched/ */
void hal_fill_shutter_bob_vtable(void);
/* gate 83: SEESAW_BOB (39), the fresh ov095 mount. A Platform subclass (RTTI
   daObjSeesaw_c); SpawnInfo and the three file tables come from ov095_syms.txt,
   the sinits from hal/actor_overlays.cpp. */
extern unsigned char SeesawBob_SpawnInfo[];       /* ov095 0x02137484 */
void *SeesawBob_Spawn(void);                      /* ov095 0x02135f94 */
void hal_fill_seesaw_bob_vtable(void);
extern unsigned char KoopaTheQuick_SpawnInfo[];   /* ov062 0x0211db78 */
void *KoopaTheQuick_Spawn(void);                  /* ov062 0x0211aee0 */
void hal_fill_koopa_the_quick_vtable(void);
/* Actor 27, the post the chomp is chained to. Its CLASS is daObjPile_c, not
   the Stump the config's _ZTV5Stump names -- see the section header in
   hal/actor_classes_bob_enemy.cpp. The SpawnInfo and factory names are the
   right ones: that record's +4 halfword reads 27. */
extern unsigned char Stump_SpawnInfo[];           /* ov091 0x02135298 */
void *Stump_Spawn(void);                          /* ov091 0x02133938 */
void hal_fill_stump_vtable(void);
extern unsigned char KoopaFlag_SpawnInfo[];       /* ov062 0x0211dc30 */
void *KoopaFlag_Spawn(void);                      /* ov062 0x0211b208 */
void hal_fill_koopa_flag_vtable(void);
/* ov078 is KING_BOB_OMB's whole overlay, and level 6 is the only level of the
   fifty-two that loads it. */
extern unsigned char KingBobOmb_SpawnInfo[];      /* ov078 0x02126e28 */
void *KingBobOmb_Spawn(void);                     /* ov078 0x021265fc */
void hal_fill_king_bob_omb_vtable(void);
/* ---- gates 60-63: Whomp's Fortress' mechanical platforms (ov015) ----------
   Wired by the vtable each factory installs, not the class name the config
   carries (the ov015 name shift; see hal/actor_classes_wf.cpp). SpawnInfo
   records from the ov015 per-symbol mount (ov015_syms.txt); each +4 halfword
   is the id below. */
extern unsigned char TowerStep_SpawnInfo[];       /* ov015 0x0211462c, id 52 */
void *TowerStep_Spawn(void);                      /* ov015 0x0211290c */
void hal_fill_tower_step_vtable(void);
extern unsigned char RotatingBridge_SpawnInfo[];  /* ov015 0x021146f0, id 51 */
void *RotatingBridge_Spawn(void);                 /* ov015 0x02112ba0 */
void hal_fill_rotating_bridge_vtable(void);
extern unsigned char PoleBillboard_SpawnInfo[];   /* ov015 0x0211433c, id 42 */
void *PoleBillboard_Spawn(void);                  /* ov015 0x021112dc */
void hal_fill_pole_billboard_vtable(void);
extern unsigned char KnockDownPlank_SpawnInfo[];  /* ov015 0x021143fc, id 44 */
void *KnockDownPlank_Spawn(void);                 /* ov015 0x02111b68 */
void hal_fill_knock_down_plank_vtable(void);
/* MOVING_BAR_BIG (53) and MOVING_BAR_SMALL (54) share vtable 0x0211458c (the
   _ZN14KnockDownPlank* methods); both factories install it. KnockDownPlank::
   Behavior forms its per-frame pointer-to-member over a FORWARD-DECLARED struct,
   so a host copy (port/unmatched/KnockDownPlank_Behavior.cpp) reads the table as
   plain ints; the same file seats both {fn, delta} tables. See the note in
   hal/actor_classes_wf.cpp. */
extern unsigned char MovingBarBig_SpawnInfo[];    /* ov015 0x02114568, id 53 */
void *MovingBarBig_Spawn(void);                   /* ov015 0x02112230 */
extern unsigned char MovingBarSmall_SpawnInfo[];  /* ov015 0x0211454c, id 54 */
void *MovingBarSmall_Spawn(void);                 /* ov015 0x02112260 */
void hal_fill_moving_bar_vtable(void);            /* shared vtable 0x0211458c */
extern unsigned char RotatingPlatformWf_SpawnInfo[]; /* ov015 0x021147c4, id 50 */
/* its factory is a host wrapper: the raw RotatingPlatformWf_Spawn stores the
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
extern unsigned char Thwomp_SpawnInfo[];          /* ov091 0x02135150, id 161 */
void *port_factory_thwomp(void);
void hal_fill_thwomp_vtable(void);
/* gate 74: SLIDING_PLATFORM_WF (55). Clean vtable install, no wrapper. */
extern unsigned char SlidingPlatformWf_SpawnInfo[]; /* ov091 0x02135008, id 55 */
void *SlidingPlatformWf_Spawn(void);
void hal_fill_sliding_platform_wf_vtable(void);
/* gate 73: ROTATING_UP_DOWN_PLATFORM (29). The ov015 name shift: its factory
   installs the RotatingUpDownPlatformUtm table, so it runs Utm's methods. */
extern unsigned char RotatingUpDownPlatform_SpawnInfo[]; /* ov091 0x02134bf8, id 29 */
void *RotatingUpDownPlatform_Spawn(void);
void hal_fill_rotating_up_down_platform_vtable(void);
/* gate 71: FIRE_PIRANHA_PLANT (253). The ov015 name shift: its factory installs
   the FirePiranhaPlantBig table, so it runs that class. No state seat. */
extern unsigned char FirePiranhaPlant_SpawnInfo[];  /* ov084 0x02130acc, id 253 */
void *FirePiranhaPlant_Spawn(void);
void hal_fill_fire_piranha_plant_vtable(void);
/* gate 70: PIRANHA_PLANT (250). Clean vtable install; its nine-state PMF
   Behavior is a host copy (port/unmatched/PiranhaPlant_States.cpp). */
extern unsigned char PiranhaPlant_SpawnInfo[];      /* ov084 0x02130c04, id 250 */
void *PiranhaPlant_Spawn(void);
void hal_fill_piranha_plant_vtable(void);
/* FALL_BLOCK_WF (45), vtable 0x021148dc (_ZTV11FallBlockWf). The inherited
   ov098-chain blocker was a phantom: the Behavior func_ov098_0213a36c is a plain
   switch machine whose same-id walk is null-safe. Every slot is matched src; a
   plain 32-slot fill and a slice reach it. See hal/actor_classes_wf.cpp. */
extern unsigned char FallBlockWf_SpawnInfo[];     /* ov015 0x021148b8, id 45 */
void *FallBlockWf_Spawn(void);                    /* ov015 0x02112dd0 */
void hal_fill_fall_block_wf_vtable(void);
/* ---- gates 64-69: Whomp's Fortress' enemy cast (ov079) --------------------
   Six classes, ids read from the arm9 ACTOR_SPAWN_TABLE (the ROM's numbers, not
   the task's shorthand) and each SpawnInfo's +4 halfword. WHOMP/WHOMP_KING
   share _ZTV5Whomp and FORTRESS_WALL/FORTRESS_WALL_BREAKABLE share
   _ZTV12FortressWall, so only the first of each pair carries the vtable fill;
   the second row's factory installs the same table (fill 0). SpawnInfo records
   from the ov079 per-symbol mount (ov079_syms.txt). See
   hal/actor_classes_wf_enemy.cpp. */
extern unsigned char Whomp_SpawnInfo[];           /* ov079 0x02127c5c, id 164 */
void *Whomp_Spawn(void);                          /* ov079 0x021265e8 */
void hal_fill_whomp_vtable_c(void);
extern unsigned char WhompKing_SpawnInfo[];       /* ov079 0x02127c40, id 165 */
void *WhompKing_Spawn(void);                      /* ov079 0x02126588 */
extern unsigned char BulletBill_SpawnInfo[];      /* ov079 0x02127ec4, id 222 */
void *BulletBill_Spawn(void);                     /* ov079 0x02126d64 */
void hal_fill_bullet_bill_vtable_c(void);
extern unsigned char BillBlaster_SpawnInfo[];     /* ov079 0x02127f94, id 43 */
void *BillBlaster_Spawn(void);                    /* ov079 0x021271b4 */
void hal_fill_bill_blaster_vtable_c(void);
extern unsigned char FortressWall_SpawnInfo[];    /* ov079 0x0212808c, id 47 */
void *FortressWall_Spawn(void);                   /* ov079 0x0212757c */
void hal_fill_fortress_wall_vtable_c(void);
extern unsigned char FortressWallBreakable_SpawnInfo[]; /* ov079 0x02128070, id 48 */
void *FortressWallBreakable_Spawn(void);          /* ov079 0x021275ac */
/* ---- gate 89: POWER_STAR (178, ov002) -------------------------------------
   An Enemy subclass whose vtable _ZTV9PowerStar (ov002 0x0210ab3c, 31 slots,
   RTTI daStar_c) PowerStar_Spawn installs directly. SpawnInfo record from the
   ov002 per-symbol mount (added to ov002_syms.txt); the record's +4 halfword
   reads 178. Its Behavior dispatches a fourteen-entry pointer-to-member state
   table, hosted in port/unmatched/PowerStar_States.cpp. See
   hal/actor_classes_star.cpp. */
extern unsigned char PowerStar_SpawnInfo[];       /* ov002 0x0210aa40, id 178 */
void *PowerStar_Spawn(void);                      /* ov002 0x020ebf30 */
void hal_fill_power_star_vtable(void);
/* ---- gate 90: STAR_CAMERA (177, ov002) ------------------------------------
   A bare Actor: StarCamera_Spawn installs NO vtable of its own, so it dispatches
   the base Actor vtable _ZTV5Actor (data_0208e3a4). hal_fill_actor_base_vtable
   fills that shared table -- provably safe, it is the only class that leaves it
   installed. SpawnInfo from the ov002 mount; the record's +4 halfword reads
   177. See hal/actor_classes_star.cpp. */
extern unsigned char StarCamera_SpawnInfo[];      /* ov002 0x0210aa94, id 177 */
void *StarCamera_Spawn(void);                     /* ov002 0x020ebe5c */
void hal_fill_actor_base_vtable(void);
/* ---- gate 102: FORTRESS_TOWER (49, ov102) ---------------------------------
   Whomp's Fortress' tower, a Platform subclass (RTTI daObjSimpleBg_c). Its
   factory FortressTower_Spawn installs _ZTV13FortressTower, the SAME table
   WARP_PIPE (id 298) installs and hal_fill_warp_pipe_vtable already fills, so
   this row carries no fill of its own. All five methods are already compiled
   (gate 33's WARP_PIPE slice) and FortressTower_SpawnInfo is already mounted
   (ov102_syms.txt); the record's +4 halfword reads 49. See slice_gate102.txt
   and the WARP_PIPE section in hal/actor_classes_bob_world.cpp. */
extern unsigned char FortressTower_SpawnInfo[];   /* ov102 0x0214e118, id 49 */
void *FortressTower_Spawn(void);                  /* ov102 0x02148f2c */
/* ---- gate 103: STAR_SWITCH (12, ov002) ------------------------------------
   Whomp's Fortress' star switch, a Platform subclass (RTTI daObjSwitch_c). Its
   factory StarSwitch_Spawn installs _ZTV10StarSwitch, the SAME table
   EXCLAMATION_SWITCH (id 11) installs and hal_fill_exclamation_switch_vtable
   already fills (including the five-state PMF seat), so this row carries no
   fill of its own. All five methods are already compiled (gate 33's
   EXCLAMATION_SWITCH slice). This gate mounts StarSwitch_SpawnInfo (added to
   ov002_syms.txt); the record's +4 halfword reads 12. See slice_gate103.txt
   and the EXCLAMATION_SWITCH section in hal/actor_classes_bob_world.cpp. */
extern unsigned char StarSwitch_SpawnInfo[];      /* ov002 0x0210991c, id 12 */
void *StarSwitch_Spawn(void);                     /* ov002 0x020baaac */
/* ---- gate 128: EXCLAMATION_BLOCK (21, ov102) ------------------------------
   Jolly Roger Bay's ship sublevel (level 9) prints id 0x15 (21) skipped. Its
   factory ExclamationBlock_Spawn installs _ZTV13QuestionBlock, the SAME table
   QUESTION_BLOCK (id 20) installs and hal_fill_question_block_vtable already
   fills, so this row carries no fill of its own -- the FORTRESS_TOWER shape.
   All of QuestionBlock's methods are already compiled and its host vtable is
   already filled (gate 23), and ExclamationBlock_SpawnInfo is already mounted
   (ov102_syms.txt); the record's +4 halfword reads 21. See slice_gate128.txt
   and the QUESTION_BLOCK section in hal/actor_classes.cpp. */
extern unsigned char ExclamationBlock_SpawnInfo[]; /* ov102 0x0214e404, id 21 */
void *ExclamationBlock_Spawn(void);                /* ov102 0x0214a8ac */
/* ---- gate 121: STATIC_ROCK (61, ov102) ------------------------------------
   Jolly Roger Bay's rocks (level 8's biggest skipped class, x33), a Platform
   subclass. Its factory StaticRock_Spawn installs _ZTV13FortressTower, the SAME
   table FORTRESS_TOWER (id 49, gate 102) and WARP_PIPE (id 298, gate 33)
   install and hal_fill_warp_pipe_vtable already fills, so this row carries no
   fill of its own. All five methods are already compiled (gate 33's WARP_PIPE
   slice) and StaticRock_SpawnInfo is already mounted (ov102_syms.txt); the
   record's +4 halfword reads 61, matching ACTOR_SPAWN_TABLE[61]. See
   slice_gate121.txt and the WARP_PIPE section in hal/actor_classes_bob_world.cpp. */
extern unsigned char StaticRock_SpawnInfo[];      /* ov102 0x0214e0e0, id 61 */
void *StaticRock_Spawn(void);                     /* ov102 0x02148f8c */
/* ---- gate 122: BRICK_BLOCK_SWITCH_ACTIVATED (19, ov002) -------------------
   Jolly Roger Bay's switch-activated brick (x8). A Platform subclass whose
   factory installs _ZTV13BigBrickBlock, the SAME table BRICK_BLOCK (15),
   BIG_BRICK_BLOCK (16) and FORTRESS_TOWER_WALL (46) install and
   hal_fill_black_brick_block_vtable already fills (gate 16). No fill of its own.
   Its SpawnInfo is mounted in ov002_syms.txt; +4 reads 19, matching
   ACTOR_SPAWN_TABLE[19]. See slice_gate122.txt. */
extern unsigned char BrickBlockSwitchActivated_SpawnInfo[]; /* ov002 0x02108a5c */
void *BrickBlockSwitchActivated_Spawn(void);                /* ov002 0x020b40cc */
/* ---- gate 123: GREEN_SHELL_BLOCK_TAG (323, ov002) ------------------------
   Jolly Roger Bay's green-shell block tag (x1). A tag body sharing
   _ZTV10BrickBlock, the SAME table ONE_UP_MUSHROOM_BLOCK_TAG (321) and
   MEGA_MUSHROOM_BLOCK_TAG (322) install and hal_fill_brick_block_tag_vtable
   already fills (gate 49). No fill of its own. Its SpawnInfo is mounted in
   ov002_syms.txt; +4 reads 323, matching ACTOR_SPAWN_TABLE[323]. See
   slice_gate123.txt. */
extern unsigned char GreenShellBlockTag_SpawnInfo[];        /* ov002 0x02108bbc */
void *GreenShellBlockTag_Spawn(void);                       /* ov002 0x020b4610 */
/* ---- gate 149: HAUNTED_CHAIR (326), Big Boo's Haunt (ov020) ----
   ov020 is level 12's own overlay, mounted whole and (gate 149) per symbol so
   the class reaches its SpawnInfo record and its model SharedFilePtr by name.
   The fill and factory are in hal/actor_classes_bbh.cpp. The level's other three
   furniture classes (BOOKEND 327, BOOK_SHOT_SPAWNER 328, BOOK_SHOT 325) stay
   skipped: BookShot's return-to-shelf behaviour calls an undecompiled ov063
   function, so they are documented blockers rather than a stubbed boot. */
extern unsigned char HauntedChair_SpawnInfo[];      /* ov020 0x021149f0 */
void *HauntedChair_Spawn(void);                     /* ov020 0x02113494 */
void hal_fill_haunted_chair_vtable(void);
/* ---- gate 172: CRATE (194), level 13's first fresh candidate (ov098) ----
   The breakable wooden box, daObjBlockS_c. ov098 was already mounted per
   symbol for the WaterBomb/FallBlockWf/ExclamationSwitch gates, so the
   SpawnInfo, the state-table statics and the file tables are all in place;
   the fill is hal/actor_classes_hmc.cpp, the closure slice_gate172.txt, and
   the state-table seat port/unmatched/Crate_StateDispatch.cpp. */
extern unsigned char Crate_SpawnInfo[];             /* ov098 0x0213c510 */
void *Crate_Spawn(void);                            /* ov098 0x02139f04 */
void hal_fill_crate_vtable(void);
/* ---- gate 173: UP_DOWN_LIFT_HMC (33), Hazy Maze Cave's second fresh class
   (ov095) ---- The up-and-down platform lift, daUdlift_c. ov095 was mounted
   per symbol at gate 83 (SEESAW_BOB); this gate adds UpDownLiftHmc_SpawnInfo,
   its two variant-indexed file tables and its CLPS table to that mount
   (ov095_syms.txt). The fill is hal/actor_classes_updownlift.cpp, the closure
   slice_gate173.txt, the Behavior host copy + state seat
   port/unmatched/UpDownLiftBbh_Behavior.cpp. */
extern unsigned char UpDownLiftHmc_SpawnInfo[];     /* ov095 0x021375e8, +4 reads 33 */
void *UpDownLiftHmc_Spawn(void);                    /* ov095 0x021367cc (matched src) */
void hal_fill_up_down_lift_hmc_vtable(void);
/* ---- gate 174: MONTY_MOLE (310), first of ov080's enemy cast ----
   The burrowing mole, daChoropu_c. ov080 was mounted per symbol at gate 50
   (PAINTING) and already carries the SpawnInfo, state statics and
   SharedFilePtrs. The fill is hal/actor_classes_montymole.cpp, the closure
   slice_gate174.txt, the Behavior host copy + state seat
   port/unmatched/MontyMole_StateDispatch.cpp. */
extern unsigned char MontyMole_SpawnInfo[];         /* ov080 0x02127fec, +4 reads 310 */
void *MontyMole_Spawn(void);                        /* ov080 0x021249e0 (matched src) */
void hal_fill_monty_mole_vtable(void);
/* ---- gate 208: MONTY_MOLE_ROCK (311), the rock the mole throws ----
   daChoro_Rock_c, ov080's second enemy class. The mole's throw state spawns
   0x137 and the port declined it, so the ROM-faithful thrower stored through
   the NULL. The whole cast is matched: no host copy, no state seat, no thunk.
   The fill is hal/actor_classes_montymolerock.cpp, the closure
   slice_gate208.txt, which also carries the 31-slot vtspan derivation and the
   slot-9 adjudication. */
extern unsigned char MontyMoleRock_SpawnInfo[];     /* ov080 0x02128008, +4 reads 311 */
void *MontyMoleRock_Spawn(void);                    /* ov080 0x02124998 (matched src) */
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
extern unsigned char DiamondLift_SpawnInfo[];       /* ov043 0x02112294, +4 reads 135 */
void *DiamondLift_Spawn(void);                      /* ov043 0x021113cc (matched src) */
void hal_fill_diamond_lift_vtable(void);
/* ---- gate 175: BLUE_FLAME (317) + RED_FLAME (316), the cave's flames ----
   One class (daObjFire_c, ov002), two matched factories installing
   _ZTV9BlueFlame; RED_FLAME's row is fill=0 (the GreenShellBlockTag shape).
   Level 13 names RED_FLAME x17. The fill and the Player::Burn face are
   hal/actor_classes_flame.cpp; the closure slice_gate175.txt; no state
   table, no seat. */
extern unsigned char BlueFlame_SpawnInfo[];         /* ov002 0x02108f14, +4 reads 317 */
void *BlueFlame_Spawn(void);                        /* ov002 0x020b59a8 (matched src) */
void hal_fill_blue_flame_vtable(void);
extern unsigned char RedFlame_SpawnInfo[];          /* ov002 0x02108ef8, +4 reads 316 */
void *RedFlame_Spawn(void);                         /* ov002 0x020b59e0 (matched src) */
/* ---- gate 176: SCUTTLEBUG (255), ov071's first mount ----
   The spider daSpd_c. Fresh per-symbol ov071 mount (ov071_syms.txt); the fill
   is hal/actor_classes_scuttlebug.cpp, the closure slice_gate176.txt, the two
   state dispatchers host-copied + the record seat in
   port/unmatched/Scuttlebug_StateDispatch.cpp. */
extern unsigned char Scuttlebug_SpawnInfo[];        /* ov071 0x02122c08, +4 reads 255 */
void *Scuttlebug_Spawn(void);                       /* ov071 0x02120618 (matched src) */
void hal_fill_scuttlebug_vtable(void);
/* ---- run linkw wave 7, lane w7b: THE MR. I FAMILY, ov071's second cast ----
   MR_I (262) and BIG_MR_I (263) are ONE class behind ONE vtable -- MrI_Spawn
   and BigMrI_Spawn are the same 0x50-byte function twice, both `new(0x218)`
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
extern unsigned char MrI_SpawnInfo[];               /* ov071 0x02122cf0, +4 reads 262 */
extern unsigned char BigMrI_SpawnInfo[];            /* ov071 0x02122d0c, +4 reads 263 */
extern unsigned char MrI_Projectile_SpawnInfo[];    /* ov071 0x02122dc4, +4 reads 264 */
void *MrI_Spawn(void);                              /* ov071 0x02121a1c (matched src) */
void *BigMrI_Spawn(void);                           /* ov071 0x021219cc (matched src) */
void *MrI_Projectile_Spawn(void);                   /* ov071 0x02121f9c (matched src) */
void hal_fill_mri_vtable(void);
void hal_fill_mri_projectile_vtable(void);
/* ---- gate 177: ov064's first mount -- BULLY (215), BIG_BULLY (216),
   ROTATING_FIREBAR (81), Lethal Lava Land's no-PMF trio. The bullies'
   factories leave the vptr on the BASE table (VT1), so the registry rows
   carry port_factory_* wrappers that reseat onto the host arrays -- the
   Thwomp treatment, here mandatory. Fills hal/actor_classes_ov064.cpp,
   closure slice_gate177.txt. */
extern unsigned char Bully_SpawnInfo[];            /* ov064 0x0211b818, +4 reads 215 */
extern unsigned char BigBully_SpawnInfo[];         /* ov064 0x0211b920, +4 reads 216 */
extern unsigned char RotatingFirebar_SpawnInfo[];  /* ov064 0x0211bdec, +4 reads 81  */
void *port_factory_bully(void);            /* wrapper: Bully_Spawn + reseat vptr */
void *port_factory_big_bully(void);        /* wrapper: BigBully_Spawn + reseat   */
void *RotatingFirebar_Spawn(void);         /* installs _ZTV15RotatingFirebar itself */
void hal_fill_bully_vtable(void);
void hal_fill_big_bully_vtable(void);
void hal_fill_rotating_firebar_vtable(void);

/* ---- gate 178: ov064's second mount -- METAL_NET_LIFT (69, daObjFl_Amilift_c)
   and LAVA_BUBBLE (214). id 69's factory installs the Amilift Platform table by
   its RTTI name; both factories leave the REAL table (no reseat wrapper). Fills
   hal/actor_classes_ov064_gate178.cpp, closure slice_gate178.txt. */
extern unsigned char MetalNetLift_SpawnInfo[];  /* ov064 0x0211bc44, +4 reads 69  */
extern unsigned char LavaBubble_SpawnInfo[];    /* ov064 0x0211bec8, +4 reads 214 */
void *MetalNetLift_Spawn(void);   /* installs _ZTV17daObjFl_Amilift_c itself */
void *LavaBubble_Spawn(void);     /* installs _ZTV10LavaBubble itself */
void hal_fill_metal_net_lift_vtable(void);
void hal_fill_lava_bubble_vtable(void);

/* ---- gate 179: ov064's BowserPuzzle pair (Lethal Lava Land). The config
   names cross the classes: id 78's factory installs _ZTV19BowserPuzzleManager
   (the 828-byte piece class), id 79 is a 216-byte coordinator shell on the
   mounted table data_ov064_0211c1d8. Both factories install their REAL table;
   no reseat wrappers. */
extern unsigned char BowserPuzzlePiece_SpawnInfo[];    /* ov064 0x0211c160, +4 reads 78 */
extern unsigned char BowserPuzzleManager_SpawnInfo[];  /* ov064 0x0211c17c, +4 reads 79 */
void *BowserPuzzlePiece_Spawn(void);     /* installs _ZTV19BowserPuzzleManager */
void *port_factory_bowser_puzzle_shell(void); /* Manager_Spawn + reseat onto the host shell table */
void hal_fill_bowser_puzzle_pair_vtables(void);
void hal_fill_bowser_puzzle_shell_vtable(void);

/* ---- gate 182: ov062's KOOPA (203) and CHUCKYA (190), Bob-omb Battlefield's
   last two NPCs. ov062 already mounted (gate 32); HAL-only gate. Both
   factories install their OWN real table; no reseat wrappers. KOOPA's closure
   reaches one unmatched particle helper (func_ov062_02117724) -- stubbed
   name-and-abort in unmatched/KoopaChuckya_StateDispatch.cpp. */
extern unsigned char Chuckya_SpawnInfo[];   /* ov062 0x0211d9bc, +4 reads 190 */
extern unsigned char Koopa_SpawnInfo[];     /* ov062 0x0211da74, +4 reads 203 */
void *Chuckya_Spawn(void);   /* installs _ZTV7Chuckya itself */
void *Koopa_Spawn(void);     /* installs _ZTV5Koopa itself */
void hal_fill_chuckya_vtable(void);
void hal_fill_koopa_vtable(void);

/* ---- gate 188: ov016's six MOVER classes (Jolly Roger Bay). Per-symbol mount
   ov016_syms.txt; fills hal/actor_classes_jrb.cpp; closure slice_gate188.txt.
   Four host vtables (Unagi 31-slot Enemy, ShipUp 32 + RockPillar 32 + FloatOn-
   Water 37, all Platform). ShipDown SHARES _ZTV6ShipUp and ShipUp_Spawn's body;
   SlidingBox SHARES _ZTV23FloatOnWaterPlatformJrb and installs it directly.
   FloatOnWaterPlatformJrb_Spawn's LAST vptr write is VT1 (the shared placeholder),
   so it takes a reseat wrapper (the bully treatment); the other five factories
   install their real table (or a shared real table) and need none.
   THE CONFIG CLASS NAMES ARE A DECOY (the gate-178 Amilift pattern), read from
   ov016's relocs: id 60 (FloatOnWaterPlatformJrb) installs data_ov016_02114bcc,
   a 32-slot daObjKi_Ita_c BASE table with arm9 Platform defaults for
   Behavior/Cleanup/Render and func_ov016_02112fa8/ef4/f44 for Init/D1/D0 -- so
   it is the SIMPLE base with no slot-5 render and no coupling. id 313
   (SlidingBox) installs _ZTV23FloatOnWaterPlatformJrb, the 37-slot DERIVED table
   whose real _ZN23FloatOnWaterPlatformJrb* methods carry BOTH the slot-5 Render
   collision AND the COUPLED Behavior (case 0 does FindWithActorID(0x39=SHIP_UP)
   and self-destructs if the ship is absent), so id 313 only survives with
   SHIP_UP hosted -- which this gate does. Both factories install their real
   table as the last vptr write; neither needs a reseat wrapper. */
extern unsigned char Unagi_SpawnInfo[];                    /* ov016 0x021148e0, +4 reads 242 */
extern unsigned char ShipDown_SpawnInfo[];                 /* ov016 0x021149fc, +4 reads 56  */
extern unsigned char ShipUp_SpawnInfo[];                   /* ov016 0x02114a18, +4 reads 57  */
extern unsigned char RockPillar_SpawnInfo[];               /* ov016 0x02114adc, +4 reads 58  */
extern unsigned char FloatOnWaterPlatformJrb_SpawnInfo[];  /* ov016 0x02114ba8, +4 reads 60  */
extern unsigned char SlidingBox_SpawnInfo[];               /* ov016 0x02114c68, +4 reads 313 */
void *Unagi_Spawn(void);                    /* installs _ZTV5Unagi itself */
void *ShipUp_Spawn(void);                   /* installs _ZTV6ShipUp (shared by ShipDown) */
void *ShipDown_Spawn(void);                 /* same body as ShipUp_Spawn */
void *RockPillar_Spawn(void);               /* installs data_ov016_02114b00 (RockPillar table) */
void *port_factory_float_on_water_jrb(void);/* id 60: FloatOnWaterPlatformJrb_Spawn writes the VT1 placeholder as its last
                                               vptr write, so this wrapper reseats p[0] onto the host data_ov016_02114bcc table */
void *SlidingBox_Spawn(void);               /* id 313: installs _ZTV23FloatOnWaterPlatformJrb (derived) directly */
void hal_fill_unagi_vtable(void);
void hal_fill_ship_up_vtable(void);
void hal_fill_rock_pillar_vtable(void);
void hal_fill_float_on_water_jrb_vtable(void);   /* fills data_ov016_02114bcc for id 60 */
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
extern unsigned char FireSeaElevator_SpawnInfo[];    /* ov045 0x02112cd0, +4 reads 140 */
void *FireSeaElevator_Spawn(void);                   /* ov045 0x021114dc */
extern unsigned char PoleLift_SpawnInfo[];           /* ov045 0x02112d98, +4 reads 142 */
void *PoleLift_Spawn(void);                          /* ov045 0x02111808 */
extern unsigned char ExtendingPlatform_SpawnInfo[];  /* ov045 0x02112e5c, +4 reads 148 */
void *ExtendingPlatform_Spawn(void);                 /* ov045 0x02111ad4 */
extern unsigned char FloatingFloorBfs_SpawnInfo[];   /* ov045 0x02112f2c, +4 reads 145 */
void *port_factory_floating_floor_bfs(void);         /* VT1 wrapper over ov045 0x02111bf4 */
extern unsigned char FallBlockBfs_SpawnInfo[];       /* ov045 0x021130d0, +4 reads 139 */
void *port_factory_fall_block_bfs(void);             /* VT1 wrapper over ov045 0x02111e24 */
/* wave 5 (lane w5-c): the sixth class, released by the hosted sqrt unit */
extern unsigned char TiltingPlatformBfs_SpawnInfo[]; /* ov045 0x02112ffc, +4 reads 141 */
void *port_factory_tilting_platform_bfs(void);       /* VT1 wrapper over ov045 0x02111d0c */
void hal_fill_fire_sea_elevator_vtable(void);        /* fills data_ov045_02112cf4, 32 slots */
void hal_fill_pole_lift_vtable(void);                /* fills _ZTV15FireSeaElevator, 32 */
void hal_fill_extending_platform_vtable(void);       /* fills _ZTV8PoleLift, 31 */
void hal_fill_floating_floor_bfs_vtable(void);       /* fills data_ov045_02112f50, 32 */
void hal_fill_fall_block_bfs_vtable(void);           /* fills _ZTV12FallBlockBfs, 32 */
void hal_fill_tilting_platform_bfs_vtable(void);     /* fills _ZTV16FloatingFloorBfs, 32 */

/* ---- run linkw wave 5 (lane w5-e): the Bowser fight arena's floor ----
   ov060 joins the per-symbol mounts (port/ov060_syms.txt, which carries the
   whole seven-class cast map, the SpikeBomb/SkyPlatform dsd label shift and
   the width pins). ONE class registers this wave: BOWSER_FIRE_SEA_ARENA
   (166, daKpa2Bg_c), level 38's walkable floor. The other six are measured
   and banked -- Bowser's two raw state-pair tables and SkyPlatform's bss
   Entry dispatch need StateDispatch seats first, and Bowser without
   BowserTail is a null child spawn (hal/actor_classes_ov060.cpp's header). */
extern unsigned char BowserFireSeaArena_SpawnInfo[]; /* ov060 0x0211a88c, +4 reads 166 */
void *BowserFireSeaArena_Spawn(void);                /* ov060 0x02117cdc */
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
extern unsigned char SpikeBomb_SpawnInfo[];          /* ov060 0x0211aa68, +4 reads 284 */
void *SpikeBomb_Spawn(void);                         /* ov060 0x02118cbc */
extern unsigned char Bowser_SpawnInfo[];             /* ov060 0x0211a5f4, +4 reads 279 */
void *Bowser_Spawn(void);                            /* ov060 0x02116428 */
extern unsigned char BowserTail_SpawnInfo[];         /* ov060 0x0211a610, +4 reads 278 */
void *BowserTail_Spawn(void);                        /* ov060 0x021163f0 */
extern unsigned char BowserFire_SpawnInfo[];         /* ov060 0x0211a7d0, +4 reads 280 */
void *BowserFire_Spawn(void);                        /* ov060 0x02117938 */
extern unsigned char BowserSkyPlatform_SpawnInfo[];  /* ov060 0x0211a964, +4 reads 167 */
void *BowserSkyPlatform_Spawn(void);                 /* ov060 0x02118408 */
extern unsigned char Key_SpawnInfo[];                /* ov089 0x02132b68, +4 reads 282 */
void *Key_Spawn(void);                               /* ov089 0x02132828 */
extern unsigned char LastStar_SpawnInfo[];           /* ov089 0x02132b84, +4 reads 283 */
void *LastStar_Spawn(void);                          /* ov089 0x021327d0 */
void hal_fill_spike_bomb_vtable(void);          /* fills _ZTV17BowserSkyPlatform, 31 */
void hal_fill_bowser_vtable(void);              /* fills _ZTV6Bowser, 31 */
void hal_fill_bowser_tail_vtable(void);         /* fills _ZTV10BowserTail, 31 */
void hal_fill_bowser_fire_vtable(void);         /* fills _ZTV10BowserFire, 31 */
void hal_fill_bowser_sky_platform_vtable(void); /* fills data_ov060_0211a9b0, 32 */
void hal_fill_key_vtable(void);                 /* fills _ZTV3Key, 31 (both ids) */
/* ---- run linkw wave 5 (lane w5-c): ov070's enemy pack + the two engine-side
   census finds for level 37. SpawnInfo records from the fresh per-symbol
   ov070 mount (port/ov070_syms.txt; ids re-read from the raw overlay image,
   NO naming shift in this overlay -- see hal/actor_classes_ov070.cpp) and,
   for 196, from the existing ov002 mount. All four factories store their own
   host table by real name, so all four register directly; every fill is in
   hal/actor_classes_ov070.cpp. FlyGuy (232) stays OFF: no bootable level
   spawns it. */
extern unsigned char Amp_SpawnInfo[];                /* ov070 0x02123254, +4 reads 266 */
void *Amp_Spawn(void);                               /* ov070 0x021210ac */
extern unsigned char FlameChomp_SpawnInfo[];         /* ov070 0x0212334c, +4 reads 270 */
void *FlameChomp_Spawn(void);                        /* ov070 0x02121af8 */
extern unsigned char FlameChompFire_SpawnInfo[];     /* ov070 0x02123424, +4 reads 271 */
void *FlameChompFire_Spawn(void);                    /* ov070 0x021221fc */
extern unsigned char PoppingLavaBubbles_SpawnInfo[]; /* ov002 0x021093bc, +4 reads 196 */
void *PoppingLavaBubbles_Spawn(void);                /* ov002 0x020b6dd8 */
void hal_fill_amp_vtable(void);                      /* fills _ZTV3Amp, 31 */
void hal_fill_flame_chomp_vtable(void);              /* fills _ZTV10FlameChomp, 31 */
void hal_fill_flame_chomp_fire_vtable(void);         /* fills _ZTV14FlameChompFire, 31 */
void hal_fill_popping_lava_bubbles_vtable(void);     /* fills data_ov002_021093e0, 31 */
/* ---- run linkw wave 5 (lane w5-B): Hazy Maze Cave's cast ----
   ov065's HMC subset + ov021's resident trio, both freshly per-symbol
   mounted (port/ov065_syms.txt, port/ov021_syms.txt). THE CAST MAP WAS
   RE-DERIVED, five reads per class, and the names are NOT shifted in either
   overlay -- the full derivation and every table width is in
   hal/actor_classes_ov065.cpp. Six factories store their own table by a
   real name and register directly; ShutterHmc_Spawn is the ov045 factory-F
   shape (base first, own second, own spelled VT1) and gets the wrapper. */
extern unsigned char Snufit_SpawnInfo[];        /* ov065 0x0211cb80, +4 reads 236 */
void *Snufit_Spawn(void);                       /* ov065 0x02116f48 */
extern unsigned char Swoop_SpawnInfo[];         /* ov065 0x0211cc7c, +4 reads 237 */
void *Swoop_Spawn(void);                        /* ov065 0x02117ee8 */
extern unsigned char Dorrie_SpawnInfo[];        /* ov065 0x0211cd84, +4 reads 168 */
void *Dorrie_Spawn(void);                       /* ov065 0x02119634 */
extern unsigned char DorrieCap_SpawnInfo[];     /* ov065 0x0211cda0, +4 reads 169 */
void *DorrieCap_Spawn(void);                    /* ov065 0x021195ec */
extern unsigned char WorkElevator_SpawnInfo[];  /* ov021 0x02114768, +4 reads 68 */
void *WorkElevator_Spawn(void);                 /* ov021 0x02112168 */
extern unsigned char RollingRock_SpawnInfo[];   /* ov021 0x02114824, +4 reads 221 */
void *RollingRock_Spawn(void);                  /* ov021 0x02112d64 */
extern unsigned char ShutterHmc_SpawnInfo[];    /* ov021 0x021148f0, +4 reads 67 */
void *port_factory_shutter_hmc(void);           /* VT1 wrapper over ov021 0x02112ed4 */
void hal_fill_snufit_vtable(void);              /* fills _ZTV6Snufit, 31 */
void hal_fill_swoop_vtable(void);               /* fills _ZTV5Swoop, 31 */
void hal_fill_dorrie_vtable(void);              /* fills _ZTV6Dorrie, 31 */
void hal_fill_dorrie_cap_vtable(void);          /* fills _ZTV9DorrieCap, 31+2 */
void hal_fill_work_elevator_vtable(void);       /* fills _ZTV12WorkElevator, 32 */
void hal_fill_rolling_rock_vtable(void);        /* fills _ZTV11RollingRock, 31 */
void hal_fill_shutter_hmc_vtable(void);         /* fills _ZTV10ShutterHmc, 32 */
/* ---- run linkw wave 5 (lane w5-A): ov063, Big Boo's Haunt's own cast ----
   Ten ids, six tables, first ov063 mount (port/ov063_syms.txt). The id map
   was re-derived from the raw image + the arm9 spawn table and this config
   has NO ov045-style naming shift -- every SpawnInfo's word[0] is its own
   name's Spawn and its +4 halfword the id (derivation in
   hal/actor_classes_ov063.cpp). BigBoo shares Boo's table (its own matched
   Spawn stores &_ZTV3Boo); the four furniture ids share MansionSteps'
   (one daTrsTrap_c class, four records, mode off the id at +0xc). Only
   FallBlockBbh needs a factory wrapper: its Spawn's last vptr write is the
   shared-header VT1 placeholder, the port_factory_fall_block_bfs shape.
   MansionSteps' InitResources SPAWNS ids 65 and 213 at runtime and writes
   through the results unchecked, so 65's own row and BOOK_SWITCH 213 (ov020,
   the straggler block) must stay registered with 158. */
extern unsigned char Boo_SpawnInfo[];            /* ov063 0x0211e770, +4 reads 209 */
void *Boo_Spawn(void);                           /* ov063 0x0211c590 */
extern unsigned char BigBoo_SpawnInfo[];         /* ov063 0x0211e78c, +4 reads 210 */
void *BigBoo_Spawn(void);                        /* ov063 0x0211c520 */
extern unsigned char BigBooIcon_SpawnInfo[];     /* ov063 0x0211e7c4, +4 reads 211 */
void *BigBooIcon_Spawn(void);                    /* ov063 0x0211c4a0 */
extern unsigned char BooCage_SpawnInfo[];        /* ov063 0x0211e7a8, +4 reads 212 */
void *BooCage_Spawn(void);                       /* ov063 0x0211c4d0 */
extern unsigned char MansionSteps_SpawnInfo[];   /* ov063 0x0211ea10, +4 reads 65 */
void *MansionSteps_Spawn(void);                  /* ov063 0x0211d360 */
extern unsigned char Bookshelf_SpawnInfo[];      /* ov063 0x0211ea2c, +4 reads 158 */
void *Bookshelf_Spawn(void);                     /* ov063 0x0211d320 */
extern unsigned char MerryGoRound_SpawnInfo[];   /* ov063 0x0211ea48, +4 reads 159 */
void *MerryGoRound_Spawn(void);                  /* ov063 0x0211d2e0 */
extern unsigned char TrapDoor_SpawnInfo[];       /* ov063 0x0211ea64, +4 reads 160 */
void *TrapDoor_Spawn(void);                      /* ov063 0x0211d2a0 */
extern unsigned char FallBlockBbh_SpawnInfo[];   /* ov063 0x0211eb34, +4 reads 66 */
void *port_factory_fall_block_bbh(void);         /* VT1 wrapper over ov063 0x0211d47c */
extern unsigned char MadPiano_SpawnInfo[];       /* ov063 0x0211ed10, +4 reads 249 */
void *MadPiano_Spawn(void);                      /* ov063 0x0211e128 */
void hal_fill_boo_vtable(void);                  /* fills _ZTV3Boo, 31 (209+210) */
void hal_fill_big_boo_icon_vtable(void);         /* fills _ZTV10BigBooIcon, 31 */
void hal_fill_boo_cage_vtable(void);             /* fills _ZTV7BooCage, 31 */
void hal_fill_mansion_steps_vtable(void);        /* fills _ZTV12MansionSteps, 31 (x4 ids) */
void hal_fill_fall_block_bbh_vtable(void);       /* fills _ZTV12FallBlockBbh, 32 */
void hal_fill_mad_piano_vtable(void);            /* fills _ZTV8MadPiano, 32 */
/* ---- w5-A stragglers: the ov020 BOOK CAST + UP_DOWN_LIFT_BBH (ov095) ----
   Four book ids on two tables (all four records config-`ambiguous`, each
   settled by its own bytes; the 213 record and factory are UNNAMED in
   config -- data_ov020_02114898 / func_ov020_021127f4). 213 is what
   Bookshelf's InitResources spawns 3x unchecked. UpDownLiftBbh (32) is the
   third id of gate 173's daUdlift_c trio and reuses its fill wholesale. */
extern unsigned char Bookend_SpawnInfo[];        /* ov020 0x02114860, +4 reads 327 */
void *Bookend_Spawn(void);                       /* ov020 0x02112880 */
extern unsigned char BookShot_SpawnInfo[];       /* ov020 0x021148b4, +4 reads 325 */
void *BookShot_Spawn(void);                      /* ov020 0x021128dc */
extern unsigned char data_ov020_02114898[];      /* BookSwitch record, +4 reads 213 */
void *func_ov020_021127f4(void);                 /* BookSwitch factory */
extern unsigned char BookShotSpawner_SpawnInfo[]; /* ov020 0x0211487c, +4 reads 328 */
void *BookShotSpawner_Spawn(void);               /* ov020 0x02112850 */
void hal_fill_book_shot_vtable(void);            /* fills _ZTV8BookShot, 31 (327/325/213) */
void hal_fill_book_shot_spawner_vtable(void);    /* fills _ZTV15BookShotSpawner, 31 */
extern unsigned char UpDownLiftBbh_SpawnInfo[];  /* ov095 0x021375cc, +4 reads 32 */
void *UpDownLiftBbh_Spawn(void);                 /* ov095 0x021367fc */
extern unsigned char CrazedCrate_SpawnInfo[];    /* ov080 0x02128174, +4 reads 193 */
void *CrazedCrate_Spawn(void);                   /* ov080 0x021253b4 */
void hal_fill_crazed_crate_vtable(void);         /* fills _ZTV11CrazedCrate, 31 */
extern unsigned char Coffin_SpawnInfo[];         /* ov071 0x02122eb0, +4 reads 64 */
void *Coffin_Spawn(void);                        /* ov071 0x02122670 */
void hal_fill_coffin_vtable(void);               /* fills _ZTV6Coffin, 32 */
/* ---- run linkw wave 6 (lane w6-f): FLAMETHROWER (318, ov095) ----
   daObjFlamethrower_c, the wall jet of fire. The one class on the board that
   skips on FOUR mounted levels at once (13 x3, 14 x5, 15 x3, 37 x3). Lane w5-C
   left it BLOCKED because Behavior has no matched TU; this gate hosts that one
   method as a per-instruction host copy
   (port/unmatched/Flamethrower_Behavior.c) and slices the other four
   (slice_w6f.txt). The fill is hal/actor_classes_flamethrower.cpp, 31 slots,
   with the reloc-pinned slot map in its banner. */
extern unsigned char Flamethrower_SpawnInfo[];   /* ov095 0x021376cc, +4 reads 318 */
void *Flamethrower_Spawn(void);                  /* ov095 0x02136ed4 */
void hal_fill_flamethrower_vtable(void);         /* fills _ZTV12Flamethrower, 31 */
/* ---- run linkw wave 6 (lane w6-D): PATH_LIFT (31), ov100's leaf over
   ov002's PathLiftActor_c. Both routes agree: arm9 spawn table
   0x02090864 + 31*4 = 0x020908e0 holds 0x02148558, and that record read RAW
   out of extracted/overlays/overlay_0100.bin is
   {0x02147328, 0x012b001f, ...} -- word[0] is PathLift_Spawn inside ov100
   and word[1]'s low half is 31. The full hierarchy, both 33-slot widths and
   the ov006 naming trap are in hal/actor_classes_ov100pl.cpp. ---- */
extern unsigned char PathLift_SpawnInfo[];       /* ov100 0x02148558, +4 reads 31 */
void *PathLift_Spawn(void);                      /* ov100 0x02147328 */
void hal_fill_path_lift_vtable(void);            /* fills data_ov100_0214857c, 33 */
/* ---- run linkw wave 7 (lane w7-c): ORANGE_BALL_BILLBOARD (301), the ONE
   class either new boss arena needed. Its overlay is the LEVEL overlay, not an
   actor overlay: arm9 spawn table 0x02090864 + 301*4 = 0x02090d18 holds
   0x021115e0, and that record read RAW out of extracted/overlays/
   overlay_0044.bin (ov044 is compressed:true, so the dsd export is the wrong
   image) is {0x021112dc, 0x00be012d, ...} -- word[0] is
   OrangeBallBillboard_Spawn inside ov044's own load window and word[1]'s low
   half is 301. RTTI at vtable[-1] names the class daObjKb1Billboard_c (kb1 =
   koopa boss 1); the width pin, the held-out D1 and the cross-overlay
   SharedFilePtr spelling are in hal/actor_classes_ov044.cpp. x16 on level 36,
   placed on no other level. ---- */
extern unsigned char OrangeBallBillboard_SpawnInfo[]; /* ov044 0x021115e0, +4 reads 301 */
void *OrangeBallBillboard_Spawn(void);                /* ov044 0x021112dc */
void hal_fill_orange_ball_billboard_vtable(void);     /* _ZTV19OrangeBallBillboard, 31 */
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
extern unsigned char VolcanoRing_SpawnInfo[];    /* ov022 0x02113cf4, +4 reads 71 */
void *VolcanoRing_Spawn(void);                   /* ov022 0x02111578 */
extern unsigned char FloatOnLavaPlatform_SpawnInfo[]; /* ov022 0x02113e88, +4 = 74 */
void *FloatOnLavaPlatform_Spawn(void);           /* ov022 0x02111950 */
extern unsigned char FloatingFloorLllBig_SpawnInfo[];   /* ov022 0x021140ec, +4 = 75 */
void *port_factory_floating_floor_lll_big(void);   /* VT1 wrapper, ov022 0x021120f4 */
extern unsigned char FloatingFloorLllSmall_SpawnInfo[]; /* ov022 0x02114108, +4 = 76 */
void *port_factory_floating_floor_lll_small(void); /* VT1 wrapper, ov022 0x021120b8 */
extern unsigned char LavaPlank_SpawnInfo[];      /* ov022 0x021141cc, +4 reads 82 */
void *LavaPlank_Spawn(void);                     /* ov022 0x02112350 */
void hal_fill_volcano_ring_vtable(void);         /* fills data_ov022_02113d18, 32 */
void hal_fill_float_on_lava_platform_vtable(void); /* _ZTV19RotatingPlatformLll, 32 */
void hal_fill_floating_floor_lll_vtable(void);   /* _ZTV21FloatingFloorLllSmall, 32
                                                    (75 + 76) */
void hal_fill_lava_plank_vtable(void);           /* _ZTV19FloatingFloorLllBig, 32 */
/* ---- run linkw wave 9 (lane w9-reg): TILTING_PLATFORM_LLL (72, ov064) ----
   ACTOR_SPAWN_TABLE 0x02090984 -> 0x0211bd08; the raw record there (out of
   extracted/overlays/overlay_0064.bin, NOT the compressed dsd export) has
   word[0] = TiltingPlatformLll_Spawn inside ov064 and its +4 halfword reads 72,
   so both attribution routes agree. Table 0x0211bd2c, 32 slots pinned twice
   (reloc run and next-symbol landing both end at 0x0211bdac); RTTI at
   vtable[-1] names it daObjFl_Gura_c, gura = the tilting one, the same root as
   ov045's TiltingPlatformBfs. x14, the biggest class in the wave-8 census.
   The factory column is a WRAPPER because the Spawn's final vptr store is the
   zeroed VT1 placeholder -- see hal/actor_classes_ov064_w9.cpp. ---- */
extern unsigned char TiltingPlatformLll_SpawnInfo[];  /* ov064 0x0211bd08, +4 reads 72 */
void *port_factory_tilting_platform_lll(void);        /* wraps ov064 0x021180fc */
void hal_fill_tilting_platform_lll_vtable(void);      /* _ZTV12MetalNetLift, 32 */
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
extern unsigned char GoombaLarge_SpawnInfo[];   /* ov084 0x02130924, +4 reads 202 */
void *GoombaLarge_Spawn(void);                  /* ov084 0x0212bff8, 1144 B, CapEnemy */
extern unsigned char FirePiranhaPlantBig_SpawnInfo[];   /* ov084 0x02130ae8, +4 = 251 */
void *FirePiranhaPlantBig_Spawn(void);                  /* 556 B, Enemy */
extern unsigned char FirePiranhaPlantSmall_SpawnInfo[]; /* ov084 0x02130b04, +4 = 252 */
void *FirePiranhaPlantSmall_Spawn(void);                /* 556 B, Enemy */
/* ---- run linkw wave 9 (lane w9-reg): three ov091 SLIDING PLATFORM siblings --
   SEVEN ids store the table 0x02135080 (55, 124, 138, 146, 147, 154, 155) and
   id 55 is already registered, so _ZTV17SlidingPlatformWf[32] in
   hal/actor_classes_l7.cpp is already filled before any of these spawn. All
   three SpawnInfo records are already mounted in port/ov091_syms.txt at 0x1c,
   and all three Spawns are the same 816-byte Platform shape as the registered
   SlidingPlatformWf_Spawn with a single real-name vptr store -- no wrapper. ---- */
extern unsigned char SlidingPlatformBdw_SpawnInfo[];    /* ov091 0x02134f60, +4 = 138 */
void *SlidingPlatformBdw_Spawn(void);                   /* ov091 0x02132908 */
extern unsigned char SlidingPlatformBsWide_SpawnInfo[]; /* ov091 0x02134f7c, +4 = 154 */
void *SlidingPlatformBsWide_Spawn(void);                /* ov091 0x021328d8 */
extern unsigned char SlidingPlatformBsLong_SpawnInfo[]; /* ov091 0x02134f98, +4 = 155 */
void *SlidingPlatformBsLong_Spawn(void);                /* ov091 0x021328a8 */
/* ---- run linkw wave 9 (lane w9-reg): three ov095 SEESAW siblings -- SEVEN ids
   store the table 0x021374fc (28, 39, 128, 133, 143, 149, 150) and id 39
   (SEESAW_BOB) is already registered, so _ZTV9SeesawBob[32] in
   hal/actor_classes_bob_world.cpp is already filled before these spawn. The
   three records join the mount at 0x1c, the stride port/ov095_syms.txt already
   measured for every record in the overlay. All three Spawns are the same
   808-byte Platform shape as SeesawBob_Spawn with a single real-name vptr
   store -- no wrapper. ---- */
extern unsigned char SeesawBdw_SpawnInfo[];        /* ov095 0x021373f8, +4 = 133 */
void *SeesawBdw_Spawn(void);                       /* ov095 0x02135f64 */
extern unsigned char SeesawBsPlank_SpawnInfo[];    /* ov095 0x02137430, +4 = 149 */
void *SeesawBsPlank_Spawn(void);                   /* ov095 0x02135f04 */
extern unsigned char SeesawBsZigZag_SpawnInfo[];   /* ov095 0x0213744c, +4 = 150 */
void *SeesawBsZigZag_Spawn(void);                  /* ov095 0x02135ed4 */
/* ---- run linkw wave 10 (lane w10-shared): ov031's SLIDE DECORATION QUAD.
   FOUR ids on ONE ROM table (0x02111984, _ZTV18daObjHsBillboard_c ==
   _ZTV25SlideDecorationSilverStar), x3 each on level 23. The class reads its
   variant out of its own actor id -- InitResources switches on the +0xc id
   halfword over 0x12e..0x131 and indexes one four-entry SharedFilePtr table --
   so ONE fill serves all four rows, and it is idempotent, so the order the
   registry installs them in does not matter. See hal/actor_classes_ov031_w10.cpp
   and port/slice_w10a.txt. ---- */
extern unsigned char SlideDecorationSilverStar_SpawnInfo[];   /* ov031 0x0211190c, +4 = 302 */
void *SlideDecorationSilverStar_Spawn(void);                  /* ov031 0x021113ec */
extern unsigned char SlideDecorationYellowStar_SpawnInfo[];   /* ov031 0x02111928, +4 = 303 */
void *SlideDecorationYellowStar_Spawn(void);                  /* ov031 0x021113b4 */
extern unsigned char SlideDecorationOrangeSmiley_SpawnInfo[]; /* ov031 0x02111944, +4 = 304 */
void *SlideDecorationOrangeSmiley_Spawn(void);                /* ov031 0x0211137c */
extern unsigned char SlideDecorationBlueSmiley_SpawnInfo[];   /* ov031 0x02111960, +4 = 305 */
void *SlideDecorationBlueSmiley_Spawn(void);                  /* ov031 0x02111344 */
void hal_fill_slide_decoration_vtable(void);     /* _ZTV18daObjHsBillboard_c, 31 */
/* ---- run linkw wave 10 (lane w10-shared): MEGA_MUSHROOM (277, ov002) x1 on
   level 22. Its factory stores _ZTV13OneUpMushroom -- the SAME ROM table
   0x021083c8 the already-registered ONE_UP_MUSHROOM (276) installs -- so it
   needs no fill of its own and the row below names 276's deliberately. One
   record in port/ov002_syms.txt, one TU in port/slice_w10b.txt, no vtable and
   no new mount. ---- */
extern unsigned char MegaMushroom_SpawnInfo[];   /* ov002 0x021083a4, +4 = 277 */
void *MegaMushroom_Spawn(void);                  /* ov002 0x020b0530 */
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
extern unsigned char RockTriangle_SpawnInfo[];       /* ov102 0x0214e0fc, +4 = 59 */
void *RockTriangle_Spawn(void);                      /* ov102 0x02148f5c, 48 B */
extern unsigned char CgStairs_SpawnInfo[];           /* ov102 0x0214e150, +4 = 340 */
void *CgStairs_Spawn(void);                          /* ov102 0x02148ecc, 48 B */
extern unsigned char RopeBarrier_SpawnInfo[];        /* ov102 0x0214e16c, +4 = 341 */
void *RopeBarrier_Spawn(void);                       /* ov102 0x02148e9c, 48 B */
/* ov102, table 0x0214e47c _ZTV13QuestionBlock, filled by id 20 */
extern unsigned char ExclamationBlockVs_SpawnInfo[]; /* ov102 0x0214e3cc, +4 = 22 */
void *ExclamationBlockVs_Spawn(void);                /* ov102 0x0214a92c, 64 B */
extern unsigned char CapBlockMario_SpawnInfo[];      /* ov102 0x0214e420, +4 = 23 */
void *CapBlockMario_Spawn(void);                     /* ov102 0x0214a86c, 64 B */
extern unsigned char CapBlockWario_SpawnInfo[];      /* ov102 0x0214e458, +4 = 24 */
void *CapBlockWario_Spawn(void);                     /* ov102 0x0214a7ec, 64 B */
extern unsigned char CapBlockLuigi_SpawnInfo[];      /* ov102 0x0214e43c, +4 = 25 */
void *CapBlockLuigi_Spawn(void);                     /* ov102 0x0214a82c, 64 B */
/* ov091, table 0x02135080 _ZTV17SlidingPlatformWf, filled by 55/138/154/155 */
extern unsigned char SlidingPlatformBfsSquare_SpawnInfo[];    /* ov091 0x02134fd0, +4 = 147 */
void *SlidingPlatformBfsSquare_Spawn(void);                   /* ov091 0x02132848 */
extern unsigned char SlidingPlatformBfsRectangle_SpawnInfo[]; /* ov091 0x02134fec, +4 = 146 */
void *SlidingPlatformBfsRectangle_Spawn(void);                /* ov091 0x02132818 */
extern unsigned char SlidingPlatformRr_SpawnInfo[];           /* ov091 0x02134fb4, +4 = 124 */
void *SlidingPlatformRr_Spawn(void);                          /* ov091 0x02132878 */
/* ov091, table 0x02134c5c _ZTV23daObjRotateUpdownLift_c, filled by id 29
   (the host array is spelled _ZTV25RotatingUpDownPlatformUtm and aliased) */
extern unsigned char RotatingUpDownPlatformUtm_SpawnInfo[];   /* ov091 0x02134c14, +4 = 30 */
void *RotatingUpDownPlatformUtm_Spawn(void);                  /* ov091 0x02131ba4, 56 B */
/* ov084 0x02130948 _ZTV6Goomba (200/202) and ov062 0x0211dab4 _ZTV5Koopa (203) */
extern unsigned char GoombaSmall_SpawnInfo[];        /* ov084 0x02130908, +4 = 201 */
void *GoombaSmall_Spawn(void);                       /* ov084 0x0212c054, 92 B */
extern unsigned char KoopaSmall_SpawnInfo[];         /* ov062 0x0211da90, +4 = 204 */
void *KoopaSmall_Spawn(void);                        /* ov062 0x021196bc, 80 B */
/* ov095, table 0x021374fc _ZTV9SeesawBob, filled by 39/133/149/150 */
extern unsigned char SeesawBfs_SpawnInfo[];          /* ov095 0x02137414, +4 = 143 */
void *SeesawBfs_Spawn(void);                         /* ov095 0x02135f34, 48 B */
extern unsigned char SeesawUtm_SpawnInfo[];          /* ov095 0x021373dc, +4 = 28 */
void *SeesawUtm_Spawn(void);                         /* ov095 0x02135fc4, 48 B */
extern unsigned char SeesawRr_SpawnInfo[];           /* ov095 0x02137468, +4 = 128 */
void *SeesawRr_Spawn(void);                          /* ov095 0x02135ea4, 48 B */
/* ov095, table 0x02137628 _ZTV13UpDownLiftBbh, filled by 32/33 */
extern unsigned char UpDownLiftRr_SpawnInfo[];       /* ov095 0x02137604, +4 = 131 */
void *UpDownLiftRr_Spawn(void);                      /* ov095 0x0213679c, 48 B */
/* ov002, 0x0210ab3c _ZTV8daStar_c (178) and 0x02108c18 _ZTV10BrickBlock (322) */
extern unsigned char SilverStar_SpawnInfo[];         /* ov002 0x0210aa5c, +4 = 179 */
void *SilverStar_Spawn(void);                        /* ov002 0x020ebed4, 92 B */
extern unsigned char SilverStarBlockTag_SpawnInfo[]; /* ov002 0x02108bd8, +4 = 324 */
void *SilverStarBlockTag_Spawn(void);                /* ov002 0x020b45e0, 48 B */
/* ---- run linkw wave 12 (lane w12): ov073, the whole cast of level 49 ----
   170/171/172 are ONE C++ class under three ids -- the three Spawns are the
   same 0x30-byte body and all three store the same table, 0x021231e8, so 171
   and 172 take the `0` fill column and row 170 carries the one fill (the
   RED_FLAME / BIG_MR_I idiom). CHIEF_CHILLY's factory is a host one: his
   matched Spawn is an ARM r0 ride-through onto Enemy::C2. Derivations in
   port/ov073_syms.txt and hal/actor_classes_ov073.cpp. */
extern unsigned char CccArena_SpawnInfo[];           /* ov073 0x02123168, +4 = 170 */
void *CccArena_Spawn(void);                          /* ov073 0x02122844, 48 B */
extern unsigned char CccBigIce_SpawnInfo[];          /* ov073 0x02123184, +4 = 171 */
void *CccBigIce_Spawn(void);                         /* ov073 0x02122814, 48 B */
extern unsigned char CccSmallIce_SpawnInfo[];        /* ov073 0x021231a0, +4 = 172 */
void *CccSmallIce_Spawn(void);                       /* ov073 0x021227e4, 48 B */
void hal_fill_ccc_arena_vtable(void);
extern unsigned char ChiefChilly_SpawnInfo[];        /* ov073 0x0212306c, +4 = 218 */
void *port_factory_chief_chilly(void);               /* ov073 0x02121ec8, 200 B */
void hal_fill_chief_chilly_vtable(void);
/* ---- run linkw wave 12 (lane w12): ov052's two platform classes, level 44 ----
   Both Spawns name their receiver and store a real table name, so both
   register directly. Derivations in port/ov052_syms.txt and
   hal/actor_classes_ov052.cpp; id 174's dsd block is unnamed, so its record
   and factory carry address names. */
extern unsigned char data_ov052_021124fc[];          /* ov052 0x021124fc, +4 = 174 */
void *func_ov052_02111410(void);                     /* ov052 0x02111410, 48 B */
void hal_fill_bobbing_log_emm_vtable(void);
extern unsigned char SquarePathLift_SpawnInfo[];     /* ov052 0x021125cc, +4 = 175 */
void *SquarePathLift_Spawn(void);                    /* ov052 0x02111830, 56 B */
void hal_fill_square_path_lift_vtable(void);
/* ---- run linkw wave 12 (lane w12): CLAM (315), ov064 ----
   Nine instances, five on level 8 and four on level 18. Its Spawn names its
   receiver and stores _ZTV4Clam by a real name, so it registers directly.
   TREASURE_CHEST (13), the sibling this wave's ranking put beside it, is
   BLOCKED on an unmatched state body -- see hal/actor_classes_ov064_w12.cpp. */
extern unsigned char Clam_SpawnInfo[];               /* ov064 0x0211c5a4, +4 = 315 */
void *Clam_Spawn(void);                              /* ov064 0x0211ad70, 64 B */
void hal_fill_clam_vtable(void);
/* ---- run linkw wave 15 (lane w15): BIG_MOVING_ICE_BLOCK (173), ov056 ----
   Nine instances on level 48 ("Wario's key course"), which is nine of that
   level's fourteen skipped instances; the other five are MR_BLIZZARD, refused
   on purpose. Its Spawn names its receiver and stores _ZTV17BigMovingIceBlock
   by a real name as its final write, so it registers directly with no factory
   wrapper. See hal/actor_classes_ov056.cpp and port/ov056_syms.txt. */
extern unsigned char BigMovingIceBlock_SpawnInfo[];  /* ov056 0x02113344, +4 = 173 */
void *BigMovingIceBlock_Spawn(void);                 /* ov056 0x02111594, 56 B */
void hal_fill_big_moving_ice_block_vtable(void);
/* ---- run linkw wave 15 (lane w15): the MEGA MUSHROOM TAG pair, ov002 ----
   ONE ROM class under TWO ids on ONE table (0x02108cf4), one instance of each
   on levels 22 and 24. The class branches on its own actor id, so both rows
   share the closure AND the fill; the fill is stores only and safe to call
   twice. See hal/actor_classes_ov002_w15.cpp and port/slice_w15b.txt. */
extern unsigned char MegaMushroomCreateTag_SpawnInfo[]; /* ov002 0x02108cd0, +4 = 319 */
void *MegaMushroomCreateTag_Spawn(void);                /* ov002 0x020b4a00, 56 B */
extern unsigned char MegaMushroomTag_SpawnInfo[];       /* ov002 0x02108cb4, +4 = 320 */
void *MegaMushroomTag_Spawn(void);                      /* ov002 0x020b4a38, 56 B */
void hal_fill_mega_mushroom_tag_vtable(void);
/* ---- run linkw wave 15 (lane w15): ov025's two pyramid classes ----
   Five of level 17's eleven skipped instances. Both Spawns name their receiver
   and store their table by a real name as the final write, so both register
   directly with no factory wrapper. See hal/actor_classes_ov025.cpp and
   port/ov025_syms.txt. */
extern unsigned char PyramidStep_SpawnInfo[];        /* ov025 0x021138f0, +4 = 87 */
void *PyramidStep_Spawn(void);                       /* ov025 0x021120ac, 56 B */
void hal_fill_pyramid_step_vtable(void);
extern unsigned char PyramidLift_SpawnInfo[];        /* ov025 0x021139b0, +4 = 88 */
void *PyramidLift_Spawn(void);                       /* ov025 0x021125f0, 100 B */
void hal_fill_pyramid_lift_vtable(void);
/* run linkw wave 16 (lane w16-unblock): FALL_BLOCK_LLL (83, ov022). The record
   and the file table were already mounted by lane w8-packs; what held the id
   back was the inferred-stub guard, and all three marker-carrying bodies ruled
   REAL DECOMP against the ROM. The factory is the port_factory_fall_block_bfs
   shape -- FallBlockLll_Spawn.c spells its own table VT1 and the intermediate
   base by the class name, so the wrapper forces the vptr afterward and neither
   spelling has to be right. See port/slice_w16a.txt. */
extern unsigned char FallBlockLll_SpawnInfo[];       /* ov022 0x021142a0, +4 = 83 */
void *port_factory_fall_block_lll(void);             /* VT1 wrapper over ov022 0x0211245c */
void hal_fill_fall_block_lll_vtable(void);
/* run linkw wave 17 (lane w17): SEAWEED (296, ov002), RTTI 13daObjWakame_c --
   x8 on level 18 and x4 on level 8, the largest single-id population left on
   the cast board. Its record needed one mount line (0x1c, NOT the 0x24 the dsd
   next-symbol delta reads -- the two extra words are the table's own Itanium
   prologue), its slot 17 needed a per-source -DVT0 because the shared VT0
   placeholder is a live 20-word dummy, and its slot 9 is the ModelAnim slot-5
   collision hosted in hal/actor_classes_ov002_w17.cpp. Seaweed_Spawn's last
   vptr write is the host array itself, so there is no factory wrapper. See
   port/slice_w17a.txt. Note dsd's _ZTV7Seaweed is a DIFFERENT class
   (12daObjHeart_c, HEALING_HEART) and is untouched by this row. */
extern unsigned char Seaweed_SpawnInfo[];            /* ov002 0x02109b94, +4 = 296 */
void *Seaweed_Spawn(void);                           /* ov002 0x020bc5a8, 56 B */
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
extern unsigned char WallSign_SpawnInfo[];           /* ov085 0x021303dc, +4 = 183 */
void *WallSign_Spawn(void);                          /* ov085 0x0212f244, 56 B */
void hal_fill_wall_sign_vtable(void);                /* _ZTV8WallSign, 32 */
extern unsigned char Toad_SpawnInfo[];               /* ov085 0x0212fe6c, +4 = 185 */
void *Toad_Spawn(void);                              /* ov085 0x02129cd0, 72 B */
void hal_fill_toad_vtable(void);                     /* _ZTV4Toad, 31 */
/* ---- run rel0215 lane cast-ov077: the ov077 SHARED ENEMY PACK, a fresh
   per-symbol mount (port/ov077_syms.txt). All three SpawnInfo records come
   from that mount; all three fills are in hal/actor_classes_ov077.cpp, and
   each fill calls port_ov077_states_seat() before it writes a slot. Every id
   below is confirmed by BOTH attribution routes (the arm9 spawn-table
   relocation AND the record's own +4 halfword) -- required here because ov077
   is the fourth occupant of the 0x02123740 window and one of the three
   relocations reads `module:overlays(77,80)`. */
extern unsigned char HeaveHo_SpawnInfo[];            /* ov077 0x02127a74, +4 = 238 */
void *HeaveHo_Spawn(void);                           /* ov077 0x021271d4, 92 B */
void hal_fill_heave_ho_vtable(void);                 /* _ZTV7HeaveHo, 31 */
extern unsigned char Spiny_SpawnInfo[];              /* ov077 0x02127960, +4 = 260 */
void *Spiny_Spawn(void);                             /* ov077 0x021261f4, 88 B */
void hal_fill_spiny_vtable(void);                    /* _ZTV5Spiny, 31 */
extern unsigned char Lakitu_SpawnInfo[];             /* ov077 0x02127848, +4 = 265 */
void *Lakitu_Spawn(void);                            /* ov077 0x02124b04, 96 B */
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
extern unsigned char RollingLogTtm_SpawnInfo[];      /* ov030 0x02115a24, +4 low = 102 */
void *RollingLogTtm_Spawn(void);                     /* ov030 0x0211164c, 60 B */
void hal_fill_rollinglogttm_vtable(void);            /* _ZTV15daObjHmMaruta_c, 32 */
extern unsigned char UkikiCage_SpawnInfo[];          /* ov030 0x02115950, +4 low = 103 */
void *UkikiCage_Spawn(void);                         /* ov030 0x02111524, 56 B */
void hal_fill_ukikicage_vtable(void);                /* _ZTV13daObjHmBskt_c, 32 */
extern unsigned char UkikiThief_SpawnInfo[];         /* ov030 0x02115b90, +4 low = 267 */
void *UkikiThief_Spawn(void);                        /* ov030 0x02114638, 88 B */
extern unsigned char UkikiStar_SpawnInfo[];          /* ov030 0x02115bac, +4 low = 268 */
void *UkikiStar_Spawn(void);                         /* ov030 0x021145e0, 88 B */
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
extern unsigned char data_ov026_02113abc[];          /* ov026 0x02113abc, +4 = 89  */
void *func_ov026_02111678(void);                     /* ov026 0x02111678, 80 B     */
void hal_fill_ov026_polelift_vtable(void);           /* _ZTV17daObjWlPolelift_c, 31 */
extern unsigned char Submarine_SpawnInfo[];          /* ov026 0x02113c48, +4 = 90  */
void *Submarine_Spawn(void);                         /* ov026 0x02111a70, 48 B     */
void hal_fill_ov026_submarine_vtable(void);          /* _ZTV18daObjWlSubmarine_c, 32 */
extern unsigned char BowserShutter_SpawnInfo[];      /* ov026 0x02113b80, +4 = 91  */
void *BowserShutter_Spawn(void);                     /* ov026 0x02111888, 48 B     */
void hal_fill_ov026_bowser_shutter_vtable(void);     /* _ZTV21daObjWlKoopaShutter_c, 32 */
extern unsigned char Whirlpool_SpawnInfo[];          /* ov026 0x02113d30, +4 = 246 */
void *Whirlpool_Spawn(void);                         /* ov026 0x021121bc, 64 B     */
void hal_fill_ov026_whirlpool_vtable(void);          /* _ZTV18daWater_Tatumaki_c, 31 */
extern unsigned char WaterSuction_SpawnInfo[];       /* ov026 0x02113e00, +4 = 247 */
void *WaterSuction_Spawn(void);                      /* ov026 0x02112450, 64 B     */
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
   id 121 has no dsd SpawnInfo name: 0x02112ac0 is ov010's Trap_SpawnInfo under
   the shared window, and ov035's own record there is reached by address. */
extern unsigned char RotatingClockHand_SpawnInfo[];  /* ov035 0x02112aa4, +4 = 119 */
void *RotatingClockHand_Spawn(void);                 /* ov035 0x021116bc, 48 B */
extern unsigned char SpinningPlatform_SpawnInfo[];   /* ov035 0x02112ba8, +4 = 120 */
void *SpinningPlatform_Spawn(void);                  /* ov035 0x02111b98, 56 B */
extern unsigned char data_ov035_02112ac0[];          /* ov035 0x02112ac0, +4 = 121 */
void *func_ov035_0211168c(void);                     /* ov035 0x0211168c, 48 B */
extern unsigned char RotatingCogSmall_SpawnInfo[];   /* ov035 0x02112adc, +4 = 122 */
void *RotatingCogSmall_Spawn(void);                  /* ov035 0x0211165c, 48 B */
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
extern unsigned char PyramidTop_SpawnInfo[];         /* ov024 0x02113804, +4 = 85 */
void *PyramidTop_Spawn(void);                        /* ov024 0x02111874, 56 B */
void hal_fill_pyramid_top_vtable(void);              /* _ZTV10PyramidTop, 32 */
extern unsigned char PyramidTag_SpawnInfo[];         /* ov024 0x02113820, +4 = 86 */
void *PyramidTag_Spawn(void);                        /* ov024 0x0211183c, 56 B */
void hal_fill_pyramid_tag_vtable(void);              /* _ZTV10PyramidTag, 31 */
/* run rel0215 wave 1 (lane cast-sweep1): OV033's OWN CAST, Tiny-Huge Island's
   small side -- two of level 25's three skipped instances. dsd names ONE
   vtable in this overlay and it is on the WRONG class: _ZTV9TinyCover
   (0x02112440, RTTI 14daObjTtWater_c) is TINY_WATER's, and TINY_COVER's own
   table is the unnamed data_ov033_0211237c (RTTI 13daObjTtFuta_c, futa = lid).
   The factory installs, the RTTI strings and the spawn records all three agree,
   so the rows below go by ADDRESS and every _ZN9TinyCover* body in src/ serves
   id 105. Both tables are 32-slot Platform; dsd's landings read 26 and THREE.
   Id 104 carries SEVEN own slots, not six -- slot 21 is a real OnGroundPounded
   override and pounding the lid is the class's whole purpose. Derivation in
   port/ov033_syms.txt, fills in hal/actor_classes_ov033.cpp, slice in
   port/slice_sweep1_ov033.txt. */
extern unsigned char TinyCover_SpawnInfo[];          /* ov033 0x02112358, +4 = 104 */
void *TinyCover_Spawn(void);                         /* ov033 0x021113a4, 48 B */
void hal_fill_tiny_cover_vtable(void);               /* data_ov033_0211237c, 32 */
extern unsigned char TinyWater_SpawnInfo[];          /* ov033 0x0211241c, +4 = 105 */
void *TinyWater_Spawn(void);                         /* ov033 0x02111690, 56 B */
void hal_fill_tiny_water_vtable(void);               /* _ZTV9TinyCover, 32 */
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
extern unsigned char Squasher_SpawnInfo[];           /* ov023 0x02111fc0, +4 = 84 */
void *Squasher_Spawn(void);                          /* ov023 0x02111728, 56 B */
void hal_fill_squasher_vtable(void);                 /* _ZTV8Squasher, 32 */
/* run rel0215 wave 1 (lane cast-sweep1): OV017's ONE CLASS, the sunken ship's
   water surface -- one of level 9's two skipped instances, and a blocker
   hal/level_boot.cpp's level-9 block wrote down and deferred. Its table is
   32-slot Platform and dsd's landing reads TEN. dsd's two names for it agree
   with the RTTI ("14daObjKsWater_c"), so unlike ov033 and ov035 there is no
   class-identity shift here. What there IS is three alias races in one
   function, one of them live on this tip: the collision SharedFilePtr is
   spelled &_ZN16FloatingFloorBfsD0Ev, an ov045 FUNCTION that is in the map
   today. All three are pinned to the ROM's own literal pool and routed by
   per-source -D. Derivation in port/ov017_syms.txt, fill in
   hal/actor_classes_ov017.cpp, slice in port/slice_sweep1_ov017.txt.
   NOTE its Behavior gates on TREASURE_CHEST (id 13, ov064, still unregistered):
   with no id-13 actor to find, the "all chests opened" test is vacuously true
   and the water drains from frame one. Visible, not fatal, and owned by the
   OTHER class's seat -- see the ov017 syms header. */
extern unsigned char ShipWater_SpawnInfo[];          /* ov017 0x02111bd4, +4 = 62 */
void *ShipWater_Spawn(void);                         /* ov017 0x02111480, 56 B */
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
extern unsigned char RecRoomCupboard_SpawnInfo[];    /* ov058 0x02111a4c, +4 = 182 */
void *RecRoomCupboard_Spawn(void);                   /* ov058 0x02111688, 92 B */
void hal_fill_rec_room_cupboard_vtable(void);        /* _ZTV15RecRoomCupboard, 31 */
}

#define PORT_ACTOR_CLASS_ROWS                                                 \
    {286, "TREE", Tree_SpawnInfo, Tree_Spawn, hal_fill_tree_vtable},          \
    {350, "AMBIENT_SOUND", AmbientSoundEffects_SpawnInfo,                     \
     AmbientSoundEffects_Spawn, hal_fill_ambient_sound_vtable},               \
    {17, "BLACK_BRICK_BLOCK", BlackBrickBlock_SpawnInfo,                      \
     BlackBrickBlock_Spawn, hal_fill_black_brick_block_vtable},               \
    /* ---- gate 78: BLUE_COIN_SWITCH (10), ov002 ---- */                      \
    {10, "BLUE_COIN_SWITCH", BlueCoinSwitch_SpawnInfo, BlueCoinSwitch_Spawn,   \
     hal_fill_blue_coin_switch_vtable},                                       \
    /* ---- gate 79: STAR_MARKER (180), ov002 ---- */                          \
    {180, "STAR_MARKER", StarMarker_SpawnInfo, StarMarker_Spawn,              \
     hal_fill_star_marker_vtable},                                            \
    {184, "SIGN_POST", SignPost_SpawnInfo, SignPost_Spawn,                    \
     hal_fill_sign_post_vtable},                                              \
    {276, "ONE_UP_MUSHROOM", OneUpMushroom_SpawnInfo, OneUpMushroom_Spawn,    \
     hal_fill_one_up_mushroom_vtable},                                        \
    {338, "CASTLE_WATER", CastleWater_SpawnInfo, CastleWater_Spawn,           \
     hal_fill_castle_water_vtable},                                           \
    {339, "METAL_NET", MetalNet_SpawnInfo, MetalNet_Spawn,                    \
     hal_fill_metal_net_vtable},                                              \
    {342, "FLAG", Flag_SpawnInfo, Flag_Spawn, hal_fill_flag_vtable},          \
    {343, "BIRD", Bird_SpawnInfo, Bird_Spawn, hal_fill_bird_vtable},          \
    {187, "RABBIT", Rabbit_SpawnInfo, Rabbit_Spawn, hal_fill_rabbit_vtable},  \
    {229, "RABBIT_KEY", RabbitKey_SpawnInfo, RabbitKey_Spawn,                 \
     hal_fill_rabbit_key_vtable},                                            \
    {235, "LAKITU_BRO", LakituBro_SpawnInfo, LakituBro_Spawn,                 \
     hal_fill_lakitu_bro_vtable},                                             \
    {207, "CANNON", Cannon_SpawnInfo, Cannon_Spawn, hal_fill_cannon_vtable}, \
    {349, "EXIT", Exit_SpawnInfo, Exit_Spawn, hal_fill_exit_vtable},         \
    {197, "WATERFALL_MIST", WaterfallMist_SpawnInfo, WaterfallMist_Spawn,    \
     hal_fill_waterfall_mist_vtable},                                        \
    {336, "BUTTERFLY", Butterfly_SpawnInfo, Butterfly_Spawn,                 \
     hal_fill_butterfly_vtable},                                             \
    {337, "UNCHAINED_CHOMP", UnchainedChomp_SpawnInfo, UnchainedChomp_Spawn, \
     hal_fill_unchained_chomp_vtable},                                       \
    {344, "FISH", Fish_SpawnInfo, Fish_Spawn, hal_fill_fish_vtable},             {353, "DOOR", Door_SpawnInfo, Door_Spawn, hal_fill_door_vtable},             {20, "QUESTION_BLOCK", QuestionBlock_SpawnInfo, QuestionBlock_Spawn,          hal_fill_question_block_vtable},         \
    {334, "HUD", _ZN3HUD9spawnDataE, port_factory_hud, hal_fill_hud_vtable},   \
    {335, "MINIMAP", _ZN7Minimap9spawnDataE, port_factory_minimap,             \
     hal_fill_minimap_vtable},                                                 \
    /* ---- gate 33: Bob-omb Battlefield, one contiguous block ---- */         \
    {288, "COIN", Coin_SpawnInfo, Coin_Spawn, hal_fill_coin_vtable},           \
    {289, "RED_COIN", RedCoin_SpawnInfo, RedCoin_Spawn, hal_fill_coin_vtable}, \
    {290, "BLUE_COIN", BlueCoin_SpawnInfo, BlueCoin_Spawn,                     \
     hal_fill_coin_vtable},                                                    \
    {15, "BRICK_BLOCK", BrickBlock_SpawnInfo, BrickBlock_Spawn, 0},            \
    /* gate 48: two more of the same class body (installs _ZTV13BigBrickBlock,  \
       filled at gate 16), for Whomp's Fortress. No fill of their own, like     \
       BRICK_BLOCK. */                                                          \
    {16, "BIG_BRICK_BLOCK", BigBrickBlock_SpawnInfo, BigBrickBlock_Spawn, 0},   \
    {46, "FORTRESS_TOWER_WALL", FortressTowerWall_SpawnInfo,                    \
     FortressTowerWall_Spawn, 0},                                              \
    {322, "MEGA_MUSHROOM_BLOCK_TAG", MegaMushroomBlockTag_SpawnInfo,           \
     MegaMushroomBlockTag_Spawn, hal_fill_mega_mushroom_block_tag_vtable},     \
    /* gate 49: same tag body (installs _ZTV10BrickBlock, filled above), for   \
       Whomp's Fortress. No fill of its own. */                               \
    {321, "ONE_UP_MUSHROOM_BLOCK_TAG", OneUpMushroomBlockTag_SpawnInfo,        \
     OneUpMushroomBlockTag_Spawn, 0},                                         \
    {14, "CANNON_HATCH", CannonHatch_SpawnInfo, CannonHatch_Spawn,             \
     hal_fill_cannon_hatch_vtable},                                            \
    {297, "HEALING_HEART", HealingHeart_SpawnInfo, HealingHeart_Spawn,      \
     hal_fill_healing_heart_vtable},                                        \
    /* CAP (269), EXCLAMATION_SWITCH (11) and ROLLING_IRON_BALL (220) are     \
       registered at gate 51 (their PMF state tables are now seated). WATER_  \
       BOMB (208) is registered at gate 55 -- see the gate-55 note in         \
       actor_classes_bob_world.cpp. */                                        \
    {208, "WATER_BOMB", WaterBomb_SpawnInfo, WaterBomb_Spawn,                 \
     hal_fill_water_bomb_vtable},                                             \
    {329, "INVISIBLE_SECRET", InvisibleSecret_SpawnInfo, InvisibleSecret_Spawn,\
     hal_fill_invisible_secret_vtable},                                        \
    /* gate 46: the moving bar (RTTI daBar_c), Whomp's Fortress */             \
    {287, "INVISIBLE_POLE", InvisiblePole_SpawnInfo, InvisiblePole_Spawn,      \
     hal_fill_invisible_pole_vtable},                                          \
    {299, "ARROW_SIGN_LEFT", ArrowSignLeft_SpawnInfo, ArrowSignLeft_Spawn,     \
     hal_fill_arrow_sign_vtable},                                              \
    {300, "ARROW_SIGN_RIGHT", ArrowSignRight_SpawnInfo, ArrowSignRight_Spawn,  \
     hal_fill_arrow_sign_vtable},                                              \
    {347, "WARP", Warp_SpawnInfo, Warp_Spawn, hal_fill_warp_vtable},           \
    {298, "WARP_PIPE", WarpPipe_SpawnInfo, WarpPipe_Spawn,                     \
     hal_fill_warp_pipe_vtable},                                           \
    /* ---- gate 32, Bob-omb Battlefield's cast (append-only block) ---- */    \
    {206, "BOB_OMB", BobOmb_SpawnInfo, BobOmb_Spawn, hal_fill_bob_omb_vtable},  \
    {200, "GOOMBA", Goomba_SpawnInfo, Goomba_Spawn, hal_fill_goomba_vtable},    \
    {181, "BOB_OMB_BUDDY", BobOmbBuddy_SpawnInfo, BobOmbBuddy_Spawn,            \
     hal_fill_bob_omb_buddy_vtable},                                           \
    {219, "CHAIN_CHOMP", ChainChomp_SpawnInfo, ChainChomp_Spawn,               \
     hal_fill_chain_chomp_vtable},                                             \
    {41, "CHAIN_CHOMP_FENCE", ChainChompFence_SpawnInfo, ChainChompFence_Spawn,\
     hal_fill_chain_chomp_fence_vtable},                                       \
    {27, "STUMP", Stump_SpawnInfo, Stump_Spawn, hal_fill_stump_vtable},        \
    {188, "KOOPA_THE_QUICK", KoopaTheQuick_SpawnInfo, KoopaTheQuick_Spawn,     \
     hal_fill_koopa_the_quick_vtable},                                         \
    {205, "KOOPA_FLAG", KoopaFlag_SpawnInfo, KoopaFlag_Spawn,                  \
     hal_fill_koopa_flag_vtable},                                              \
    {189, "KING_BOB_OMB", KingBobOmb_SpawnInfo, KingBobOmb_Spawn,              \
     hal_fill_king_bob_omb_vtable},                                            \
    /* ---- gates 40-44: levels 1 and 2 (append-only block) ---- */            \
    {354, "STAR_DOOR", StarDoor_SpawnInfo, StarDoor_Spawn,                     \
     hal_fill_star_door_vtable},                                              \
    {36, "TRAP", Trap_SpawnInfo, Trap_Spawn, hal_fill_trap_vtable},            \
    /* LIGHT_BEAM has no fill of its own: its factory installs _ZTV4Trap,     \
       which the TRAP row above already fills. */                            \
    {37, "LIGHT_BEAM", LightBeam_SpawnInfo, LightBeam_Spawn, 0},               \
    /* ---- gate 50: ov080's PAINTING (append-only block) ---- */              \
    {307, "PAINTING", Painting_SpawnInfo, Painting_Spawn,                      \
     hal_fill_painting_vtable},                                               \
    /* ---- gate 51: CAP (269), the metal/vanish/wing cap on the ground ---- */\
    {269, "CAP", Cap_SpawnInfo, Cap_Spawn, hal_fill_cap_vtable},               \
    /* ---- gate 51: EXCLAMATION_SWITCH (11), the floor switch ---- */         \
    {11, "EXCLAMATION_SWITCH", ExclamationSwitch_SpawnInfo,                    \
     ExclamationSwitch_Spawn, hal_fill_exclamation_switch_vtable},             \
    /* ---- gate 51: ROLLING_IRON_BALL (220), Whomp's Fortress ---- */         \
    {220, "ROLLING_IRON_BALL", RollingIronBall_SpawnInfo, RollingIronBall_Spawn,\
     hal_fill_rolling_iron_ball_vtable},                                       \
    /* ---- gate 60: TOWER_STEP (52), Whomp's Fortress (ov015) ---- */         \
    {52, "TOWER_STEP", TowerStep_SpawnInfo, TowerStep_Spawn,                    \
     hal_fill_tower_step_vtable},                                              \
    /* ---- gate 61: ROTATING_BRIDGE (51), Whomp's Fortress (ov015) ---- */    \
    {51, "ROTATING_BRIDGE", RotatingBridge_SpawnInfo, RotatingBridge_Spawn,    \
     hal_fill_rotating_bridge_vtable},                                         \
    /* ---- gate 62: POLE_BILLBOARD (42), KNOCK_DOWN_PLANK (44) ---- */         \
    {42, "POLE_BILLBOARD", PoleBillboard_SpawnInfo, PoleBillboard_Spawn,       \
     hal_fill_pole_billboard_vtable},                                          \
    {44, "KNOCK_DOWN_PLANK", KnockDownPlank_SpawnInfo, KnockDownPlank_Spawn,   \
     hal_fill_knock_down_plank_vtable},                                        \
    /* ---- gate 63: ROTATING_PLATFORM_WF (50), Whomp's Fortress (ov015) ---- */\
    {50, "ROTATING_PLATFORM_WF", RotatingPlatformWf_SpawnInfo,                 \
     port_factory_rotating_platform_wf, hal_fill_rotating_platform_wf_vtable}, \
    /* ---- gate 76: MOVING_BAR_BIG (53), MOVING_BAR_SMALL (54) ---- */         \
    /* Both install vtable 0x0211458c (the _ZN14KnockDownPlank* methods) and    \
       run KnockDownPlank's lifecycle; the shared fill is idempotent so the     \
       second row re-runs it harmlessly. */                                    \
    {53, "MOVING_BAR_BIG", MovingBarBig_SpawnInfo, MovingBarBig_Spawn,         \
     hal_fill_moving_bar_vtable},                                              \
    {54, "MOVING_BAR_SMALL", MovingBarSmall_SpawnInfo, MovingBarSmall_Spawn,   \
     hal_fill_moving_bar_vtable},                                              \
    /* ---- gate 77: FALL_BLOCK_WF (45), Whomp's Fortress (ov015/ov098) ---- */ \
    {45, "FALL_BLOCK_WF", FallBlockWf_SpawnInfo, FallBlockWf_Spawn,            \
     hal_fill_fall_block_wf_vtable},                                          \
    /* ---- gate 42: ov010's PEACH_PAINTING (38) ---- */                       \
    {38, "PEACH_PAINTING", PeachPainting_SpawnInfo, PeachPainting_Spawn,       \
     hal_fill_peach_painting_vtable},                                          \
    /* ---- gates 70-74: Whomp's Fortress' level-7 cast (append-only) ---- */   \
    /* gate 72: THWOMP (161), ov091 */                                         \
    {161, "THWOMP", Thwomp_SpawnInfo, port_factory_thwomp,                     \
     hal_fill_thwomp_vtable},                                                  \
    /* gate 74: SLIDING_PLATFORM_WF (55), ov091 */                             \
    {55, "SLIDING_PLATFORM_WF", SlidingPlatformWf_SpawnInfo,                   \
     SlidingPlatformWf_Spawn, hal_fill_sliding_platform_wf_vtable},           \
    /* gate 73: ROTATING_UP_DOWN_PLATFORM (29), ov091 (Utm name shift) */      \
    {29, "ROTATING_UP_DOWN_PLATFORM", RotatingUpDownPlatform_SpawnInfo,        \
     RotatingUpDownPlatform_Spawn, hal_fill_rotating_up_down_platform_vtable}, \
    /* gate 71: FIRE_PIRANHA_PLANT (253), ov084 (FirePiranhaPlantBig shift) */ \
    {253, "FIRE_PIRANHA_PLANT", FirePiranhaPlant_SpawnInfo,                    \
     FirePiranhaPlant_Spawn, hal_fill_fire_piranha_plant_vtable},             \
    /* gate 70: PIRANHA_PLANT (250), ov084 */                                  \
    {250, "PIRANHA_PLANT", PiranhaPlant_SpawnInfo, PiranhaPlant_Spawn,         \
     hal_fill_piranha_plant_vtable}, \
    /* ---- gate 82: SHUTTER_BOB (40), Bob-omb Battlefield (ov014) ---- */ \
    {40, "SHUTTER_BOB", ShutterBob_SpawnInfo, ShutterBob_Spawn, \
     hal_fill_shutter_bob_vtable}, \
    /* ---- gate 83: SEESAW_BOB (39), Bob-omb Battlefield (ov095) ---- */ \
    {39, "SEESAW_BOB", SeesawBob_SpawnInfo, SeesawBob_Spawn, \
     hal_fill_seesaw_bob_vtable}, \
    /* ---- gate 89: POWER_STAR (178), ov002 ---- */ \
    {178, "POWER_STAR", PowerStar_SpawnInfo, PowerStar_Spawn, \
     hal_fill_power_star_vtable}, \
    /* ---- gate 90: STAR_CAMERA (177), ov002 (bare Actor, base vtable) ---- */ \
    {177, "STAR_CAMERA", StarCamera_SpawnInfo, StarCamera_Spawn, \
     hal_fill_actor_base_vtable}, \
    /* ---- gates 64-69: Whomp's Fortress' enemy cast (ov079), now live ---- */ \
    {164, "WHOMP", Whomp_SpawnInfo, Whomp_Spawn, hal_fill_whomp_vtable_c}, \
    {165, "WHOMP_KING", WhompKing_SpawnInfo, WhompKing_Spawn, 0}, \
    {222, "BULLET_BILL", BulletBill_SpawnInfo, BulletBill_Spawn, \
     hal_fill_bullet_bill_vtable_c}, \
    {43, "BILL_BLASTER", BillBlaster_SpawnInfo, BillBlaster_Spawn, \
     hal_fill_bill_blaster_vtable_c}, \
    {47, "FORTRESS_WALL", FortressWall_SpawnInfo, FortressWall_Spawn, \
     hal_fill_fortress_wall_vtable_c}, \
    {48, "FORTRESS_WALL_BREAKABLE", FortressWallBreakable_SpawnInfo, \
     FortressWallBreakable_Spawn, 0}, \
    /* ---- gate 102: FORTRESS_TOWER (49), Whomp's Fortress (ov102) ---- */ \
    /* Installs _ZTV13FortressTower, filled by the WARP_PIPE row above; no \
       fill of its own, the BIG_BRICK_BLOCK / LIGHT_BEAM shape. */ \
    {49, "FORTRESS_TOWER", FortressTower_SpawnInfo, FortressTower_Spawn, 0}, \
    /* ---- gate 103: STAR_SWITCH (12), Whomp's Fortress (ov002) ---- */ \
    /* Installs _ZTV10StarSwitch, filled by the EXCLAMATION_SWITCH row (with \
       its PMF state seat); no fill of its own. */ \
    {12, "STAR_SWITCH", StarSwitch_SpawnInfo, StarSwitch_Spawn, 0}, \
    /* ---- gate 128: EXCLAMATION_BLOCK (21), Jolly Roger Bay ship (ov102) ---- */ \
    /* Installs _ZTV13QuestionBlock, filled by the QUESTION_BLOCK row above; no \
       fill of its own, the FORTRESS_TOWER / STAR_SWITCH shape. */ \
    {21, "EXCLAMATION_BLOCK", ExclamationBlock_SpawnInfo, ExclamationBlock_Spawn, 0}, \
    /* ---- gate 121: STATIC_ROCK (61), Jolly Roger Bay (ov102) ---- */ \
    /* Installs _ZTV13FortressTower, filled by the WARP_PIPE row above; no \
       fill of its own, the FORTRESS_TOWER shape. */ \
    {61, "STATIC_ROCK", StaticRock_SpawnInfo, StaticRock_Spawn, 0}, \
    /* ---- gate 122: BRICK_BLOCK_SWITCH_ACTIVATED (19), Jolly Roger Bay ---- */ \
    /* Installs _ZTV13BigBrickBlock, filled at gate 16; no fill of its own, the \
       BIG_BRICK_BLOCK shape. */ \
    {19, "BRICK_BLOCK_SWITCH_ACTIVATED", BrickBlockSwitchActivated_SpawnInfo, \
     BrickBlockSwitchActivated_Spawn, 0}, \
    /* ---- gate 123: GREEN_SHELL_BLOCK_TAG (323), Jolly Roger Bay ---- */ \
    /* Installs _ZTV10BrickBlock, filled at gate 49; no fill of its own, the \
       ONE_UP_MUSHROOM_BLOCK_TAG shape. */ \
    {323, "GREEN_SHELL_BLOCK_TAG", GreenShellBlockTag_SpawnInfo, \
     GreenShellBlockTag_Spawn, 0}, \
     /* ---- gate 143: IceSlideManager (356), Cool Cool Mountain slide (ov019) ---- */ \
     /* Level 11's own overlay class; SpawnInfo from the per-symbol ov019 mount, \
        vtable filled by hal/actor_classes_ccm.cpp. */ \
     {356, "ICE_SLIDE_MANAGER", IceSlideManager_SpawnInfo, IceSlideManager_Spawn, \
      hal_fill_ice_slide_manager_vtable}, \
     /* ---- gate 190: CCM slice A -- ICE_SHEET + POWER_STAR_CREATE (ov018), \
        ONE_UP_LOGO (ov002) ---- */ \
     {295, "ICE_SHEET", IceSheet_SpawnInfo, IceSheet_Spawn, \
      hal_fill_ice_sheet_vtable}, \
     {355, "POWER_STAR_CREATE", PowerStarCreate_SpawnInfo, PowerStarCreate_Spawn, \
      hal_fill_power_star_create_vtable}, \
     {331, "ONE_UP_LOGO", OneUpLogo_SpawnInfo, OneUpLogo_Spawn, \
      hal_fill_one_up_logo_vtable}, \
     {359, "SOUND_OBJECT", SoundObject_SpawnInfo, SoundObject_Spawn, \
      hal_fill_sound_object_vtable}, \
     /* ---- gate 191: SKI_LIFT (63) + MOTHER_PENGUIN (257), ov018 ---- */ \
     {63, "SKI_LIFT", SkiLift_SpawnInfo, SkiLift_Spawn, \
      hal_fill_ski_lift_vtable}, \
     {257, "MOTHER_PENGUIN", MotherPenguin_SpawnInfo, MotherPenguin_Spawn, \
      hal_fill_mother_penguin_vtable}, \
     /* ---- run lvled, lane intro-cutscene: the OPENING's whole cast ---- */ \
     /* CUTSCENE_OBJECT (0x160) is every visible thing in the new-file opening: \
        the Peach letter (kuppa cmd 0x17), the Lakitu flight (0x12), Peach \
        (0x13), the three warp pipes (0x14/0x15/0x16) and the cloud (0x19). \
        One class, spawned repeatedly by the script with a different model each \
        time. Fully matched and offered nowhere until port/slice_intro.txt, so \
        the opening used to log "[spawn-declined] actor 0x160 not registered" \
        and play to an empty stage. Seat in hal/actor_classes_intro.cpp. */ \
     {0x160, "CUTSCENE_OBJECT", CutsceneObject_SpawnInfo, CutsceneObject_Spawn, \
      hal_fill_cutscene_object_vtable}, \
     /* ---- gate 192: ICE_BLOCK (18) + SPINDRIFT (312), ov081 ---- */ \
     /* MR_BLIZZARD (223) is NOT registered here. The ORIGINAL exclusion
        reason (a func_0204488c "unvalidated raw-DMA source" fear) is
        RESOLVED -- the dma verdict established Part.fc/f8 are rebased at
        load and the synchronous forward is safe. Registration was then
        attempted (babypenguin-crash lane, 2026-08-10) and found a
        DIFFERENT, REAL blocker, measured not reasoned: a deterministic
        frame-0 corruption in MrBlizzard::Behavior's callee chain. The
        register MSVC keeps `this` in comes back from one of Behavior's
        callees skewed to this+0x150 (the mWithMeshClsn member), after
        which Behavior's tail stores (anim speed 0x1000 at +0x368) land
        0x4b8 past its own base -- on the NEXT MrBlizzard's render-list
        node next pointer -- and the render walk then follows 0x1000 and
        dies (or, per-actor caught, quarantines both instances and
        latches the class off). Proven by DR0 write-watch (writer EIP =
        the +0x368 store in ?Behavior@MrBlizzard, register capture
        esi=instance+0x150), an entry log showing the face only ever
        receives correct `this` (so the skew happens MID-body), and the
        bug SHAPE-SHIFTING when per-callee probes change codegen (the
        same binary then faults at self+0xc inside the behavior dispatch
        instead) -- a codegen-sensitive ABI/register-state violation in
        one of: the seated tick handler, Actor::UpdatePos,
        Enemy::UpdateWMClsn, func_ov081_021254d8, func_ov081_021243cc,
        CylinderClsn::Clear/Update. The instrument that settles it:
        single-step the callee list at the machine level (or a
        frame-pointer-preserving wrapper around each callee) watching the
        callee-saved register set across each boundary. Until that lands,
        223 stays out: it cannot meet the zero-quarantine bar. */ \
     {18, "ICE_BLOCK", IceBlock_SpawnInfo, IceBlock_Spawn, \
      hal_fill_ice_block_vtable}, \
     {312, "SPINDRIFT", Spindrift_SpawnInfo, Spindrift_Spawn, \
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
        header and the commit message. SnowmanHead (273) and SnowmanBody
        (274) stay unregistered -- see port/ov072_syms.txt for the
        class-identity derivation and the SnowmanHead spawn-then-orphan
        coupling. */ \
     {272, "SNOWMAN", data_ov072_02122954, func_ov072_02120c00, \
      hal_fill_snowman_vtable}, \
     {256, "BABY_PENGUIN", BabyPenguin_SpawnInfo, BabyPenguin_Spawn, \
      hal_fill_baby_penguin_vtable}, \
     /* ---- gate 194: HOOT_THE_OWL (234), ov094 ---- */ \
     {234, "HOOT_THE_OWL", HootTheOwl_SpawnInfo, HootTheOwl_Spawn, \
      hal_fill_hoot_the_owl_vtable}, \
     /* ---- gate 199: SWITCH_PILLAR (34) + BASEMENT_WATER (35), ov012 ---- */ \
     {34, "SWITCH_PILLAR", SwitchPillar_SpawnInfo, SwitchPillar_Spawn, \
      hal_fill_switch_pillar_vtable}, \
     {35, "BASEMENT_WATER", BasementWater_SpawnInfo, BasementWater_Spawn, \
      hal_fill_basement_water_vtable}, \
     /* ---- gate 200: PUSH_BLOCK (306, the "PowerFlower" decoy) + MUGEN_BGM
        (351), ov002 ---- */ \
     {306, "PUSH_BLOCK", PowerFlower_SpawnInfo, PowerFlower_Spawn, \
      hal_fill_pushblock_vtable}, \
     {351, "MUGEN_BGM", MugenBgm_SpawnInfo, MugenBgm_Spawn, \
      hal_fill_mugen_bgm_vtable}, \
     /* ---- gate 202: the 2F clock-painting furniture trio, ov013 ---- */ \
     {292, "CLOCK_HAND_SHORT", ClockPaintingHandShort_SpawnInfo, \
      ClockPaintingHandShort_Spawn, hal_fill_clock_hand_vtable}, \
     {293, "CLOCK_HAND_LONG", ClockPaintingHandLong_SpawnInfo, \
      ClockPaintingHandLong_Spawn, hal_fill_clock_hand_vtable}, \
     {294, "CLOCK_PENDULUM", ClockPaintingPendulum_SpawnInfo, \
      ClockPaintingPendulum_Spawn, hal_fill_clock_pendulum_vtable}, \
    /* ---- gate 149: HAUNTED_CHAIR (326), Big Boo's Haunt (ov020, level 12) ---- */ \
    {326, "HAUNTED_CHAIR", HauntedChair_SpawnInfo, HauntedChair_Spawn, \
     hal_fill_haunted_chair_vtable}, \
    /* ---- gate 172: CRATE (194), Hazy Maze Cave's first fresh class (ov098) ---- */ \
    {194, "CRATE", Crate_SpawnInfo, Crate_Spawn, hal_fill_crate_vtable}, \
    /* ---- gate 173: UP_DOWN_LIFT_HMC (33), Hazy Maze Cave (ov095) ---- */ \
    {33, "UP_DOWN_LIFT_HMC", UpDownLiftHmc_SpawnInfo, UpDownLiftHmc_Spawn, \
     hal_fill_up_down_lift_hmc_vtable}, \
    /* ---- gate 174: MONTY_MOLE (310), ov080's enemy cast ---- */ \
    {310, "MONTY_MOLE", MontyMole_SpawnInfo, MontyMole_Spawn, \
     hal_fill_monty_mole_vtable}, \
    /* ---- gate 208: MONTY_MOLE_ROCK (311), the rock the mole throws ---- */ \
    {311, "MONTY_MOLE_ROCK", MontyMoleRock_SpawnInfo, MontyMoleRock_Spawn, \
     hal_fill_monty_mole_rock_vtable}, \
    /* ---- gate 206: DIAMOND_LIFT (135), ov043, level 35's six lifts ---- */ \
    {135, "DIAMOND_LIFT", DiamondLift_SpawnInfo, DiamondLift_Spawn, \
     hal_fill_diamond_lift_vtable}, \
    /* ---- gate 175: the flames (ov002) -- BLUE_FLAME carries the fill, \
       RED_FLAME reuses it, the GreenShellBlockTag shape ---- */ \
    {317, "BLUE_FLAME", BlueFlame_SpawnInfo, BlueFlame_Spawn, \
     hal_fill_blue_flame_vtable}, \
    {316, "RED_FLAME", RedFlame_SpawnInfo, RedFlame_Spawn, 0}, \
    /* ---- gate 176: SCUTTLEBUG (255), ov071's first mount ---- */ \
    {255, "SCUTTLEBUG", Scuttlebug_SpawnInfo, Scuttlebug_Spawn, \
     hal_fill_scuttlebug_vtable}, \
    /* ---- run linkw wave 7, lane w7b: the MR. I family (ov071) -- MR_I \
       carries the fill, BIG_MR_I reuses it (one class, one vtable, the \
       RED_FLAME shape), MR_I_PROJECTILE has its own ---- */ \
    {262, "MR_I", MrI_SpawnInfo, MrI_Spawn, hal_fill_mri_vtable}, \
    {263, "BIG_MR_I", BigMrI_SpawnInfo, BigMrI_Spawn, 0}, \
    {264, "MR_I_PROJECTILE", MrI_Projectile_SpawnInfo, MrI_Projectile_Spawn, \
     hal_fill_mri_projectile_vtable}, \
    /* ---- gate 177: ov064's Bully family + firebar (Lethal Lava Land) ---- */ \
    {215, "BULLY", Bully_SpawnInfo, port_factory_bully, hal_fill_bully_vtable}, \
    {216, "BIG_BULLY", BigBully_SpawnInfo, port_factory_big_bully, \
     hal_fill_big_bully_vtable}, \
    {81, "ROTATING_FIREBAR", RotatingFirebar_SpawnInfo, RotatingFirebar_Spawn, \
     hal_fill_rotating_firebar_vtable},     /* ---- gate 178: ov064's METAL_NET_LIFT + LAVA_BUBBLE (Lethal Lava Land) ---- */     {69, "METAL_NET_LIFT", MetalNetLift_SpawnInfo, MetalNetLift_Spawn,      hal_fill_metal_net_lift_vtable},     {214, "LAVA_BUBBLE", LavaBubble_SpawnInfo, LavaBubble_Spawn,      hal_fill_lava_bubble_vtable},     /* ---- gate 179: ov064's BowserPuzzle pair (Lethal Lava Land) ---- */     {78, "BOWSER_PUZZLE_PIECE", BowserPuzzlePiece_SpawnInfo,      BowserPuzzlePiece_Spawn, hal_fill_bowser_puzzle_pair_vtables},     {79, "BOWSER_PUZZLE_MANAGER", BowserPuzzleManager_SpawnInfo,      port_factory_bowser_puzzle_shell, hal_fill_bowser_puzzle_shell_vtable},     /* ---- gate 182: ov062's KOOPA + CHUCKYA (Bob-omb Battlefield) ---- */     {190, "CHUCKYA", Chuckya_SpawnInfo, Chuckya_Spawn, hal_fill_chuckya_vtable}, \
    /* KOOPA (203) restored: his model fileptrs (data_ov062_0211cee0 -> ov002)
       now rebase through ovdata.py's per-target cross pass, so SF4 boots. */ \
    {203, "KOOPA", Koopa_SpawnInfo, Koopa_Spawn, hal_fill_koopa_vtable}, \
    /* ---- gate 188: ov016's six MOVER classes (Jolly Roger Bay) ---- */ \
    {242, "UNAGI", Unagi_SpawnInfo, Unagi_Spawn, hal_fill_unagi_vtable}, \
    {57, "SHIP_UP", ShipUp_SpawnInfo, ShipUp_Spawn, hal_fill_ship_up_vtable}, \
    {56, "SHIP_DOWN", ShipDown_SpawnInfo, ShipDown_Spawn, hal_fill_ship_up_vtable}, \
    {58, "ROCK_PILLAR", RockPillar_SpawnInfo, RockPillar_Spawn, \
     hal_fill_rock_pillar_vtable}, \
    {60, "FLOAT_ON_WATER_PLATFORM_JRB", FloatOnWaterPlatformJrb_SpawnInfo, \
     port_factory_float_on_water_jrb, hal_fill_float_on_water_jrb_vtable}, \
    {313, "SLIDING_BOX", SlidingBox_SpawnInfo, SlidingBox_Spawn, \
     hal_fill_sliding_box_vtable}, \
    /* ---- run linkw wave 4: ov045's platform cast, five of six (level 37);
       wave 5 (lane w5-c) released the sixth, 141, on the hosted sqrt unit --
       see hal/actor_classes_ov045.cpp ---- */ \
    {140, "FIRE_SEA_ELEVATOR", FireSeaElevator_SpawnInfo, \
     FireSeaElevator_Spawn, hal_fill_fire_sea_elevator_vtable}, \
    {142, "POLE_LIFT", PoleLift_SpawnInfo, PoleLift_Spawn, \
     hal_fill_pole_lift_vtable}, \
    {148, "EXTENDING_PLATFORM", ExtendingPlatform_SpawnInfo, \
     ExtendingPlatform_Spawn, hal_fill_extending_platform_vtable}, \
    {145, "FLOATING_FLOOR_BFS", FloatingFloorBfs_SpawnInfo, \
     port_factory_floating_floor_bfs, hal_fill_floating_floor_bfs_vtable}, \
    {139, "FALL_BLOCK_BFS", FallBlockBfs_SpawnInfo, \
     port_factory_fall_block_bfs, hal_fill_fall_block_bfs_vtable}, \
    {141, "TILTING_PLATFORM_BFS", TiltingPlatformBfs_SpawnInfo, \
     port_factory_tilting_platform_bfs, hal_fill_tilting_platform_bfs_vtable}, \
    /* ---- run linkw wave 5: the Bowser fight arena's floor (level 38);
       the other six ov060 classes are measured but not hosted, see
       hal/actor_classes_ov060.cpp ---- */ \
    {166, "BOWSER_FIRE_SEA_ARENA", BowserFireSeaArena_SpawnInfo, \
     BowserFireSeaArena_Spawn, hal_fill_bowser_fire_sea_arena_vtable}, \
    /* ---- run linkw wave 6 (lane w6-A): the rest of the Bowser fight.
       284 is the level's eight placed mines; 279 is the boss and 278/280 are
       the two children his init spawns with unchecked writes through the
       results; 167 is level 40's floor (not placed in 38); 282/283 are
       ov089's boss reward, which the arena already has resident through the
       selector row, and 282 is spawned-and-written-through on the defeat
       path. LAST_STAR rides KEY's fill -- ov089 defines one vtable and both
       factories store it (the BLUE_FLAME/RED_FLAME shape). ---- */ \
    {284, "SPIKE_BOMB", SpikeBomb_SpawnInfo, SpikeBomb_Spawn, \
     hal_fill_spike_bomb_vtable}, \
    {279, "BOWSER", Bowser_SpawnInfo, Bowser_Spawn, hal_fill_bowser_vtable}, \
    {278, "BOWSER_TAIL", BowserTail_SpawnInfo, BowserTail_Spawn, \
     hal_fill_bowser_tail_vtable}, \
    {280, "BOWSER_FIRE", BowserFire_SpawnInfo, BowserFire_Spawn, \
     hal_fill_bowser_fire_vtable}, \
    {167, "BOWSER_SKY_PLATFORM", BowserSkyPlatform_SpawnInfo, \
     BowserSkyPlatform_Spawn, hal_fill_bowser_sky_platform_vtable}, \
    {282, "KEY", Key_SpawnInfo, Key_Spawn, hal_fill_key_vtable}, \
    {283, "LAST_STAR", LastStar_SpawnInfo, LastStar_Spawn, 0}, \
    /* ---- run linkw wave 5 (lane w5-c): the rest of level 37's roaming cast.
       FLAME_CHOMP_FIRE is not level-placed; the chomp spawns it live. ---- */ \
    {266, "AMP", Amp_SpawnInfo, Amp_Spawn, hal_fill_amp_vtable}, \
    {270, "FLAME_CHOMP", FlameChomp_SpawnInfo, FlameChomp_Spawn, \
     hal_fill_flame_chomp_vtable}, \
    {271, "FLAME_CHOMP_FIRE", FlameChompFire_SpawnInfo, FlameChompFire_Spawn, \
     hal_fill_flame_chomp_fire_vtable}, \
    {196, "POPPING_LAVA_BUBBLES", PoppingLavaBubbles_SpawnInfo, \
     PoppingLavaBubbles_Spawn, hal_fill_popping_lava_bubbles_vtable}, \
    /* ---- run linkw wave 5 (lane w5-B): Hazy Maze Cave's cast (level 13);
       DORRIE_CAP is never in a level's object list -- Dorrie spawns id 169
       at runtime, and the registry row is what that spawn finds ---- */ \
    {236, "SNUFIT", Snufit_SpawnInfo, Snufit_Spawn, hal_fill_snufit_vtable}, \
    {237, "SWOOP", Swoop_SpawnInfo, Swoop_Spawn, hal_fill_swoop_vtable}, \
    {168, "DORRIE", Dorrie_SpawnInfo, Dorrie_Spawn, hal_fill_dorrie_vtable}, \
    {169, "DORRIE_CAP", DorrieCap_SpawnInfo, DorrieCap_Spawn, \
     hal_fill_dorrie_cap_vtable}, \
    {68, "WORK_ELEVATOR", WorkElevator_SpawnInfo, WorkElevator_Spawn, \
     hal_fill_work_elevator_vtable}, \
    {221, "ROLLING_ROCK", RollingRock_SpawnInfo, RollingRock_Spawn, \
     hal_fill_rolling_rock_vtable}, \
    {67, "SHUTTER_HMC", ShutterHmc_SpawnInfo, port_factory_shutter_hmc, \
     hal_fill_shutter_hmc_vtable}, \
    /* ---- run linkw wave 5 (lane w5-A): ov063, Big Boo's Haunt's cast ---- */ \
    {209, "BOO", Boo_SpawnInfo, Boo_Spawn, hal_fill_boo_vtable}, \
    {210, "BIG_BOO", BigBoo_SpawnInfo, BigBoo_Spawn, hal_fill_boo_vtable}, \
    {211, "BIG_BOO_ICON", BigBooIcon_SpawnInfo, BigBooIcon_Spawn, \
     hal_fill_big_boo_icon_vtable}, \
    {212, "BOO_CAGE", BooCage_SpawnInfo, BooCage_Spawn, \
     hal_fill_boo_cage_vtable}, \
    {65, "MANSION_STEPS", MansionSteps_SpawnInfo, MansionSteps_Spawn, \
     hal_fill_mansion_steps_vtable}, \
    {158, "BOOKSHELF", Bookshelf_SpawnInfo, Bookshelf_Spawn, \
     hal_fill_mansion_steps_vtable}, \
    {159, "MERRY_GO_ROUND", MerryGoRound_SpawnInfo, MerryGoRound_Spawn, \
     hal_fill_mansion_steps_vtable}, \
    {160, "TRAP_DOOR", TrapDoor_SpawnInfo, TrapDoor_Spawn, \
     hal_fill_mansion_steps_vtable}, \
    {66, "FALL_BLOCK_BBH", FallBlockBbh_SpawnInfo, \
     port_factory_fall_block_bbh, hal_fill_fall_block_bbh_vtable}, \
    {249, "MAD_PIANO", MadPiano_SpawnInfo, MadPiano_Spawn, \
     hal_fill_mad_piano_vtable}, \
    /* ---- w5-A stragglers: the book cast (ov020) + the ov095 lift ---- */ \
    {327, "BOOKEND", Bookend_SpawnInfo, Bookend_Spawn, \
     hal_fill_book_shot_vtable}, \
    {325, "BOOK_SHOT", BookShot_SpawnInfo, BookShot_Spawn, \
     hal_fill_book_shot_vtable}, \
    {213, "BOOK_SWITCH", data_ov020_02114898, func_ov020_021127f4, \
     hal_fill_book_shot_vtable}, \
    {328, "BOOK_SHOT_SPAWNER", BookShotSpawner_SpawnInfo, \
     BookShotSpawner_Spawn, hal_fill_book_shot_spawner_vtable}, \
    {32, "UP_DOWN_LIFT_BBH", UpDownLiftBbh_SpawnInfo, UpDownLiftBbh_Spawn, \
     hal_fill_up_down_lift_hmc_vtable}, \
    {193, "CRAZED_CRATE", CrazedCrate_SpawnInfo, CrazedCrate_Spawn, \
     hal_fill_crazed_crate_vtable}, \
    {64, "COFFIN", Coffin_SpawnInfo, Coffin_Spawn, hal_fill_coffin_vtable}, \
    /* ---- run linkw wave 6 (lane w6-f): the ov095 flamethrower ---- */ \
    {318, "FLAMETHROWER", Flamethrower_SpawnInfo, Flamethrower_Spawn, \
     hal_fill_flamethrower_vtable}, \
    /* ---- run linkw wave 6 (lane w6-D): PATH_LIFT, the first three-level
       hierarchy this port hosts that is SPLIT ACROSS OVERLAYS
       (daObjPathLift_c in ov100 over PathLiftActor_c in ov002 over
       Platform). Both 33-slot tables, the inherited slot-32 AfterClsn and
       the shared WAIT/PATH/FALL machine are in
       hal/actor_classes_ov100pl.cpp. PathLift_Spawn's FINAL vptr write is
       its own table by a real name, so no factory wrapper. Placed on
       levels 13 (x1) and 15 (x2). ---- */ \
    {31, "PATH_LIFT", PathLift_SpawnInfo, PathLift_Spawn, \
     hal_fill_path_lift_vtable}, \
    /* ---- run linkw wave 7 (lane w7-c): the Dark World arena's own class.
       Spawn stores its table by a real name, so no factory wrapper. ---- */ \
    {301, "ORANGE_BALL_BILLBOARD", OrangeBallBillboard_SpawnInfo, \
     OrangeBallBillboard_Spawn, hal_fill_orange_ball_billboard_vtable}, \
    /* ---- run linkw wave 8 (lane w8-packs): ov022, Lethal Lava Land's own
       level overlay. Five ids, four tables; 75 rides 76's fill. Level 14's
       baseline census had 70/71/72/73/74/75/76/77/80/82/83/262 skipped, and
       these five are the ov022 classes whose slot bodies are all real
       decompilations -- 73/77/80/83 are held out because sixteen of their
       slot bodies are `recovered from vtable slot identity` guesses and
       port/tools/inferred_stub_guard.py freezes the seated-guess set at a
       ratchet that may only shrink. ---- */ \
    {71, "VOLCANO_RING", VolcanoRing_SpawnInfo, VolcanoRing_Spawn, \
     hal_fill_volcano_ring_vtable}, \
    {74, "FLOAT_ON_LAVA_PLATFORM", FloatOnLavaPlatform_SpawnInfo, \
     FloatOnLavaPlatform_Spawn, hal_fill_float_on_lava_platform_vtable}, \
    {76, "FLOATING_FLOOR_LLL_SMALL", FloatingFloorLllSmall_SpawnInfo, \
     port_factory_floating_floor_lll_small, hal_fill_floating_floor_lll_vtable}, \
    {75, "FLOATING_FLOOR_LLL_BIG", FloatingFloorLllBig_SpawnInfo, \
     port_factory_floating_floor_lll_big, hal_fill_floating_floor_lll_vtable}, \
    {82, "LAVA_PLANK", LavaPlank_SpawnInfo, LavaPlank_Spawn, \
     hal_fill_lava_plank_vtable}, \
    /* ---- run linkw wave 9 (lane w9-reg): TILTING_PLATFORM_LLL, the largest \
       single class in the wave-8 skip census -- FOURTEEN instances, x10 on \
       level 44 (ex_m_map) and x4 on level 48 (ex_w_map). ov064 is already \
       mounted (gates 177/178), so this is registration with no new mount. \
       The dsd block shift spells its bodies _ZN12MetalNetLift* and its table \
       _ZTV12MetalNetLift; the class is id 72, not the gate-178 id 69, and \
       hal/actor_classes_ov064_gate178.cpp's header says so itself. Its Spawn \
       stores base-then-own and the own store is the zeroed VT1 placeholder, so \
       it needs a FACTORY WRAPPER; the disassembly that settles the store order \
       is in hal/actor_classes_ov064_w9.cpp. ---- */ \
    {72, "TILTING_PLATFORM_LLL", TiltingPlatformLll_SpawnInfo, \
     port_factory_tilting_platform_lll, hal_fill_tilting_platform_lll_vtable}, \
    /* ---- run linkw wave 9 (lane w9-reg): the three ov084 shared-table \
       siblings. GOOMBA_LARGE x9 on level 24; FIRE_PIRANHA_PLANT_BIG x5 on \
       level 24; FIRE_PIRANHA_PLANT_SMALL x2 on level 39. Each reuses the fill \
       its already-registered sibling installs on the SAME ROM table, so the \
       fill column is deliberately the sibling's. ---- */ \
    {202, "GOOMBA_LARGE", GoombaLarge_SpawnInfo, GoombaLarge_Spawn, \
     hal_fill_goomba_vtable}, \
    {251, "FIRE_PIRANHA_PLANT_BIG", FirePiranhaPlantBig_SpawnInfo, \
     FirePiranhaPlantBig_Spawn, hal_fill_fire_piranha_plant_vtable}, \
    {252, "FIRE_PIRANHA_PLANT_SMALL", FirePiranhaPlantSmall_SpawnInfo, \
     FirePiranhaPlantSmall_Spawn, hal_fill_fire_piranha_plant_vtable}, \
    /* ---- run linkw wave 9 (lane w9-reg): the three ov091 sliding-platform \
       siblings. x1 on level 35, x1 and x2 on level 39. Each reuses the fill \
       the registered id 55 installs on the SAME ROM table. ---- */ \
    {138, "SLIDING_PLATFORM_BDW", SlidingPlatformBdw_SpawnInfo, \
     SlidingPlatformBdw_Spawn, hal_fill_sliding_platform_wf_vtable}, \
    {154, "SLIDING_PLATFORM_BS_WIDE", SlidingPlatformBsWide_SpawnInfo, \
     SlidingPlatformBsWide_Spawn, hal_fill_sliding_platform_wf_vtable}, \
    {155, "SLIDING_PLATFORM_BS_LONG", SlidingPlatformBsLong_SpawnInfo, \
     SlidingPlatformBsLong_Spawn, hal_fill_sliding_platform_wf_vtable}, \
    /* ---- run linkw wave 9 (lane w9-reg): the three ov095 seesaw siblings. \
       x2 on level 35, x1 and x1 on level 39. Each reuses the fill the \
       registered id 39 installs on the SAME ROM table. ---- */ \
    {133, "SEESAW_BDW", SeesawBdw_SpawnInfo, SeesawBdw_Spawn, \
     hal_fill_seesaw_bob_vtable}, \
    {149, "SEESAW_BS_PLANK", SeesawBsPlank_SpawnInfo, SeesawBsPlank_Spawn, \
     hal_fill_seesaw_bob_vtable}, \
    {150, "SEESAW_BS_ZIGZAG", SeesawBsZigZag_SpawnInfo, SeesawBsZigZag_Spawn, \
     hal_fill_seesaw_bob_vtable}, \
    /* ---- run linkw wave 10 (lane w10-shared): ov031's SLIDE DECORATION \
       QUAD. The lane's tool resolves all 512 spawn-table entries to the table \
       each factory stores; for ov031 it returns ONE table for FOUR ids, and \
       the class picks its model out of its own actor id (InitResources \
       switches on the +0xc halfword over 0x12e/0x12f/0x130/0x131 and indexes \
       data_ov031_02111424). So all four rows name the SAME fill deliberately. \
       x3 each on level 23, whose entire baseline skip set was these twelve \
       instances of these four classes. ---- */ \
    {302, "SLIDE_DECORATION_SILVER_STAR", SlideDecorationSilverStar_SpawnInfo, \
     SlideDecorationSilverStar_Spawn, hal_fill_slide_decoration_vtable},      \
    {303, "SLIDE_DECORATION_YELLOW_STAR", SlideDecorationYellowStar_SpawnInfo, \
     SlideDecorationYellowStar_Spawn, hal_fill_slide_decoration_vtable},      \
    {304, "SLIDE_DECORATION_ORANGE_SMILEY",                                   \
     SlideDecorationOrangeSmiley_SpawnInfo,                                   \
     SlideDecorationOrangeSmiley_Spawn, hal_fill_slide_decoration_vtable},    \
    {305, "SLIDE_DECORATION_BLUE_SMILEY", SlideDecorationBlueSmiley_SpawnInfo, \
     SlideDecorationBlueSmiley_Spawn, hal_fill_slide_decoration_vtable}, \
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
    {277, "MEGA_MUSHROOM", MegaMushroom_SpawnInfo, MegaMushroom_Spawn, 0}, \
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
    {59, "ROCK_TRIANGLE", RockTriangle_SpawnInfo, RockTriangle_Spawn, \
     hal_fill_warp_pipe_vtable}, \
    {340, "CG_STAIRS", CgStairs_SpawnInfo, CgStairs_Spawn, \
     hal_fill_warp_pipe_vtable}, \
    {341, "ROPE_BARRIER", RopeBarrier_SpawnInfo, RopeBarrier_Spawn, \
     hal_fill_warp_pipe_vtable}, \
    {22, "EXCLAMATION_BLOCK_VS", ExclamationBlockVs_SpawnInfo, \
     ExclamationBlockVs_Spawn, hal_fill_question_block_vtable}, \
    {23, "CAP_BLOCK_MARIO", CapBlockMario_SpawnInfo, CapBlockMario_Spawn, \
     hal_fill_question_block_vtable}, \
    {24, "CAP_BLOCK_WARIO", CapBlockWario_SpawnInfo, CapBlockWario_Spawn, \
     hal_fill_question_block_vtable}, \
    {25, "CAP_BLOCK_LUIGI", CapBlockLuigi_SpawnInfo, CapBlockLuigi_Spawn, \
     hal_fill_question_block_vtable}, \
    {147, "SLIDING_PLATFORM_BFS_SQUARE", SlidingPlatformBfsSquare_SpawnInfo, \
     SlidingPlatformBfsSquare_Spawn, hal_fill_sliding_platform_wf_vtable}, \
    {146, "SLIDING_PLATFORM_BFS_RECTANGLE", SlidingPlatformBfsRectangle_SpawnInfo, \
     SlidingPlatformBfsRectangle_Spawn, hal_fill_sliding_platform_wf_vtable}, \
    {124, "SLIDING_PLATFORM_RR", SlidingPlatformRr_SpawnInfo, \
     SlidingPlatformRr_Spawn, hal_fill_sliding_platform_wf_vtable}, \
    {30, "ROTATING_UP_DOWN_PLATFORM_UTM", RotatingUpDownPlatformUtm_SpawnInfo, \
     RotatingUpDownPlatformUtm_Spawn, hal_fill_rotating_up_down_platform_vtable}, \
    {201, "GOOMBA_SMALL", GoombaSmall_SpawnInfo, GoombaSmall_Spawn, \
     hal_fill_goomba_vtable}, \
    {204, "KOOPA_SMALL", KoopaSmall_SpawnInfo, KoopaSmall_Spawn, \
     hal_fill_koopa_vtable}, \
    {143, "SEESAW_BFS", SeesawBfs_SpawnInfo, SeesawBfs_Spawn, \
     hal_fill_seesaw_bob_vtable}, \
    {28, "SEESAW_UTM", SeesawUtm_SpawnInfo, SeesawUtm_Spawn, \
     hal_fill_seesaw_bob_vtable}, \
    {128, "SEESAW_RR", SeesawRr_SpawnInfo, SeesawRr_Spawn, \
     hal_fill_seesaw_bob_vtable}, \
    {131, "UP_DOWN_LIFT_RR", UpDownLiftRr_SpawnInfo, UpDownLiftRr_Spawn, \
     hal_fill_up_down_lift_hmc_vtable}, \
    {179, "SILVER_STAR", SilverStar_SpawnInfo, SilverStar_Spawn, \
     hal_fill_power_star_vtable}, \
    {324, "SILVER_STAR_BLOCK_TAG", SilverStarBlockTag_SpawnInfo, \
     SilverStarBlockTag_Spawn, hal_fill_mega_mushroom_block_tag_vtable}, \
    /* ---- run linkw wave 12 (lane w12): ov073, level 49's whole cast ---- */ \
    {170, "CCC_ARENA", CccArena_SpawnInfo, CccArena_Spawn, \
     hal_fill_ccc_arena_vtable}, \
    {171, "CCC_BIG_ICE", CccBigIce_SpawnInfo, CccBigIce_Spawn, 0}, \
    {172, "CCC_SMALL_ICE", CccSmallIce_SpawnInfo, CccSmallIce_Spawn, 0}, \
    {218, "CHIEF_CHILLY", ChiefChilly_SpawnInfo, port_factory_chief_chilly, \
     hal_fill_chief_chilly_vtable}, \
    /* ---- run linkw wave 12 (lane w12): ov052, level 44's platform pair ---- */ \
    {174, "BOBBING_LOG_EMM", data_ov052_021124fc, func_ov052_02111410, \
     hal_fill_bobbing_log_emm_vtable}, \
    {175, "SQUARE_PATH_LIFT", SquarePathLift_SpawnInfo, SquarePathLift_Spawn, \
     hal_fill_square_path_lift_vtable}, \
    /* ---- run linkw wave 12 (lane w12): CLAM (315), ov064 ---- */ \
    {315, "CLAM", Clam_SpawnInfo, Clam_Spawn, hal_fill_clam_vtable}, \
    /* ---- run linkw wave 15 (lane w15): ov056, level 48's ice block ---- */ \
    {173, "BIG_MOVING_ICE_BLOCK", BigMovingIceBlock_SpawnInfo, \
     BigMovingIceBlock_Spawn, hal_fill_big_moving_ice_block_vtable}, \
    /* ---- run linkw wave 15 (lane w15): ov002's mega mushroom tag pair ---- */ \
    {319, "MEGA_MUSHROOM_CREATE_TAG", MegaMushroomCreateTag_SpawnInfo, \
     MegaMushroomCreateTag_Spawn, hal_fill_mega_mushroom_tag_vtable}, \
    {320, "MEGA_MUSHROOM_TAG", MegaMushroomTag_SpawnInfo, \
     MegaMushroomTag_Spawn, hal_fill_mega_mushroom_tag_vtable}, \
    /* ---- run linkw wave 15 (lane w15): ov025, level 17's pyramid pair ---- */ \
    {87, "PYRAMID_STEP", PyramidStep_SpawnInfo, PyramidStep_Spawn, \
     hal_fill_pyramid_step_vtable}, \
    {88, "PYRAMID_LIFT", PyramidLift_SpawnInfo, PyramidLift_Spawn, \
     hal_fill_pyramid_lift_vtable}, \
    /* ---- run linkw wave 16 (lane w16-unblock): ov022's fall block, the id \
       the inferred-stub guard held back; x9 on level 14 ---- */ \
    {83, "FALL_BLOCK_LLL", FallBlockLll_SpawnInfo, port_factory_fall_block_lll, \
     hal_fill_fall_block_lll_vtable}, \
    /* ---- run linkw wave 17 (lane w17): ov002's SEAWEED, x8 on level 18 and \
       x4 on level 24 ---- */ \
    {296, "SEAWEED", Seaweed_SpawnInfo, Seaweed_Spawn, \
     hal_fill_seaweed_vtable}, \
    /* ---- run link60 (lane A1): ov085's WALL_SIGN and TOAD, nineteen placed \
       instances across levels 2, 4, 5 and 50 ---- */ \
    {183, "WALL_SIGN", WallSign_SpawnInfo, WallSign_Spawn, \
     hal_fill_wall_sign_vtable}, \
    {185, "TOAD", Toad_SpawnInfo, Toad_Spawn, hal_fill_toad_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-ov077): the ov077 shared enemy pack. \
       Placed instances measured from this tree's own baseline census, not \
       quoted: HEAVE_HO x3 on level 21, x1 on 27, x3 on 42; LAKITU x1 on 24, \
       x2 on 28, x2 on 43, x1 on 44. SPINY is placed on NO level of the \
       fifty-two -- it is registered because it is the overlay's third class \
       and its seat is as derived as the other two, but nothing in a level \
       walk exercises it (see port/slice_ov077.txt). ---- */ \
    {238, "HEAVE_HO", HeaveHo_SpawnInfo, HeaveHo_Spawn, \
     hal_fill_heave_ho_vtable}, \
    {260, "SPINY", Spiny_SpawnInfo, Spiny_Spawn, hal_fill_spiny_vtable}, \
    {265, "LAKITU", Lakitu_SpawnInfo, Lakitu_Spawn, hal_fill_lakitu_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-ov030): ov030's Tall Tall Mountain \
       cast. Four ids, three classes: 267 and 268 are one class under two \
       spawn entry points and share hal_fill_ukiki_vtable. On level 22 only \
       102 and 267 are placed (x1 each); 103 and 268 have no placed instance \
       there and are registered for the spawn path rather than for a census \
       count ---- */ \
    {102, "ROLLING_LOG_TTM", RollingLogTtm_SpawnInfo, RollingLogTtm_Spawn, \
     hal_fill_rollinglogttm_vtable}, \
    {103, "UKIKI_CAGE", UkikiCage_SpawnInfo, UkikiCage_Spawn, \
     hal_fill_ukikicage_vtable}, \
    {267, "UKIKI_THIEF", UkikiThief_SpawnInfo, UkikiThief_Spawn, \
     hal_fill_ukiki_vtable}, \
    {268, "UKIKI_STAR", UkikiStar_SpawnInfo, UkikiStar_Spawn, \
     hal_fill_ukiki_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-ov026): ov026's whole cast, level 18 \
       (Dire Dire Docks). Ten placed instances on 18 across the first four; \
       WATER_SUCTION is not placed on 18 and proves by no-regression. ---- */ \
    {89, "POLE_LIFT_DDD", data_ov026_02113abc, func_ov026_02111678, \
     hal_fill_ov026_polelift_vtable}, \
    {90, "SUBMARINE", Submarine_SpawnInfo, Submarine_Spawn, \
     hal_fill_ov026_submarine_vtable}, \
    {91, "BOWSER_SHUTTER", BowserShutter_SpawnInfo, BowserShutter_Spawn, \
     hal_fill_ov026_bowser_shutter_vtable}, \
    {246, "WHIRLPOOL", Whirlpool_SpawnInfo, Whirlpool_Spawn, \
     hal_fill_ov026_whirlpool_vtable}, \
    {247, "WATER_SUCTION", WaterSuction_SpawnInfo, WaterSuction_Spawn, \
     hal_fill_ov026_water_suction_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov035, level 27's own cast. \
       Three ids take the SAME fill on purpose -- one table, one idempotent \
       fill, the ov013 CLOCK_HAND_SHORT/LONG shape. ---- */ \
    {119, "ROTATING_CLOCK_HAND", RotatingClockHand_SpawnInfo, \
     RotatingClockHand_Spawn, hal_fill_rotating_cog_vtable}, \
    {120, "SPINNING_PLATFORM", SpinningPlatform_SpawnInfo, \
     SpinningPlatform_Spawn, hal_fill_spinning_platform_vtable}, \
    {121, "ROTATING_COG_BIG", data_ov035_02112ac0, \
     func_ov035_0211168c, hal_fill_rotating_cog_vtable}, \
    {122, "ROTATING_COG_SMALL", RotatingCogSmall_SpawnInfo, \
     RotatingCogSmall_Spawn, hal_fill_rotating_cog_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov024, level 16's pyramid \
       pair. Two tables, two different widths -- 32 and 31. ---- */ \
    {85, "PYRAMID_TOP", PyramidTop_SpawnInfo, PyramidTop_Spawn, \
     hal_fill_pyramid_top_vtable}, \
    {86, "PYRAMID_TAG", PyramidTag_SpawnInfo, PyramidTag_Spawn, \
     hal_fill_pyramid_tag_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov033, Tiny-Huge Island's \
       small side. The dsd vtable label is on the water, not the cover. ---- */ \
    {104, "TINY_COVER", TinyCover_SpawnInfo, TinyCover_Spawn, \
     hal_fill_tiny_cover_vtable}, \
    {105, "TINY_WATER", TinyWater_SpawnInfo, TinyWater_Spawn, \
     hal_fill_tiny_water_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov023, the volcano \
       squasher. Level 15's entire skip list, one row. ---- */ \
    {84, "SQUASHER", Squasher_SpawnInfo, Squasher_Spawn, \
     hal_fill_squasher_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov017, the sunken ship's \
       water. The blocker level_boot.cpp's level-9 block deferred. ---- */ \
    {62, "SHIP_WATER", ShipWater_SpawnInfo, ShipWater_Spawn, \
     hal_fill_ship_water_vtable}, \
    /* ---- run rel0215 wave 1 (lane cast-sweep1): ov058, the Rec Room's \
       cupboard. Level 50's entire skip list, one row. ---- */ \
    {182, "REC_ROOM_CUPBOARD", RecRoomCupboard_SpawnInfo, \
     RecRoomCupboard_Spawn, hal_fill_rec_room_cupboard_vtable},
