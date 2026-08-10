/* Originally generated from matched-function evidence by tools/gen_header.py,
 * now hand-maintained. Offsets and widths are observed, not guessed; gaps are
 * explicit padding. Field NAMES cannot change codegen, so they are safe to
 * improve -- but the OFFSETS and WIDTHS are pinned by the bytes.
 *
 * Player derives from Actor: ActorBase -> ActorDerived -> Actor -> Player.
 * See notes/actor-vtables.md. That IS expressed here now: `struct Player :
 * Actor` inherits 0x000..0x0cf rather than duplicating it, and the 31-slot
 * vtable (_ZTV6Player, 0x0210a83c in ov002) has seven of its overrides
 * declared -- the destructor pair plus slots 0, 3, 6, 9, 12 and 18. See the
 * vtable block above the method list; the key-function rule documented there
 * is load-bearing, not advisory.
 *
 * This header is C++ only. The old #ifndef __cplusplus twin is gone, so a
 * consumer that still needs the C spelling has none -- 201 files include it.
 *
 * NOT yet expressed: the nested type `Player::State`. Both
 * _ZN6Player7IsStateERNS_5StateE and _ZN6Player11ChangeStateERNS_5StateE take
 * a `State&`, and every St_*_Init/_Main/_Cleanup handler is reached through
 * it, so it wants settling before those are migrated.
 *
 * sizeof(Player) is 0x768 -- _ZN6PlayerC3Ev asks operator new for exactly that.
 */
#ifndef PLAYER_H
#define PLAYER_H
#include "types.h"
#include "Actor.h"
#include "ShadowModel.h"
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "MaterialChanger.h"

/* fwd. Only these three are types. gen_header.py used to emit a `struct X;`
   for every parameter NAME it could not resolve as well -- 26 of them, `struct
   a;` through `struct x;` -- which declared nothing any declaration below
   refers to. Removed; none was used as a type anywhere in src/ or include/. */
struct Actor;
struct ActorBase;
struct Vector3;

struct Player : Actor {
    /* 0x000..0x0cf is Actor's, inherited rather than duplicated. It used to be
       written out inline here -- mParam at 0x008 was ActorBase's param1, mPosX
       at 0x05c was Actor's, and so on. The names were reconciled first so that
       deleting the block is all that happens here. sizeof(Actor) is 0xd0, so
       Player's own fields start exactly where the base ends. */

    /* One entry of the player state machine. _ZN6Player7IsStateERNS_5StateE
       and _ZN6Player11ChangeStateERNS_5StateE both take a State&, so the type
       is nested in Player and is named State; the mangling fixes both facts.

       Each member is a POINTER TO MEMBER FUNCTION, not a plain function
       pointer. Both callers use the ARM/Itanium pmf sequence verbatim --
       ChangeState at 0x020e30d4:

           add   r3, r1, #0x10        ; &state->mCleanup
           ldr   r1, [r3, #4]         ; the adjustment word
           add   r0, r5, r1, asr #1   ; this + (adj >> 1)
           ands  r1, r1, #1           ; virtual bit lives in the ADJUSTMENT
           ldrne r2, [r0]             ; ...virtual: load the vtable
           ldrne r1, [r3]
           ldrne r1, [r2, r1]         ; ...and index it by the ptr word
           ldreq r1, [r3]             ; ...non-virtual: ptr IS the address
           blx   r1

       so each member occupies two words, {ptr, adj}, and the offsets below
       are the ptr word. A plain function pointer cannot produce that shape.

       Who calls what, and when:
         mInit     ChangeState, LAST, on the state being entered (+0x00)
         mMain     Player::Behavior, per tick, at 0x020e5118       (+0x08)
         mCleanup  ChangeState, FIRST, on the state being left     (+0x10)

       mCleanup is a veto, not a notification: ChangeState tests its result
       and returns 0 without transitioning when it is 0. Each of the three is
       null-checked on its ptr word before the call, so all three are
       optional -- which is why the symbol table has _Init/_Main/_Cleanup for
       some states and only _Init/_Main for most.

       The State objects themselves live in ov002's .bss (0x0211013c,
       0x0211022c, 0x02110364, 0x0211067c, 0x021106ac are the five ChangeState
       names directly), populated by ov002's static constructors.

       CORRECTION, and it reverses what this note used to say. It used to warn
       that ov006's _ZN6Player7ST_WAITE and _ZN6Player6ST_OWLE, at addresses in
       the same 0x0211xxxx window, were NOT these -- that overlays 2 and 6
       share the window and the coincidence meant nothing. The first half is
       true and the conclusion was backwards: those two symbols ARE these, and
       it was ov006's claim on them that was the coincidence.

       ov002 has kind:bss data at both 0x02110154 and 0x02110244, inside an
       unbroken 0x18-stride run that starts at 0x0211013c -- 0x18 being exactly
       sizeof(State). ov002 loads 0x02110154 twenty times and 0x02110244 five
       times, every one module:overlay(2). __sinit_ov002_021019d0 fills
       0x02110154's three slots from 0x0210a25c, 0x02109e14 and 0x02109f84, all
       inside ov002's pointer-to-member constant pool: a State being built
       member by member. And Player::CanEnterDoor already cast the raw address
       to State& to hand it to IsState.

       The two symbols were registered as ov006 FUNCTIONS. They are now ov002
       objects, declared below as the static members whose mangling they match. */
    struct State {
        int (Player::*mInit)();       /* 0x00 */
        int (Player::*mMain)();       /* 0x08 */
        int (Player::*mCleanup)();    /* 0x10 */
    };

