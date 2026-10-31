/* Originally generated from matched-function evidence by tools/gen_header.py,
 * now hand-maintained. Offsets and widths are observed, not guessed; gaps are
 * explicit padding. Field NAMES cannot change codegen, so they are safe to
 * improve -- but the OFFSETS and WIDTHS are pinned by the bytes.
 *
 * Player derives from dActor_c: fBase_c -> dBase_c -> dActor_c -> Player.
 * See notes/actor-vtables.md. That IS expressed here now: `struct Player :
 * dActor_c` inherits 0x000..0x0cf rather than duplicating it, and the 31-slot
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
#include "dActor_c.h"
#include "dExtShadowModel_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"
#include "dExtAnmModel_c.h"
#include "dExtAnmTexPat_c.h"
#include "dExtAnmMaterial_c.h"

/* fwd. Only these three are types. gen_header.py used to emit a `struct X;`
   for every parameter NAME it could not resolve as well -- 26 of them, `struct
   a;` through `struct x;` -- which declared nothing any declaration below
   refers to. Removed; none was used as a type anywhere in src/ or include/. */
struct dActor_c;
struct fBase_c;
struct Vector3;
/* Declared, not included: dExtAnm2Model_c.h carries an inline operator delete, and
   any function body in a header this widely consumed renumbers every
   consumer's compiler temporaries. mBodyModels only needs the name. */
struct dExtAnm2Model_c;

