#ifndef PARTICLE_IDS_H
#define PARTICLE_IDS_H

/* Effect IDs: indices into the SPA particle resource that arm9 embeds
 * (data_02075f14, 321 definitions). The resource carries no names, so an ID
 * is named for the code that spawns it -- the actor and the event -- and
 * takes a word for its look (Flame, Smoke, Ripple, Splash, Sparkle...) only
 * where a ROM callback class, an existing member name or a runtime check
 * backs it. The comment after each name is its first spawner;
 * notes/particle-ids.md has the evidence per ID, and lists the IDs left as
 * bare hex (unused, or the spawning context is still unknown).
 * tools/spa_dump.py prints a definition and tools/particle_id_sites.py lists
 * every literal that spawns it.
 *
 * Conventions: effects spawned together share a stem and take A/B/C in spawn
 * order; Mega and Wario variants of a player effect take that prefix; Dry
 * marks the member spawned only out of shallow water; minigame effects take
 * Mg.
 *
 * Evidence for the original nine, all on texture 40 and billboards:
 *  - kPtclLandingDust / kPtclHugeLandingDust: dActor_c::LandingDust spawns
 *    0xb2 (circle rim radius 2.0, 8 per burst, 25 frames) and
 *    dActor_c::HugeLandingDust spawns 0xb1 (radius 8.0, 10 per burst, larger
 *    scale, 30 frames), each through a *At helper whose decomp name is the
 *    other one's: HugeLandingDustAt spawns 0xb2.
 *  - kPtclGroundPuff: emitter on a circle rim, radius 1.0, 18 frames.  func_0200fc44 spawns it 25.0
 *    above a (raycast) ground point.  Seen at runtime on Mario's landing
 *    (daPly_c::St_Land_Init); the non-Mega player's hurt, thrown and
 *    punch/kick states reach it through the same helper, all skipped in
 *    shallow water (Mega gets kPtclMegaGroundPuff instead).
 *  - kPtclFlame, kPtclFlameSmall, kPtclFlameBlue: daKpaFire_c's flames.  Spawned
 *    with NewSimple at a player's feet in melonDS (no actor, so unscaled): 0x9c
 *    draws a full red-orange flame with a yellow-white core, 0x9e a much
 *    smaller one, 0xa4 the same shape in blue-green.  0xa4's table base
 *    colour is cyan.  daKpaFire_c starts kind+1 in a second handle beside
 *    each kind; those partners take ...Second.
 *  - kPtclFlameOutPuff: emitter on a circle rim, radius 0.5, 35 frames, base colour dark grey on
 *    the same puff texture 40.  Spawned where a flame ends: daKpaFire_c's
 *    variants 6/7 and daObjFire_c when the flame is being eaten.
 *  - kPtclTriplePoof*: dActor_c::TriplePoofDustAt spawns the three in order.
 *    Their letter suffixes only record that order. */