    s32 mEatingPlayer;            /* 0x0d0 */
    u8  pad_0d4[0x8];
    u8  mBodyModels;            /* 0x0dc */
    u8  pad_0dd[0x3];
    u8  unk_0e0;            /* 0x0e0 */
    u8  pad_0e1[0xb];
    u8  unk_0ec;            /* 0x0ec */
    u8  pad_0ed[0x3];
    /* ~Player calls _ZN9ModelAnimD1Ev on this LAST of the two, and ModelAnim
       asserts 0x64 -- 0x0f0..0x154, closing exactly at unk_154. The markers
       this replaces are its own sub-objects: mAnimation1 at 0x140 was the
       Animation base (+0x50) and unk_148 at 0x148 was that base's currFrame
       (+0x58). Both are reachable through the member now. */
    ModelAnim mModelAnim3;            /* 0x0f0 */
    u8  unk_154;            /* 0x154 */
    u8  pad_155[0x3];
    u8  unk_158;            /* 0x158 */
    u8  pad_159[0x7];
    u8  unk_160;            /* 0x160 */
    u8  pad_161[0x13];
    /* The other ModelAnim, destroyed FIRST of the two. 0x174..0x1d8, closing
       exactly at unk_1d8; mAnimation2 at 0x1c4 was its Animation base. */
    ModelAnim mModelAnim4;            /* 0x174 */
    u8  unk_1d8;            /* 0x1d8 */
    u8  pad_1d9[0x3];
    /* The three __destroy_arr calls in ~Player, in the order it makes them
       (last array first). Both element types assert 0x14, which is exactly
       the stride the destructor passes. unk_25c and unk_270 were the
       currFrame of mTexSeqPlayer[0] and [1] -- +0x08 into each element. */
    TextureSequence mTexSeqBody[4];            /* 0x1dc */
    MaterialChanger mMatChanger[2];            /* 0x22c */
    TextureSequence mTexSeqPlayer[2];            /* 0x254 */
    u8  unk_27c;            /* 0x27c */
    u8  pad_27d[0xf];
    u8  unk_28c;            /* 0x28c */
    u8  pad_28d[0x1f];
    /* ~Player calls _ZN11ShadowModelD1Ev on this, and ShadowModel asserts
       0x28 -- which closes exactly at mMovingCylinderClsnWithPos. */
    ShadowModel mShadowModel;            /* 0x2ac */
    /* ~Player calls _ZN25MovingCylinderClsnWithPosD1Ev on this too, and the
       0x40 it asserts closes exactly at mAttackClsn. The four markers it
       absorbs are all CylinderClsn's own, reached through the base:
       unk_2d8 = radius (+0x04), unk_2dc = height (+0x08),
       mBodyClsnFlags = flags (+0x18), unk_2f0 = vulnFlags (+0x1c).
       mBodyClsnFlags in particular was never a Player field -- it is the
       body collider's flags word, which is why thirteen Player methods
       set and clear bits in it. */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;            /* 0x2d4 */
    /* ~Player calls _ZN25MovingCylinderClsnWithPosD1Ev on this, and that
       type asserts 0x40 -- which closes exactly at mRidingShell. */
    MovingCylinderClsnWithPos mAttackClsn;            /* 0x314 */
    s32 mRidingShell;            /* 0x354 */
    s32 mHeldObj;            /* 0x358 */
    s32 unk_35c;            /* 0x35c */
    s32 mObjInMouth;            /* 0x360 */
    s32 mAttachedActor;            /* 0x364 */
    u8  mTalkActor;            /* 0x368 */
    u8  pad_369[0x7];
    /* IsState compares this pointer for identity and nothing else:
       ldr r0,[r0,#0x370] / cmp r0,r1 / moveq #1 / movne #0. */
    State *mState;            /* 0x370 */
    /* ChangeState saves the outgoing mState here (ldr ip,[r5,#0x370] then
       str ip,[r5,#0x374]) immediately before overwriting it. */
    State *mPrevState;            /* 0x374 */
    /* Written from the argument on ENTRY to ChangeState, before the cleanup
       veto can abort the transition -- so it records what was asked for, not
       what was taken. */
    State *mRequestedState;            /* 0x378 */
    u8  unk_37c;            /* 0x37c */
    u8  pad_37d[0x3];
    /* ~Player calls _ZN12WithMeshClsnD1Ev on this FIRST, and WithMeshClsn
       asserts 0x1bc -- which closes exactly at mSpawnPosX. */
    WithMeshClsn mMeshClsn;            /* 0x380 */
    /* The spawn point, saved once and restored on death. Player::InitResources
       writes all four from the live values (0x53c..0x544 <- mPosX/Y/Z, and
       mSpawnAngleY <- mAngleY); St_Respawn_Init reads them back the other way.
       Two functions, opposite directions, the same four slots -- which is what
       makes these names evidenced rather than guessed. */
    s32 mSpawnPosX;            /* 0x53c */
    s32 mSpawnPosY;            /* 0x540 */
    s32 mSpawnPosZ;            /* 0x544 */
    s32 unk_548;            /* 0x548 */
    s32 unk_54c;            /* 0x54c */
    s32 unk_550;            /* 0x550 */
    /* Floor normal, fx12 (1.0 == 0x1000 -- already rescaled from KCL's 0x400).
     * Stored as three consecutive words by Player::SetFloorSurfaceInfo
     * (func_ov002_020c16ec) at 0x020c1768 / 0x020c1770 / 0x020c1778, sourced
     * from SurfaceInfo::CopyNormalTo (func_02037dcc). 0x554 and 0x55c were
     * marked padding until those stores were disassembled.
     * mFloorNormalY is the slope-test input: func_ov002_020f035c compares it
     * against a per-floor-class cosine threshold. */
    s32 mFloorNormalX;            /* 0x554 */
    s32 mFloorNormalY;            /* 0x558 */
    s32 mFloorNormalZ;            /* 0x55c */
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
    /* --- Unpacked CLPS (collision property) fields ---------------------
     * Player::SetFloorSurfaceInfo (func_ov002_020c16ec) calls one bitfield
     * getter per field on the floor's CLPS and stores each result here. The
     * getters are 3-instruction accessors at 0x02037e14..0x02037e90; the
     * bit extents below are read off those instructions, so the mapping is
     * exact even where the meaning is not yet known.
     *
     * mFloorClass is the slipperiness class, 0..5. It drives five separate
     * tables: the slope threshold (func_ov002_020f035c), slope accel
     * (func_ov002_020c04e0), slide accel (func_ov002_020f02c8), slide loss
     * factor (func_ov002_020f030c) and slope decel (func_ov002_020bf56c).
     * It is the DS equivalent of N64's SURFACE_CLASS_*, but authored per
     * triangle in the collision data rather than derived from a surface ID.
     * NOTE the latch at 0x020c1738: if mFloorClass is already 1 the function
     * returns early, refreshing neither the class nor the floor normal. */
    s32 mFloorClass;            /* 0x658  CLPS bits 12-14, getter func_02037e58 */
    s32 mClpsBits15_18;            /* 0x65c  getter func_02037e48 */
    s32 mClpsBits6_11;            /* 0x660  getter func_02037e68 */
    s32 mSurfaceType;            /* 0x664  CLPS bits 19-23, getter func_02037e38; values 6..9 are the quicksand tiers */
    u8  pad_668[0x4];
    s32 mGroundSoundType;            /* 0x66c  CLPS bits 0-4, getter func_02037e84 */
    s32 mClpsWord1Lo;            /* 0x670  CLPS word1 bits 0-7, getter func_02037e90 */
    s32 mHurtDamage;            /* 0x674 */
    u8  pad_678[0xc];
    s32 mPeakY;            /* 0x684 */
    s32 mAttachOffsetY;            /* 0x688 */
    s32 mSinkDepth;            /* 0x68c */
    s32 unk_690;            /* 0x690 */
    s32 unk_694;            /* 0x694 */
    s16 mSpawnAngleY;            /* 0x698 */
    /* Downhill direction of the current floor, binary angle (0x10000 per turn).
     * Player::SetFloorSurfaceInfo computes it as cstd::atan2(mFloorNormalX,
     * mFloorNormalZ) and stores it with a 16-bit strh at 0x020c178c (written
     * as [r5 + 0x600] + 0x9a). Consumed by Player::ApplySlopeAccel
     * (func_ov002_020c04e0) via AngleDiff(mFloorAngle, moveYaw) to decide
     * whether a slope speeds you up or slows you down. */
    s16 mFloorAngle;            /* 0x69a */
    s16 mAngleYSpeed;            /* 0x69c */
    s16 unk_69e;            /* 0x69e */
    u16 mInvincibleTimer;            /* 0x6a0 */
    u16 unk_6a2;            /* 0x6a2 */
    u16 mStateTimer;            /* 0x6a4 */
    u16 mStateWaitTimer;            /* 0x6a6 */
    /* u16, not s16: every load of this slot in the ROM is an ldrh. Declared
       signed, St_Hurt_Main's `!= 0` test compiles to ldrsh and the function
       misses by exactly one word. Nothing anywhere reads it signed -- the
       other five users either store, or cast to u16* first (including
       Behavior's DecIfAbove0_Short(u16*)). Same defect class as the imported
       parameter widths in Actor.h: a declared type nothing had checked. */
    u16 mJumpComboTimer;            /* 0x6a8 */
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
    /* u16, not s16: every READ of this slot in the ROM is an ldrh --
       func_ov002_020e4bb8 tests `< 0x3f` and `& 1`, Behavior passes it to
       DecIfAbove0_Short(u16*), St_Balloon_Main tests `== 0`. Only the stores
       use short, and a store cannot distinguish the two. Declared signed,
       St_Balloon_Main misses by one word. Same defect as mJumpComboTimer.
       Named for what those five uses do: InitBalloonMario sets it to 0x258,
       Behavior counts it down, St_Balloon_Main exits when it reaches 0, and
       020e4bb8 blinks the balloon below 0x3f. */
    u16 mBalloonTimer;            /* 0x6c0 */
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
    u8  unk_725;            /* 0x725  latch: St_Talk_Cleanup reloads music layer 1 when set */
    s8  unk_726;            /* 0x726 */
    u8  unk_727;            /* 0x727 */
    u8  unk_728;            /* 0x728 */
    u8  pad_729[0x13];
    u16 unk_73c;            /* 0x73c */
    u8  pad_73e[0x4];
    u8  unk_742;            /* 0x742 */
    u8  unk_743;            /* 0x743 */
    /* 0x744..0x74c is a Vector3, not three bytes: five files cast it as one
       -- ShowMessage2, TryEnterStarDoor, func_ov002_020c47f4 and
       func_ov002_020c8a4c all spell (Vector3*)(this + 0x744), and
       JumpIntoBooCage writes all three as int. Declared u8, any natural
       access is an ldrb. Widths only; the names stay unk_ because what the
       vector MEANS is not settled -- 0x750 is the anchor and 0x744 the point
       Vec3_RotateYAndTranslate derives from it. */
    s32 unk_744;            /* 0x744 */
    s32 unk_748;            /* 0x748 */
    s32 unk_74c;            /* 0x74c */
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
    /* methods */
    /* Two of the State objects, as the static members they are. `ST_WAIT`
       mangles to _ZN6Player7ST_WAITE and `ST_OWL` to _ZN6Player6ST_OWLE --
       which is what those two symbols always were, and the reason they read
       as ALL_CAPS constants rather than St_Foo_Bar handlers.

       Both live in ov002 .bss at 0x02110154 and 0x02110244, inside the same
       unbroken 0x18-stride run as the five this header already names from
       ChangeState. __sinit_ov002_021019d0 fills 0x02110154's three slots from
       0x0210a25c, 0x02109e14 and 0x02109f84 -- all inside ov002's
       pointer-to-member constant pool -- which is a State being constructed,
       member by member, in front of you.

       They are DECLARED and never defined here: the objects are the ROM's,
       carved out into the gap, so a definition in any translation unit would
       be a duplicate. Referencing them emits the undefined symbol the gap
       supplies, which is what the four Player methods below already did by
       hand. */
    static State ST_WAIT;
    static State ST_OWL;