struct Player : dActor_c {
    /* 0x000..0x0cf is dActor_c's, inherited rather than duplicated. It used to be
       written out inline here -- mParam at 0x008 was fBase_c's param1, mPosX
       at 0x05c was dActor_c's, and so on. The names were reconciled first so that
       deleting the block is all that happens here. sizeof(dActor_c) is 0xd0, so
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
    /* 0x0dc..0x2ac: the models and the texture/material animations drawn on
       them, grouped as one embedded sub-object. The grouping is what the
       constructor proves: retail Player::Player() (0x020e68f4) keeps `this` in
       r5 and `this + 0xdc` in r4, and builds the two dExtAnmModel_c and the
       three animation arrays off r4 (+0x14, +0x98, +0x100, +0x150, +0x178)
       while everything after 0x2ac is built off r5. That second base is the
       inlined implicit constructor of a member whose `this` is Player + 0xdc;
       with these fields flat in Player the constructor comes out one
       instruction short. The bytes require 0x0dc..0x27c inside; the two
       saved-texparam arrays at 0x27c..0x2ac match either way and are kept
       with the models they save state for. The type name is not recovered:
       its constructor and destructor are only ever inlined. Offsets in the
       field comments are Player offsets. */
    struct Models {
        /* FOUR body models, not one pointer plus padding. Player::CleanupResources
           walks this run as `*(this + i * 4 + 0xdc)` for `i < 4` and calls each
           element's deleting destructor; Player::Render indexes it with
           GetBodyModelID(mBodyModelId, 1) and TurnOffToonShading with
           GetBodyModelID(j, 0). The element type is dExtAnm2Model_c, not dExtAnmModel_c:
           func_ov002_020e5948 allocates every slot with operator new(0x78), which
           is sizeof(dExtAnm2Model_c), and builds it with _ZN15dExtAnm2Model_cC1Ev, and the
           only methods called on the elements by their own class are dExtAnm2Model_c's
           (Func_020162C4, Copy). The matched bodies' other accesses all land in
           the dExtAnmModel_c base: +0x08 (dExtSimpleModel_c's ModelComponents), +0x14
           (dExtSimpleModel_c::mat4x3), +0x50 (the dExtFrameCtrl_c base) and +0x58 (its
           currFrame), and vtable slots 5 (Render) and 6 (Virtual18).
           FIVE slots: GetBodyModelID returns 4 while mIsMetal is set, and
           func_ov002_020e5948 builds [4] from wario_metal_model.bmd (arc0 0x54c).
           CleanupResources deletes [4] on its own after the `i < 4` loop. */
        dExtAnm2Model_c *mBodyModels[5]; /* 0x0dc */
        /* ~Player calls _ZN14dExtAnmModel_cD1Ev on this LAST of the two, and dExtAnmModel_c
           asserts 0x64 -- 0x0f0..0x154, closing exactly at mHeadModels. The markers
           this replaces are its own sub-objects: mAnimation1 at 0x140 was the
           dExtFrameCtrl_c base (+0x50) and unk_148 at 0x148 was that base's currFrame
           (+0x58). Both are reachable through the member now. */
        dExtAnmModel_c mAnmModel1; /* 0x0f0 */
        /* EIGHT more models, dExtSimpleModel_c not dExtAnmModel_c: func_ov002_020e5948 news
           [0..2] and [4..7] as 0x50 bytes + _ZN17dExtSimpleModel_cC1Ev and only [3] as 0x64 +
           _ZN14dExtAnmModel_cC1Ev (it alone gets SetAnim, and the Yoshi sites that drive
           its frame downcast it). Every other access is dExtSimpleModel_c-level: data,
           mat4x3, SetPolygonMode, Render. CleanupResources
           destroys `i < 4` and then `i + 4` over this one base, TurnOffToonShading
           indexes it at `j` and `j + 4`, and Render indexes it with
           func_ov002_020becf4(mBodyModelId, 1). That helper can also return 8 or 9,
           which are "no model" sentinels -- Render loads the slot FIRST and only then
           tests `i != 9 && i != 8`, so the ROM itself reads one word past this run in
           those two cases. Kept as written; the read lands in mAnmModel2 below and
           its result is discarded. [0..3] are the per-character head_cap models
           (Yoshi: yoshi_head) from data_ov002_0210a69c, [4..7] the head_nocap ones
           (Yoshi: yoshi_head_fill) from data_ov002_0210a6ac; Render pins each to body
           bone 15 (face_1). */
        dExtSimpleModel_c *mHeadModels[8]; /* 0x154 */
        /* The other dExtAnmModel_c, destroyed FIRST of the two. 0x174..0x1d8, closing
           exactly at mSleepBubbleModel; mAnimation2 at 0x1c4 was its dExtFrameCtrl_c base. */
        dExtAnmModel_c mAnmModel2; /* 0x174 */
        /* func_ov002_020e5948 news a 0x50-byte dExtSimpleModel_c here (_ZN17dExtSimpleModel_cC1Ev) and SetFiles it
           from data_ov002_0210da38 (file 0x8004, the bubble d_a_pkn also sleeps
           with); func_ov002_020e444c places it in front of the face, Render draws
           it at mSleepBubbleScale while func_ov002_020d225c (Yoshi, sleep stage 2)
           holds, and CleanupResources deletes it. */
        dExtSimpleModel_c *mSleepBubbleModel; /* 0x1d8 */
        /* The three __cxa_vec_cleanup calls in ~Player, in the order it makes them
           (last array first). Both element types assert 0x14, which is exactly
           the stride the destructor passes. unk_25c and unk_270 were the
           currFrame of mTexSeqPlayer[0] and [1] -- +0x08 into each element. */
        dExtAnmTexPat_c mTexSeqBody[4];   /* 0x1dc */
        dExtAnmMaterial_c mMatChanger[2]; /* 0x22c */
        dExtAnmTexPat_c mTexSeqPlayer[2]; /* 0x254 */
        /* One saved word per material, per model: func_ov002_020e6bb0 allocates
           numMaterials words and copies each runtime material's +0x1c
           (TEXIMAGE_PARAM) into them. func_ov002_020e6b74 writes them back
           (TurnOffToonShading, cap-change phases 3/4) after func_ov002_020e6b3c
           zeroed them for the untextured flash. Element type kept as stored. */
        s32 mBodyTexParams[4]; /* 0x27c */
        s32 mHeadTexParams[8]; /* 0x28c */
    } mModels;                 /* 0x0dc */
    /* ~Player calls _ZN17dExtShadowModel_cD1Ev on this, and dExtShadowModel_c asserts
       0x28 -- which closes exactly at mdCcAcPos_c. */
    dExtShadowModel_c mShadowModel;            /* 0x2ac */
    /* ~Player calls _ZN10dCcAcPos_cD1Ev on this too, and the
       0x40 it asserts closes exactly at mAttackClsn. The four markers it
       absorbs are all dCc_c's own, reached through the base:
       unk_2d8 = radius (+0x04), unk_2dc = height (+0x08),
       mBodyClsnFlags = flags (+0x18), unk_2f0 = vulnFlags (+0x1c).
       mBodyClsnFlags in particular was never a Player field -- it is the
       body collider's flags word, which is why thirteen Player methods
       set and clear bits in it. */
    dCcAcPos_c mdCcAcPos_c;            /* 0x2d4 */
    /* ~Player calls _ZN10dCcAcPos_cD1Ev on this, and that
       type asserts 0x40 -- which closes exactly at mRidingShell. */
    dCcAcPos_c mAttackClsn;            /* 0x314 */
    s32 mRidingShell;            /* 0x354 */
    s32 mHeldObj;            /* 0x358 */
    s32 mGrabbedByActor;            /* 0x35c */
    s32 mObjInMouth;            /* 0x360 */
    s32 mAttachedActor;            /* 0x364 */
    /* An actor pointer, not a byte: StartTalk stores its fBase_c& argument
       here, St_Talk_Cleanup clears a bit in the pointee's +0xb0 (dActor_c's
       mFlags), and every access in the ROM is a full word. */
    fBase_c *mTalkActor; /* 0x368 */
    /* The actor owning the surface func_ov002_020c607c's vertical line probe
       hit: cleared at entry, then dActor_c::FindWithID(GetClsnID()) on a hit.
       Its caller compares it across two probes to see whether both landed on
       the same actor. */
    dActor_c *mLineClsnActor; /* 0x36c */
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
    /* The collider being climbed: StartClimb, reached from a dCc_c hit
       callback, stores it and enters St_Climb; St_Headstand_Main stands on its
       top (+0x08 height); func_ov002_020caf68 clears it. Every access is a word. */
    dCc_c *mClimbClsn; /* 0x37c */
    /* ~Player calls _ZN10dBgCh_ActrD1Ev on this FIRST, and dBgCh_Actr
       asserts 0x1bc -- which closes exactly at mSpawnPosX. */
    dBgCh_Actr mMeshClsn;            /* 0x380 */
    /* The spawn point, saved once and restored on death. Player::InitResources
       writes all four from the live values (0x53c..0x544 <- mPosX/Y/Z, and
       mSpawnAngleY <- mAngleY); St_Respawn_Init reads them back the other way.
       Two functions, opposite directions, the same four slots -- which is what
       makes these names evidenced rather than guessed. */
    s32 mSpawnPosX;            /* 0x53c */
    s32 mSpawnPosY;            /* 0x540 */
    s32 mSpawnPosZ;            /* 0x544 */
    s32 mPreClsnPosX;            /* 0x548 */
    s32 mPreClsnPosY;            /* 0x54c */
    s32 mPreClsnPosZ;            /* 0x550 */
    /* Floor normal, fx12 (1.0 == 0x1000 -- already rescaled from KCL's 0x400).
     * Stored as three consecutive words by Player::SetFloorSurfaceInfo
     * (0x020c16ec) at 0x020c1768 / 0x020c1770 / 0x020c1778, sourced
     * from SurfaceInfo::CopyNormalTo (func_02037dcc). 0x554 and 0x55c were
     * marked padding until those stores were disassembled.
     * mFloorNormalY is the slope-test input: func_ov002_020f035c compares it
     * against a per-floor-class cosine threshold. */
    s32 mFloorNormalX;            /* 0x554 */
    s32 mFloorNormalY;            /* 0x558 */
    s32 mFloorNormalZ;            /* 0x55c */
    /* Wall normal, fx12, and the exact counterpart of the floor normal above.
     * func_ov002_020c25a8 writes all three from
     * mMeshClsn.GetWallResult()->surface.CopyNormalTo(wn)
     * -- the same helper, the same shape, one surface over. It then pushes the
     * actor out along it (`mPosX -= mWallNormalX * 2`, `mPosZ -= mWallNormalZ * 2`).
     * Seven bodies read the pair back as `cstd::atan2(mWallNormalX, mWallNormalZ)`
     * to get the wall's facing: St_Shell_Main, St_OnWall_Main (twice),
     * St_Balloon_Main, St_CrazedCrate_Main, func_ov002_020c2138,
     * func_ov002_020dd2f4 and func_ov002_020e28d4. 0x564 was marked padding until
     * that store was disassembled, exactly as 0x554/0x55c were. */
    s32 mWallNormalX;            /* 0x560 */
    s32 mWallNormalY;            /* 0x564 */
    s32 mWallNormalZ;            /* 0x568 */
    /* The sleep bubble's scale, fx12, the Vector3 Render passes to the bubble
       model's draw. func_ov002_020d228c and St_Wait_Main zero all three as
       they enter sleep stage 2; func_ov002_020d2f24 (Yoshi only) shrinks it by 0x180
       over breath frames 6..0x1c and grows it over 0x24..0x3a, clamped to
       0x400..0x4000. St_Wait_Main wakes the player once X reaches 0x3000, and
       func_ov002_020e444c pushes the bubble out by Z * 0x2400. */
    s32 mSleepBubbleScaleX; /* 0x56c */
    s32 mSleepBubbleScaleY; /* 0x570 */
    s32 mSleepBubbleScaleZ; /* 0x574 */
    /* A 50-entry trail buffer, built by InitResources (new Vector3[50], new
       u8[50]). func_ov002_020bf13c writes entry 0 for Yoshi only: a point 0x78
       behind him, and speed/airborne/moving flags. No other reader in the ROM. */
    Vector3 *mTrailPos; /* 0x578 */
    u8 *mTrailFlags;    /* 0x57c */
    u8  pad_580[0x8];
    s32 mHeldObjQueue;            /* 0x588 */
    u8  pad_58c[0x74];
    /* Not a field of its own: byte 0x14 of the head Matrix4x3 at 0x5ec..0x61c
       that func_ov002_020e4768 stores (body matrix times bone 15) and
       d_a_bmb.cpp's func_ov102_0214b53c copies out. Nothing reads 0x600 alone;
       the matrix still straddles pad_58c, this byte and pad_601. */
    u8  unk_600;            /* 0x600 */
    u8  pad_601[0x1b];
    /* Yoshi's per-player material word for VS mode. func_ov002_020e5948 sets it
       to word +0x20 of material 0 of Yoshi's body model (GetBodyModelID(3))
       plus mPlayerNo * 2; the draw path copies it into word +0x20 of every
       body and head material when data_0209f2d8 == 1 and param1 == 3. */
    s32 mVsYoshiMaterialParam;          /* 0x61c */
    s32 mLoopingSoundHandle;            /* 0x620 */
    s32 mSurfaceSoundHandle;            /* 0x624  Sound::PlayLong handle recycled by func_ov002_020bfa74 */
    s32 mParticle1;            /* 0x628 */
    s32 mParticle2;            /* 0x62c */
    s32 mParticle3;            /* 0x630 */
    /* Particle handles at the water surface. func_ov002_020ce8bc passes each
       back to func_02022cbc (Particle::System::New) at x/z of the player and
       y = data_0209f32c + 0x3000: the first when its speed argument (mHorzSpeed
       or mPrevVertSpeed) is nonzero, the second when it is zero. Leaving Mega
       (func_ov002_020bdd2c) zeroes both along with mParticle1. */
    s32 mWaterMoveParticle;       /* 0x634 */
    s32 mWaterStillParticle;      /* 0x638 */
    u32 mCharFileBase;            /* 0x63c */
    s32 mPrevVertSpeed;            /* 0x640 */
    s32 mGroundY;            /* 0x644 */
    /* The floor probe in func_ov002_020c14b8 stores its clsnY here
       (0x80000000 on a miss) and copies it to mGroundY; four no-control
       sequences then overwrite mGroundY with mPosY, but not this copy.
       func_ov002_020c0fb4 tests mPosY against it on a lava floor. */
    s32 mProbedGroundY; /* 0x648 */
    /* Height of the water surface above the player: set from a dBgCh_Gnd
       probe that detects water when the hit's CLPS has the water bit (0x20),
       0x80000000 when there is none, and copied to data_0209f32c. Read by
       St_Swim_Main and func_ov002_020c231c (no floor snap over water). */
    s32 mWaterSurfaceY; /* 0x64c */
    /* Height of the toxic-gas surface: func_ov002_020c14b8's water/toxic
       probe stores clsnY when the hit's CLPS has the toxic bit (0x02000000).
       More than 0x64000 below it, func_ov002_020c133c hurts the player and
       St_Wait_Init plays anim 5. */
    s32 mToxicSurfaceY; /* 0x650 */
    /* Yoshi's tongue against walls. func_ov002_020d6dac, run by the tongue
       grab func_ov002_020d6998 (mHatCharacter 3), casts a dBgCh_Lin between
       two bones of mHeadModels[3] and stores func_02053274's distance from
       the first to the hit point here; Behavior scales that model's bone by
       |dist| >> 3 while it is nonzero, and ChangeState zeroes it. */
    s32 mTongueHitDist; /* 0x654 */
    /* --- Unpacked CLPS (collision property) fields ---------------------
     * Player::SetFloorSurfaceInfo (0x020c16ec) calls one bitfield
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
    s32 mFloorClass;    /* 0x658  CLPS bits 12-14, getter CLPS::GetFloorClass */
    s32 mClpsBits15_18; /* 0x65c  CLPS bits 15-18, getter CLPS::func_02037e48 */
    s32 mClpsBits6_11;  /* 0x660  CLPS bits 6-11, getter CLPS::func_02037e68 */
    s32 mSurfaceType; /* 0x664  CLPS bits 19-23, getter CLPS::GetSurfaceType; values 6..9 are the quicksand tiers */
    /* mSurfaceType as func_ov002_020c14b8's floor probe left it (0 on a
       miss), kept beside mProbedGroundY. func_ov002_020c0fb4 tests it == 1,
       the lava floor: within 0x2000 of mProbedGroundY it starts St_BurnLava. */
    s32 mProbedSurfaceType;          /* 0x668 */
    s32 mGroundSoundType;            /* 0x66c  CLPS bits 0-4, getter CLPS::func_02037e84 */
    s32 mClpsWord1Lo;                /* 0x670  CLPS word1 bits 0-7, getter CLPS::func_02037e90 */
    s32 mHurtDamage;            /* 0x674 */
    /* Two music layers over the level music, both Sound::SetMusic ids.
       StartPowerupMusic(id) starts a power-up's music in mPowerupMusic
       (Mega 0x31, Fire Yoshi 0x32, Vanish/Wings 0x33, Metal 0x34, Balloon
       0x30) and EndPowerupMusic ends it; StartStateMusic and
       EndStateMusic do the same for mStateMusic (St_Owl 0x2f, St_Shell
       and St_Swim 0x33). mPlayingMusic is the one SetMusic last started;
       ending a layer restarts the other if it is still set. */
    u32 mPowerupMusic;     /* 0x678 */
    u32 mStateMusic;       /* 0x67c */
    u32 mPlayingMusic;     /* 0x680 */
    s32 mPeakY;            /* 0x684 */
    s32 mAttachOffsetY;            /* 0x688 */
    s32 mSinkDepth;            /* 0x68c */
    /* Body model offset from mPos. func_ov002_020e444c builds the model
       matrix from (0, mModelOffsetY - mSinkDepth, mModelOffsetZ) rotated by
       mAngleY, and func_ov002_020e4374 adds mModelOffsetY to the shadow
       height. States set them (balloon 0x64000, swim from mAngleX, the
       water-run sink); ChangeState zeroes both. */
    s32 mModelOffsetY;           /* 0x690 */
    s32 mModelOffsetZ;           /* 0x694 */
    s16 mSpawnAngleY;            /* 0x698 */
    /* Downhill direction of the current floor, binary angle (0x10000 per turn).
     * Player::SetFloorSurfaceInfo computes it as cstd::atan2(mFloorNormalX,
     * mFloorNormalZ) and stores it with a 16-bit strh at 0x020c178c (written
     * as [r5 + 0x600] + 0x9a). Consumed by Player::ApplySlopeAccel
     * (func_ov002_020c04e0) via AngleDiff(mFloorAngle, moveYaw) to decide
     * whether a slope speeds you up or slows you down. */
    s16 mFloorAngle;            /* 0x69a */
    s16 mAngleYSpeed;            /* 0x69c */
    /* X-axis counterpart of mAngleYSpeed. St_Fly_Init zeroes both, and
       func_ov002_020df8f0 eases this one toward a stick-times-speed target
       and adds it to mPrevAngleX (clamped to +-0x2aaa) as mAngleYSpeed is
       added to mPrevAngleY. St_Balloon_Init sets both to 0x100, and
       St_Balloon_Main runs (mAngleX, mAngleXSpeed) and (mAngleZ,
       mAngleYSpeed) through func_ov002_020de3d0. Other code borrows the
       slot: St_Swim_Main loads it from mAngleZ, func_ov002_020cd448 eases
       it toward mAngleZ by 0x200 and writes it back, and TryEnterStarDoor
       stores its `kind` here as the yaw func_ov002_020c8a4c rotates the
       approach offset by. */
    s16 mAngleXSpeed;                /* 0x69e */
    u16 mInvincibleTimer;            /* 0x6a0 */
    u16 mPrevAreaId;            /* 0x6a2 */
    u16 mStateTimer;            /* 0x6a4 */
    u16 mStateWaitTimer;            /* 0x6a6 */
    /* u16, not s16: every load of this slot in the ROM is an ldrh. Declared
       signed, St_Hurt_Main's `!= 0` test compiles to ldrsh and the function
       misses by exactly one word. Nothing anywhere reads it signed -- the
       other five users either store, or cast to u16* first (including
       Behavior's DecIfAbove0_Short(u16*)). Same defect class as the imported
       parameter widths in dActor_c.h: a declared type nothing had checked. */
    u16 mJumpComboTimer;            /* 0x6a8 */
    u16 mPunchKickCooldown;            /* 0x6aa */
    /* func_ov002_020d3b9c sets it to 4 each frame the stick is held,
       Behavior counts it down; the no-stick branch only decelerates (and
       may start mIsBraking) once it reaches 0. */
    u16 mStickGraceTimer;         /* 0x6ac */
    u16 mPowerupTimer;            /* 0x6ae */
    u16 mCrouchTimer;            /* 0x6b0 */
    /* Hold between two steps of the scale animation at mScaleAnimStep:
       func_ov002_020db704 sets it to 10 when step 1 or 6 reaches its
       target and moves on when Behavior has counted it down to 1. */
    u16 mScaleAnimTimer;            /* 0x6b2 */
    u16 mHoldHeavyTimer;            /* 0x6b4 */
    /* Climb re-grab cooldown: St_Climb_Cleanup sets it to 8 when the player
       leaves the climb for a non-climb state, Behavior counts it down, and
       StartClimb (the climb-start collider callback) refuses while
       it is nonzero. */
    u16 mClimbCooldown;        /* 0x6b6 */
    u16 mWalkTimer;            /* 0x6b8 */
    /* Luigi's run-over-liquid windows, both counted down in Behavior.
       func_ov002_020c2b08 arms the first (0x3c or 0x1e) on a water floor;
       func_ov002_020c29d4 arms the second (0x1e), and while it runs the
       quicksand handler skips sinking. */
    u16 mWaterRunTimer;     /* 0x6ba */
    u16 mQuicksandRunTimer; /* 0x6bc */
    /* Fire Yoshi: func_ov002_020d8118 sets it to 0x258 with mIsFire and the
       power-up music 0x32; Behavior counts it down and, at 0, calls
       func_ov002_020d80d0, which clears all three. While it runs,
       St_YoshiPower_Init refills mFireBreaths to 2. */
    u16 mFireTimer; /* 0x6be */
    /* u16, not s16: every READ of this slot in the ROM is an ldrh --
       func_ov002_020e4bb8 tests `< 0x3f` and `& 1`, Behavior passes it to
       DecIfAbove0_Short(u16*), St_Balloon_Main tests `== 0`. Only the stores
       use short, and a store cannot distinguish the two. Declared signed,
       St_Balloon_Main misses by one word. Same defect as mJumpComboTimer.
       Named for what those five uses do: InitBalloonMario sets it to 0x258,
       Behavior counts it down, St_Balloon_Main exits when it reaches 0, and
       020e4bb8 blinks the balloon below 0x3f. */
    u16 mBalloonTimer;            /* 0x6c0 */
    /* Mega: func_ov002_020bdf8c sets mIsMega and this to 0x258 (with the
       power-up music 0x31), func_ov002_020c8d14 resets it to 0x258, and
       when Behavior has counted it down to 0 it calls func_ov002_020bdd2c,
       which shrinks the player back and ends the music. */
    u16 mMegaTimer; /* 0x6c2 */
    /* Set to 2 by the quicksand handler func_ov002_020bfec0 in the land
       state (0x02110424) unless the previous state was 0x021104cc; Behavior
       counts it down. func_ov002_020e2ad0, run by the jump inits, damps the
       jump out of quicksand only once it is 0; before that it just keeps
       mSinkDepth at least 0xb000. */
    u16 mQuicksandJumpTimer;        /* 0x6c4 */
    u16 mMouthHoldTimer;            /* 0x6c6 */
    u16 mTeleportTimer;            /* 0x6c8 */
    /* func_ov002_020cec2c (the water states): counts up while the player is
       0x8c000 below the water surface or in levels 0x13/0x14/0x30, and
       costs 0x100 health on reaching 0xf0 (0x50 in those levels); reset to
       0 otherwise. */
    u16 mAirDrainCounter;   /* 0x6ca */
    /* St_Jump_Init zeroes it; no reader in the matched code. */
    u16 unk_6cc;            /* 0x6cc */
    u16 mStateFlags;            /* 0x6ce */
    u16 mMegaKillCount;            /* 0x6d0 */
    s16 mDesiredAngleY;            /* 0x6d2 */
    s16 mPrevDesiredAngleY;            /* 0x6d4 */
    s16 mPreClsnAngleY;            /* 0x6d6 */
    u8  mPlayerNo;            /* 0x6d8 */
    u8  mCharacter;            /* 0x6d9 */
    u8  pad_6da[0x1];
    u8  mBodyModelId;            /* 0x6db */
    u8  mPrevCharacter;            /* 0x6dc */
    u8  mHatCharacter;            /* 0x6dd */
    u8  mIsAirborne;            /* 0x6de */
    u8  mLandSoundPlayed;            /* 0x6df */
    /* Set by func_ov002_020d454c when the stick is let go above walking
       speed 0x10000 (anim 0x36, speed capped); while set, St_Walk_Main plays
       the ground-type sound 0xe2 + mGroundSoundType and func_ov002_020d3b9c
       decelerates. Stick input, anim 0x37 ending and ChangeState clear it
       (func_ov002_020d4540). */
    u8 mIsBraking;                  /* 0x6e0 */
    u8  mJumpComboStage;            /* 0x6e1 */
    u8  mPunchKickStep;            /* 0x6e2 */
    u8  mStateStep;            /* 0x6e3 */
    u8  mIsSlidingOnGround;            /* 0x6e4 */
    u8  mStateWork;            /* 0x6e5 */
    u8  mStatePhase;            /* 0x6e6 */
    u8  mSlideStoppedTimer;            /* 0x6e7 */
    u8  mTeleportId;            /* 0x6e8 */
    u8  mClsnFlags;            /* 0x6e9 */
    /* Latches for the two timers at 0x6ba/0x6bc: set when the water run is
       armed, cleared on land; bit 0 set by the quicksand handler, 0x80 once
       func_ov002_020c29d4 has evaluated it. Cap change zeroes both. */
    u8 mWaterRunLatch;      /* 0x6ea */
    u8 mQuicksandRunFlags;  /* 0x6eb */
    /* When set, func_ov002_020c25a8 makes mMeshClsn detect water, so a water
       surface holds the player up. Only Luigi (param1 1) sets it: St_Walk_Init
       outside shallow water, and func_ov002_020c2b08 when he walks on
       non-water ground after a water run. ChangeState clears it. */
    u8 mDetectWater; /* 0x6ec */
    /* 0x1e once func_ov002_020d2fdc has ramped mStateWork to 0x1e with input
       flag 0x800 (data_0209f49c) held; func_ov002_020d3b9c then counts it
       down while the flag stays set, scaling walk speed by 0x1333 with accel
       0x1e000, and clears it when the flag drops. While it is nonzero,
       St_Walk_Init and St_Crouch_Init play a voice. */
    u8 mDashBoostTimer;     /* 0x6ed */
    /* St_YoshiPower_Main sets it to 1 when the swallowed object's v18() is
       4 or 2, St_YoshiPower_Init clears it; no reader in the matched code. */
    u8  unk_6ee;            /* 0x6ee */
    u8 pad_6ef[0x1];
    /* Turn ramp in func_ov002_020d3b9c's walk turning (input mode byte
       data_0209f4ac == 0, stick at least 0xa00): counts up to 0x18 while the
       stick is 0x1000 or more off mPrevAngleY and drops by 2 otherwise. The
       ApproachAngle divisor is 0x10 - ramp / 2, so the longer a sharp turn is
       held, the faster the player swings round. */
    u16 mTurnRamp; /* 0x6f0 */
    /* A squash-and-stretch on mScale, run each frame by func_ov002_020db704
       and started by func_ov002_020db8bc(kind), which sets the step to 1 and
       zeroes mScaleAnimTimer. Kinds 0 and 1 step mScaleY through the tables
       data_ov002_020ff2c0 / data_ov002_020ff230 with X and Z set to
       (0x1000 - Y) / 2 + 0x1000: 0 after a stomp (func_ov002_020d869c),
       1 after St_Squish and a long fall (func_ov002_020e2c84). Kind 2 grows
       the scale to 0x3000, kind 3 shrinks it to 0x1000 (leaving Mega,
       func_ov002_020bdd2c). Step 0 is idle; starting Mega
       (func_ov002_020bdf8c), func_ov002_020c61ac and Unk_020c6a10 reset it. */
    u8 mScaleAnimStep; /* 0x6f2 */
    u8 mScaleAnimKind; /* 0x6f3 */
    /* Fire breaths Yoshi has left: St_YoshiPower_Init plays the breath
       (anim 0x6d, voice 0x101) instead of the tongue while it is nonzero and
       takes one. Swallowing a type-5 object leaves one (func_ov002_020d6790,
       St_YoshiPower_Main), InitFireYoshi's func_ov002_020d8158 sets 1, and
       mFireTimer refills it to 2; func_ov002_020d80d0 and the cap change
       clear it. */
    u8 mFireBreaths;         /* 0x6f4 */
    u8  mOpacity;            /* 0x6f5 */
    u8  mIsControlDisabled;            /* 0x6f6 */
    u8  mSwimMusicPushed;            /* 0x6f7 */
    /* Fire Yoshi is on: func_ov002_020d8118 sets it with mFireTimer, and
       func_ov002_020d80d0 does nothing unless it is 1, then clears it,
       mFireTimer and mFireBreaths and ends the music. */
    u8 mIsFire;              /* 0x6f8 */
    u8  mIsMetal;            /* 0x6f9 */
    /* The power-up state as drawn. func_ov002_020e4bb8 copies mIsMetal,
       mIsBalloon, mHasWings and HasNoCap() here after the update (mIsVanish
       goes to mPlayerTexFrame), dropping the power-ups on the off frames of
       the blink when mPowerupTimer or mBalloonTimer is below 0x3f; Render,
       GetBodyModelID(id, true) and func_ov002_020becf4(id, 1) read these
       copies rather than the live flags. */
    u8 mDrawnIsMetal;         /* 0x6fa */
    u8  mIsVanish;            /* 0x6fb */
    u8  mPlayerTexFrame;            /* 0x6fc */
    u8  mIsBalloon;            /* 0x6fd */
    u8 mDrawnIsBalloon;        /* 0x6fe */
    u8  mHasWings;            /* 0x6ff */
    u8 mDrawnHasWings;        /* 0x700 */
    u8 mDrawnHasNoCap;        /* 0x701 */
    /* func_ov002_020e4bb8's blink phase: 0, then mPowerupTimer & 1 once the
       timer is below 0x3f; a 0 there drops the wings, vanish and metal from
       that frame's drawn copies. */
    u8 mPowerupBlinkOn;     /* 0x702 */
    u8  mIsMega;            /* 0x703 */
    u8  mEggParams;            /* 0x704 */
    u8  pad_705[0x1];
    u8  mIsUnderwater;            /* 0x706 */
    u8  mIsInShallowWater;            /* 0x707 */
    /* u8, not s8: every read of this slot is an ldrb (func_ov002_020d0580,
       func_ov002_020d06c0, daKuriKing, and the raw +0x708 tests). Declared
       signed, those two functions load it with ldrsb and miss. */
    u8 mIsTakingDamage;          /* 0x708 */
    u8  mIsNoControl;            /* 0x709 */
    u8  mNoCtrlKind;            /* 0x70a */
    u8  mIsOpeningBigDoor;            /* 0x70b */
    u8  mStateArg;            /* 0x70c */
    u8  mIsFallScreaming;            /* 0x70d */
    u8  mSlideType;            /* 0x70e */
    /* Set when func_ov002_020c231c snaps mPosY onto mGroundY on a class-1
       floor; while set, the next call snaps again within the same 0xc8000
       window. Cleared on every other exit. */
    u8 mSnapToFloor;        /* 0x70f */
    /* 1 while the move input asks for a walk rather than a run:
       func_ov002_020d4748 sets it each frame from the stick magnitude
       against 0xdc7 (0xe00 in input mode 2) plus 0x80 while it is already
       set, or, in input mode 0, from input flag 0x800 not being held. It
       picks animation 0x48 there, and St_Walk_Main lowers its stop
       threshold by 0x80 while it is set. */
    u8 mIsWalkPace; /* 0x710 */
    /* Set to 1 only by func_ov002_020c71e0, the last step of St_LevelEnter;
       dScStage_c::RenderVsModeCountdown counts the players that have it. */
    u8 mLevelEntryDone;           /* 0x711 */
    u8  mIsInAirState;            /* 0x712 */
    u8  mIsBodyClsnEnabled;            /* 0x713 */
    u8  mUseAltBodyModel;            /* 0x714 */
    u8  mUseFarCamera;            /* 0x715 */
    /* func_ov002_020c2db8 runs the floor probe, then returns before
       everything that reacts to it (slide type, floor snap, wall push,
       ground sounds, the lava and water-run checks) while this is set.
       Talk, DeadPit, Squish, Grabbed and the scripted sequences
       (func_ov002_020c3bdc, func_ov002_020c9ac0 and its siblings) set it;
       ChangeState and St_Respawn_Init clear it. */
    u8 mSkipGroundUpdate; /* 0x716 */
    /* func_ov002_020e444c skips DropShadowRadHeight while it is set. The
       Headstand, Climb and Grabbed inits set it, 020e444c itself sets it
       when the player is fully transparent, and ChangeState and the balloon
       blink in func_ov002_020e4bb8 clear it. */
    u8 mHideShadow;                      /* 0x717 */
    u8  mLoadedResourceFlags;            /* 0x718 */
    s8  mKeyModelId;            /* 0x719 */
    u8  mHasNoCap;            /* 0x71a */
    u8  mJumpedFromQuicksand;            /* 0x71b */
    /* func_ov002_020c1234 sets this on mGroundSoundType 0xa/0xb; while it is
       set, each call issues Sound::PlaySub(0x35, ...), and the first one that
       succeeds on any other ground type clears it. */
    u8 mGroundSubSoundActive; /* 0x71c */
    /* func_ov002_020c133c's damage cadence: 0x40 after each Hurt, counted
       down one per call while the hazard persists, cleared when it ends. */
    u8 mDamageTickTimer; /* 0x71d */
    /* A one-shot event sequence. func_ov002_020c43c4(id) latches the id
       (once per save, via a bit in data_0209caa0[2]) and zeroes the step;
       func_ov002_020c4188 then walks mEventStep 0..4, holding mEventWait
       ticks before handing data_ov002_020ff26c[mEventId - 1] to
       func_0201f32c. */
    u8 mEventId;                /* 0x71e */
    u8 mEventStep;              /* 0x71f */
    u8 mEventWait;              /* 0x720 */
    u8  mSleepStage;            /* 0x721 */
    /* func_ov002_020c8b78 holds the music at volume 0x14 while its animation
       runs, then sets this and opens message 0x188 (0x29); St_Talk_Main
       calls Sound::ChangeMusicVolume(0x7f, ...) each frame while it is set
       and clears it once the volume is back. */
    u8 mRestoreMusicVolume; /* 0x722 */
    /* While set, St_Wait_Main skips the two sleep voices (0x2a in step 2,
       0x2b in step 4). daC_Jugem_c::StateArriveMain sets it on the closest
       player at its first tick and clears it at tick 0x82. */
    u8 mMuteSleepVoice; /* 0x723 */
    /* St_Owl_Main sets it in step 1 once mPosY is below mAttachOffsetY -
       0xed8000, calls func_0201f32c(0xa3) and hands the camera to
       func_0200d3f8 with no look target; it clears it in step 3 once camera
       flag 0x8000 is clear. That camera state's main, func_02005110, pulls
       the eye out to 0xd0000 instead of 0x48000 while it is set. */
    u8 mOwlFarCamera; /* 0x724 */
    /* func_ov002_020c44c4 sets it when it opens the one-time message 0x177;
       St_Talk_Cleanup then calls Sound::LoadAndSetMusic_Layer1(0x39) and
       clears it. */
    u8 mPlayMusicAfterTalk; /* 0x725 */
    /* Set on both players when their attacks meet: func_ov002_020d8944
       (two facing players, actor 0xbf) knocks both back and spawns clash
       particles 0x12a/0x12b, and func_ov002_020d8360/func_ov002_020d8854
       latch it on both sides. While it is set, func_ov002_020d8360 ignores
       that player's hit and func_ov002_020d8a50 skips the attack update.
       St_PunchKick_Init, St_SweepKick_Init and ChangeState clear it. */
    s8 mAttackClashed; /* 0x726 */
    /* The scripted animation sequence the Kuppa script started. Command
       func_ov002_020bd20c stores its parameter byte here (1-based, 0 for
       none) and zeroes mAnimScriptStep, after func_ov002_020bd06c has
       closed the old one; func_ov002_020bcdf0 plays entry mAnimScriptStep of
       data_ov002_0210a7a0[mAnimScriptId - 1] (anim id, parameter) and steps
       to the next when the animation finishes, until an id of 0. */
    u8 mAnimScriptId;   /* 0x727 */
    u8 mAnimScriptStep; /* 0x728 */
    u8 pad_729[0x3];
    /* A cap (actor 0x10d, the actor daMky_c spawns as the lost cap).
       func_ov002_020bd06c spawns it when animation script 8, 9 or 10 starts
       (param 0x14..0x16 | param1 << 8) and destroys it when one of those
       scripts is left; func_ov002_020bcdf0 keeps it on mPos and the
       player's angles. */
    dActor_c *mCapActor; /* 0x72c */
    /* A flight along the control points data_ov002_0210a8b8 ({speed, x, y,
       z} rows), a uniform cubic B-spline: func_ov002_020c3148 resets it,
       func_ov002_020c3160 weights four rows from mSplinePoint by the basis
       at mSplineT (fx12), using mSplineSpan 1..5 to pick the clamped-end
       variants, and func_ov002_020c2fec advances mSplineT by the row's
       speed / 1000, moving to the next point at 0x1000. Driven by
       func_ov002_020c38a0 (anim 0xc1). */
    u8 mSplinePoint; /* 0x730 */
    u8 pad_731[0x3];
    s32 mSplineT;             /* 0x734 */
    s32 mSplineSpan;          /* 0x738 */
    u16 mCapFlags;            /* 0x73c */
    /* Ticks left in the current mCapFlags phase. The cap-change update loads
       it from a per-phase table, counts it down every frame, and advances
       mCapFlags to the next phase when it reaches 0. */
    u16 mCapTimer; /* 0x73e */
    /* fx12 weight, 0..0x1000: raised by 0x199 a frame in cap phase 2,
       lowered in phase 6, and passed every frame to func_ov002_020e6a78,
       which runs Color_Interp over 0x20 palette entries with it. */
    u16 mCapPaletteBlend; /* 0x740 */
    /* How func_ov002_020e640c applies mHeadRot to body bone 15 (face_1):
       0 off, 1 add, 2 set, 3 set swizzled (and mSpineRot to bone 8),
       4 ease back to the animated pose. func_ov002_020e6780 zeroes it. */
    u8 mHeadRotMode; /* 0x742 */
    /* Root motion for body bone 1, run by func_ov002_020e640c: 0 off; an odd
       value (1 horizontal, 3 with Y) latches the bone's translation into
       mStateTarget and mStateAnchor and steps to the even value, which then
       moves mPos by the bone's per-frame delta rotated by mAngleY and zeroes
       the bone. Armed by scripted animations (event case 14 in
       func_ov002_020bd06c, func_ov002_020c3bdc, func_ov002_020c9ac0,
       func_ov002_020c9c4c) and cleared when they end. */
    u8 mRootMotionMode; /* 0x743 */
    /* A point the current state works with, written per state rather than
       owned by one: the talk/look target (ShowMessage2, func_ov002_020c51d0,
       func_ov002_020c47f4, St_Talk_Main's camera focus, daC_Jugem's position),
       the walk-to point (func_ov002_020c8a4c, TryEnterStarDoor), and the
       cage JumpIntoBooCage records. mRootMotionMode reuses it as last frame's
       bone translation and func_ov002_020e3f90 as an eased carry offset.
       Kept as three s32: a Vector3 overlay would draw @NNN numbers. */
    s32 mStateTargetX; /* 0x744 */
    s32 mStateTargetY; /* 0x748 */
    s32 mStateTargetZ; /* 0x74c */
    /* The fixed point mStateTarget is derived from: the door position
       TryEnterStarDoor records (the approach point is rotated off it), or
       the bone translation mRootMotionMode latches on its first frame. */
    s32 mStateAnchorX; /* 0x750 */
    s32 mStateAnchorY; /* 0x754 */
    s32 mStateAnchorZ; /* 0x758 */
    /* Extra rotation for body bone 8 ("spin", the spine): func_ov002_020e640c
       adds [1]/[2] every frame and sets all three in mHeadRotMode 3. */
    s16 mSpineRot[3]; /* 0x75c */
    /* Extra rotation for body bone 15 (face_1, the head), applied per
       mHeadRotMode. Every access is a halfword. */
    s16 mHeadRot[3]; /* 0x762 */
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

