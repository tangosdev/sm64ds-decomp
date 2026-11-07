# Particle effect IDs

Evidence for the names in `include/ParticleIds.h`. The SPA resource
(arm9 `data_02075f14`, 321 definitions) carries no names, so each ID is named
for the code that spawns it; a look word (Flame, Smoke, Ripple, Splash,
Sparkle, ...) needs a ROM callback class, an existing member or local name, a
source comment, or a runtime check behind it.

Confidence: **high** = the spawner and event are read straight from matched
source; **medium** = the spawner is certain but the event word is an
inference from the surrounding code; **low** = not applied, the ID stays bare
hex and the name is only a candidate.

Surveyed at origin/main 574f09c8e8. `tools/particle_id_sites.py` sees only
literal IDs; about forty more are spawned through named constants, `kind + 1`
arithmetic, per-course tables and the `NewBigSplash`/`NewRipple` helpers, and
were traced by hand (the site column cites them).

| ID | name | conf. | site | evidence | decoded (shape; draw; life; emitter; colour) |
|---|---|---|---|---|---|
| 0x000 | `kPtclOneUpTrail` | medium | `src/game/actors/d_a_1up.cpp:227` | da1up_c::func_ov002_020aeee4 renews effect 0 above a 1-Up mushroom every frame (0x108 for the Mega Mushroom); table base colour is 1-Up green. | volume (per-axis rand); billboard; life 10f; forever; #9cff6a |
| 0x001 | `kPtclAbukuBubble` | medium | `src/game/actors/d_a_obj_abuku.cpp:89` | OBJ_ABUKU renews it every frame in daObjAbuku_c::Behavior while the actor lives. | point; billboard; life 16f; forever; #ffffff |
| 0x002 | `kPtclAbukuPop` | high | `src/game/actors/d_a_obj_abuku.cpp:125` | OBJ_ABUKU Pop() spawns it, plays the pop sound and destroys the actor. | volume (per-axis rand); billboard; life 90f; emitter 5f; #ffffff |
| 0x003 | `kPtclWhompPounded` | medium | `src/game/actors/d_a_btn.cpp:1018` | daBtn_c spawns it at the pounding player when Whomp is ground-pounded on its back and its HP drops. | circle rim, random; spinning; life 40f; emitter 4f; #41414a |
| 0x004 | `kPtclWhompKingDefeat` | high | `src/game/actors/d_a_btn.cpp:1289` | daBtn_c defeat branch that does TriplePoofDustAt and spawns the star (King Whomp). | disc (per-axis rand); spinning; life 80f; emitter 6f; #52525a |
| 0x005 | `kPtclWhompDefeat` | high | `src/game/actors/d_a_btn.cpp:1305 (+1 more)` | daBtn_c defeat branch without a star (plain Whomp) and OnHitByMegaChar, each with PoofDustAt and MarkForDestruction. | disc (per-axis rand); spinning; life 32f; emitter 6f; #41414a |
| 0x006 | `kPtclBattaBlockBreak` | high | `src/game/actors/d_a_btt_bk.cpp:115 (+2 more)` | BATTA_BLOCK crate breaks: HandleCollision, StateCarriedMain and OnTurnIntoEgg, always with PoofDustAt. | disc (per-axis rand); spinning; life 24f; emitter 4f; #947b31 |
| 0x007 | `kPtclCannonShotA` | high | `src/game/actors/d_a_cnn.cpp:342` | CANNON StateFire spawns 0x07 and 0x08 together at the muzzle, aimed along the barrel, when it fires. | circle rim, even steps; billboard; life 15f; emitter 1f; #ffffff |
| 0x008 | `kPtclCannonShotB` | high | `src/game/actors/d_a_cnn.cpp:344` | Second member of the CANNON StateFire muzzle pair (see 0x07). | disc (per-axis rand); billboard; life 22f; emitter 1f; #ffffff |
| 0x009 | `kPtclCapBlockBreak` | high | `src/game/actors/d_a_obj_hatena_block.cpp:847` | daObjHatenaBlock_c picks it for CAP_BLOCK_M/W/L when the block breaks; table colour green. | volume (per-axis rand); billboard; life 35f; emitter 6f; #31ff41 |
| 0x00a | `kPtclBgActorBreak` | medium | `src/game/actors/d_a_obj_ewb_ice.cpp:319 (+1 more)` | dBgActor_c::Kill (the default kill of every dBgActor_c: poof 100 units up, sound, gone) and daObjEwbIce_c::Kill. | disc (per-axis rand); billboard; life 28f; emitter 1f; #ff5a00 |
| 0x00b | `kPtclBlockBreakA` | medium | `src/game/actors/d_a_obj_hatena_block.cpp:836 (+1 more)` | Spawned for every daObjHatenaBlock_c kind when it breaks, and by HM_BASKET landing/destruction; paired with 0x0d. | sphere surface; billboard; life 15f; emitter 1f; #ffffff |
| 0x00c | `kPtclHatenaBlockBreak` | high | `src/game/actors/d_a_obj_hatena_block.cpp:844` | daObjHatenaBlock_c picks it for HATENA_BLOCK when it breaks; table colour red. | volume (per-axis rand); billboard; life 35f; emitter 6f; #ff0000 |
| 0x00d | `kPtclBlockBreakB` | medium | `src/game/actors/d_a_obj_hatena_block.cpp:837 (+1 more)` | Second shared block-break member, always spawned with 0x0b. | sphere surface; velocity-aligned; life 8f; emitter 1f; #ffffff |
| 0x00e | (candidate `kPtclWoodBreak`) | low | `src/game/actors/d_a_obj_block_s.cpp:859 (+2 more)` | BLOCK_S, TATEFUDA (signpost) and YAJIRUSI (arrow sign) Kill paths spawn it; brown (#8b7b6a) spinning discs suggest wood, no runtime check. | disc (per-axis rand); spinning; life 24f; emitter 4f; #8b7b6a |
| 0x010 | `kPtclItemBlockBreak` | high | `src/game/actors/d_a_obj_hatena_block.cpp:841` | daObjHatenaBlock_c picks it for ITEM_BLOCK / VS_ITEM_BLOCK when it breaks; table colour yellow. | volume (per-axis rand); billboard; life 35f; emitter 6f; #ffff00 |
| 0x013 | `kPtclBobOmbCarried` | medium | `src/game/actors/d_a_bmb.cpp:845` | daBmb_c renews it into mCarryParticle while carried (header: 'effect 0x13 while carried'). | sphere surface; billboard; life 6f; forever; #e6ffff |
| 0x014 | `kPtclBobOmbExplodeA` | high | `src/game/actors/d_a_bmb.cpp:1145` | BOMBHEI explosion spawns 0x14-0x18 in order (explosion sound, coin drop); letters record order only. | point; billboard; life 5f; emitter 1f; #ff0000 |
| 0x015 | `kPtclBobOmbExplodeB` | high | `src/game/actors/d_a_bmb.cpp:1146` | Bob-omb explosion set member (see 0x14). | sphere surface; billboard; life 35f; emitter 1f; #525252 |
| 0x016 | `kPtclBobOmbExplodeC` | high | `src/game/actors/d_a_bmb.cpp:1147` | Bob-omb explosion set member (see 0x14). | point; velocity-aligned; life 8f; emitter 1f; #ffcd9c |
| 0x017 | `kPtclBobOmbExplodeD` | high | `src/game/actors/d_a_bmb.cpp:1148` | Bob-omb explosion set member (see 0x14). | sphere surface; billboard; life 25f; emitter 1f; #ff9c08 |
| 0x018 | `kPtclBobOmbExplodeE` | high | `src/game/actors/d_a_bmb.cpp:1149` | Bob-omb explosion set member (see 0x14). | volume (per-axis rand); billboard; life 12f; emitter 20f; #6a6a6a |
| 0x019 | `kPtclBobOmbFuse` | high | `src/game/actors/d_a_bmb.cpp:900` | daBmb_c renews it into mFuseParticle ('effect 0x19 while the fuse is lit'). | volume (per-axis rand); billboard; life 7f; forever; #6a6a6a |
| 0x01a | `kPtclKingBobOmbDefeat` | high | `src/game/actors/d_a_bombking.cpp:265` | BOMBKING spawns it with the defeat sound and the star. | sphere surface; spinning; life 50f; emitter 8f; #626262 |
| 0x01b | `kPtclBossLandingDust` | medium | `src/runtime/actor/d_actor.cpp:1503` | dActor_c helper func_0200fa04 (as func_0200fc44 but 120 units up); used by Whomp falling flat and by Bowser. | circle rim, even steps; billboard; life 50f; emitter 1f; #ffffff |
| 0x01c | `kPtclWaterBombBurstA` | high | `src/game/actors/d_a_wbm.cpp:407` | daWbm_c: the falling water bomb (variant 1) hurts the player, spawns 0x1c+0x1d and Burst()s. | volume (per-axis rand); billboard; life 15f; emitter 2f; #ffffff |
| 0x01d | `kPtclWaterBombBurstB` | high | `src/game/actors/d_a_wbm.cpp:408` | Second member of the water-bomb burst pair (see 0x1c). | volume (per-axis rand); velocity-aligned; life 16f; emitter 1f; #ffffff |
| 0x01e | `kPtclChompGateBreak` | high | `src/game/actors/d_a_obj_wanwan_shutter.cpp:116` | daObjWanwanShutter_c::Break (the Chain Chomp's gate) spawns it. | disc (per-axis rand); spinning; life 45f; emitter 6f; #393941 |
| 0x01f | `kPtclPyramidTopSunkA` | high | `src/game/actors/d_a_obj_dl_pyramid.cpp:106` | DL_PYRAMID FinishDescent spawns 0x1f-0x22 together at the old summit once the top has sunk. | volume (per-axis rand); billboard; life 42f; emitter 1f; #ffee8b |
| 0x020 | `kPtclPyramidTopSunkB` | high | `src/game/actors/d_a_obj_dl_pyramid.cpp:107` | Pyramid-top set member (see 0x1f). | circle rim, even steps; billboard; life 46f; emitter 1f; #ffee8b |
| 0x021 | `kPtclPyramidTopSunkC` | high | `src/game/actors/d_a_obj_dl_pyramid.cpp:108` | Pyramid-top set member (see 0x1f). | disc (per-axis rand); spinning; life 130f; emitter 4f; #7b4a00 |
| 0x022 | `kPtclPyramidTopSunkD` | high | `src/game/actors/d_a_obj_dl_pyramid.cpp:109` | Pyramid-top set member (see 0x1f). | circle rim, even steps; billboard; life 130f; emitter 1f; #ac8b41 |
| 0x023 | `kPtclPyramidTopSinking` | high | `src/game/actors/d_a_obj_dl_pyramid.cpp:138` | DL_PYRAMID Descend renews it every frame while the top sinks. | circle rim, random; billboard; life 50f; forever; #ff9400 |
| 0x024 | `kPtclWaterfallMist` | high | `src/game/actors/d_a_obj_waterfall.cpp:54` | daObjWaterfall_c (file comment 'Waterfall mist') default particle; renewed every frame at the actor. | disc (per-axis rand); billboard; life 20f; forever; #ffffff |
| 0x025 | `kPtclChairBreakA` | high | `src/game/actors/d_a_chair.cpp:162` | CHAIR Break spawns 0x25 and 0x26 (identical table entries). | circle rim, even steps; spinning; life 26f; emitter 1f; #ffa47b |
| 0x026 | `kPtclChairBreakB` | high | `src/game/actors/d_a_chair.cpp:163` | Second member of the chair break pair (see 0x25). | circle rim, even steps; spinning; life 26f; emitter 1f; #ffa47b |
| 0x027 | (candidate `kPtclCapPowerEndSparkle`) | low | `src/game/player/d_a_ply_methods.cpp:6878` | daPly_c::func_ov002_020c8714 (St_NoControl step 24) renews it 60 units up; a source comment calls it the 'sparkle trail' of a cap-power ending that spawns OBJ_MARIO_CAP or a star. Event reading is the comment's. | volume (per-axis rand); billboard; life 35f; forever; #ffffd5 |
| 0x028 | `kPtclDrainLidPounded` | medium | `src/game/actors/d_a_obj_tt_futa.cpp:53` | TT_FUTA lid OnGroundPounded: at the pounder, sets drain event bit 0xe and destroys the lid. | disc (per-axis rand); billboard; life 28f; emitter 1f; #8b7b5a |
| 0x029 | `kPtclMontyMoleEmerge` | high | `src/game/actors/d_a_choropu.cpp:321` | daChoropu_c (Monty Mole) emerge state renews it while the mole comes out of the ground. | disc (per-axis rand); billboard; life 12f; forever; #eeac6a |
| 0x02a | `kPtclMontyMoleLeapA` | medium | `src/game/actors/d_a_choropu.cpp:152` | daChoropu_c leap state spawns 0x2a and 0x2b on frame 15. | circle rim, even steps; billboard; life 21f; emitter 1f; #eeac6a |
| 0x02b | `kPtclMontyMoleLeapB` | medium | `src/game/actors/d_a_choropu.cpp:153` | Second leap member, 30 units above 0x2a. | circle rim, even steps; billboard; life 15f; emitter 1f; #ffcd94 |
| 0x02c | `kPtclYoshiEggTrail` | medium | `src/game/actors/d_a_yegg.cpp:540` | daYegg_c SEEK state renews it behind the thrown egg, aimed along its heading. | point; velocity-aligned; life 28f; forever; #ffffff |
| 0x02d | `kPtclSpindelDust` | medium | `src/game/actors/d_a_dgr.cpp:222 (+1 more)` | d_a_dgr's kDustEffect: spawned at both ends of the Spindel roller as it rolls. | disc (per-axis rand); billboard; life 20f; forever; #ffffd5 |
| 0x02e | `kPtclGrindelLanding` | high | `src/game/actors/d_a_dkk.cpp:163 (+1 more)` | DONKAKU StateFall and the non-Thwomp slam on touchdown, with camera shake and the slam sound. | circle rim, even steps; billboard; life 22f; emitter 1f; #ffffd5 |
| 0x02f | `kPtclStarPose` | medium | `src/game/actors/d_a_star.cpp:2454` | daStar_c 'pose effects': renewed at the star's centre while its animation plays from frame 2. | sphere surface; billboard; life 16f; forever; #ffffd5 |
| 0x030 | `kPtclStarPoseEnd` | medium | `src/game/actors/d_a_star.cpp:2461` | daStar_c pose effects: the one-shot effect when the animation reaches frame 0x75. | circle rim, random; billboard; life 30f; emitter 15f; #ffffa4 |
| 0x031 | `kPtclDemoSpinA` | medium | `src/game/actors/d_a_demo.cpp:4025` | daDemo_c KuppaScript handler 6 spins the actor at 3x scale and renews 0x31-0x33 at it. | volume (per-axis rand); billboard; life 40f; forever; #ffffd5 |
| 0x032 | `kPtclDemoSpinB` | medium | `src/game/actors/d_a_demo.cpp:4028` | daDemo_c handler 6 set member (see 0x31). | point; spinning; life 35f; forever; #ffff5a |
| 0x033 | `kPtclDemoSpinC` | medium | `src/game/actors/d_a_demo.cpp:4031` | daDemo_c handler 6 set member (see 0x31). | point; billboard; life 4f; forever; #83ff00 |
| 0x034 | `kPtclJumboStarFlyA` | medium | `src/game/player/d_a_ply_methods.cpp:4106` | daPly_c::func_ov002_020c37a4 at a body bone, from JumboStarCutsceneFlying/TakingOff; paired with 0x35. | volume (per-axis rand); billboard; life 30f; forever; #ffff00 |
| 0x035 | `kPtclJumboStarFlyB` | medium | `src/game/player/d_a_ply_methods.cpp:4107` | Second Jumbo Star cutscene member (see 0x34). | volume (per-axis rand); billboard; life 45f; forever; #ffffd5 |
| 0x036 | (candidate `kPtclScriptedAnimA`) | low | `src/game/player/d_a_ply_methods.cpp:781` | func_ov002_020bcdf0 (KuppaScript anim step, Mario only, anims 0xbb/0xbc) renews it 75 units up; event unknown. | volume (per-axis rand); billboard; life 47f; forever; #ffffb4 |
| 0x037 | (candidate `kPtclScriptedAnimB`) | low | `src/game/player/d_a_ply_methods.cpp:755` | Same anim-script step; one-shot 80 units up with voice 0x33 (see 0x36). | sphere surface; billboard; life 35f; emitter 35f; #ffffd5 |
| 0x038 | `kPtclDemoFadeInA` | medium | `src/game/actors/d_a_demo.cpp:4001` | daDemo_c handler 7 ramps opacity in and renews 0x38-0x3a 80 units up; pink. | volume (per-axis rand); billboard; life 32f; forever; #ff83bd |
| 0x039 | `kPtclDemoFadeInB` | medium | `src/game/actors/d_a_demo.cpp:4002` | daDemo_c handler 7 set member (see 0x38). | volume (per-axis rand); billboard; life 34f; forever; #ff83bd |
| 0x03a | `kPtclDemoFadeInC` | medium | `src/game/actors/d_a_demo.cpp:4003` | daDemo_c handler 7 set member (see 0x38). | point; velocity-aligned; life 30f; forever; #ffa4b4 |
| 0x03b | `kPtclDemoDescendA` | medium | `src/game/actors/d_a_demo.cpp:3972` | daDemo_c handler 8 lowers the actor from y>=0x514000 and renews 0x3b-0x3d; pink. | volume (per-axis rand); billboard; life 32f; forever; #ff83bd |
| 0x03c | `kPtclDemoDescendB` | medium | `src/game/actors/d_a_demo.cpp:3973` | daDemo_c handler 8 set member (see 0x3b). | volume (per-axis rand); billboard; life 80f; forever; #ff83bd |
| 0x03d | `kPtclDemoDescendC` | medium | `src/game/actors/d_a_demo.cpp:3974` | daDemo_c handler 8 set member (see 0x3b). | point; billboard; life 50f; forever; #ffffff |
| 0x03e | `kPtclEndingStarSparkle` | high | `src/game/actors/d_a_demo.cpp:947` | Spawned with edStarKiraCallback_c, a ROM RTTI callback name ('ending star sparkle', staff roll). | circle rim, random; billboard; life 48f; forever; #ffffff |
| 0x03f | `kPtclYoshiEggBreakA` | high | `src/game/actors/d_a_yegg.cpp:1220` | daYegg_c: the egg spawns 0x3f-0x42 (and a silver star when due), then destroys itself. | point; billboard; life 6f; emitter 1f; #ff0000 |
| 0x040 | `kPtclYoshiEggBreakB` | high | `src/game/actors/d_a_yegg.cpp:1221` | Yoshi egg break set member (see 0x3f). | sphere surface; billboard; life 25f; emitter 1f; #ffffff |
| 0x041 | `kPtclYoshiEggBreakC` | high | `src/game/actors/d_a_yegg.cpp:1222` | Yoshi egg break set member (see 0x3f). | disc (per-axis rand); spinning; life 25f; emitter 1f; #009c18 |
| 0x042 | `kPtclYoshiEggBreakD` | high | `src/game/actors/d_a_yegg.cpp:1223` | Yoshi egg break set member (see 0x3f). | disc (per-axis rand); spinning; life 25f; emitter 2f; #9c9c9c |
| 0x043 | `kPtclKnockAwayA` | medium | `src/game/actors/d_a_brq.cpp:323 (+11 more)` | Spawned with 0x44 when an enemy is knocked flying: Amp, Lakitu, Pokey, Spiny, KillByInvincibleChar, SpawnMegaCharParticles and more. | point; billboard; life 4f; emitter 1f; #ff0000 |
| 0x044 | `kPtclKnockAwayB` | medium | `src/game/actors/d_a_brq.cpp:324 (+11 more)` | Second knock-away member, always with 0x43. | sphere surface; billboard; life 22f; emitter 1f; #ffff08 |
| 0x045 | `kPtclMrIBeamBurst` | high | `src/game/actors/d_a_ey_bm.cpp:219` | EYEKUN_BEAM SpawnDestroyEffect spawns it when the beam is destroyed. | volume (per-axis rand); velocity-aligned; life 16f; emitter 1f; #6affa4 |
| 0x046 | `kPtclMrIBeamA` | medium | `src/game/actors/d_a_ey_bm.cpp:148` | EYEKUN_BEAM Render renews 0x46 and 0x47 every frame (0x47 with cleanParticleCallback). | point; billboard; life 20f; forever; #6affac |
| 0x047 | `kPtclMrIBeamB` | medium | `src/game/actors/d_a_ey_bm.cpp:149` | Second beam member (see 0x46). | volume (per-axis rand); billboard; life 25f; forever; #6affa4 |
| 0x048 | `kPtclStoneBlockBreak` | medium | `src/game/actors/d_a_dsn.cpp:204 (+2 more)` | Thwomp OnHitByMegaChar, BK_LIFT, PUSHBLOCK and FALL_BLOCK Kill; grey discs. | disc (per-axis rand); billboard; life 30f; emitter 4f; #acacac |
| 0x049 | `kPtclFireSeaBlockBreak` | medium | `src/game/actors/d_a_obj_fall_block.cpp:182` | daObjFallBlock_c::Kill uses it instead of 0x48 for FALL_BLOCK_BFS; orange. | disc (per-axis rand); billboard; life 30f; emitter 2f; #ffbd39 |
| 0x04a | `kPtclFeather` | medium | `src/game/actors/d_a_feather.cpp:127` | FEATHER Behavior renews it every frame. | volume (per-axis rand); billboard; life 10f; forever; #a4d5ff |
| 0x04b | `kPtclFireBarA` | medium | `src/game/actors/d_a_obj_fl_koma_u.cpp:151` | FL_KOMA_U, the rotating fire bar, renews 0x4b and 0x4c along the bar. | point; billboard; life 8f; forever; #ffcd9c |
| 0x04c | `kPtclFireBarB` | medium | `src/game/actors/d_a_obj_fl_koma_u.cpp:153` | Second fire-bar member (see 0x4b). | volume (per-axis rand); billboard; life 6f; forever; #ff4100 |
| 0x04d | `kPtclLavaBubbleFlame` | medium | `src/game/actors/d_a_bbl.cpp:241` | daBbl_c::ExecuteJump renews it into mFireParticle while the lava bubble jumps. | point; billboard; life 1f; forever; #ffc529 |
| 0x04e | `kPtclLavaBubbleSmoke` | medium | `src/game/actors/d_a_bbl.cpp:243` | daBbl_c::ExecuteJump renews it into mSmokeParticle while the lava bubble jumps. | volume (per-axis rand); billboard; life 30f; forever; #ff0000 |
| 0x04f | `kPtclFlamethrowerBlueA` | medium | `src/engine/particle/Particle.cpp:444` | Helper System::NewFlamethrowerBlueA; d_a_obj_flamethrower spawns 0x4f+0x50 for every param except kAltParam; table colour blue. | point; billboard; life 1f; forever; #008bbd |
| 0x050 | `kPtclFlamethrowerBlueB` | medium | `src/engine/particle/Particle.cpp:453` | Helper System::NewFlamethrowerBlueB; second member of the 0x4f flamethrower pair. | point; billboard; life 5f; forever; #399cff |
| 0x051 | `kPtclFlamethrowerRedA` | medium | `src/engine/particle/Particle.cpp:426` | Helper System::NewFlamethrowerRedA; d_a_obj_flamethrower spawns 0x51+0x52 when param1 == kAltParam (0xff); table colour red. | point; billboard; life 1f; forever; #ff0000 |
| 0x052 | `kPtclFlamethrowerRedB` | medium | `src/engine/particle/Particle.cpp:435` | Helper System::NewFlamethrowerRedB; second member of the 0x51 pair. | point; billboard; life 6f; forever; #ff6229 |
| 0x053 | `kPtclHuwahuwaDefeat` | high | `src/game/actors/d_a_huwa.cpp:143` | HUWAHUWA KillWithCoins spawns it as the actor dies and pays out coins. | circle rim, even steps; spinning; life 120f; emitter 1f; #ffffff |
| 0x054 | `kPtclMegaWaterEntryRaised` | medium | `src/game/actors/d_a_bakubaku.cpp:466` | daPly_c::func_ov002_020ce9c8 picks 0x54 over 0xce for Mega (above the surface, fitWater offset); Bubba's dive reuses the Mega set. | circle rim, even steps; billboard; life 10f; emitter 1f; #ffffff |
| 0x055 | `kPtclMegaWaterEntrySurface` | medium | `src/game/actors/d_a_bakubaku.cpp:462` | func_ov002_020ce9c8 picks 0x55 over 0xcf for Mega, at the water surface; Bubba's dive reuses it. | point; spinning; life 15f; emitter 12f; #ffffff |
| 0x056 | `kPtclMegaSplash` | medium | `src/game/actors/d_a_bakubaku.cpp:463 (+2 more)` | Mega's replacement for NewBigSplash (0xdd) and System::NewSplash (0xdf) in the player's water-entry paths; also Bubba's dive. | circle rim, random; billboard; life 39f; emitter 1f; #ffffff |
| 0x057 | `kPtclMegaGroundPoundA` | high | `src/game/player/d_a_ply_methods.cpp:18485` | St_GroundPound_Main, Mega branch: 0x57 and 0x58 on every landing. | circle rim, even steps; billboard; life 16f; emitter 1f; #ffff00 |
| 0x058 | `kPtclMegaGroundPoundB` | high | `src/game/player/d_a_ply_methods.cpp:18486` | Mega ground-pound member (see 0x57). | circle rim, even steps; velocity-aligned; life 12f; emitter 1f; #ffff6a |
| 0x059 | `kPtclMegaGroundPoundDryB` | high | `src/game/player/d_a_ply_methods.cpp:18493` | Mega ground pound, only out of shallow water: 0x5a at the feet, 0x59 60 units up. | circle rim, even steps; billboard; life 42f; emitter 1f; #ffffff |
| 0x05a | `kPtclMegaGroundPoundDryA` | high | `src/game/player/d_a_ply_methods.cpp:18488` | Mega ground pound, only out of shallow water (see 0x59). | point; spinning; life 5f; emitter 1f; #ffcd9c |
| 0x05b | `kPtclMegaGroundPuff` | high | `src/game/player/d_a_ply_methods.cpp:2307` | daPly_c::func_ov002_020bf9d4 (landing/knock-down dust): Mega on dry ground; non-Mega uses func_0200fccc (kPtclGroundPuff). | circle rim, even steps; billboard; life 40f; emitter 1f; #ffffff |
| 0x05c | `kPtclMegaGroundPuffWater` | high | `src/game/player/d_a_ply_methods.cpp:2304 (+1 more)` | func_ov002_020bf9d4, Mega in shallow water; also St_MetalWaterWater_Main for Mega. | circle rim, even steps; billboard; life 40f; emitter 1f; #e6ffff |
| 0x05d | `kPtclMegaGrowA` | high | `src/game/player/d_a_ply_methods.cpp:7044` | func_ov002_020c8d14 (NoControl step 17: scale grows to 3.0, Mega timer reset) spawns it on anim frame 4. | circle rim, even steps; billboard; life 30f; emitter 1f; #ffff29 |
| 0x05e | `kPtclMegaGrowB` | high | `src/game/player/d_a_ply_methods.cpp:7048` | Same Mega growth step, on anim frame 2 (see 0x5d). | circle rim, even steps; billboard; life 37f; emitter 1f; #ffff29 |
| 0x05f | `kPtclMegaGrowStartA` | medium | `src/game/player/d_a_ply_methods.cpp:7480` | func_ov002_020c9a04 sets step 17 (the Mega growth) and spawns 0x5f 40 units up. | circle rim, even steps; billboard; life 44f; emitter 1f; #ffff29 |
| 0x060 | `kPtclMegaGrowStartB` | medium | `src/game/player/d_a_ply_methods.cpp:7483` | func_ov002_020c9a04, at scaled body height (see 0x5f). | circle rim, even steps; billboard; life 30f; emitter 6f; #ffb47b |
| 0x061 | `kPtclMegaRunningDust` | high | `src/engine/particle/Particle.cpp:348` | MegaRunningSlidingDustAt: the player spawns it for Mega where everyone else gets RunningSlidingDustAt (0xda). | circle rim, random; billboard; life 20f; forever; #ffffff |
| 0x062 | `kPtclHeavyStepRipple` | medium | `src/game/player/d_a_ply_methods.cpp:2207` | daPly_c::func_ov002_020bf5e0 (Mega/metal footfall, sound 0xd0) in shallow water, checkWaterRipple wrapper. | circle rim, random; billboard; life 30f; emitter 1f; #ffffff |
| 0x063 | `kPtclHeavyStep` | medium | `src/game/player/d_a_ply_methods.cpp:2201` | func_ov002_020bf5e0 heavy footfall 64 units ahead on dry ground. | circle rim, even steps; billboard; life 15f; emitter 1f; #ffffff |
| 0x064 | `kPtclHeavyStepWater` | medium | `src/game/player/d_a_ply_methods.cpp:2199` | func_ov002_020bf5e0 heavy footfall in shallow water or under water. | circle rim, even steps; billboard; life 15f; emitter 1f; #e6ffff |
| 0x065 | `kPtclMegaWaterSurfaceMove` | high | `src/game/player/d_a_ply_methods.cpp:10038` | func_ov002_020ce8bc, Mega, nonzero speed: kept in mWaterMoveParticle at the water surface. | point; spinning; life 10f; forever; #ffffff |
| 0x066 | `kPtclMegaWaterSurfaceStill` | high | `src/game/player/d_a_ply_methods.cpp:10041` | func_ov002_020ce8bc, Mega, zero speed: kept in mWaterStillParticle. | point; spinning; life 30f; forever; #ffffff |
| 0x067 | `kPtclBigSteelieBreak` | high | `src/game/actors/d_a_grock.cpp:159 (+2 more)` | GORO_ROCK (daGrock_c) breaks with TriplePoofDust and the star. | disc (per-axis rand); billboard; life 60f; emitter 1f; #9c6a4a |
| 0x068 | `kPtclBigSteelieBounce` | medium | `src/game/actors/d_a_grock.cpp:218` | daGrock_c::HandleMeshCollision on JustHitGround, with the bounce sound. | circle rim, even steps; billboard; life 18f; emitter 1f; #cdbd94 |
| 0x069 | `kPtclBigSteelieRollDust` | medium | `src/game/actors/d_a_grock.cpp:222` | daGrock_c::HandleMeshCollision renews it while the rock rolls on the ground. | circle rim, random; billboard; life 20f; forever; #cdbd94 |
| 0x06d | `kPtclHatenaSwitchPressA` | medium | `src/game/actors/d_a_obj_hatena_switch.cpp:237` | HATENA_SWITCH Behavior spawns 0x6d-0x70 together when pressed. | circle rim, random; billboard; life 230f; emitter 80f; #ff6a9c |
| 0x06e | `kPtclHatenaSwitchPressB` | medium | `src/game/actors/d_a_obj_hatena_switch.cpp:238` | Switch-press set member (see 0x6d). | disc (per-axis rand); billboard; life 40f; emitter 1f; #ffff00 |
| 0x06f | `kPtclHatenaSwitchPressC` | medium | `src/game/actors/d_a_obj_hatena_switch.cpp:239` | Switch-press set member (see 0x6d). | circle rim, even steps; velocity-aligned; life 60f; emitter 1f; #ff41d5 |
| 0x070 | `kPtclHatenaSwitchPressD` | medium | `src/game/actors/d_a_obj_hatena_switch.cpp:241` | Switch-press set member, spawned at the base (see 0x6d). | circle rim, even steps; billboard; life 40f; emitter 1f; #ffffff |
| 0x071 | `kPtclWaterfallMistCourse16` | high | `src/game/actors/d_a_obj_waterfall.cpp:57` | daObjWaterfall_c picks it on LEVEL_ID 0x16 (Tall Tall Mountain). | disc (per-axis rand); billboard; life 14f; forever; #ffffff |
| 0x072 | `kPtclFwooshGustA` | high | `src/game/actors/d_a_hyuhyu.cpp:340` | HYUHYU (Fwoosh) blow state streams the gust particles 0x72 and 0x73. | point; velocity-aligned; life 4f; forever; #ffffff |
| 0x073 | `kPtclFwooshGustB` | high | `src/game/actors/d_a_hyuhyu.cpp:342` | Second gust stream (see 0x72). | volume (per-axis rand); velocity-aligned; life 12f; forever; #ffffff |
| 0x074 | `kPtclIceBreakA` | high | `src/game/actors/d_a_obj_ice_block.cpp:187 (+1 more)` | daObjIceBlock_c::Kill and the ice board's Kill spawn 0x74-0x76 together. | sphere surface; spinning; life 36f; emitter 3f; #94ffff |
| 0x075 | `kPtclIceBreakB` | high | `src/game/actors/d_a_obj_ice_block.cpp:188 (+1 more)` | Ice break set member (see 0x74). | sphere surface; spinning; life 24f; emitter 3f; #94ffff |
| 0x076 | `kPtclIceBreakC` | high | `src/game/actors/d_a_obj_ice_block.cpp:189 (+1 more)` | Ice break set member (see 0x74). | point; billboard; life 10f; emitter 1f; #bdffff |
| 0x077 | (candidate `kPtclIceShimmerA`) | low | `src/game/actors/d_a_king_donketu.cpp:295 (+1 more)` | Chief Chilly func_ov073_0211f144 keeps 0x77/0x78 alive 200 units ahead during cutscenes and defeat; daObjIceBlock_c also uses it. Event unclear. | sphere surface; billboard; life 20f; forever; #ffffff |
| 0x078 | (candidate `kPtclIceShimmerB`) | low | `src/game/actors/d_a_king_donketu.cpp:296 (+1 more)` | Second member with 0x77. | sphere surface; velocity-aligned; life 9f; forever; #deeeff |
| 0x07b | `kPtclWaterSpout` | medium | `src/game/actors/d_a_water_hakidasi.cpp:219` | WATER_HAKIDASI State0 renews it every frame (the variant without rings; 0x138 is the ring variant). | disc (per-axis rand); billboard; life 40f; forever; #ffffff |
| 0x07c | `kPtclEyerokDefeatA` | high | `src/game/actors/d_a_iwante.cpp:804` | daIwante_c spawns 0x7c and 0x7d once the defeat animation finishes. | disc (per-axis rand); billboard; life 70f; emitter 1f; #bd8b00 |
| 0x07d | `kPtclEyerokDefeatB` | high | `src/game/actors/d_a_iwante.cpp:805` | Second Eyerok defeat member (see 0x7c). | disc (per-axis rand); billboard; life 80f; emitter 1f; #bd8b00 |
| 0x07e | `kPtclKleptoHurt` | medium | `src/game/actors/d_a_jango.cpp:1062` | daJango_c (Klepto) hurt enter: drops the held item, local PARTICLE_AT_HURT_ENTER. | point; spinning; life 90f; emitter 1f; #f6c5f6 |
| 0x07f | `kPtclKeronpaFireA` | medium | `src/game/actors/d_a_kp_fr.cpp:106` | KERONPA_FIRE Render renews 0x7f and 0x80 (with cleanParticleCallback) every frame. | point; billboard; life 8f; forever; #ffffff |
| 0x080 | `kPtclKeronpaFireB` | medium | `src/game/actors/d_a_kp_fr.cpp:108` | Second KERONPA_FIRE member (see 0x7f). | volume (per-axis rand); billboard; life 8f; forever; #ff4100 |
| 0x081 | `kPtclKeyFall` | medium | `src/game/actors/d_a_obj_key.cpp:472 (+1 more)` | OBJ_KEY / LAST_STAR local FX_FALL: StateStarJump / StateDrop. | volume (per-axis rand); billboard; life 28f; forever; #ffffe6 |
| 0x082 | `kPtclKeyBurstA` | medium | `src/game/actors/d_a_obj_key.cpp:832` | OBJ_KEY local FX_BURST_A, at a bone in Behavior. | volume (per-axis rand); billboard; life 45f; forever; #ffffe6 |
| 0x083 | `kPtclKeyBurstB` | medium | `src/game/actors/d_a_obj_key.cpp:834` | OBJ_KEY local FX_BURST_B, with 0x82. | point; billboard; life 90f; emitter 1f; #ffffff |
| 0x084 | `kPtclChiefChillyHitA` | medium | `src/game/actors/d_a_king_donketu.cpp:455` | func_ov073_0211f494 on Chief Chilly's collision sphere, when an attack lands on him. | disc (per-axis rand); spinning; life 40f; emitter 4f; #94ffff |
| 0x085 | `kPtclChiefChillyHitB` | medium | `src/game/actors/d_a_king_donketu.cpp:456` | Second member with 0x84. | volume (per-axis rand); billboard; life 40f; emitter 2f; #a4ffff |
| 0x086 | (candidate `kPtclWeatherCourse31A`) | low | `src/engine/view/d_camera.cpp:327` | dCamera_c::Behavior weather (NewWeather) for gCurrentSublevel 0x31 only, renewed every frame with 0x87. | disc (per-axis rand); velocity-aligned; life 30f; forever; #ffffff |
| 0x087 | (candidate `kPtclWeatherCourse31B`) | low | `src/engine/view/d_camera.cpp:327 (+1 tool site)` | dCamera_c::Behavior adds it alongside 0x86 on sublevel 0x31. | disc (per-axis rand); velocity-aligned; life 9f; forever; #ffffff |
| 0x088 | `kPtclChiefChillyStepA` | medium | `src/game/actors/d_a_king_donketu.cpp:399` | func_ov073_0211f2c0(strength): Chief Chilly steps or lands outside the chase state; camera shake. | circle rim, random; spinning; life 40f; emitter 4f; #94ffff |
| 0x089 | `kPtclChiefChillyStepB` | medium | `src/game/actors/d_a_king_donketu.cpp:401` | Second member with 0x88. | point; spinning; life 10f; emitter 6f; #6affff |
| 0x08a | `kPtclChiefChillyHitPlayer` | medium | `src/game/actors/d_a_king_donketu.cpp:617` | func_ov073_0211f61c: at the player after Chief Chilly's Hurt succeeds. | sphere surface; velocity-aligned; life 22f; emitter 1f; #ffffa4 |
| 0x08b | `kPtclEwbIceSettleA` | medium | `src/game/actors/d_a_obj_ewb_ice.cpp:181` | EWB_ICE settles into the water (comment: 'ice-splash particles') with 0x8c. | circle rim, even steps; velocity-aligned; life 25f; emitter 1f; #ffffff |
| 0x08c | `kPtclEwbIceSettleB` | medium | `src/game/actors/d_a_obj_ewb_ice.cpp:182` | Second EWB_ICE settle member (see 0x8b). | circle rim, random; billboard; life 60f; emitter 1f; #ffffff |
| 0x08d | `kPtclKillerDaiBreak` | high | `src/game/actors/d_a_obj_bk_killerdai.cpp:109` | BK_KILLER_DAI (Bullet Bill launcher) Kill. | disc (per-axis rand); spinning; life 45f; emitter 6f; #393941 |
| 0x08f | `kPtclBulletBillBurst` | high | `src/game/actors/d_a_klr.cpp:108 (+2 more)` | d_a_klr kBurstParticle: Bullet Bill explodes on hit, timeout or being shot down. | sphere surface; spinning; life 45f; emitter 3f; #626262 |
| 0x090 | `kPtclBulletBillSmoke` | high | `src/game/actors/d_a_klr.cpp:195` | d_a_klr kSmokeParticle: renewed at the nose every frame in flight. | volume (per-axis rand); billboard; life 80f; forever; #ffffff |
| 0x091 | `kPtclGoombossVanishA` | medium | `src/game/actors/d_a_kuri_king.cpp:509` | daKuriKing_c hides the model 400 units up with a sound (a KURIKING_VANISH profile exists). | sphere surface; billboard; life 50f; emitter 1f; #ffff52 |
| 0x092 | `kPtclGoombossVanishB` | medium | `src/game/actors/d_a_kuri_king.cpp:510` | Second member with 0x91. | sphere surface; billboard; life 60f; emitter 1f; #ff9c6a |
| 0x093 | `kPtclBooDefeatA` | medium | `src/game/actors/d_a_trs.cpp:1060` | daTrs_c hit state when mHitPoints reaches 0; with 0x94. | volume (per-axis rand); billboard; life 80f; emitter 20f; #ffffff |
| 0x094 | `kPtclBooDefeatB` | medium | `src/game/actors/d_a_trs.cpp:1061` | Second Boo defeat member (see 0x93). | volume (per-axis rand); billboard; life 80f; emitter 1f; #bdeeff |
| 0x095 | (candidate `kPtclBooRelocateA`) | low | `src/game/actors/d_a_trs.cpp:889` | daTrs_c::func_ov063_02117364 (fades out, flips mirrored, fades in) renews 0x95/0x96. | sphere surface; billboard; life 30f; forever; #83e6ff |
| 0x096 | (candidate `kPtclBooRelocateB`) | low | `src/game/actors/d_a_trs.cpp:891` | Second member with 0x95. | sphere surface; billboard; life 45f; emitter 30f; #83e6ff |
| 0x097 | (candidate `kPtclBooRelocateC`) | low | `src/game/actors/d_a_trs.cpp:900` | Same relocate handler, branch where mRelocateTimer > 0x1c; table entry equals 0x95. | sphere surface; billboard; life 30f; forever; #83e6ff |
| 0x098 | (candidate `kPtclBooRelocateD`) | low | `src/game/actors/d_a_trs.cpp:902` | Same branch as 0x97; table entry equals 0x96. | sphere surface; billboard; life 45f; forever; #83e6ff |
| 0x099 | (candidate `kPtclBowserFireBreath`) | low | `src/game/actors/d_a_kpa.cpp:2692 (+1 more)` | daKpa_c::func_ov060_021132a4 / 02112ee0 spawn it 50 units out; dark-red volume. Event inferred. | volume (per-axis rand); billboard; life 6f; forever; #5a0000 |
| 0x09a | `kPtclFlameScaled` | medium | `src/game/actors/d_a_kpa_fire.cpp:592` | daKpaFire_c variants 1 and 5 pass it to func_ov060_02116518; the one kind whose emitRadius scales with mFireScale (local PARTICLE_SCALED_9A). | volume (per-axis rand); billboard; life 6f; forever; #ff9431 |
| 0x09b | `kPtclFlameScaledSecond` | medium | `src/game/actors/d_a_kpa_fire.cpp:859` | func_ov060_02116518 starts kind+1 in the second handle; 0x9b pairs with 0x9a. | point; billboard; life 1f; forever; #ffffff |
| 0x09c | `kPtclFlame` | existing | `src/game/actors/d_a_kpa_fire.cpp:523` | Existing name in include/ParticleIds.h (kept). Bowser's fire once landed, scaled by mFireScale. | point; billboard; life 8f; forever; #ffffff |
| 0x09d | `kPtclFlameSecond` | medium | `src/game/actors/d_a_kpa_fire.cpp:859` | Second-handle partner (kind+1) of kPtclFlame in func_ov060_02116518. | volume (per-axis rand); billboard; life 8f; forever; #ff2900 |
| 0x09e | `kPtclFlameSmall` | existing | `src/game/actors/d_a_kpa_fire.cpp:504` | Existing name in include/ParticleIds.h (kept). Bowser's fire while airborne (mLanded 0). | point; billboard; life 8f; forever; #ffffff |
| 0x09f | `kPtclFlameOutPuff` | existing | `src/game/actors/d_a_kpa_fire.cpp:370 (+ d_a_obj_fire.cpp:139)` | Existing name in include/ParticleIds.h (kept). One-shot puff when a fire goes out. | circle rim, random; billboard; life 35f; emitter 30f; #393939 |
| 0x0a2 | `kPtclFlameInFlight` | medium | `src/game/actors/d_a_kpa_fire.cpp:642` | daKpaFire_c variant 2 (flame in flight) sub-variant 0 via data_ov060_02119358. | point; billboard; life 8f; forever; #ffffff |
| 0x0a3 | `kPtclFlameInFlightSecond` | medium | `src/game/actors/d_a_kpa_fire.cpp:859` | Second-handle partner of 0xa2 (kind+1). | volume (per-axis rand); billboard; life 8f; forever; #ff2900 |
| 0x0a4 | `kPtclFlameBlue` | existing | `src/game/actors/d_a_kpa_fire.cpp:837` | Existing name in include/ParticleIds.h (kept). Blue variant of Bowser's fire. | point; billboard; life 8f; forever; #39ffff |
| 0x0a5 | `kPtclFlameBlueSecond` | medium | `src/game/actors/d_a_kpa_fire.cpp:859` | Second-handle partner of kPtclFlameBlue (kind+1). | volume (per-axis rand); billboard; life 8f; forever; #393931 |
| 0x0a6 | `kPtclFlameBouncing` | medium | `src/game/actors/d_a_kpa_fire.cpp:745` | daKpaFire_c variant 4 (the bouncing flame) passes it to func_ov060_02116518. | point; billboard; life 1f; forever; #4adeff |
| 0x0a7 | `kPtclFlameBouncingSecond` | medium | `src/game/actors/d_a_kpa_fire.cpp:859` | Second-handle partner of 0xa6 (kind+1). | volume (per-axis rand); billboard; life 17f; forever; #10ac41 |
| 0x0a8 | `kPtclSpikeBombExplodeA` | high | `src/game/actors/d_a_kirai.cpp:121 (+1 more)` | daKirai_c::Explode spawns 0xa8-0xac together; letters record order. | circle rim, random; billboard; life 80f; emitter 4f; #ff7339 |
| 0x0a9 | `kPtclSpikeBombExplodeB` | high | `src/game/actors/d_a_kirai.cpp:122 (+1 more)` | Spike bomb explosion set member (see 0xa8). | disc (per-axis rand); billboard; life 80f; emitter 10f; #cd0000 |
| 0x0aa | `kPtclSpikeBombExplodeC` | high | `src/game/actors/d_a_kirai.cpp:123 (+1 more)` | Spike bomb explosion set member (see 0xa8). | point; billboard; life 11f; emitter 1f; #ffff6a |
| 0x0ab | `kPtclSpikeBombExplodeD` | high | `src/game/actors/d_a_kirai.cpp:124 (+1 more)` | Spike bomb explosion set member (see 0xa8). | disc (per-axis rand); billboard; life 35f; emitter 10f; #ffff00 |
| 0x0ac | `kPtclSpikeBombExplodeE` | high | `src/game/actors/d_a_kirai.cpp:125 (+1 more)` | Spike bomb explosion set member (see 0xa8). | point; spinning; life 14f; emitter 1f; #ffcd9c |
| 0x0ad | `kPtclBowserDefeatA` | medium | `src/game/actors/d_a_kpa.cpp:2571` | daKpa_c::func_ov060_021135fc spawns 0xad and 0xae as it spawns OBJ_KEY (Bowser beaten, key drop). | sphere surface; velocity-aligned; life 50f; emitter 1f; #ff9c9c |
| 0x0ae | `kPtclBowserDefeatB` | medium | `src/game/actors/d_a_kpa.cpp:2572` | Second member with 0xad. | volume (per-axis rand); billboard; life 60f; emitter 1f; #6a006a |
| 0x0b1 | `kPtclHugeLandingDust` | high | `src/game/actors/d_a_kuri_king.cpp:951 (+1 more)` | dActor_c::HugeLandingDust spawns it (through the helper decomp-named LandingDustAt, 90 units up); radius 8.0, 10 per burst, larger scale than 0xb2. Also the Goomboss shrink tick. | circle rim, even steps; billboard; life 30f; emitter 1f; #ffffff |
| 0x0b2 | `kPtclLandingDust` | existing | `src/runtime/actor/d_actor.cpp:1439 (+ d_a_btn, d_a_nknk, d_a_r_nk, d_a_spd, d_a_tgz x2, d_a_ply_methods:15417)` | dActor_c::LandingDust spawns it (through the helper decomp-named HugeLandingDustAt, 40 units up); radius 2.0, 8 per burst. Whomp, Koopa the Quick, Scuttlebug, daTgz_c and the player spawn it directly. | circle rim, even steps; billboard; life 25f; emitter 1f; #ffffff |
| 0x0b3 | `kPtclGroundPuff` | existing | `src/runtime/actor/d_actor.cpp:1408` | Existing name in include/ParticleIds.h (kept). Ground puff 25 units up; player non-Mega landing dust. | circle rim, even steps; billboard; life 18f; emitter 1f; #ffffff |
| 0x0b4 | `kPtclKeyTrailA` | medium | `src/game/actors/d_a_obj_key.cpp:452` | OBJ_KEY local FX_TRAIL_A, renewed in StateFlyToCenter. | volume (per-axis rand); billboard; life 50f; forever; #ffffd5 |
| 0x0b5 | `kPtclKeyTrailB` | medium | `src/game/actors/d_a_obj_key.cpp:453` | OBJ_KEY local FX_TRAIL_B. | point; spinning; life 35f; forever; #ffff5a |
| 0x0b6 | `kPtclKeyTrailC` | medium | `src/game/actors/d_a_obj_key.cpp:454` | OBJ_KEY local FX_TRAIL_C. | point; billboard; life 4f; forever; #62ff00 |
| 0x0b7 | `kPtclLavaBubbles` | medium | `src/game/actors/d_a_obj_lava.cpp:54` | daObjLava_c (file comment 'Popping lava bubbles') renews it at the closest player, kept over lava by checkYoganCallback_c. | disc (per-axis rand); billboard; life 24f; forever; #ff2000 |
| 0x0b8 | (candidate `kPtclClimbTreeBattlefield`) | low | `src/game/player/d_a_ply_methods.cpp:8465` | St_Climb_Main -> func_ov002_020cb474 picks it on sublevel 6 (read as Bob-omb Battlefield, inferred); olive spinning spheres. | sphere surface; spinning; life 50f; forever; #6a7b00 |
| 0x0b9 | `kPtclClimbTree` | medium | `src/game/player/d_a_ply_methods.cpp:8470` | func_ov002_020cb474 default (not sublevel 6/0xa/0x13/0x10), renewed 50 units up while climbing; green spinning spheres. | sphere surface; spinning; life 50f; forever; #207b00 |
| 0x0ba | `kPtclClimbTreeSnow` | medium | `src/game/player/d_a_ply_methods.cpp:8462` | func_ov002_020cb474 on sublevels 0xa/0x13, the same two that get snow weather 0x112; white volume. | volume (per-axis rand); billboard; life 30f; forever; #ffffff |
| 0x0bb | `kPtclUpdownLiftBreak` | high | `src/game/actors/d_a_obj_rotate_updown_lift.cpp:184` | ROTATE_UPDOWN_LIFT Kill. | disc (per-axis rand); spinning; life 30f; emitter 4f; #de9c20 |
| 0x0bc | `kPtclUnderwaterAmbient` | medium | `src/engine/view/d_camera.cpp:325` | dCamera_c::Behavior weather on sublevels 8/0x12/0x15/0x1e (water courses), emitted only while the camera is under water. | volume (per-axis rand); billboard; life 80f; forever; #ffffff |
| 0x0bd | `kPtclPlayerBurnA` | high | `src/game/player/d_a_ply_methods.cpp:13453 (+1 more)` | St_BurnLava_Main / St_BurnFire_Main renew 0xbd (red) and 0xbe (dark) with the burn loop sound. | point; billboard; life 4f; forever; #ff2900 |
| 0x0be | `kPtclPlayerBurnB` | high | `src/game/player/d_a_ply_methods.cpp:13455 (+1 more)` | Second burn member (see 0xbd). | point; billboard; life 30f; forever; #313131 |
| 0x0bf | `kPtclAttackHitA` | high | `src/game/player/d_a_ply_methods.cpp:15649 (+1 more)` | func_ov002_020d8d10, the player's attack landing (attacker not Wario); also dEnemyBase_c::SpawnMegaCharParticles. | disc (per-axis rand); velocity-aligned; life 8f; emitter 1f; #ffff73 |
| 0x0c0 | `kPtclAttackHitB` | high | `src/game/player/d_a_ply_methods.cpp:15650 (+1 more)` | Second member with 0xbf. | point; billboard; life 4f; emitter 1f; #ff0000 |
| 0x0c1 | `kPtclElectrocuteA` | high | `src/game/player/d_a_ply_methods.cpp:13726` | St_Electrocute_Main renews 0xc1-0xc3. | sphere surface; velocity-aligned; life 2f; forever; #cdffff |
| 0x0c2 | `kPtclElectrocuteB` | high | `src/game/player/d_a_ply_methods.cpp:13727` | Electrocute set member (see 0xc1). | volume (per-axis rand); billboard; life 1f; forever; #ffffff |
| 0x0c3 | `kPtclElectrocuteC` | high | `src/game/player/d_a_ply_methods.cpp:13728` | Electrocute set member (see 0xc1). | point; billboard; life 1f; forever; #ffff6a |
| 0x0c4 | `kPtclBubble` | high | `src/engine/particle/Particle.cpp:379` | System::NewBubble with bubbleCallback_c (ROM RTTI): particles rise and spawn a ripple at the surface. | volume (per-axis rand); billboard; life 150f; forever; #ffffff |
| 0x0c5 | `kPtclBalloonPowerA` | medium | `src/game/player/d_a_ply_methods.cpp:18902 (+1 more)` | InitBalloonMario and the balloon end (EndPowerupMusic) spawn 0xc5+0xc6 80 units up. | volume (per-axis rand); billboard; life 50f; emitter 1f; #ffffff |
| 0x0c6 | `kPtclBalloonPowerB` | medium | `src/game/player/d_a_ply_methods.cpp:18903 (+1 more)` | Second member with 0xc5. | circle rim, even steps; billboard; life 24f; emitter 1f; #ffffff |
| 0x0c7 | `kPtclStuckInGround` | medium | `src/game/player/d_a_ply_methods.cpp:5133` | daPly_c::func_ov002_020c5444 (St_StuckInGround) for mStateArg other than 0/1. | disc (per-axis rand); billboard; life 30f; emitter 2f; #ffffff |
| 0x0c8 | `kPtclWallBonk` | medium | `src/game/player/d_a_ply_methods.cpp:3410` | func_ov002_020c1eb4 aimed away from the wall when the player hits a wall fast (shell, cannon shot). | circle rim, even steps; billboard; life 30f; emitter 1f; #ffff08 |
| 0x0c9 | `kPtclPlayerHurtA` | medium | `src/game/player/d_a_ply_methods.cpp:15782` | daPly_c::Hurt, 80 units up, when mHurtDamage != 1; paired with 0xca. | circle rim, even steps; billboard; life 25f; emitter 1f; #ffbd20 |
| 0x0ca | `kPtclPlayerHurtB` | medium | `src/game/player/d_a_ply_methods.cpp:15783` | Second member with 0xc9. | point; billboard; life 4f; emitter 1f; #ff9439 |
| 0x0cb | `kPtclPlayerHurtOneA` | medium | `src/game/player/d_a_ply_methods.cpp:15779` | daPly_c::Hurt when mHurtDamage == 1; table entry equals 0xc9. | circle rim, even steps; billboard; life 25f; emitter 1f; #ffbd20 |
| 0x0cc | `kPtclPlayerHurtOneB` | medium | `src/game/player/d_a_ply_methods.cpp:15780` | Second member with 0xcb; table entry equals 0xca. | point; billboard; life 4f; emitter 1f; #ff9439 |
| 0x0cd | (candidate `kPtclSwimStroke`) | low | `src/game/player/d_a_ply_methods.cpp:9609` | St_Swim_Main renews it 50 units up while mJumpComboTimer != 0, with checkWaterCallback (dies above water). | volume (per-axis rand); billboard; life 90f; forever; #ffffff |
| 0x0ce | `kPtclWaterEntryRaised` | medium | `src/game/player/d_a_ply_methods.cpp:10066` | func_ov002_020ce9c8 (entering water), non-Mega, above the surface (fitWater offset). | point; billboard; life 8f; emitter 1f; #ffffff |
| 0x0cf | `kPtclWaterEntrySurface` | medium | `src/game/player/d_a_ply_methods.cpp:10072` | func_ov002_020ce9c8, non-Mega, at the surface (fitWaterCallback). | point; spinning; life 20f; emitter 12f; #ffffff |
| 0x0d0 | (candidate `kPtclFlyHand`) | low | `src/game/player/d_a_ply_methods.cpp:19641 (+1 more)` | St_Fly_Main renews it at hand bones 14 and 11 (mParticle2/3). | point; velocity-aligned; life 16f; forever; #ffffff |
| 0x0d1 | (candidate `kPtclFlyFast`) | low | `src/game/player/d_a_ply_methods.cpp:19622` | St_Fly_Main renews it at the player while mPrevVertSpeed >= 0x28000. | point; billboard; life 12f; forever; #ffffff |
| 0x0d2 | `kPtclItemVanish` | medium | `src/game/actors/d_a_1up.cpp:1082 (+5 more)` | 1-Up/coin CleanupResources (local PTCL_TYPE_11_12_CLEANUP), feather lifetime end, 1UpLogo (local kPopParticle) and Number falling below their start. | sphere surface; billboard; life 8f; emitter 6f; #ffee62 |
| 0x0d3 | `kPtclGroundPound` | high | `src/game/player/d_a_ply_methods.cpp:18498` | St_GroundPound_Main, Mario/Luigi/Yoshi branch (not Mega, param1 != 2). | circle rim, even steps; billboard; life 12f; emitter 1f; #ffff08 |
| 0x0d4 | `kPtclGroundPoundDryA` | high | `src/game/player/d_a_ply_methods.cpp:18504` | Same branch, only out of shallow water, 60 units up (with 0xd5). | circle rim, even steps; billboard; life 35f; emitter 1f; #ffffff |
| 0x0d5 | `kPtclGroundPoundDryB` | high | `src/game/player/d_a_ply_methods.cpp:18505` | Second dry-ground member (see 0xd4). | point; spinning; life 8f; emitter 1f; #ffffff |
| 0x0d6 | `kPtclDizzyStars` | medium | `src/game/player/d_a_ply_methods.cpp:16113` | func_ov002_020d98b4 at the head bone, called from St_DizzyStars_Main and St_Hurt_Main. | circle rim, even steps; billboard; life 21f; forever; #ffff29 |
| 0x0d7 | (candidate `kPtclGroundPuffWater`) | low | `src/game/player/d_a_ply_methods.cpp:9205` | St_MetalWaterWater_Main, non-Mega counterpart of 0x5c, 50 units up. | circle rim, even steps; billboard; life 40f; emitter 1f; #e6ffff |
| 0x0d8 | (candidate `kPtclStuckInGroundArg1A`) | low | `src/game/player/d_a_ply_methods.cpp:5129` | func_ov002_020c5444 when mStateArg == 1; sandy colour (#ffee94). | disc (per-axis rand); billboard; life 20f; emitter 3f; #ffee94 |
| 0x0d9 | (candidate `kPtclStuckInGroundArg1B`) | low | `src/game/player/d_a_ply_methods.cpp:5130` | Second member with 0xd8. | circle rim, even steps; billboard; life 25f; emitter 1f; #ffee8b |
| 0x0da | `kPtclRunningDust` | high | `src/engine/particle/Particle.cpp:336` | Particle::RunningSlidingDustAt (non-Mega running/sliding). | circle rim, random; billboard; life 20f; forever; #ffffff |
| 0x0db | (candidate `kPtclStuckInGroundArg0A`) | low | `src/game/player/d_a_ply_methods.cpp:5124` | func_ov002_020c5444 when mStateArg == 0; white. | disc (per-axis rand); billboard; life 20f; emitter 3f; #ffffff |
| 0x0dc | (candidate `kPtclStuckInGroundArg0B`) | low | `src/game/player/d_a_ply_methods.cpp:5125` | Second member with 0xdb. | circle rim, even steps; billboard; life 30f; emitter 1f; #ffffff |
| 0x0dd | `kPtclBigSplash` | high | `src/engine/particle/Particle.cpp:357` | Particle::System::NewBigSplash with splashCallback_c (ROM RTTI); the player's water-entry splash. | disc (per-axis rand); billboard; life 39f; emitter 1f; #ffffff |
| 0x0de | `kPtclShallowWaterLand` | medium | `src/game/player/d_a_ply_methods.cpp:11879 (+1 more)` | PlayLandingEffects in shallow water and func_ov002_020d1f78 (carry step), checkWaterRipple wrapper. | disc (per-axis rand); billboard; life 12f; emitter 1f; #ffffff |
| 0x0df | `kPtclSplash` | medium | `src/engine/particle/Particle.cpp:369` | System::NewSplash (splashCallback): player crossing the shallow-water edge in the air; MIPS fleeing. | disc (per-axis rand); billboard; life 30f; emitter 1f; #ffffff |
| 0x0e0 | `kPtclShellRideTrail` | medium | `src/game/actors/d_a_shl.cpp:617` | daShl_c default per-frame effect under a ridden shell. | volume (per-axis rand); billboard; life 18f; forever; #e6ffff |
| 0x0e1 | `kPtclShellRideLava` | medium | `src/game/actors/d_a_shl.cpp:592` | daShl_c on lava/other surface, 10 units above ground. | circle rim, random; billboard; life 20f; forever; #ff0000 |
| 0x0e2 | `kPtclShellRideWaterA` | medium | `src/game/actors/d_a_shl.cpp:566 (+1 more)` | daShl_c on water (checkWaterRipple wrapper), with 0xe3. | circle rim, random; billboard; life 20f; forever; #ffffff |
| 0x0e3 | `kPtclShellRideWaterB` | medium | `src/game/actors/d_a_shl.cpp:567 (+1 more)` | Second on-water member (see 0xe2). | point; billboard; life 5f; forever; #ffffff |
| 0x0e4 | (candidate `kPtclLandGroundType8`) | low | `src/game/player/d_a_ply_methods.cpp:13282` | PlayLandingEffects / func_ov002_020bf5e0 / func_ov002_020d1f78 pick it for mGroundSoundType 8 (pale yellow; sand?). | disc (per-axis rand); billboard; life 10f; emitter 1f; #fff6a4 |
| 0x0e5 | (candidate `kPtclLandGroundType6`) | low | `src/game/player/d_a_ply_methods.cpp:13284` | Same pick for mGroundSoundType 6 (white; snow?). | disc (per-axis rand); billboard; life 10f; emitter 1f; #ffffff |
| 0x0e6 | (candidate `kPtclLandGroundType9`) | low | `src/game/player/d_a_ply_methods.cpp:13280` | Same pick for mGroundSoundType 9 (green spinning; grass/leaves?). | disc (per-axis rand); spinning; life 35f; emitter 1f; #9cac29 |
| 0x0e7 | `kPtclCurrentFlow` | medium | `src/game/player/d_a_ply_methods.cpp:2250` | func_ov002_020bf800 from func_ov002_020bfa74 ('the flow particle') on CLPS surface 0xb (current/conveyor). | volume (per-axis rand); velocity-aligned; life 14f; forever; #ffffff |
| 0x0e8 | `kPtclWaterSurfaceMove` | high | `src/game/actors/d_a_mip.cpp:805 (+2 more)` | func_ov002_020ce8bc, non-Mega, moving: mWaterMoveParticle; MIPS reuses it. | point; spinning; life 12f; forever; #ffffff |
| 0x0e9 | `kPtclWaterSurfaceStill` | high | `src/game/player/d_a_ply_methods.cpp:10049` | func_ov002_020ce8bc, non-Mega, still: mWaterStillParticle. | point; spinning; life 30f; forever; #ffffff |
| 0x0ea | `kPtclSkeeterRipple` | high | `src/game/actors/d_a_menbo.cpp:178` | daMenbo_c (Skeeter): one ripple under each of four leg bones (mRippleParticles). | point; spinning; life 2f; forever; #ffffff |
| 0x0eb | `kPtclWaterfallMistCourse21` | high | `src/game/actors/d_a_obj_waterfall.cpp:60` | daObjWaterfall_c picks it on LEVEL_ID 0x21. | disc (per-axis rand); billboard; life 16f; forever; #ffffff |
| 0x0ec | `kPtclMg3DEspSparks` | medium | `src/minigames/d_mg3d_esp_anim_set.cpp:340` | dMg3DEspModel_c::Behavior renews it while mShowSparks is set. | disc (per-axis rand); billboard; life 60f; forever; #ffffff |
| 0x0ed | `kPtclMgJump3DJumpA` | medium | `src/minigames/d_mg_jump3d_mario.cpp:648` | dMgJump3DMario_c::StateMove spawns 0xed-0xef on a jump with its sound. | sphere surface; billboard; life 19f; emitter 1f; #ffffff |
| 0x0ee | `kPtclMgJump3DJumpB` | medium | `src/minigames/d_mg_jump3d_mario.cpp:649` | Set member (see 0xed). | point; billboard; life 6f; emitter 1f; #ffff08 |
| 0x0ef | `kPtclMgJump3DJumpC` | medium | `src/minigames/d_mg_jump3d_mario.cpp:650` | Set member (see 0xed). | circle rim, even steps; velocity-aligned; life 4f; emitter 1f; #ffff08 |
| 0x0f0 | (candidate `kPtclMgJumpTrail`) | low | `src/minigames/d_s_mg_jump2.cpp:383 (+1 more)` | dScMgJump2_c / dScMgTrampoline2_c::Behavior renew it (NewCleanParticle) at a fixed scene point. | disc (per-axis rand); billboard; life 120f; forever; #ffff00 |
| 0x0f1 | `kPtclMgSnowballHitA` | medium | `src/minigames/d_s_mg_snowball.cpp:609` | dScMgSnowball_c::UpdateSnowball spawns 0xf1+0xf2 at a ring hit. | disc (per-axis rand); billboard; life 20f; emitter 1f; #ffffff |
| 0x0f2 | `kPtclMgSnowballHitB` | medium | `src/minigames/d_s_mg_snowball.cpp:610` | Second member with 0xf1. | circle rim, even steps; billboard; life 33f; emitter 1f; #ffffff |
| 0x0f3 | `kPtclMgTrampolineBounceA` | medium | `src/minigames/d_mg_trmpln2_mario.cpp:539 (+1 more)` | Trampoline minigame Mario BounceOffLineA spawns 0xf3+0xf4 aimed off the line. | circle rim, even steps; billboard; life 15f; emitter 1f; #ffff08 |
| 0x0f4 | `kPtclMgTrampolineBounceB` | medium | `src/minigames/d_mg_trmpln2_mario.cpp:541 (+1 more)` | Second member with 0xf3. | point; billboard; life 15f; emitter 1f; #ff9c00 |
| 0x0f5 | `kPtclMgTrampolineTrail` | medium | `src/minigames/d_mg_trmpln2_mario.cpp:247 (+3 more)` | Trampoline minigame Mario Update renews it while the bounce animation rises fast. | point; billboard; life 36f; forever; #ffffff |
| 0x0f6 | `kPtclEnemyHit` | medium | `src/game/actors/d_a_kuri_king.cpp:1163` | d_a_ots (Bully) kParticleHit; Goomboss mode 0. | sphere surface; velocity-aligned; life 18f; emitter 1f; #ffffa4 |
| 0x0f9 | `kPtclFootstepDust` | medium | `src/game/actors/d_a_nknk.cpp:1088 (+1 more)` | Existing local name in d_a_r_nk.cpp (Koopa the Quick); also daNknk_c. | circle rim, random; billboard; life 20f; emitter 1f; #ffffff |
| 0x0fa | `kPtclTumbleBoxBreak` | medium | `src/game/actors/d_a_onms.cpp:265` | ONIMASU StateKnocked: the rolling crush box breaks on a wall. | disc (per-axis rand); billboard; life 60f; emitter 1f; #834141 |
| 0x0fb | `kPtclPiranhaDeathBurst` | high | `src/game/actors/d_a_f_pkn.cpp:192 (+1 more)` | daFPkn_c::SpawnDeathBurst; Piranha Plant StateDie. | sphere surface; billboard; life 20f; emitter 5f; #e6316a |
| 0x0fc | `kPtclPiranhaDeathSmokeA` | high | `src/game/actors/d_a_f_pkn.cpp:153 (+1 more)` | daFPkn_c::SpawnDeathSmoke (member name) with 0xfd. | volume (per-axis rand); billboard; life 9f; emitter 30f; #ffffff |
| 0x0fd | `kPtclPiranhaDeathSmokeB` | high | `src/game/actors/d_a_f_pkn.cpp:162 (+1 more)` | Second member with 0xfc. | volume (per-axis rand); billboard; life 9f; emitter 30f; #bddede |
| 0x0fe | `kPtclPiranhaWakeA` | medium | `src/game/actors/d_a_pkn.cpp:328 (+1 more)` | daPkn_c::StateSleep at mBubblePos on waking; also player func_ov002_020d2e74. | point; billboard; life 5f; emitter 1f; #ffffff |
| 0x0ff | `kPtclPiranhaWakeB` | medium | `src/game/actors/d_a_pkn.cpp:329 (+1 more)` | Second member with 0xfe. | sphere surface; billboard; life 15f; emitter 1f; #ffffff |
| 0x100 | `kPtclSnowmanBreathHit` | medium | `src/game/actors/d_a_snm_bth.cpp:90 (+1 more)` | SnowmanBreathParticle::HitPlayer / CheckSnowman. | volume (per-axis rand); billboard; life 12f; emitter 1f; #ffffff |
| 0x101 | `kPtclSlideDust` | medium | `src/game/actors/d_a_kpa.cpp:2233 (+1 more)` | PENGUIN_RACER slide (mDustParticle with the slide sound); also daKpa_c case 3. | circle rim, random; billboard; life 24f; forever; #ffffff |
| 0x102 | `kPtclPowerFlowerOpen` | high | `src/game/actors/d_a_obj_power_up_item.cpp:201` | POWER_UP_ITEM BeginOpening. | volume (per-axis rand); billboard; life 25f; emitter 3f; #ff7bc5 |
| 0x103 | `kPtclPowerFlowerLaunched` | medium | `src/game/actors/d_a_obj_power_up_item.cpp:226` | POWER_UP_ITEM StateLaunched renews it. | volume (per-axis rand); billboard; life 20f; forever; #ff83c5 |
| 0x104 | `kPtclPowerFlowerResting` | medium | `src/game/actors/d_a_obj_power_up_item.cpp:140` | POWER_UP_ITEM StateResting renews it. | sphere surface; billboard; life 15f; forever; #ff83c5 |
| 0x107 | `kPtclPillarCrash` | medium | `src/game/actors/d_a_obj_ki_hasira.cpp:184` | daObjKi_Hasira_c::Behavior spawns it 900 units ahead where the falling pillar lands. | circle rim, even steps; billboard; life 25f; emitter 1f; #9494b4 |
| 0x108 | `kPtclMegaMushroomTrail` | high | `src/game/actors/d_a_1up.cpp:228` | da1up_c for actor 277 (local PTCL_SCALEUP_KINOKO_TRAIL). | volume (per-axis rand); billboard; life 10f; forever; #ff5252 |
| 0x109 | `kPtclRipple` | high | `src/game/actors/d_a_tgz.cpp:696` | Particle::System::NewRipple (fitWaterSimpleCallback), spawned by the splash/bubble/checkWaterRipple callbacks; also daTgz_c. | point; spinning; life 5f; emitter 1f; #ffffff |
| 0x10a | `kPtclDossunbarBreak` | high | `src/game/actors/d_a_obj_bk_dossunbar.cpp:322` | BK_DOSSUNBAR Kill. | disc (per-axis rand); billboard; life 60f; emitter 1f; #7b7b7b |
| 0x10b | `kPtclClamLunge` | medium | `src/game/actors/d_a_obj_shell.cpp:174` | OBJ_SHELL (clam) Behavior when it lunges. | disc (per-axis rand); billboard; life 90f; emitter 20f; #ffffff |
| 0x10c | `kPtclClamShut` | medium | `src/game/actors/d_a_obj_shell.cpp:163` | OBJ_SHELL Behavior at frame 0x19 when it shuts. | circle rim, even steps; billboard; life 40f; emitter 1f; #cde6ff |
| 0x10d | (candidate `kPtclMipsGlow`) | low | `src/game/actors/d_a_mip.cpp:1314` | MIP (daMip_c::Behavior) renews it; look not verified. | volume (per-axis rand); billboard; life 28f; forever; #ffffff |
| 0x10e | `kPtclSilverStarTrail` | high | `src/game/actors/d_a_star.cpp:2502` | daStar_c: 'Silver star only: keeps trail effect 0x10e'. | volume (per-axis rand); billboard; life 28f; forever; #ffffee |
| 0x10f | `kPtclBigSnowmanHeadLandA` | high | `src/game/actors/d_a_bg_snm_hed.cpp:135` | BIG_SNOWMAN_HEAD lands with an earthquake: 0x10f+0x110. | disc (per-axis rand); billboard; life 30f; emitter 2f; #ffffff |
| 0x110 | `kPtclBigSnowmanHeadLandB` | high | `src/game/actors/d_a_bg_snm_hed.cpp:136` | Second member with 0x10f. | circle rim, even steps; billboard; life 45f; emitter 1f; #ffffff |
| 0x111 | `kPtclSnowmanBreath` | medium | `src/game/actors/d_a_snm_bth.cpp:167` | SnowmanBreathParticle::Render renews it. | point; billboard; life 1f; forever; #ffffff |
| 0x112 | `kPtclSnowfall` | medium | `src/engine/view/d_camera.cpp:315` | dCamera_c::Behavior weather on sublevels 0xa and 0x13 (the two snow courses, inferred). | disc (per-axis rand); billboard; life 120f; forever; #ffffff |
| 0x113 | `kPtclStarTrail` | medium | `src/game/actors/d_a_star.cpp:2403` | daStar_c: 'Keeps trail effect 0x113 13.0 above the star'. | volume (per-axis rand); billboard; life 28f; forever; #ffffd5 |
| 0x114 | `kPtclStarSparkle` | medium | `src/game/actors/d_a_star.cpp:2489` | daStar_c keeps it until mSparkleTimer runs out. | volume (per-axis rand); billboard; life 20f; forever; #ffffd5 |
| 0x115 | `kPtclStarCollectA` | medium | `src/game/actors/d_a_star.cpp:2432` | daStar_c 'collection effects' with 0x116. | volume (per-axis rand); billboard; life 45f; forever; #ffffd5 |
| 0x116 | `kPtclStarCollectB` | medium | `src/game/actors/d_a_star.cpp:2434` | Second member with 0x115. | volume (per-axis rand); billboard; life 45f; forever; #ffffd5 |
| 0x117 | `kPtclObjBlueFire` | high | `src/engine/particle/Particle.cpp:415` | System::NewBlueFlame, the OBJ_BLUE_FIRE flame (IsBlueFlame in d_a_obj_fire.cpp). | point; billboard; life 8f; forever; #ffffff |
| 0x118 | `kPtclObjRedFire` | high | `src/engine/particle/Particle.cpp:405` | System::NewRedFlame, the OBJ_RED_FIRE flame. | point; billboard; life 8f; forever; #ffffff |
| 0x119 | `kPtclBooCageFall` | medium | `src/game/actors/d_a_trs.cpp:3328` | T_BASKET (Boo cage) Behavior renews it while not on the ground. | sphere surface; billboard; life 10f; forever; #ff6a9c |
| 0x11a | `kPtclSnowmanRiseA` | medium | `src/game/actors/d_a_snowman.cpp:662` | daSnowman_c rising out of the snow, with 0x11b. | disc (per-axis rand); billboard; life 29f; emitter 3f; #ffffff |
| 0x11b | `kPtclSnowmanRiseB` | medium | `src/game/actors/d_a_snowman.cpp:665` | Second member with 0x11a. | circle rim, even steps; billboard; life 21f; emitter 1f; #ffffff |
| 0x11c | `kPtclSnowballBurst` | high | `src/game/actors/d_a_snowball.cpp:130` | daSnowball_c::HandleContact (local PTCL_SNOWBALL_BURST). | point; billboard; life 12f; emitter 1f; #ffffff |
| 0x11d | `kPtclSnowmanSinkA` | medium | `src/game/actors/d_a_snowman.cpp:476` | daSnowman_c sinking, with 0x11e. | disc (per-axis rand); billboard; life 16f; forever; #ffffff |
| 0x11e | `kPtclSnowmanSinkB` | medium | `src/game/actors/d_a_snowman.cpp:479` | Second member with 0x11d. | circle rim, random; billboard; life 15f; forever; #ffffff |
| 0x11f | `kPtclTornadoA` | medium | `src/game/actors/d_a_tor.cpp:191` | daTor_c::State1 renews 0x11f and 0x120 each frame. | disc (per-axis rand); billboard; life 18f; forever; #b46229 |
| 0x120 | `kPtclTornadoB` | medium | `src/game/actors/d_a_tor.cpp:192` | Second member with 0x11f. | circle rim, random; billboard; life 20f; forever; #ffe66a |
| 0x121 | `kPtclBkKabeBreak` | high | `src/game/actors/d_a_obj_bk_kabe.cpp:114 (+1 more)` | BK_KABE Kill (local kBreakParticle). | disc (per-axis rand); billboard; life 32f; emitter 1f; #392000 |
| 0x122 | `kPtclPoofDustA` | high | `src/minigames/d_mg3d_heyho_obj_adapter.cpp:1407 (+1 more)` | dActor_c::PoofDustAt spawns 0x122 then 0x123. | volume (per-axis rand); billboard; life 18f; emitter 1f; #ffffff |
| 0x123 | `kPtclPoofDustB` | high | `src/minigames/d_mg3d_heyho_obj_adapter.cpp:1409 (+1 more)` | Second member of PoofDustAt. | circle rim, even steps; billboard; life 18f; emitter 1f; #ffffff |
| 0x124 | `kPtclTriplePoofA` | existing | `src/runtime/actor/d_actor.cpp:1349 (+ d_a_eykn.cpp:463)` | Existing name in include/ParticleIds.h (kept). TriplePoofDustAt member 1. | volume (per-axis rand); billboard; life 40f; emitter 1f; #ffffff |
| 0x125 | `kPtclTriplePoofB` | existing | `src/runtime/actor/d_actor.cpp:1350 (+ d_a_eykn.cpp:464)` | Existing name in include/ParticleIds.h (kept). TriplePoofDustAt member 2. | circle rim, even steps; billboard; life 31f; emitter 1f; #ffffff |
| 0x126 | `kPtclTriplePoofC` | existing | `src/runtime/actor/d_actor.cpp:1351 (+ d_a_eykn.cpp:465)` | Existing name in include/ParticleIds.h (kept). TriplePoofDustAt member 3. | volume (per-axis rand); billboard; life 44f; emitter 1f; #ffffff |
| 0x127 | `kPtclDisappearPoofA` | high | `src/runtime/actor/d_actor.cpp:1373` | dActor_c::DisappearPoofDustAt spawns 0x127 then 0x128. | circle rim, even steps; billboard; life 10f; emitter 1f; #ffffff |
| 0x128 | `kPtclDisappearPoofB` | high | `src/runtime/actor/d_actor.cpp:1374` | Second member of DisappearPoofDustAt. | volume (per-axis rand); billboard; life 10f; emitter 1f; #ffffff |
| 0x129 | (candidate `kPtclVolcanoCannon`) | low | `src/game/actors/d_a_obj_volcano_cannon.cpp:139` | daObj_volcanoCannon_c::Behavior renews it; event unknown. | point; billboard; life 8f; forever; #ffffff |
| 0x12a | `kPtclAttackClashA` | high | `src/game/player/d_a_ply_methods.cpp:15516` | func_ov002_020d8944 at the midpoint when two players' attacks meet; sets mAttackClashed on both. | point; billboard; life 5f; emitter 1f; #6affb4 |
| 0x12b | `kPtclAttackClashB` | high | `src/game/player/d_a_ply_methods.cpp:15517` | Second member with 0x12a. | circle rim, even steps; billboard; life 8f; emitter 1f; #ffff73 |
| 0x12c | `kPtclStarMarkerCollectA` | high | `src/game/actors/d_a_star.cpp:2566` | daStarBase_c::Collect spawns 0x12c-0x12e with its sound. | sphere surface; spinning; life 36f; emitter 3f; #bddeff |
| 0x12d | `kPtclStarMarkerCollectB` | high | `src/game/actors/d_a_star.cpp:2567` | Set member (see 0x12c). | volume (per-axis rand); billboard; life 20f; emitter 1f; #bddeff |
| 0x12e | `kPtclStarMarkerCollectC` | high | `src/game/actors/d_a_star.cpp:2568` | Set member (see 0x12c). | disc (per-axis rand); billboard; life 35f; emitter 4f; #ff6a39 |
| 0x12f | `kPtclWarioAttackHitA` | high | `src/game/player/d_a_ply_methods.cpp:15652` | func_ov002_020d8d10 when the attacker's param1 == 2 (Wario): 0x12f-0x131 instead of 0xbf/0xc0. | disc (per-axis rand); velocity-aligned; life 8f; emitter 1f; #ffff41 |
| 0x130 | `kPtclWarioAttackHitB` | high | `src/game/player/d_a_ply_methods.cpp:15653` | Wario hit set member (see 0x12f). | volume (per-axis rand); billboard; life 10f; emitter 1f; #ffff9c |
| 0x131 | `kPtclWarioAttackHitC` | high | `src/game/player/d_a_ply_methods.cpp:15654` | Wario hit set member (see 0x12f). | point; billboard; life 4f; emitter 1f; #ff0000 |
| 0x132 | `kPtclSwingSpinNeg` | medium | `src/game/player/d_a_ply_methods.cpp:16412` | St_SwingPlayer_Main 60 units up while \|mAngleYSpeed\| >= 0xf99 and it is negative. | point; spinning; life 15f; forever; #ffff52 |
| 0x133 | `kPtclSwingSpinPos` | medium | `src/game/player/d_a_ply_methods.cpp:16418` | Same, when mAngleYSpeed > 0. | point; spinning; life 15f; forever; #ffff52 |
| 0x134 | `kPtclWarioGroundPoundA` | high | `src/game/player/d_a_ply_methods.cpp:18510` | St_GroundPound_Main, param1 == 2 (Wario) branch: 0x134+0x135. | circle rim, even steps; billboard; life 16f; emitter 1f; #ffff00 |
| 0x135 | `kPtclWarioGroundPoundB` | high | `src/game/player/d_a_ply_methods.cpp:18511` | Wario ground-pound member (see 0x134). | circle rim, even steps; velocity-aligned; life 12f; emitter 1f; #ffff00 |
| 0x136 | `kPtclWarioGroundPoundDryA` | high | `src/game/player/d_a_ply_methods.cpp:18517` | Wario branch, only out of shallow water, 50 units up (with 0x137). | circle rim, even steps; billboard; life 37f; emitter 1f; #ffffff |
| 0x137 | `kPtclWarioGroundPoundDryB` | high | `src/game/player/d_a_ply_methods.cpp:18518` | Second dry-ground member (see 0x136). | point; spinning; life 5f; emitter 1f; #ffcd9c |
| 0x138 | `kPtclWaterSpoutRings` | medium | `src/game/actors/d_a_water_hakidasi.cpp:142` | WATER_HAKIDASI State0 renews it for the spout that spawns WATER_RING actors. | disc (per-axis rand); billboard; life 50f; forever; #ffffff |
| 0x139 | `kPtclWaterTornadoSpray` | high | `src/game/actors/d_a_water_tatumaki.cpp:331 (+1 more)` | WATER_TATUMAKI local kSprayEffect. | circle rim, random; billboard; life 50f; forever; #ffffff |
| 0x13a | `kPtclDeathA` | medium | `src/game/actors/d_a_eykn.cpp:338 (+7 more)` | EnemyDeathC and Mr. I (local kDeathPtcl0), BlockS/Tatefuda break timers, Heyho, Pokey. | point; billboard; life 30f; forever; #313131 |
| 0x13b | `kPtclDeathB` | medium | `src/game/actors/d_a_eykn.cpp:340 (+7 more)` | Always with 0x13a, through NewCleanParticle (local kDeathPtcl1). | point; billboard; life 8f; forever; #ffffff |
| 0x13c | `kPtclYoshiFireA` | medium | `src/game/player/d_a_ply_methods.cpp:15021` | St_YoshiPower_Main renews 0x13c+0x13d along Yoshi's heading (Yoshi's fire breath). | point; billboard; life 35f; forever; #ffa431 |
| 0x13d | `kPtclYoshiFireB` | medium | `src/game/player/d_a_ply_methods.cpp:15023` | Second member with 0x13c. | point; billboard; life 10f; forever; #ffcd08 |
| 0x13e | `kPtclTongueHitA` | high | `src/game/player/d_a_ply_methods.cpp:14419 (+1 more)` | TongueWallProbe and the tongue grab spawn 0x13e+0x13f where Yoshi's tongue hits. | disc (per-axis rand); velocity-aligned; life 8f; emitter 1f; #deffff |
| 0x13f | `kPtclTongueHitB` | high | `src/game/player/d_a_ply_methods.cpp:14421 (+1 more)` | Second member with 0x13e. | point; billboard; life 4f; emitter 1f; #9c39ff |
| 0x140 | (candidate `kPtclVsFigureTrail`) | low | `src/game/stages/d_ent_obj.cpp:287` | UnknownVsPlayer keeps it on bone 15 in movement state 9; event unknown. | circle rim, random; velocity-aligned; life 20f; forever; #e6ffff |

## Unused IDs

No spawn found under any spelling.

| ID | Decoded (shape; draw; particle life; emitter; base colour) | Remark |
|---|---|---|
| 0x00f | disc (per-axis rand); billboard; 35f; emitter 1f; #393939 | dark one-shot; fits the 0x0e/0x0f break family |
| 0x011 | disc (per-axis rand); billboard; 17f; forever; #ffffff | |
| 0x012 | point; spinning; 90f; emitter 1f; #393939 | dark one-shot debris |
| 0x06a | sphere surface; billboard; 7f; forever; #deeeff | |
| 0x06b | circle rim, random; velocity-aligned; 6f; forever; #ffffff | byte-like pair with 0x06c |
| 0x06c | circle rim, random; velocity-aligned; 6f; forever; #ffffff | |
| 0x079 | disc (per-axis rand); billboard; 25f; forever; #ffffff | sits between the ice sets |
| 0x07a | point; spinning; 20f; forever; #ffffff | |
| 0x08e | volume (per-axis rand); billboard; 40f; emitter 1f; #ffffff | next to KillerDai 0x8d |
| 0x0a0 | point; billboard; 8f; forever; #ffffff | same recipe as the fire pairs (white point + coloured volume) |
| 0x0a1 | volume (per-axis rand); billboard; 12f; forever; #ff2900 | red partner of 0xa0, the kind+1 pattern |
| 0x0af | point; billboard; 8f; forever; #ffffff | fire-pair recipe again |
| 0x0b0 | volume (per-axis rand); billboard; 14f; forever; #009cff | blue partner of 0xaf |
| 0x0f7 | disc (per-axis rand); billboard; 14f; forever; #ffffff | |
| 0x0f8 | point; spinning; 40f; emitter 1f; #393939 | dark one-shot debris |
| 0x105 | point; billboard; 8f; forever; #ffffff | fire-pair recipe again |
| 0x106 | volume (per-axis rand); billboard; 7f; forever; #ff4100 | orange partner of 0x105 |

0xa0/0xa1, 0xaf/0xb0 and 0x105/0x106 share the white-point plus coloured-volume
recipe of daKpaFire_c's kind / kind+1 flame pairs; possibly cut flame kinds.
Not proven.

## Follow-ups

- `dActor_c::HugeLandingDustAt` and `dActor_c::LandingDustAt` (decomp names)
  look swapped: `HugeLandingDust()` calls `LandingDustAt`, which spawns the
  larger 0xb1, and `LandingDust()` calls `HugeLandingDustAt`, which spawns
  0xb2. Renaming them is a symbol change, not done here.
- File-local constants for IDs that now have names (`kDustEffect` in d_a_dgr,
  `FX_*` in d_a_obj_key, `kBurstParticle`/`kSmokeParticle` in d_a_klr,
  `kDeathPtcl0/1` in d_a_eykn, `kSprayEffect`, `kBreakParticle`,
  `PTCL_*` in d_a_1up and d_a_snowball, ...) could be retired for the shared
  names.