    /* --- vtable. _ZTV6Player (0x0210a83c, ov002) is Actor's 31 slots with
           eight overridden and NO new virtuals; see notes/actor-vtables.md.

           The destructor is declared FIRST on purpose. CW 1.2 emits the vtable
           into the translation unit that DEFINES the first non-inline virtual
           declared -- the key function -- and that copy collides with the one
           the module's gap object supplies from ROM data. Because Player only
           overrides, an override takes its base's slot wherever it is declared,
           so putting ~Player first costs nothing and pins the key function to
           the destructor, which is only ever defined as an extern "C" free
           function in D0Ev/D2Ev, never as Player::~Player.

           So: ~Player must never be defined ANYWHERE, including inline. An
           inline definition would shift key-function status to Behavior, which
           IS a real method, and emit a duplicate vtable. --- */
    virtual ~Player();                  /* slots 16, 17 */
    virtual s32  InitResources();       /* slot  0 */
    virtual s32  CleanupResources();    /* slot  3 */
    virtual s32  Behavior();            /* slot  6 */
    virtual s32  Render();              /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnYoshiTryEat();       /* slot 18 -- int, not u32: it overrides
                                           Actor::OnYoshiTryEat and CW rejects a
                                           mismatched return type on an override */

    int Burn();
    int CanEnterDoor(unsigned char door);
    int CanPause();
    int CanWarp();
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
    /* Pointer identity against mState. bool, not int: the ROM normalises with
       a moveq #1 / movne #0 pair rather than returning a raw value. */
    int ChangeState(State &state);
    bool IsState(State &state);
    int IsStateEnteringLevel();
    int JumpIntoBooCage(Vector3 & v_);
    int LostGrabbedObject();
    int SetNoControlState(unsigned char a_, int b, unsigned char c_);
    int Shock(unsigned int j);
    /* The last two are unsigned char, not unsigned int: both functions read
       their stack-passed arguments with ldrb, and mwcc never fuses a
       narrowing cast into a narrower load. The symbols were imported ending
       jj and are corrected to hh in this commit. */
    int ShowMessage(ActorBase & a_, unsigned int b, const Vector3 * v, unsigned char d_, unsigned char e_);
    int ShowMessage2(ActorBase & actor_, unsigned int msg, const Vector3 * pos, unsigned char d_, unsigned char e_);
    int St_BackFlip_Init();
    int St_Balloon_Cleanup();
    int St_Balloon_Init();
    s32 St_Balloon_Main();
    int St_Bonk_Init();
    int St_Bonk_Main();
    int St_BowserEarthquake_Init();
    int St_BowserEarthquake_Main();
    int St_BurnFire_Init();
    int St_BurnFire_Main();
    int St_BurnLava_Init();
    int St_BurnLava_Main();
    int St_ButtSlide_Init();
    int St_ButtSlide_Main();
    int St_CameraZoom_Cleanup();
    int St_CameraZoom_Init();
    int St_CameraZoom_Main();
    int St_Cannon_Cleanup();
    int St_Cannon_Init();
    int St_Cannon_Main();
    int St_CeilingGrate_Init();
    int St_CeilingGrate_Main();
    int St_Climb_Cleanup();
    int St_Climb_Main();
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
    int St_DizzyStars_Main();
    int St_Electrocute_Init();
    int St_Electrocute_Main();
    int St_EndingFly_Init();
    /* ov002 0x020c3d1c, immediately before St_EndingFly_Init. `int`, not the
       `void` this header used to declare: the ROM body returns 1. */
    int St_EndingFly_Main();
    int St_Fall_Init();
    int St_Fall_Main();
    int St_Fly_Init();
    int St_GrabBowserTail_Cleanup();
    int St_GrabBowserTail_Init();
    int St_GrabBowserTail_Main();
    int St_Grabbed_Cleanup();
    int St_Grabbed_Init();
    int St_Grabbed_Main();
    int St_GroundPound_Cleanup();
    int St_GroundPound_Init();
    int St_GroundPound_Main();
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
    int St_Hurt_Main();
    /* ov002 0x020d6084, immediately before St_InYoshiMouth_Main. `int`, not
       `void`: ChangeState TESTS a cleanup's result and a 0 vetoes the
       transition, so this return value is load-bearing. */
    int St_InYoshiMouth_Cleanup();
    int St_InYoshiMouth_Init();
    int St_InYoshiMouth_Main();
    int St_JumpQuicksand_Init();
    int St_JumpQuicksand_Main();
    int St_Jump_Init();
    int St_Jump_Main();
    int St_Land_Main();
    /* ov002 0x020cac30, size 0x8: `return 1` and nothing else. Reached from
       ov002's State pointer-to-member table -- see the relocation note below. */
    int St_Null_Init();
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
    int St_Respawn_Init();
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
    int St_Spin_Main();
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
    int St_Swim_Main();
    int St_SwingPlayer_Cleanup();
    int St_SwingPlayer_Init();
    int St_Talk_Cleanup();
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
    /* ov002 0x020e17f8, size 0xb0, immediately after St_WallJump_Main. */
    int St_WallJump_Init();
    int St_WallJump_Main();
    int St_WallSlide_Init();
    int St_WallSlide_Main();
    int St_WaterJump_Init();
    int St_WindCarry_Init();
    int St_YoshiPower_Cleanup();
    int St_YoshiPower_Init();
    int St_YoshiPower_Main();
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
    unsigned int GetBodyModelID(unsigned int a, bool b_) const;
    void BlowAway(short v);
    void EnterWhirlpool();
    void IncMegaKillCount();
    void InitBalloonMario();
    void InitFireYoshi();
    void InitMetalWario();
    void InitVanishLuigi();
    void InitWingFeathers(bool b_);
    void OpenBigDoor();
    void PlayMammaMiaSound();
    void RegisterEggCoinCount(unsigned int count, bool b2, bool b3);
    void SetNewHatCharacter(unsigned int p1, unsigned int p2, bool p3);
    void SetRealCharacter(unsigned int chr_);
    void TurnOffToonShading(unsigned int j);
    void Unk_020ca488();
};