    /* --- vtable. _ZTV6Player (0x0210a83c, ov002) is dActor_c's 31 slots with
           eight overridden and NO new virtuals; see notes/actor-vtables.md.

           The destructor is declared FIRST on purpose. CW 1.2 emits the vtable
           into the translation unit that DEFINES the first non-inline virtual
           declared -- the key function -- and that copy collides with the one
           the module's gap object supplies from ROM data. Because Player only
           overrides, an override takes its base's slot wherever it is declared,
           so putting ~Player first costs nothing and pins the key function to
           the destructor. src/game/player/Player.cpp defines it out of line,
           so that TU alone emits _ZTV6Player, and ov002/Player.json
           externalizes the copy to the ROM table. Never define ~Player inline:
           key-function status would pass to InitResources, and the retail D0/D1
           order comes from the out-of-line definition (see its comment there).
           Player() is defined in the same file; the ROM homes only its C1. --- */
    Player();
    virtual ~Player();                  /* slots 16, 17 */
    virtual s32  InitResources();       /* slot  0 */
    virtual s32  CleanupResources();    /* slot  3 */
    virtual s32  Behavior();            /* slot  6 */
    virtual s32  Render();              /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */
    virtual int  OnYoshiTryEat();       /* slot 18 -- int, not u32: it overrides
                                           dActor_c::OnYoshiTryEat and CW rejects a
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
    /* Unnamed parameters: a named one advances @NNN in every Player.h consumer. */
    int Hurt(const Vector3 &, u32, Fix12i, u8, u8, u8);
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
    int ShowMessage(fBase_c & a_, unsigned int b, const Vector3 * v, unsigned char d_, unsigned char e_);
    int ShowMessage2(fBase_c & actor_, unsigned int msg, const Vector3 * pos, unsigned char d_, unsigned char e_);
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
    /* Offered a collider by dCc_c's player-hit callback; grabs it and enters
       St_Climb. Returns 1 if it did. Coined name. */
    int StartClimb(dCc_c *);
    int St_Climb_Cleanup();
    int St_Climb_Init();
    int St_Climb_Main();
    int St_Crawl_Init();
    int St_Crawl_Main();
    int St_CrazedCrate_Cleanup();
    int St_CrazedCrate_Init();
    int St_CrazedCrate_Main();
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
    int St_DizzyStars_Cleanup();
    int St_Electrocute_Init();
    int St_Electrocute_Main();
    int St_EndingFly_Init();
    /* ov002 0x020c3d1c, immediately before St_EndingFly_Init. `int`, not the
       `void` this header used to declare: the ROM body returns 1. */
    int St_EndingFly_Main();
    int St_Fall_Init();
    int St_Fall_Main();
    int St_Fly_Init();
    int St_Fly_Main();
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
    int St_Land_Init();
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
    int St_SwingPlayer_Main();
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
    int St_WindCarry_Main();
    int St_YoshiPower_Cleanup();
    int St_YoshiPower_Init();
    int St_YoshiPower_Main();
    int StartTalk(fBase_c & actor_, bool b_);
    int TryEnterStarDoor(Vector3 & pos_, short kind);
    int TryExitWhiteDoorWithStar();
    int TryGrab(dActor_c & actor_);
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
    /* Unnamed parameters: a named one advances @NNN in every Player.h consumer. */
    void Bounce(Fix12i);
    void SpinBounce(Fix12i);
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
    /* Unnamed parameters: a named one advances @NNN in every Player.h consumer. */
    void SetAnim(u32, int, Fix12i, u16);
    void SetNewHatCharacter(unsigned int p1, unsigned int p2, bool p3);
    void SetRealCharacter(unsigned int chr_);
    void TurnOffToonShading(unsigned int j);
    int Unk_020ca488();

