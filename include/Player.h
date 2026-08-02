/* Originally generated from matched-function evidence by tools/gen_header.py,
 * now hand-maintained. Offsets and widths are observed, not guessed; gaps are
 * explicit padding. Field NAMES cannot change codegen, so they are safe to
 * improve -- but the OFFSETS and WIDTHS are pinned by the bytes.
 *
 * Player derives from Actor: ActorBase -> ActorDerived -> Actor -> Player.
 * See notes/actor-vtables.md. That is not yet expressed here -- 0x000..0x0cf
 * still duplicates Actor's layout inline rather than inheriting it, and the
 * 31-slot vtable (_ZTV6Player, 0x0210a83c in ov002) is unrepresented. Fields
 * below 0x0d0 are being reconciled with Actor.h/ActorBase.h so that the switch
 * to real inheritance becomes a header change rather than a rewrite of 197
 * files.
 *
 * sizeof(Player) is 0x768 -- _ZN6PlayerC3Ev asks operator new for exactly that.
 */
#ifndef PLAYER_H
#define PLAYER_H
#include "types.h"

/* fwd */
struct Actor;
struct ActorBase;
struct Vector3;
struct a;
struct a_;
struct actor_;
struct amt;
struct arg_;
struct b;
struct b2;
struct b3;
struct b_;
struct c_;
struct chr_;
struct count;
struct d_;
struct e_;
struct h;
struct j;
struct kind;
struct msg;
struct p1;
struct p2;
struct p3;
struct pos;
struct pos_;
struct v;
struct v_;
struct x;
struct Player {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    /* 0x00c..0x05b. unk_00c, unk_030 and mModelAnim1 used to be named in here.
       Their only evidence was src/_ZN6Player6ST_OWLE.c, which is ov006 code
       carrying a Player name -- it reads 0x08/0x0c as a fix12 2D coordinate
       pair, which cannot be ActorBase's param1 and actorID. That file now uses
       its own local struct, and nothing else referenced these three. */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    /* 0x074..0x07f is a Vector3, not a byte plus padding: 13 files take
       &mCamSpacePosX and cast it to Vector3* for Sound::PlayCharVoice and
       friends. Named to match Actor.h, which decomposes the same bytes into
       mCamSpacePosX / Y / Z. */
    s32 mCamSpacePosX;      /* 0x074 */
    s32 mCamSpacePosY;      /* 0x078 */
    s32 mCamSpacePosZ;      /* 0x07c */
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    /* 0x092..0x097 is a prev-angle triple. 0x094 was called mTargetAngleY here
       and mPrevAngleY in Actor.h; the two had to agree before Player can
       inherit these bytes. Renamed to mPrevAngleY on the structural argument:
       its siblings at 0x092 and 0x096 are unambiguously mPrevAngleX/Z, and
       Player uses 0x094 exactly as it uses them -- `mPrevAngleY = mAngleY` to
       save and `mAngleY = mPrevAngleY` to restore, mirroring
       `mAngleX = mPrevAngleX`. It is also assigned from mDesiredAngleY, which
       reads as a target, but a previous-angle slot being reused to stage a
       desired angle is ordinary; a prev-triple with a target in the middle is
       not. Names cannot change codegen, so this is reversible. */
    s16 mPrevAngleX;        /* 0x092 */
    s16 mPrevAngleY;        /* 0x094 */
    s16 mPrevAngleZ;        /* 0x096 */
    s32 mHorzSpeed;            /* 0x098 */
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;             /* 0x0b0 -- bitwise: `&= ~0x80`, `& 0x10` */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x3];
    s32 mEatingPlayer;            /* 0x0d0 */
    u8  pad_0d4[0x8];
    u8  mBodyModels;            /* 0x0dc */
    u8  pad_0dd[0x3];
    u8  unk_0e0;            /* 0x0e0 */
    u8  pad_0e1[0xb];
    u8  unk_0ec;            /* 0x0ec */
    u8  pad_0ed[0x3];
    u8  mModelAnim3;            /* 0x0f0 */
    u8  pad_0f1[0x4f];
    u8  mAnimation1;            /* 0x140 */
    u8  pad_141[0x7];
    s32 unk_148;            /* 0x148 */
    u8  pad_14c[0x8];
    u8  unk_154;            /* 0x154 */
    u8  pad_155[0x3];
    u8  unk_158;            /* 0x158 */
    u8  pad_159[0x7];
    u8  unk_160;            /* 0x160 */
    u8  pad_161[0x13];
    u8  mModelAnim4;            /* 0x174 */
    u8  pad_175[0x4f];
    u8  mAnimation2;            /* 0x1c4 */
    u8  pad_1c5[0x13];
    u8  unk_1d8;            /* 0x1d8 */
    u8  pad_1d9[0x83];
    s32 unk_25c;            /* 0x25c */
    u8  pad_260[0x10];
    s32 unk_270;            /* 0x270 */
    u8  pad_274[0x8];
    u8  unk_27c;            /* 0x27c */
    u8  pad_27d[0xf];
    u8  unk_28c;            /* 0x28c */
    u8  pad_28d[0x1f];
    u8  mShadowModel;            /* 0x2ac */
    u8  pad_2ad[0x27];
    u8  mMovingCylinderClsnWithPos;            /* 0x2d4 */
    u8  pad_2d5[0x3];
    s32 unk_2d8;            /* 0x2d8 */
    s32 unk_2dc;            /* 0x2dc */
    u8  pad_2e0[0xc];
    u8  mBodyClsnFlags;            /* 0x2ec */
    u8  pad_2ed[0x3];
    u8  unk_2f0;            /* 0x2f0 */
    u8  pad_2f1[0x23];
    u8  mAttackClsn;            /* 0x314 */
    u8  pad_315[0x3f];
    s32 mRidingShell;            /* 0x354 */
    s32 mHeldObj;            /* 0x358 */
    s32 unk_35c;            /* 0x35c */
    s32 mObjInMouth;            /* 0x360 */
    s32 mAttachedActor;            /* 0x364 */
    u8  mTalkActor;            /* 0x368 */
    u8  pad_369[0x7];
    u8  unk_370;            /* 0x370 */
    u8  pad_371[0x3];
    u8  unk_374;            /* 0x374 */
    u8  pad_375[0x3];
    u8  unk_378;            /* 0x378 */
    u8  pad_379[0x3];
    u8  unk_37c;            /* 0x37c */
    u8  pad_37d[0x3];
    u8  mMeshClsn;            /* 0x380 */
    u8  pad_381[0x1c7];
    s32 unk_548;            /* 0x548 */
    s32 unk_54c;            /* 0x54c */
    s32 unk_550;            /* 0x550 */
    u8  pad_554[0x4];
    s32 unk_558;            /* 0x558 */
    u8  pad_55c[0x4];
    s32 unk_560;            /* 0x560 */
    u8  pad_564[0x4];
    s32 unk_568;            /* 0x568 */
    s32 unk_56c;            /* 0x56c */
    s32 unk_570;            /* 0x570 */
    s32 unk_574;            /* 0x574 */
    s32 unk_578;            /* 0x578 */
    s32 unk_57c;            /* 0x57c */
    u8  pad_580[0x8];
    s32 mHeldObjQueue;            /* 0x588 */
    u8  pad_58c[0x74];
    u8  unk_600;            /* 0x600 */
    u8  pad_601[0x1b];
    s32 unk_61c;            /* 0x61c */
    s32 mLoopingSoundHandle;            /* 0x620 */
    u8  pad_624[0x4];
    s32 unk_628;            /* 0x628 */
    s32 mParticle2;            /* 0x62c */
    s32 unk_630;            /* 0x630 */
    u8  pad_634[0x8];
    u32 mCharFileBase;            /* 0x63c */
    s32 mPrevVertSpeed;            /* 0x640 */
    s32 mGroundY;            /* 0x644 */
    u8  pad_648[0x4];
    s32 unk_64c;            /* 0x64c */
    s32 unk_650;            /* 0x650 */
    s32 unk_654;            /* 0x654 */
    s32 unk_658;            /* 0x658 */
    u8  pad_65c[0x8];
    s32 unk_664;            /* 0x664 */
    u8  pad_668[0x4];
    s32 mGroundSoundType;            /* 0x66c */
    u8  pad_670[0x4];
    s32 mHurtDamage;            /* 0x674 */
    u8  pad_678[0xc];
    s32 mPeakY;            /* 0x684 */
    s32 mAttachOffsetY;            /* 0x688 */
    s32 mSinkDepth;            /* 0x68c */
    s32 unk_690;            /* 0x690 */
    s32 unk_694;            /* 0x694 */
    u8  pad_698[0x4];
    s16 mAngleYSpeed;            /* 0x69c */
    s16 unk_69e;            /* 0x69e */
    u16 mInvincibleTimer;            /* 0x6a0 */
    u16 unk_6a2;            /* 0x6a2 */
    u16 mStateTimer;            /* 0x6a4 */
    u16 mStateWaitTimer;            /* 0x6a6 */
    s16 mJumpComboTimer;            /* 0x6a8 */
    u16 unk_6aa;            /* 0x6aa */
    u8  unk_6ac;            /* 0x6ac */
    u8  pad_6ad[0x1];
    u16 unk_6ae;            /* 0x6ae */
    u16 unk_6b0;            /* 0x6b0 */
    u8  unk_6b2;            /* 0x6b2 */
    u8  pad_6b3[0x1];
    u16 unk_6b4;            /* 0x6b4 */
    u8  unk_6b6;            /* 0x6b6 */
    u8  pad_6b7[0x1];
    s16 unk_6b8;            /* 0x6b8 */
    u8  unk_6ba;            /* 0x6ba */
    u8  pad_6bb[0x1];
    u8  unk_6bc;            /* 0x6bc */
    u8  pad_6bd[0x1];
    u16 unk_6be;            /* 0x6be */
    s16 unk_6c0;            /* 0x6c0 */
    u8  unk_6c2;            /* 0x6c2 */
    u8  pad_6c3[0x1];
    u8  unk_6c4;            /* 0x6c4 */
    u8  pad_6c5[0x1];
    u16 unk_6c6;            /* 0x6c6 */
    u16 unk_6c8;            /* 0x6c8 */
    u8  pad_6ca[0x2];
    u16 unk_6cc;            /* 0x6cc */
    u16 mStateFlags;            /* 0x6ce */
    u8  unk_6d0;            /* 0x6d0 */
    u8  pad_6d1[0x1];
    s16 mDesiredAngleY;            /* 0x6d2 */
    s16 unk_6d4;            /* 0x6d4 */
    s16 unk_6d6;            /* 0x6d6 */
    u8  mPlayerNo;            /* 0x6d8 */
    u8  mCharacter;            /* 0x6d9 */
    u8  pad_6da[0x1];
    u8  unk_6db;            /* 0x6db */
    u8  unk_6dc;            /* 0x6dc */
    u8  mHatCharacter;            /* 0x6dd */
    u8  mIsAirborne;            /* 0x6de */
    u8  mLandSoundPlayed;            /* 0x6df */
    u8  unk_6e0;            /* 0x6e0 */
    u8  mJumpComboStage;            /* 0x6e1 */
    u8  mPunchKickStep;            /* 0x6e2 */
    u8  mStateStep;            /* 0x6e3 */
    u8  mIsSlidingOnGround;            /* 0x6e4 */
    u8  mStateWork;            /* 0x6e5 */
    u8  unk_6e6;            /* 0x6e6 */
    u8  mSlideStoppedTimer;            /* 0x6e7 */
    u8  unk_6e8;            /* 0x6e8 */
    u8  mClsnFlags;            /* 0x6e9 */
    u8  pad_6ea[0x2];
    u8  unk_6ec;            /* 0x6ec */
    u8  unk_6ed;            /* 0x6ed */
    u8  unk_6ee;            /* 0x6ee */
    u8  pad_6ef[0x3];
    u8  unk_6f2;            /* 0x6f2 */
    u8  pad_6f3[0x1];
    u8  unk_6f4;            /* 0x6f4 */
    u8  mOpacity;            /* 0x6f5 */
    u8  mIsControlDisabled;            /* 0x6f6 */
    u8  unk_6f7;            /* 0x6f7 */
    u8  pad_6f8[0x1];
    u8  mIsMetal;            /* 0x6f9 */
    u8  unk_6fa;            /* 0x6fa */
    u8  unk_6fb;            /* 0x6fb */
    u8  unk_6fc;            /* 0x6fc */
    u8  mIsBalloon;            /* 0x6fd */
    u8  pad_6fe[0x1];
    u8  mHasWings;            /* 0x6ff */
    u8  unk_700;            /* 0x700 */
    u8  pad_701[0x2];
    u8  mIsMega;            /* 0x703 */
    u8  mEggParams;            /* 0x704 */
    u8  pad_705[0x1];
    u8  mIsUnderwater;            /* 0x706 */
    u8  mIsInShallowWater;            /* 0x707 */
    s8  mIsTakingDamage;            /* 0x708 */
    u8  mIsNoControl;            /* 0x709 */
    u8  mNoCtrlKind;            /* 0x70a */
    u8  mIsOpeningBigDoor;            /* 0x70b */
    u8  mStateArg;            /* 0x70c */
    u8  mIsFallScreaming;            /* 0x70d */
    u8  mSlideType;            /* 0x70e */
    u8  pad_70f[0x1];
    u8  unk_710;            /* 0x710 */
    u8  pad_711[0x1];
    u8  mIsInAirState;            /* 0x712 */
    u8  mIsBodyClsnEnabled;            /* 0x713 */
    u8  mUseAltBodyModel;            /* 0x714 */
    u8  unk_715;            /* 0x715 */
    u8  unk_716;            /* 0x716 */
    u8  unk_717;            /* 0x717 */
    u8  mLoadedResourceFlags;            /* 0x718 */
    s8  unk_719;            /* 0x719 */
    u8  unk_71a;            /* 0x71a */
    u8  mJumpedFromQuicksand;            /* 0x71b */
    u8  pad_71c[0x5];
    u8  mSleepStage;            /* 0x721 */
    u8  unk_722;            /* 0x722 */
    u8  unk_723;            /* 0x723 */
    u8  unk_724;            /* 0x724 */
    u8  pad_725[0x1];
    s8  unk_726;            /* 0x726 */
    u8  unk_727;            /* 0x727 */
    u8  unk_728;            /* 0x728 */
    u8  pad_729[0x13];
    u16 unk_73c;            /* 0x73c */
    u8  pad_73e[0x4];
    u8  unk_742;            /* 0x742 */
    u8  unk_743;            /* 0x743 */
    u8  unk_744;            /* 0x744 */
    u8  pad_745[0x3];
    u8  unk_748;            /* 0x748 */
    u8  pad_749[0x3];
    u8  unk_74c;            /* 0x74c */
    u8  pad_74d[0x3];
    s32 unk_750;            /* 0x750 */
    s32 unk_754;            /* 0x754 */
    s32 unk_758;            /* 0x758 */
    u8  pad_75c[0x4];
    s16 unk_760;            /* 0x760 */
    u8  unk_762;            /* 0x762 */
    u8  pad_763[0x1];
    u8  unk_764;            /* 0x764 */
    u8  pad_765[0x1];
    s16 unk_766;            /* 0x766 */