/* Hold both claims with the compiler rather than a comment. State's 0x18 is
   the load-bearing one: it is three pointers-to-member at 8 bytes each, and
   if this toolchain represented a pmf any other way the offsets above --
   +0x00, +0x08, +0x10, read straight off ChangeState and Behavior -- would
   not line up and this would refuse to compile. */
typedef char Player_size_must_be_0x768[sizeof(struct Player) == 0x768 ? 1 : -1];
typedef char Player_State_size_must_be_0x18[sizeof(Player::State) == 0x18 ? 1 : -1];

/* Offsets seen through this-pointer but far outside the object (sizeof(Player)
 * is 0x768). These are NOT Player fields and must not be typed as such.
 *
 *   0x4eb0, 0x4eb4, 0x4ee5  -- FIXED. The symbol was moved, which is what the
 *     earlier revision of this note said the repair had to be; the offsets were
 *     never Player's to type. Kept as history because the diagnosis is reusable.
 *
 *     ov002 and ov006 overlap in address space (ov002 0x020AD660..0x0210D9A0,
 *     ov006 0x020BFEC0..0x02140260), and 0x020e17f8 falls inside both.
 *     _ZN6Player16St_WallJump_InitEv had been attached to the ov006 copy (size
 *     0x5c), so that file decompiled unrelated ov006 code while claiming to be
 *     a Player method -- hence the nonsense offsets. It now names the ov002
 *     copy (size 0xb0), and the ov006 one is func_ov006_020e17f8 again.
 *
 *     FOUR INDEPENDENT LINES OF EVIDENCE agreed, which is what made the move
 *     safe. (1) ov002/relocs.txt carries TWO `kind:load to:0x020e17f8
 *     module:overlay(2)` entries, from 0x0210a33c and 0x0210a3ec, and those sit
 *     in a run of ov002 data symbols with an EIGHT-BYTE stride -- the {ptr,adj}
 *     shape of a pointer-to-member documented at the top of this header, i.e.
 *     the State tables. ov006's only reference is a `kind:arm_call ...
 *     module:overlay(6)`. (2) The ov002 function sits immediately after
 *     St_WallJump_Main (0x020e1714 +0xe4, ending exactly at 0x020e17f8) and
 *     immediately before St_BackFlip_Init; Init/Main pairs are adjacent
 *     throughout this overlay. (3) Its draft names its own shadow struct
 *     `Player` and touches nothing above 0x71b. (4) Its body adds 0x8000 --
 *     half a turn -- to the facing angle, copies it to the previous angle, sets
 *     anim 0x28 and a launch speed and plays a character voice. That is a wall
 *     jump.
 *
 *     This is the failure mode described in notes/overlay-ambiguous-references.md.
 *
 *   St_Null_Init was the SAME defect and moved in the same commit: ov006 had it
 *     at 0x020cac30 (size 0x6c), ov002 has an eight-byte `return 1` there,
 *     reached by `kind:load ... module:overlay(2)` from 0x0210a17c -- the same
 *     stride-8 table. A State whose Init only succeeds is exactly that long,
 *     and unlike WallJump its ov002 source byte-matches, so it is now a real
 *     method.
 *
 *     EXPECT MORE. The 0x0210a1xx-0x0210a3xx table is Player state pointers;
 *     any ov006 symbol its loads land on is a candidate for the same repair.
 *
 *   St_Null_Init is the SAME DEFECT, found the same way. ov006 carries
 *     _ZN6Player12St_Null_InitEv at 0x020cac30 size 0x6c and ov002 carries an
 *     unnamed func_ov002_020cac30 of size 0x8 at the same address. ov002
 *     reaches it by `kind:load ... module:overlay(2)` from 0x0210a17c -- the
 *     same pointer-to-member region as above -- while ov006 reaches its own
 *     copy by two direct arm_calls. An eight-byte body is exactly what a state
 *     called Null should be. Expect more of these: the whole 0x0210a1xx-0x0210a3xx
 *     table is Player state pointers, and any ov006 symbol its loads land on
 *     is a candidate.
 *
 *   0x62ad, 0x62af  -- still unexplained; source not yet traced. Needs semantic
 *     review before they can be typed.
 */