    /* Player commands of the Kuppa script (ov002 0x020bccfc..0x020bd828).
       func_ov002_020bd664 dispatches a command record to one of fourteen of
       these through a table of pointers to members, all of one signature:
       the command's parameter bytes and its begin and end frame.
       func_ov002_020bd06c is the step change func_ov002_020bd20c makes.
       The parameters are left unnamed on purpose: each NAMED parameter here
       advances mwccarm's @NNN counter in every file that includes this
       header (measured: 33 of 258 consumers renumbered their @NNN locals),
       and unnamed they leave all 258 objects byte-identical. */
    int func_ov002_020bccfc(unsigned char *, int, int);
    int func_ov002_020bcd18(unsigned char *, int, int);
    int func_ov002_020bcd38(unsigned char *, int, int);
    void func_ov002_020bd06c(unsigned int);
    int func_ov002_020bd20c(unsigned char *, int, int);
    int func_ov002_020bd250(unsigned char *, int, int);
    int func_ov002_020bd354(unsigned char *, int, int);
    int func_ov002_020bd3a0(unsigned char *, int, int);
    int func_ov002_020bd438(unsigned char *, int, int);
    int func_ov002_020bd45c(unsigned char *, int, int);
    int func_ov002_020bd480(unsigned char *, int, int);
    int func_ov002_020bd4ac(unsigned char *, int, int);
    int func_ov002_020bd4c8(unsigned char *, int, int);
    int func_ov002_020bd4e0(unsigned char *, int, int);
    int func_ov002_020bd600(unsigned char *, int, int);
    int func_ov002_020bd664(unsigned char *, int, int);