#ifdef __cplusplus
    /* methods */
    int * TryExitCharacterDoorWithIntro();
    int Behavior();
    int Burn();
    int CanPause();
    int CanWarp();
    int CleanupResources();
    int DropActor();
    int FinishedAnim();
    int GetHealth();
    int GetHurtState();
    int GetTalkState();
    int HasFinishedTalking();
    int HasNoCap();
    int Heal(int amt);
    int IsAnim(unsigned int a);
    int IsBeingShotOutOfCannon();
    int IsCollectingCap();
    int IsDiving();
    int IsEnteringLevel();
    int IsFrontSliding();
    int IsInAir();
    int IsInsideOfCannon();
    int IsOnShell();
    int IsOpeningDoorWithStar();
    int IsStateEnteringLevel();
    int JumpIntoBooCage(Vector3 & v_);
    int LostGrabbedObject();
    int Render();
    int SetNoControlState(unsigned char a_, int b, unsigned char c_);
    int Shock(unsigned int j);
    int ShowMessage(ActorBase & a_, unsigned int b, const Vector3 * v, unsigned int d_, unsigned int e_);
    int ShowMessage2(ActorBase & actor_, unsigned int msg, const Vector3 * pos, unsigned int d_, unsigned int e_);
    int St_BackFlip_Init();
    int St_Balloon_Cleanup();
    int St_Balloon_Init();
    int St_Bonk_Init();
    int St_Bonk_Main();
    int St_BowserEarthquake_Init();
    int St_BowserEarthquake_Main();
    int St_BurnFire_Init();
    int St_BurnLava_Init();
    int St_ButtSlide_Init();
    int St_ButtSlide_Main();
    int St_CameraZoom_Cleanup();
    int St_CameraZoom_Init();
    int St_Cannon_Cleanup();
    int St_Cannon_Init();
    int St_Cannon_Main();
    int St_CeilingGrate_Init();
    int St_CeilingGrate_Main();
    int St_Climb_Cleanup();
    int St_Crawl_Init();
    int St_Crawl_Main();
    int St_CrazedCrate_Init();
    int St_Crouch_Init();
    int St_Crouch_Main();
    int St_DeadHit_Init();
    int St_DeadHit_Main();
    int St_DeadPit_Init();
    int St_DeadPit_Main();
    int St_DebugFly_Init();
    int St_DebugFly_Main();
    int St_Dive_Init();
    int St_DizzyStars_Init();
    int St_Electrocute_Init();
    int St_Electrocute_Main();
    int St_EndingFly_Init();
    int St_Fall_Init();
    int St_Fall_Main();
    int St_Fly_Init();
    int St_GrabBowserTail_Cleanup();
    int St_GrabBowserTail_Init();
    int St_GrabBowserTail_Main();
    int St_Grabbed_Init();
    int St_Grabbed_Main();
    int St_GroundPound_Cleanup();
    int St_GroundPound_Init();
    int St_HeadstandJump_Init();
    int St_Headstand_Init();
    int St_Headstand_Main();
    int St_HoldHeavy_Init();
    int St_HoldHeavy_Main();
    int St_HoldLight_Cleanup();
    int St_HoldLight_Init();
    int St_HoldLight_Main();
    int St_HurtWater_Init();
    int St_HurtWater_Main();
    int St_Hurt_Cleanup();
    int St_Hurt_Init();
    int St_InYoshiMouth_Init();
    int St_InYoshiMouth_Main();
    int St_JumpQuicksand_Init();
    int St_JumpQuicksand_Main();
    int St_Jump_Main();
    int St_Land_Main();
    int St_LedgeGrab_Init();
    int St_LedgeGrab_Main();
    int St_LedgeHang_Cleanup();
    int St_LedgeHang_Init();
    int St_LedgeHang_Main();
    int St_LevelEnter_Init();
    int St_LevelEnter_Main();
    int St_LongJump_Init();
    int St_LongJump_Main();
    int St_MetalWaterGround_Init();
    int St_MetalWaterGround_Main();
    int St_MetalWaterWater_Init();
    int St_MetalWaterWater_Main();
    int St_NoControl_Cleanup();
    int St_NoControl_Init();
    int St_NoControl_Main();
    int St_OnWall_Init();
    int St_OnWall_Main();
    int St_OpeningWakeUp_Init();
    int St_OpeningWakeUp_Main();
    int St_Owl_Cleanup();
    int St_Owl_Init();
    int St_Owl_Main();
    int St_PunchKick_Init();
    int St_PunchKick_Main();
    int St_Respawn_Main();
    int St_Shell_Cleanup();
    int St_Shell_Init();
    int St_Shell_Main();
    int St_SideFlip_Init();
    int St_SideFlip_Main();
    int St_SlideKickRecover_Init();
    int St_SlideKick_Init();
    int St_SlideKick_Main();
    int St_SlopeJump_Init();
    int St_SlopeJump_Main();
    int St_SmallLaunchUp_Init();
    int St_SmallLaunchUp_Main();
    int St_Spin_Cleanup();
    int St_Spin_Init();
    int St_Squish_Cleanup();
    int St_Squish_Init();
    int St_Squish_Main();
    int St_StomachSlide_Init();
    int St_StomachSlide_Main();
    int St_StuckInGround_Init();
    int St_StuckInGround_Main();
    int St_Swallow_Init();
    int St_Swallow_Main();
    int St_SweepKick_Init();
    int St_SweepKick_Main();
    int St_Swim_Cleanup();
    int St_Swim_Init();
    int St_SwingPlayer_Cleanup();
    int St_SwingPlayer_Init();
    int St_Talk_Init();
    int St_Talk_Main();
    int St_Teleport_Init();
    int St_Teleport_Main();
    int St_Throw_Init();
    int St_Throw_Main();
    int St_Thrown_Cleanup();
    int St_Thrown_Init();
    int St_Thrown_Main();
    int St_TornadoSpin_Init();
    int St_TornadoSpin_Main();
    int St_TurnAround_Init();
    int St_TurnAround_Main();
    int St_WaitQuicksand_Init();
    int St_WaitQuicksand_Main();
    int St_Wait_Cleanup();
    int St_Wait_Init();
    int St_Wait_Main();
    int St_Walk_Init();
    int St_Walk_Main();
    int St_WallJump_Main();
    int St_WallSlide_Init();
    int St_WallSlide_Main();
    int St_WaterJump_Init();
    int St_WindCarry_Init();
    int St_YoshiPower_Cleanup();
    int St_YoshiPower_Init();
    int StartTalk(ActorBase & actor_, bool b_);
    int TryEnterStarDoor(Vector3 & pos_, short kind);
    int TryExitWhiteDoorWithStar();
    int TryGrab(Actor & actor_);
    int TryTalkToDoor(unsigned char a);
    int TryTalkToKeyDoor();
    int Unk_020c4f40(unsigned short x);
    int Unk_020c6a10(unsigned int arg_);
    int Unk_020c9e5c(unsigned char h);
    int Unk_020ca150(unsigned char a);
    int Unk_020ca8f8();
    s32 St_LevelEnter_Cleanup();
    s32 St_Null_Cleanup();
    s32 St_Null_Main();
    u32 OnYoshiTryEat();
    unsigned int GetBodyModelID(unsigned int a, bool b_) const;
    void BlowAway(short v);
    void EnterWhirlpool();
    void IncMegaKillCount();
    void InitBalloonMario();
    void InitFireYoshi();
    void InitMetalWario();
    void InitVanishLuigi();
    void InitWingFeathers(bool b_);
    void OnPendingDestroy();
    void OpenBigDoor();
    void PlayMammaMiaSound();
    void RegisterEggCoinCount(unsigned int count, bool b2, bool b3);
    void ST_WAIT();
    void SetNewHatCharacter(unsigned int p1, unsigned int p2, bool p3);
    void SetRealCharacter(unsigned int chr_);
    void St_EndingFly_Main();
    void St_InYoshiMouth_Cleanup();
    void St_WallJump_Init();
    void TurnOffToonShading(unsigned int j);
    void Unk_020ca488();
#endif
};

/* Offsets seen through this-pointer but far outside the object - almost
 * certainly a different base (containing object or global). Needs semantic
 * review before they can be typed:
 *   0x4eb0, 0x4eb4, 0x4ee5, 0x62ad, 0x62af
 */

#endif