enum ParticleId {
    kPtclOneUpTrail            = 0x000, /* da1up_c::func_ov002_020aeee4 */
    kPtclAbukuBubble           = 0x001, /* daObjAbuku_c::Behavior */
    kPtclAbukuPop              = 0x002, /* daObjAbuku_c::Pop */
    kPtclWhompPounded          = 0x003, /* daBtn_c::func_ov079_02124ed4 */
    kPtclWhompKingDefeat       = 0x004, /* daBtn_c::func_ov079_021246dc */
    kPtclWhompDefeat           = 0x005, /* daBtn_c::func_ov079_021246dc */
    kPtclBattaBlockBreak       = 0x006, /* daBttBk_c::HandleCollision */
    kPtclCannonShotA           = 0x007, /* daCnn_c::StateFire */
    kPtclCannonShotB           = 0x008, /* daCnn_c::StateFire */
    kPtclCapBlockBreak         = 0x009, /* daObjHatenaBlock_c::func_ov102_021498e0 */
    kPtclBgActorBreak          = 0x00a, /* daObjEwbIce_c::Kill */
    kPtclBlockBreakA           = 0x00b, /* daObjHatenaBlock_c::func_ov102_021498e0 */
    kPtclHatenaBlockBreak      = 0x00c, /* daObjHatenaBlock_c::func_ov102_021498e0 */
    kPtclBlockBreakB           = 0x00d, /* daObjHatenaBlock_c::func_ov102_021498e0 */
    kPtclItemBlockBreak        = 0x010, /* daObjHatenaBlock_c::func_ov102_021498e0 */
    kPtclBobOmbCarried         = 0x013, /* daBmb_c::func_ov102_0214b53c */
    kPtclBobOmbExplodeA        = 0x014, /* func_ov102_0214ae1c */
    kPtclBobOmbExplodeB        = 0x015, /* func_ov102_0214ae1c */
    kPtclBobOmbExplodeC        = 0x016, /* func_ov102_0214ae1c */
    kPtclBobOmbExplodeD        = 0x017, /* func_ov102_0214ae1c */
    kPtclBobOmbExplodeE        = 0x018, /* func_ov102_0214ae1c */
    kPtclBobOmbFuse            = 0x019, /* daBmb_c::func_ov102_0214b53c */
    kPtclKingBobOmbDefeat      = 0x01a, /* daBombking_c::func_ov078_021238ac */
    kPtclBossLandingDust       = 0x01b, /* func_0200fa04 */
    kPtclWaterBombBurstA       = 0x01c, /* daWbm_c::func_ov098_0213b6e0 */
    kPtclWaterBombBurstB       = 0x01d, /* daWbm_c::func_ov098_0213b6e0 */
    kPtclChompGateBreak        = 0x01e, /* daObjWanwanShutter_c::Break */
    kPtclPyramidTopSunkA       = 0x01f, /* daObjDlPyramid_c::FinishDescent */
    kPtclPyramidTopSunkB       = 0x020, /* daObjDlPyramid_c::FinishDescent */
    kPtclPyramidTopSunkC       = 0x021, /* daObjDlPyramid_c::FinishDescent */
    kPtclPyramidTopSunkD       = 0x022, /* daObjDlPyramid_c::FinishDescent */
    kPtclPyramidTopSinking     = 0x023, /* daObjDlPyramid_c::Descend */
    kPtclWaterfallMist         = 0x024, /* daObjWaterfall_c::InitResources */
    kPtclChairBreakA           = 0x025, /* daChair_c::Break */
    kPtclChairBreakB           = 0x026, /* daChair_c::Break */
    kPtclDrainLidPounded       = 0x028, /* daObjTtFuta_c::OnGroundPounded */
    kPtclMontyMoleEmerge       = 0x029, /* daChoropu_c::func_ov080_02123c24 */
    kPtclMontyMoleLeapA        = 0x02a, /* daChoropu_c::func_ov080_02123860 */
    kPtclMontyMoleLeapB        = 0x02b, /* daChoropu_c::func_ov080_02123860 */
    kPtclYoshiEggTrail         = 0x02c, /* daYegg_c::func_ov002_020ecb0c */
    kPtclSpindelDust           = 0x02d, /* daDgr_c::Behavior */
    kPtclGrindelLanding        = 0x02e, /* daDkk_c::StateFall */
    kPtclStarPose              = 0x02f, /* daStar_c::func_ov002_020e7fcc */
    kPtclStarPoseEnd           = 0x030, /* daStar_c::func_ov002_020e7fcc */
    kPtclDemoSpinA             = 0x031, /* daDemo_c::func_ov002_020f72bc */
    kPtclDemoSpinB             = 0x032, /* daDemo_c::func_ov002_020f72bc */
    kPtclDemoSpinC             = 0x033, /* daDemo_c::func_ov002_020f72bc */
    kPtclJumboStarFlyA         = 0x034, /* daPly_c::func_ov002_020c37a4 */
    kPtclJumboStarFlyB         = 0x035, /* daPly_c::func_ov002_020c37a4 */
    kPtclDemoFadeInA           = 0x038, /* daDemo_c::func_ov002_020f71c4 */
    kPtclDemoFadeInB           = 0x039, /* daDemo_c::func_ov002_020f71c4 */
    kPtclDemoFadeInC           = 0x03a, /* daDemo_c::func_ov002_020f71c4 */
    kPtclDemoDescendA          = 0x03b, /* daDemo_c::func_ov002_020f7038 */
    kPtclDemoDescendB          = 0x03c, /* daDemo_c::func_ov002_020f7038 */
    kPtclDemoDescendC          = 0x03d, /* daDemo_c::func_ov002_020f7038 */
    kPtclEndingStarSparkle     = 0x03e, /* func_ov002_020f2790 */
    kPtclYoshiEggBreakA        = 0x03f, /* func_ov002_020edca4 */
    kPtclYoshiEggBreakB        = 0x040, /* func_ov002_020edca4 */
    kPtclYoshiEggBreakC        = 0x041, /* func_ov002_020edca4 */
    kPtclYoshiEggBreakD        = 0x042, /* func_ov002_020edca4 */
    kPtclKnockAwayA            = 0x043, /* daBrq_c::EnterDefeatedState */
    kPtclKnockAwayB            = 0x044, /* daBrq_c::EnterDefeatedState */
    kPtclMrIBeamBurst          = 0x045, /* daEyBm_c::SpawnDestroyEffect */
    kPtclMrIBeamA              = 0x046, /* daEyBm_c::Render */
    kPtclMrIBeamB              = 0x047, /* daEyBm_c::Render */
    kPtclStoneBlockBreak       = 0x048, /* daDsn_c::OnHitByMegaChar */
    kPtclFireSeaBlockBreak     = 0x049, /* daObjFallBlock_c::Kill */
    kPtclFeather               = 0x04a, /* daFeather_c::Behavior */
    kPtclFireBarA              = 0x04b, /* daObjFl_KomaU_c::Behavior */
    kPtclFireBarB              = 0x04c, /* daObjFl_KomaU_c::Behavior */
    kPtclLavaBubbleFlame       = 0x04d, /* daBbl_c::ExecuteJump */
    kPtclLavaBubbleSmoke       = 0x04e, /* daBbl_c::ExecuteJump */
    kPtclFlamethrowerBlueA     = 0x04f, /* Particle::System::NewFlamethrowerBlueA */
    kPtclFlamethrowerBlueB     = 0x050, /* Particle::System::NewFlamethrowerBlueB */
    kPtclFlamethrowerRedA      = 0x051, /* Particle::System::NewFlamethrowerRedA */
    kPtclFlamethrowerRedB      = 0x052, /* Particle::System::NewFlamethrowerRedB */
    kPtclHuwahuwaDefeat        = 0x053, /* daHuwa_c::KillWithCoins */
    kPtclMegaWaterEntryRaised  = 0x054, /* daBakubaku_c::func_ov032_02111830 */
    kPtclMegaWaterEntrySurface = 0x055, /* daBakubaku_c::func_ov032_02111830 */
    kPtclMegaSplash            = 0x056, /* daBakubaku_c::func_ov032_02111830 */
    kPtclMegaGroundPoundA      = 0x057, /* daPly_c::St_GroundPound_Main */
    kPtclMegaGroundPoundB      = 0x058, /* daPly_c::St_GroundPound_Main */
    kPtclMegaGroundPoundDryB   = 0x059, /* daPly_c::St_GroundPound_Main */
    kPtclMegaGroundPoundDryA   = 0x05a, /* daPly_c::St_GroundPound_Main */
    kPtclMegaGroundPuff        = 0x05b, /* daPly_c::func_ov002_020bf9d4 */
    kPtclMegaGroundPuffWater   = 0x05c, /* daPly_c::func_ov002_020bf9d4 */
    kPtclMegaGrowA             = 0x05d, /* daPly_c::func_ov002_020c8d14 */
    kPtclMegaGrowB             = 0x05e, /* daPly_c::func_ov002_020c8d14 */
    kPtclMegaGrowStartA        = 0x05f, /* daPly_c::func_ov002_020c9a04 */
    kPtclMegaGrowStartB        = 0x060, /* daPly_c::func_ov002_020c9a04 */
    kPtclMegaRunningDust       = 0x061, /* Particle::MegaRunningDustAt */
    kPtclHeavyStepRipple       = 0x062, /* daPly_c::func_ov002_020bf5e0 */
    kPtclHeavyStep             = 0x063, /* daPly_c::func_ov002_020bf5e0 */
    kPtclHeavyStepWater        = 0x064, /* daPly_c::func_ov002_020bf5e0 */
    kPtclMegaWaterSurfaceMove  = 0x065, /* daPly_c::func_ov002_020ce8bc */
    kPtclMegaWaterSurfaceStill = 0x066, /* daPly_c::func_ov002_020ce8bc */
    kPtclBigSteelieBreak       = 0x067, /* daGrock_c::HandleCylinderHit */
    kPtclBigSteelieBounce      = 0x068, /* daGrock_c::HandleMeshCollision */
    kPtclBigSteelieRollDust    = 0x069, /* daGrock_c::HandleMeshCollision */
    kPtclHatenaSwitchPressA    = 0x06d, /* daObjHatenaSwitch_c::Behavior */
    kPtclHatenaSwitchPressB    = 0x06e, /* daObjHatenaSwitch_c::Behavior */
    kPtclHatenaSwitchPressC    = 0x06f, /* daObjHatenaSwitch_c::Behavior */
    kPtclHatenaSwitchPressD    = 0x070, /* daObjHatenaSwitch_c::Behavior */
    kPtclWaterfallMistCourse16 = 0x071, /* daObjWaterfall_c::InitResources */
    kPtclFwooshGustA           = 0x072, /* daHyuhyu_c::StateBlowMain */
    kPtclFwooshGustB           = 0x073, /* daHyuhyu_c::StateBlowMain */
    kPtclIceBreakA             = 0x074, /* daObjIceBlock_c::Kill */
    kPtclIceBreakB             = 0x075, /* daObjIceBlock_c::Kill */
    kPtclIceBreakC             = 0x076, /* daObjIceBlock_c::Kill */
    kPtclWaterSpout            = 0x07b, /* daWater_Hakidasi_c::State0 */
    kPtclEyerokDefeatA         = 0x07c, /* daIwante_c::func_ov066_02116c6c */
    kPtclEyerokDefeatB         = 0x07d, /* daIwante_c::func_ov066_02116c6c */
    kPtclKleptoHurt            = 0x07e, /* daJango_c::func_ov062_0211b930 */
    kPtclKeronpaFireA          = 0x07f, /* daKpFr_c::Render */
    kPtclKeronpaFireB          = 0x080, /* daKpFr_c::Render */
    kPtclKeyFall               = 0x081, /* daObjKey_c::StateStarJump */
    kPtclKeyBurstA             = 0x082, /* daObjKey_c::Behavior */
    kPtclKeyBurstB             = 0x083, /* daObjKey_c::Behavior */
    kPtclChiefChillyHitA       = 0x084, /* daKing_Donketu_c::func_ov073_0211f494 */
    kPtclChiefChillyHitB       = 0x085, /* daKing_Donketu_c::func_ov073_0211f494 */
    kPtclChiefChillyStepA      = 0x088, /* daKing_Donketu_c::func_ov073_0211f2c0 */
    kPtclChiefChillyStepB      = 0x089, /* daKing_Donketu_c::func_ov073_0211f2c0 */
    kPtclChiefChillyHitPlayer  = 0x08a, /* daKing_Donketu_c::func_ov073_0211f61c */
    kPtclEwbIceSettleA         = 0x08b, /* daObjEwbIce_c::StateSettled_Enter */
    kPtclEwbIceSettleB         = 0x08c, /* daObjEwbIce_c::StateSettled_Enter */
    kPtclKillerDaiBreak        = 0x08d, /* daObjBkKillerdai_c::Kill */
    kPtclBulletBillBurst       = 0x08f, /* daKlr_c::Behavior */
    kPtclBulletBillSmoke       = 0x090, /* daKlr_c::StateFly */
    kPtclGoombossVanishA       = 0x091, /* daKuriKing_c::func_ov074_0211f5b8 */
    kPtclGoombossVanishB       = 0x092, /* daKuriKing_c::func_ov074_0211f5b8 */
    kPtclBooDefeatA            = 0x093, /* daTrs_c::func_ov063_021177b0 */
    kPtclBooDefeatB            = 0x094, /* daTrs_c::func_ov063_021177b0 */
    kPtclFlameScaled           = 0x09a, /* daKpaFire_c::func_ov060_02116d78 */
    kPtclFlameScaledSecond     = 0x09b, /* daKpaFire_c::func_ov060_02116518 */
    kPtclFlame                 = 0x09c, /* daKpaFire_c::func_ov060_02116d78 */
    kPtclFlameSecond           = 0x09d, /* daKpaFire_c::func_ov060_02116518 */
    kPtclFlameSmall            = 0x09e, /* daKpaFire_c */
    kPtclFlameOutPuff          = 0x09f, /* daKpaFire_c */
    kPtclFlameInFlight         = 0x0a2, /* daKpaFire_c::func_ov060_02116b68 */
    kPtclFlameInFlightSecond   = 0x0a3, /* daKpaFire_c::func_ov060_02116518 */
    kPtclFlameBlue             = 0x0a4, /* daKpaFire_c::func_ov060_02116518 */
    kPtclFlameBlueSecond       = 0x0a5, /* daKpaFire_c::func_ov060_02116518 */
    kPtclFlameBouncing         = 0x0a6, /* daKpaFire_c::func_ov060_021168c4 */
    kPtclFlameBouncingSecond   = 0x0a7, /* daKpaFire_c::func_ov060_02116518 */
    kPtclSpikeBombExplodeA     = 0x0a8, /* daKirai_c::Explode */
    kPtclSpikeBombExplodeB     = 0x0a9, /* daKirai_c::Explode */
    kPtclSpikeBombExplodeC     = 0x0aa, /* daKirai_c::Explode */
    kPtclSpikeBombExplodeD     = 0x0ab, /* daKirai_c::Explode */
    kPtclSpikeBombExplodeE     = 0x0ac, /* daKirai_c::Explode */
    kPtclBowserDefeatA         = 0x0ad, /* daKpa_c::func_ov060_021135fc */
    kPtclBowserDefeatB         = 0x0ae, /* daKpa_c::func_ov060_021135fc */
    kPtclHugeLandingDust       = 0x0b1, /* dActor_c::HugeLandingDust */
    kPtclLandingDust           = 0x0b2, /* dActor_c::LandingDust */
    kPtclGroundPuff            = 0x0b3, /* dActor_c::func_0200fc44 */
    kPtclKeyTrailA             = 0x0b4, /* daObjKey_c::StateFlyToCenter */
    kPtclKeyTrailB             = 0x0b5, /* daObjKey_c::StateFlyToCenter */
    kPtclKeyTrailC             = 0x0b6, /* daObjKey_c::StateFlyToCenter */
    kPtclLavaBubbles           = 0x0b7, /* daObjLava_c::Behavior */
    kPtclClimbTree             = 0x0b9, /* func_ov002_020cb474 */
    kPtclClimbTreeSnow         = 0x0ba, /* func_ov002_020cb474 */
    kPtclUpdownLiftBreak       = 0x0bb, /* daObjRotateUpdownLift_c::Kill */
    kPtclUnderwaterAmbient     = 0x0bc, /* dCamera_c::Behavior */
    kPtclPlayerBurnA           = 0x0bd, /* daPly_c::St_BurnLava_Main */
    kPtclPlayerBurnB           = 0x0be, /* daPly_c::St_BurnLava_Main */
    kPtclAttackHitA            = 0x0bf, /* func_ov002_020d8d10 */
    kPtclAttackHitB            = 0x0c0, /* func_ov002_020d8d10 */
    kPtclElectrocuteA          = 0x0c1, /* daPly_c::St_Electrocute_Main */
    kPtclElectrocuteB          = 0x0c2, /* daPly_c::St_Electrocute_Main */
    kPtclElectrocuteC          = 0x0c3, /* daPly_c::St_Electrocute_Main */
    kPtclBubble                = 0x0c4, /* Particle::System::NewBubble */
    kPtclBalloonPowerA         = 0x0c5, /* func_ov002_020de968 */
    kPtclBalloonPowerB         = 0x0c6, /* func_ov002_020de968 */
    kPtclStuckInGround         = 0x0c7, /* daPly_c::func_ov002_020c5444 */
    kPtclWallBonk              = 0x0c8, /* daPly_c::func_ov002_020c1eb4 */
    kPtclPlayerHurtA           = 0x0c9, /* daPly_c::Hurt */
    kPtclPlayerHurtB           = 0x0ca, /* daPly_c::Hurt */
    kPtclPlayerHurtOneA        = 0x0cb, /* daPly_c::Hurt */
    kPtclPlayerHurtOneB        = 0x0cc, /* daPly_c::Hurt */
    kPtclWaterEntryRaised      = 0x0ce, /* daPly_c::func_ov002_020ce9c8 */
    kPtclWaterEntrySurface     = 0x0cf, /* daPly_c::func_ov002_020ce9c8 */
    kPtclItemVanish            = 0x0d2, /* da1up_c::CleanupResources */
    kPtclGroundPound           = 0x0d3, /* daPly_c::St_GroundPound_Main */
    kPtclGroundPoundDryA       = 0x0d4, /* daPly_c::St_GroundPound_Main */
    kPtclGroundPoundDryB       = 0x0d5, /* daPly_c::St_GroundPound_Main */
    kPtclDizzyStars            = 0x0d6, /* daPly_c::func_ov002_020d98b4 */
    kPtclRunningDust           = 0x0da, /* Particle::RunningSlidingDustAt */
    kPtclBigSplash             = 0x0dd, /* Particle::System::NewBigSplash */
    kPtclShallowWaterLand      = 0x0de, /* func_ov002_020d1f78 */
    kPtclSplash                = 0x0df, /* Particle::System::NewSplash */
    kPtclShellRideTrail        = 0x0e0, /* daShl_c::func_ov102_0214c84c */
    kPtclShellRideLava         = 0x0e1, /* daShl_c::func_ov102_0214c84c */
    kPtclShellRideWaterA       = 0x0e2, /* daShl_c::func_ov102_0214c84c */
    kPtclShellRideWaterB       = 0x0e3, /* daShl_c::func_ov102_0214c84c */
    kPtclCurrentFlow           = 0x0e7, /* func_ov002_020bf800 */
    kPtclWaterSurfaceMove      = 0x0e8, /* daMip_c::StateFleeMain */
    kPtclWaterSurfaceStill     = 0x0e9, /* daPly_c::func_ov002_020ce8bc */
    kPtclSkeeterRipple         = 0x0ea, /* func_ov090_02130f94 */
    kPtclWaterfallMistCourse21 = 0x0eb, /* daObjWaterfall_c::InitResources */
    kPtclMg3DEspSparks         = 0x0ec, /* dMg3DEspModel_c::Behavior */
    kPtclMgJump3DJumpA         = 0x0ed, /* dMgJump3DMario_c::StateMove */
    kPtclMgJump3DJumpB         = 0x0ee, /* dMgJump3DMario_c::StateMove */
    kPtclMgJump3DJumpC         = 0x0ef, /* dMgJump3DMario_c::StateMove */
    kPtclMgSnowballHitA        = 0x0f1, /* dScMgSnowball_c::UpdateSnowball */
    kPtclMgSnowballHitB        = 0x0f2, /* dScMgSnowball_c::UpdateSnowball */
    kPtclMgTrampolineBounceA   = 0x0f3, /* Trmp2Mario_BounceOffLineA */
    kPtclMgTrampolineBounceB   = 0x0f4, /* Trmp2Mario_BounceOffLineA */
    kPtclMgTrampolineTrail     = 0x0f5, /* Trmp2Mario_Update */
    kPtclEnemyHit              = 0x0f6, /* daKuriKing_c::func_ov074_02120bb8 */
    kPtclFootstepDust          = 0x0f9, /* daNknk_c::func_ov062_02117724 */
    kPtclTumbleBoxBreak        = 0x0fa, /* daOnms_c::StateKnocked */
    kPtclPiranhaDeathBurst     = 0x0fb, /* daFPkn_c::SpawnDeathBurst */
    kPtclPiranhaDeathSmokeA    = 0x0fc, /* daFPkn_c::SpawnDeathSmoke */
    kPtclPiranhaDeathSmokeB    = 0x0fd, /* daFPkn_c::SpawnDeathSmoke */
    kPtclPiranhaWakeA          = 0x0fe, /* daPkn_c::StateSleep */
    kPtclPiranhaWakeB          = 0x0ff, /* daPkn_c::StateSleep */
    kPtclSnowmanBreathHit      = 0x100, /* SnowmanBreathParticle::HitPlayer */
    kPtclSlideDust             = 0x101, /* daKpa_c::func_ov060_02113d8c */
    kPtclPowerFlowerOpen       = 0x102, /* daObjPowerUpItem_c::BeginOpening */
    kPtclPowerFlowerLaunched   = 0x103, /* daObjPowerUpItem_c::StateLaunched */
    kPtclPowerFlowerResting    = 0x104, /* daObjPowerUpItem_c::StateResting */
    kPtclPillarCrash           = 0x107, /* daObjKi_Hasira_c::Behavior */
    kPtclMegaMushroomTrail     = 0x108, /* da1up_c::func_ov002_020aeee4 */
    kPtclRipple                = 0x109, /* daTgz_c::func_ov077_02125bb4 */
    kPtclDossunbarBreak        = 0x10a, /* daObjBk_Dossunbar_c::Kill */
    kPtclClamLunge             = 0x10b, /* daObjShell_c::Behavior */
    kPtclClamShut              = 0x10c, /* daObjShell_c::Behavior */
    kPtclSilverStarTrail       = 0x10e, /* daStar_c::func_ov002_020e7eb8 */
    kPtclBigSnowmanHeadLandA   = 0x10f, /* daBgSnmHed_c::State2 */
    kPtclBigSnowmanHeadLandB   = 0x110, /* daBgSnmHed_c::State2 */
    kPtclSnowmanBreath         = 0x111, /* SnowmanBreathParticle::Render */
    kPtclSnowfall              = 0x112, /* dCamera_c::Behavior */
    kPtclStarTrail             = 0x113, /* daStar_c::func_ov002_020e81e0 */
    kPtclStarSparkle           = 0x114, /* daStar_c::func_ov002_020e7f2c */
    kPtclStarCollectA          = 0x115, /* daStar_c::func_ov002_020e8098 */
    kPtclStarCollectB          = 0x116, /* daStar_c::func_ov002_020e8098 */
    kPtclObjBlueFire           = 0x117, /* Particle::System::NewBlueFire */
    kPtclObjRedFire            = 0x118, /* Particle::System::NewRedFire */
    kPtclBooCageFall           = 0x119, /* daTBasket_c::Behavior */
    kPtclSnowmanRiseA          = 0x11a, /* daSnowman_c::func_ov081_02124f7c */
    kPtclSnowmanRiseB          = 0x11b, /* daSnowman_c::func_ov081_02124f7c */
    kPtclSnowballBurst         = 0x11c, /* daSnowball_c::HandleContact */
    kPtclSnowmanSinkA          = 0x11d, /* daSnowman_c::func_ov081_021249f4 */
    kPtclSnowmanSinkB          = 0x11e, /* daSnowman_c::func_ov081_021249f4 */
    kPtclTornadoA              = 0x11f, /* daTor_c::State1 */
    kPtclTornadoB              = 0x120, /* daTor_c::State1 */
    kPtclBkKabeBreak           = 0x121, /* daObjBk_Kabe_c::Kill */
    kPtclPoofDustA             = 0x122, /* Heyho_MotionCircleLower */
    kPtclPoofDustB             = 0x123, /* Heyho_MotionCircleLower */
    kPtclTriplePoofA           = 0x124, /* dActor_c::TriplePoofDustAt */
    kPtclTriplePoofB           = 0x125, /* dActor_c::TriplePoofDustAt */
    kPtclTriplePoofC           = 0x126, /* dActor_c::TriplePoofDustAt */
    kPtclDisappearPoofA        = 0x127, /* dActor_c::DisappearPoofDustAt */
    kPtclDisappearPoofB        = 0x128, /* dActor_c::DisappearPoofDustAt */
    kPtclAttackClashA          = 0x12a, /* func_ov002_020d8944 */
    kPtclAttackClashB          = 0x12b, /* func_ov002_020d8944 */
    kPtclStarMarkerCollectA    = 0x12c, /* daStarBase_c::Collect */
    kPtclStarMarkerCollectB    = 0x12d, /* daStarBase_c::Collect */
    kPtclStarMarkerCollectC    = 0x12e, /* daStarBase_c::Collect */
    kPtclWarioAttackHitA       = 0x12f, /* func_ov002_020d8d10 */
    kPtclWarioAttackHitB       = 0x130, /* func_ov002_020d8d10 */
    kPtclWarioAttackHitC       = 0x131, /* func_ov002_020d8d10 */
    kPtclSwingSpinNeg          = 0x132, /* daPly_c::St_SwingPlayer_Main */
    kPtclSwingSpinPos          = 0x133, /* daPly_c::St_SwingPlayer_Main */
    kPtclWarioGroundPoundA     = 0x134, /* daPly_c::St_GroundPound_Main */
    kPtclWarioGroundPoundB     = 0x135, /* daPly_c::St_GroundPound_Main */
    kPtclWarioGroundPoundDryA  = 0x136, /* daPly_c::St_GroundPound_Main */
    kPtclWarioGroundPoundDryB  = 0x137, /* daPly_c::St_GroundPound_Main */
    kPtclWaterSpoutRings       = 0x138, /* daWater_Hakidasi_c::State0 */
    kPtclWaterTornadoSpray     = 0x139, /* daWater_Tatumaki_c::Behavior */
    kPtclDeathA                = 0x13a, /* daEykn_c::CheckAttacks */
    kPtclDeathB                = 0x13b, /* daEykn_c::CheckAttacks */
    kPtclYoshiFireA            = 0x13c, /* daPly_c::St_YoshiPower_Main */
    kPtclYoshiFireB            = 0x13d, /* daPly_c::St_YoshiPower_Main */
    kPtclTongueHitA            = 0x13e, /* daPly_c::func_ov002_020d6998 */
    kPtclTongueHitB            = 0x13f  /* daPly_c::func_ov002_020d6998 */
};

#endif