    /* The two music layers documented at mPowerupMusic. The argument is the
       layer id being started or ended. Ending always clears the layer; with
       the other layer empty it stops the music outright, otherwise it switches
       to the other layer only if the id is the one playing and differs from
       it. Unnamed, like the declarations below, so Player.h consumers keep
       their @NNN numbering. */
    void EndStateMusic(u32);
    void StartStateMusic(u32);
    void EndPowerupMusic(u32);
    void StartPowerupMusic(u32);
    /* Player state helpers in the 0x020d2f24..0x020db8d8 span (method review
       020d2f24-020db8d8). Address-named Player methods, verified against the
       ROM bodies; parameters stay unnamed so the 258 Player.h consumers keep
       byte-identical objects (see the Kuppa note above). */
    void func_ov002_020d4748();
    int func_ov002_020d5ed0();
    int func_ov002_020d5338();
    int func_ov002_020d5f98(unsigned char *, unsigned char *);
    int func_ov002_020d6048();
    int func_ov002_020d6998();
    char *func_ov002_020d708c();
    void func_ov002_020d718c();
    void func_ov002_020d71a0();
    int func_ov002_020d93ac();
    int func_ov002_020d94cc();
    void func_ov002_020d98b4();
    int func_ov002_020daa74();
    void func_ov002_020db8d8();