/* WHY FIVE NAMES ARE NOT DECLARED IN THIS CLASS.
 *
 * ST_WAIT, St_EndingFly_Main, St_InYoshiMouth_Cleanup, St_WallJump_Init and
 * TryExitCharacterDoorWithIntro were declared here until this commit, while
 * each of their sources already said, in terms, "NOT a Player method" and
 * "detached from Player.h". The header was the last thing still making the
 * claim. Nothing called them through the declaration -- every caller in the
 * tree spells the mangled name in its own `extern` -- so removing them is
 * codegen-neutral, and the eligible name list is unchanged by it.
 *
 * ST_WAIT is a separate and still-open question rather than a settled
 * misattribution, and it is NOT declared here for that reason. Three sources
 * disagree about what it even is: this header called it `void ST_WAIT()`,
 * ov006/symbols.txt calls it `kind:function(size=0x68)` at 0x02110154, and
 * four migrated Player methods -- CanWarp, TryTalkToDoor, TryTalkToKeyDoor,
 * St_WaitQuicksand_Main -- use `_ZN6Player7ST_WAITE` as a Player::State
 * OBJECT, passing its address to Player::IsState alongside
 * data_ov002_0211013c. 0x02110154 is 0x0211013c + 0x18, exactly one State
 * apart. Do not resolve this from the arithmetic: ov002 and ov006 share that
 * RAM window, the byte compare wildcards every relocated word, so both
 * spellings match whichever is right. It needs the module question answered
 * first.
 */

#endif