    void func_ov002_020bda48();
    void func_ov002_020bdb50(int);
    void func_ov002_020bdc18();
    void func_ov002_020bdd2c();
    void func_ov002_020bdd9c();
    void func_ov002_020bdef0();
    void func_ov002_020bdf8c();
    void func_ov002_020be3b0();
    int func_ov002_020bea7c();
    void func_ov002_020beabc();
    int func_ov002_020beb38();
    int func_ov002_020becf4(int, int);
    void func_ov002_020bf13c();
    void func_ov002_020bf2d8(Fix12i);
    void func_ov002_020bf36c(void *);
    int func_ov002_020bf40c();
    void func_ov002_020bf5e0();
    void func_ov002_020bf88c();
    void func_ov002_020bf90c();
    void func_ov002_020bf9d4();
    int func_ov002_020bfa74();
    int func_ov002_020bfec0();
    int func_ov002_020c0108(int);
    int func_ov002_020c031c();
    void func_ov002_020c0364(u32);
    int func_ov002_020c0434();
    int func_ov002_020c04ac();
    int func_ov002_020c04e0();
    int func_ov002_020c0688();
    int func_ov002_020c06fc(int);
    int func_ov002_020c0cbc();
    int func_ov002_020c0d90(int);
    int func_ov002_020c0fb4();

    void func_ov002_020c1234();
    void func_ov002_020c133c();
    void func_ov002_020c14b8();
    /* Unnamed parameter on purpose; see the Kuppa note above. */
    void SetFloorSurfaceInfo(const dBgPi &);
    int func_ov002_020c19d0(int, int);
    int func_ov002_020c1ad8(int);
    int func_ov002_020c1c84(int);
    void func_ov002_020c1eb4(int);
    int func_ov002_020c200c(int);
    int func_ov002_020c2138();
    int func_ov002_020c231c();
    int func_ov002_020c25a8(int);
    void func_ov002_020c29d4();
    void func_ov002_020c2b08();
    void func_ov002_020c2db8();
    void func_ov002_020c2e78();
    void func_ov002_020c2f64();
    int func_ov002_020c2fec(int *);
    void func_ov002_020c3148();
    void func_ov002_020c37a4(char *);
    void func_ov002_020c38a0();
    int func_ov002_020c3a48();
    void func_ov002_020c3bdc();
    void func_ov002_020c3cf0();
    int func_ov002_020c3ea0();
    int func_ov002_020c4188();
    void func_ov002_020c43c4(unsigned int);
    int func_ov002_020c44c4();
    int func_ov002_020c47f4();
    void func_ov002_020c5444();
    int func_ov002_020c5d0c();
    void func_ov002_020c5d60();
    int func_ov002_020c5dec(int);
    int func_ov002_020c607c(int, int, int *);
    int func_ov002_020c61ac();
    int func_ov002_020c647c(int);
    int func_ov002_020c6538();
    int func_ov002_020c6908();
    int func_ov002_020c6adc();
    void func_ov002_020c6fe4();

    void func_ov002_020c70ac();
    void func_ov002_020c7194();
    void func_ov002_020c71e0();
    void func_ov002_020c7350();
    void func_ov002_020c75f0();
    int func_ov002_020c7cbc();
    void func_ov002_020c7dd0(int);
    int func_ov002_020c7f10();
    void func_ov002_020c7ff8();
    unsigned char func_ov002_020c84b0();
    int func_ov002_020c8540();
    int func_ov002_020c8714();
    int func_ov002_020c897c();
    int func_ov002_020c8a4c();
    int func_ov002_020c8b54();
    int func_ov002_020c8b78();
    int func_ov002_020c8cb0();
    int func_ov002_020c8d14();
    int func_ov002_020c8f0c();
    int func_ov002_020c8f80();
    int func_ov002_020c904c();
    int func_ov002_020c9128();
    int func_ov002_020c91bc();

    int func_ov002_020c924c();
    int func_ov002_020c9288();
    int func_ov002_020c92fc();
    int func_ov002_020c94a4();
    int func_ov002_020c965c();
    void func_ov002_020c9718();
    void func_ov002_020c97e0();
    void func_ov002_020c97f8();
    void func_ov002_020c9840();
    void func_ov002_020c98a4();
    void func_ov002_020c9a04();
    void func_ov002_020c9ac0();
    int func_ov002_020c9b10();
    void func_ov002_020c9b5c();
    int func_ov002_020c9c00();
    void func_ov002_020c9c4c();
    void func_ov002_020c9cc0();
    int func_ov002_020c9d1c();
    void func_ov002_020c9d68();
    void func_ov002_020c9e18();
    void func_ov002_020ca108();
    int func_ov002_020cede0();
    void func_ov002_020de428();
    int func_ov002_020df7ac();
    int func_ov002_020df840(void *, Vector3 *);
};

/* Hold both claims with the compiler rather than a comment. State's 0x18 is
   the load-bearing one: it is three pointers-to-member at 8 bytes each, and
   if this toolchain represented a pmf any other way the offsets above --
   +0x00, +0x08, +0x10, read straight off ChangeState and Behavior -- would
   not line up and this would refuse to compile. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Player_size_must_be_0x768[sizeof(struct Player) == 0x768 ? 1 : -1];
typedef char Player_Models_size_must_be_0x1d0[sizeof(Player::Models) == 0x1d0 ? 1 : -1];
typedef char Player_State_size_must_be_0x18[sizeof(Player::State) == 0x18 ? 1 : -1];
#endif

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
 *     copy (size 0xb0), and the ov006 one is DrawAimMarker again.
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
