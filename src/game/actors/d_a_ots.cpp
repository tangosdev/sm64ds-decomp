//cpp
/* daOts_c -- shared base of the three Bully variants (BULLY 215 / BIG_BULLY 216
 * / CHILL_BULLY 217 (debug ICE_DONKETU)), ov064 0x02115ee0..0x02117070.
 *
 * ov064 is mixed (treasure chest, metal net lift, LLL tilting platform, daKpa_c
 * puzzle, rotating firebar, lava bubble, bully, water ring, jet stream, clam).
 * RTTI names this class daOts_c; overlay_actors maps the three children to
 * BULLY 215 / BIG_BULLY 216 / CHILL_BULLY 217 (debug ICE_DONKETU). Ugly RTTI
 * name is final.
 *
 * One translation unit, twenty-four functions, the way the cartridge's own build
 * had it. This replaces twenty-four one-function shards; their content is
 * unchanged except where several of them carried stand-in `dExtAnmModel_c` and
 * `dActor_c` structs that cannot coexist with include/daOts_c.h in a single TU,
 * and where four different spellings of the same two ROM symbols had to be
 * collapsed into one (see the extern block below). InitResourcesCommon and
 * BehaviorCommon sat immediately after Render as leftover shards; they belong
 * here (this-pointer layout plus named daOts callees), not with daDonketu_c.
 *
 * THIS TU OWNS THE CLASS VTABLE. CleanupResources (vtable slot 3) is the first
 * virtual daOts_c declares out of line -- the destructor is inline in the class
 * body on purpose, so that the three derived classes inline its vptr store --
 * which makes CleanupResources the key function and anchors _ZTV7daOts_c
 * (ov064 0x0211b768) here, along with the D1/D0 pair the table points at.
 *
 * The RTTI records do NOT live in this overlay. _ZTI7daOts_c and _ZTS7daOts_c
 * are vague-linkage symbols the linker kept in ov027 (0x021138bc and
 * 0x021138a4), next to daIDonketu_c's own pair; ov064's vtable header word at
 * 0x0211b764 relocates across to them. None of that data lies inside this
 * entry's licensed .text range, so dsd supplies the cartridge's own bytes and
 * production isolation discards the emitted duplicates; the manifest's
 * compiler_only_output block licenses each one at its measured ROM home.
 *
 * The shards for D1 and D0 each needed a forcing scaffold (`p->~daOts_c()` and
 * a stand-in `CleanupResources` returning 0). This file needs neither: it owns
 * the real key function, so the vtable emission drags both variants in by
 * itself.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function in reverse source order, so the highest-address
 * ROM function is written first. Do not reorder.
 *
 * No factory: the class is abstract (slots 0 and 6 are zero). Children own
 * daDonketu_c_classInit / daBDonketu_c_classInit / daIDonketu_c_classInit.
 *
 *
 * deslop leftovers:
 * - SetAnim 6az: helpers pass Fix12<int> by value; header method form size-DIFFs.
 * - IsTooFarAwayFromPlayer / IsPlayerInRange(Fix12,Fix12,Fix12,s32) 6az:
 *   func_ov064_021165d8 / 02116560. Header methods carry Fix12<int>.
 * - Particle::System::New / NewSimple 6az: func_ov064_0211616c / 02115f98.
 * - func_0201267c: PlayStepSound / PlayHitSound / PlayDeathSound at mCamSpacePosX.
 * - 0x398..0x3f9 stay children's padding (annexing them as members would move
 *   the fields the children declare from 0x3fa on); the helpers reach them
 *   through daOts_c::tail(), a cast of the whole object to the nested Tail struct
 *   (a Tail that starts at 0x398 costs an extra add per address-of).
 * - func_ov064_* helpers still in this TU keep cartridge addresses (no
 *   identifiers). InitResourcesCommon / BehaviorCommon are the two that
 *   were leftover shards; names describe the children's slot-0 / slot-6
 *   wrappers.
 * - InitResourcesCommon 6az: dBgCh_Actr::Init / SetAnim keep mangled forms (Fix12<int> by value).
 * - BehaviorCommon: the mStateTimer increment stays unsigned short (ldrh).
 * - daOts_c.h first: nested Matrix4x3 for mAnmModel.mat4x3.t (02116bac).
 *   common.h's flat m[12] would stand down if it came first.
 * - Vec3_Sub / HorzLen / atan2 take int*; 02115f98 keeps the int-array pos copy.
 * - bool-widening `int isD8 = (int)(actorID == kBigBullyActorId)` / `isBf` in
 *   02116754 / 02116460: the temporary is load-bearing.
 * - CleanupResources and InitResourcesCommon read the config block through
 *   mFileTable every time; caching it in a local size-DIFFs.
 * - func_ov064_021163c0: `c->mPrevAngleY += 0x8000` is the match form (a
 *   separate read and add of the field size-DIFFs).
 * - mStateTimer compares stay unsigned short (ldrh) in 0211616c / 02116460 /
 *   021163c0; named s16 > / < DIFFs.
 *
 * Leftover (what is still not recovered, or only known by what the code does):
 * - Names. State, animation-slot and sound names describe the behaviour of the
 *   handlers in this TU; the stripped image carries none of the originals. The
 *   six STATE_* names, mHomePos, mPosBeforeMove, mHopCount and the kAnim* slot
 *   names are readings of use, not recovered identifiers.
 * - mHomePos is dual-use: InitResourcesCommon seeds it with the spawn position
 *   and the death branch of func_ov064_02116220 overwrites it with the death spot
 *   for the particles.
 * - mPrevAngleY looks like the travel heading: the hit reactions set it, the
 *   walking states copy it into mAngleY after moving, and
 *   dActor_c::UpdatePosWithHorzSpeedAndAng reads the s16 at 0x94 as the angle it
 *   moves along. Its real name is not settled here.
 * - The Tail view stands in for members: the children's pad_398 and fields from
 *   0x3fa on still sit where a real daOts_c would end. No function in this TU
 *   reads or writes byte 0x3f8.
 * - InitResourcesCommon does not write Tail::mState; where it is first set is
 *   not established here.
 * - Player::Hurt's literal arguments (0, 0x14000, 1, 0, 1) are not interpreted.
 * - Player::param1 == 2 in the hit reaction (the 50-unit rather than 40-unit
 *   knockback) is unexplained, as is hit flag 0x40000's identity.
 * - Surface types 1, 4, 5 and 0x13 (func_ov064_02116220) are unnamed.
 */
/* daOts_c.h FIRST: it pulls math/Matrix.h ahead of common.h through dExtAnmModel_c,
 * which is the nested Matrix4x3 spelling 02116bac needs for .t. */
#include "daOts_c.h"
#include "common.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "math/Approach.h"
namespace cstd {
s16 atan2(s32, s32);
}


/* Actor IDs (symbols/actor_debug_names.tsv). */
enum {
    kPlayerActorId = 0xbf,      /* 191 PLAYER */
    kBigBullyActorId = 0xd8,    /* 216 BOSS_DONKETU */
    kChillBullyActorId = 0xd9   /* 217 ICE_DONKETU */
};

/* Sound IDs, as played by this class's own sound hooks. daDonketu_c overrides all
 * four hooks (0xca step, 0xcb hit, 0xc9 shell hit, 0xc8 death); daBDonketu_c and
 * daIDonketu_c override none, so they play these. */
enum {
    kSfxStep = 0xcc,        /* PlayStepSound: frames 4 and 7 of whichever animation is playing (the walk cycle's two footfalls) */
    kSfxHit = 0xcd,         /* PlayHitSound: the actor enters STATE_KNOCKED_BACK or is struck */
    kSfxDeath = 0xce        /* PlayDeathSound: the actor enters STATE_DYING */
};

/* Particle effect id func_ov064_02115f98 spawns at the point the bully's hit
 * reaction aims at. */
enum { kParticleHit = 0xf6 };

/* Which SharedFilePtr in the resource block holds what: slot 0 is the model, the
 * other four are animation (BCA) files, named by the state that plays them. The
 * walk animation is also the one SetAnim is given after a shell hit and after the
 * bully hurts the player. */
enum {
    kFileModel = 0,
    kAnimDeath = 1,
    kAnimHit = 2,
    kAnimLedgeTurn = 3,
    kAnimWalk = 4
};

/* dExtFrameCtrl_c playback speeds, 20.12 fixed point. */
enum { kAnimSpeedNormal = 0x1000, kAnimSpeedDouble = 0x2000 };

/* dCc_c hitFlags / vulnFlags bits this class tests (the bit table in dCc_c.h,
 * whose meanings are a best-effort reading apart from the egg bit). */
enum {
    kHitAttackMask = 0x7c0,     /* punch, kick, breakdance, slide kick, dive */
    kHitMegaChar = 0x10,
    kHitEgg = 0x2000,
    kHitBit40000 = 0x40000      /* "fire" in that table */
};

/* Ranges. kFarFromPlayer is 20.12 (1500 units); the other two are whole units. */
enum {
    kFarFromPlayer = 0x5dc000,
    kStartChaseRange = 0x320,   /* 800: player within this of mHomePos -> STATE_CHASE */
    kKeepChaseRange = 0x3e8     /* 1000: player not within this of mHomePos -> give up the chase */
};

/* Speeds, 20.12 fixed point (0x1000 is one unit). */
enum {
    kWalkSpeed = 0x5000,            /* 5 units a frame: STATE_RETURN_HOME and STATE_LEDGE_TURN */
    kHitSpeedLight = 0x28000,       /* 40 units a frame */
    kHitSpeedHeavy = 0x32000,       /* 50 units a frame */
    kHitSpeedBit40000 = 0x39800,    /* 57.5 units a frame */
    kHurtRecoilSpeed = 0x14000,     /* 20 units a frame, after hurting the player */
    kHurtRecoilSpeedBigBully = 0xa000,  /* 10 units a frame: the same, for the Big Bully */
    kHopSpeed = 0xf000              /* 15 units a frame upward */
};

/* Local stand-ins with no header of their own. */

/* The resource block mFileTable points at. CleanupResources releases files[0..4];
 * SetAnim loads each file's BCA through filePtr; OnAimedAtWithEgg reads
 * eggAimHeight (slot-29 return, added to pos.y); 02115f98 / 021165d8 / 0211616c
 * read the rest. */
struct BullyResourceConfig {
    SharedFilePtr *files[5];    /* +0x00 -- see kFileModel / kAnim* */
    s32 cylinderRadius;         /* +0x14 -- dCcAc_c radius; also how far along the aim direction func_ov064_02115f98 places the hit effect */
    s32 cylinderHeight;         /* +0x18 -- dCcAc_c height (InitResourcesCommon) */
    s32 horzDecel;              /* +0x1c -- copied to Tail::mHorzDecel */
    s32 modelYOffset;           /* +0x20 -- copied to Tail::mModelYOffset */
    Fix12i eggAimHeight;        /* +0x24 -- passed as both radius and height to dBgCh_Actr::Init; OnAimedAtWithEgg returns it as the aim height */
    s32 shadowRadius;           /* +0x28 -- copied to Tail::mShadowRadius */
    s32 cliffDown;              /* +0x2c -- IsGoingOffCliff's `down` probe depth */
    u32 particleId;             /* +0x30 -- first of two ids func_ov064_0211616c spawns */
};

/* The resource block of an actor, and the BCA of one of its animation slots. */
#define CONFIG(obj) ((BullyResourceConfig *)(obj)->mFileTable)
#define CONFIG_BCA(obj, slot) ((BCA_File *)CONFIG(obj)->files[slot]->filePtr)

/* Spelled and guarded exactly as include/Particle__System.h spells it, so this
 * TU agrees with that header if it is ever pulled in. Only ever used here as a
 * null pointer argument. */
#ifndef VECTOR3_16F_DEFINED
#define VECTOR3_16F_DEFINED
struct Vector3_16f { s16 x, y, z; };
#endif
struct Callback {};

/* ROM symbols with no header of their own. `extern` on every data declaration:
 * without it the block form defines rather than declares. */
extern "C" {
extern s16 data_02082214[];
extern void Vec3_Sub(int* out, int* a, int* b);
extern int Vec3_HorzLen(int* v);
extern int atan2_hold_d_a_ots_1(int a, int b);
namespace Particle {
struct Callback;
struct System {
    static void NewSimple(unsigned effect, int x, int y, int z);

    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
};
}
extern void *d_a_otsNewHold(u32 id, u32 param, Fix12i x, Fix12i y, Fix12i z, const Vector3_16f *pos,
                            struct Callback *cb);
extern void Matrix4x3_FromRotationY(void* m, short angle);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);


void func_ov064_021163c0(daOts_c *c);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19InitResourcesCommonEv
/* recovered: named members + shared header, real C++ method
 *
 * Shared InitResources body. daOts_c leaves slot 0 pure virtual; all three
 * children set mFileTable and call this. Loads the five SharedFilePtrs, inits
 * dExtAnmModel_c / dExtShadowModel_c / dCcAc_c / dBgCh_Actr, and seeds the Tail words
 * (position snapshot, home position, config copies) from the config block and
 * the spawn position. Returns 0 if the model or shadow cannot be set up, else 1.
 *
 * The English name describes those call sites; the stripped image carries no
 * original method name.
 */
int daOts_c::InitResourcesCommon()
{
    BMD_File *bmd;

    dExtFrameCtrl_c::LoadFile(*CONFIG(this)->files[kAnimDeath]);
    dExtFrameCtrl_c::LoadFile(*CONFIG(this)->files[kAnimHit]);
    dExtFrameCtrl_c::LoadFile(*CONFIG(this)->files[kAnimLedgeTurn]);
    dExtFrameCtrl_c::LoadFile(*CONFIG(this)->files[kAnimWalk]);
    bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(*CONFIG(this)->files[kFileModel]);
    if (mAnmModel.SetFile(bmd, 1, 1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimWalk)), 0, kAnimSpeedNormal, 0);
    mdCcAc_c.Init(this, CONFIG(this)->cylinderRadius, CONFIG(this)->cylinderHeight, 0x200000,
                  kHitAttackMask | kHitEgg);
    {
        int isD9 = (int)(actorID == kChillBullyActorId);
        if (isD9) {
            mdCcAc_c.vulnFlags |= kHitBit40000;
        }
    }
    tail().mHomePosX = mPosX;
    tail().mHomePosY = mPosY;
    tail().mHomePosZ = mPosZ;
    tail().mPosBeforeMoveX = mPosX;
    tail().mPosBeforeMoveY = mPosY;
    tail().mPosBeforeMoveZ = mPosZ;
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x32000;
    tail().mHorzDecel = CONFIG(this)->horzDecel;
    tail().mModelYOffset = CONFIG(this)->modelYOffset;
    mStateTimer = 0;
    mWithMeshClsn.Init((dActor_c *)(int)this, CONFIG(this)->eggAimHeight,
                       CONFIG(this)->eggAimHeight, 0, 0);
    tail().mShadowRadius = CONFIG(this)->shadowRadius;
    mParticle1 = 0;
    mParticle0 = mParticle1;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c14BehaviorCommonEv
/* recovered: named members + shared header, real C++ method
 *
 * Shared Behavior body. daOts_c leaves slot 6 pure virtual; all three
 * daDonketu_c and daIDonketu_c fall through to this after
 * UpdateKillByInvincibleChar; daBDonketu_c after its secret-sound check once
 * mNumBulliesKilled is 4 or more. Skips the whole frame (returns 1) when
 * the actor is on the ground and IsTooFarAwayFromPlayer(1500 units) says so.
 * Otherwise it
 * snapshots mPos into Tail::mPosBeforeMove, runs the hit reaction
 * (func_ov064_02116754), then switches on Tail::mState -- see daOts_c::State --
 * calling this TU's helpers and UpdateDeathState. Every frame that is not skipped
 * it ends by advancing the animation, bumping mStateTimer (zeroed when the state changed),
 * rebuilding the model and shadow matrices, and refreshing the collider.
 *
 * The English name describes those call sites; the stripped image carries no
 * original method name.
 */
int daOts_c::BehaviorCommon()
{
    if (mWithMeshClsn.IsOnGround() != 0) {
        if (IsTooFarAwayFromPlayer(kFarFromPlayer) != 0)
            return 1;
    }

    int four = tail().mState;   /* the state this frame started in */
    tail().mPosBeforeMoveX = mPosX;
    tail().mPosBeforeMoveY = mPosY;
    tail().mPosBeforeMoveZ = mPosZ;
    MakeVanishLuigiWork(mdCcAc_c);
    func_ov064_02116754();

    switch (tail().mState) {
    case STATE_RETURN_HOME:
        mHorzSpeed = kWalkSpeed;
        if (func_ov064_021166f0() != 0) {
            tail().mState = STATE_CHASE;
            mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimWalk)), 0, kAnimSpeedDouble, 0);
        }
        func_ov064_021165d8();
        mAngleY = mPrevAngleY;
        break;
    case STATE_CHASE:
        func_ov064_02116560();
        func_ov064_021165d8();
        mAngleY = mPrevAngleY;
        mAnmModel.speed = kAnimSpeedDouble;
        break;
    case STATE_KNOCKED_BACK:
        func_ov064_02116460();
        func_ov064_021165d8();
        break;
    case STATE_LEDGE_TURN:
        func_ov064_021163c0(this);
        func_ov064_021165d8();
        break;
    case STATE_DYING:
        UpdateDeathState();
        break;
    case STATE_REMOVE:
        MarkForDestruction();
        break;
    default:
        break;
    }

    mAnmModel.Advance();
    unsigned short *p100 = (unsigned short *)&mStateTimer;
    *p100 = *p100 + 1;
    if (four != tail().mState)
        *p100 = 0;
    func_ov064_02116bac();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c6RenderEv
/* recovered: named members + shared header, real C++ method -- vtable slot 9
 *
 * WAS _ZN11daDonketu_c6RenderEv, and misattributed: slot 9 holds 0x02116cf0 in daOts_c,
 * daDonketu_c AND daIDonketu_c. daBDonketu_c is the only one of the three that overrides
 * it (0x0211764c), which is exactly the pattern of an inherited method with one
 * child that replaces it.
 *
 * Draws the animated model.
 */
int daOts_c::Render()
{
    mAnmModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method -- vtable slot 3
 *
 * WAS _ZN11daDonketu_c16CleanupResourcesEv, and that was a misattribution, not a
 * spelling choice: slot 3 holds 0x02116ca0 in daOts_c's table AND in all three
 * children's, so daDonketu_c does not override this -- it inherits it.
 *
 * THE KEY FUNCTION. It is the first virtual daOts_c declares out of line, so
 * this TU emits _ZTV7daOts_c and the destructor pair the table points at.
 *
 * Releases the five SharedFilePtrs the file table points at. mFileTable is the
 * base's field, which is the other half of the same evidence: all three
 * children declare it.
 */
int daOts_c::CleanupResources()
{
    /* Caching the block in a local (`cfg->files[i]->Release()`) size-DIFFs; going
       through mFileTable each time is the MATCH form. */
    CONFIG(this)->files[kFileModel]->Release();
    CONFIG(this)->files[kAnimDeath]->Release();
    CONFIG(this)->files[kAnimHit]->Release();
    CONFIG(this)->files[kAnimLedgeTurn]->Release();
    CONFIG(this)->files[kAnimWalk]->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_02116bacEv
/* Rebuilds the two matrices every frame: the model matrix (a rotation about Y
 * by mAngleY, translated to mPos plus the model's Y offset) and the drop-shadow
 * matrix (the same rotation, translated to mPos), then drops the shadow. The
 * translation rows are positions divided by 8 (>> 3). The shadow's radius
 * shrinks by 3/32 (0x180 / 0x1000) of the actor's height above the floor, but not
 * below 10 units, and its depth is that height plus 40 units. Opacity is 0xf. */
void daOts_c::func_ov064_02116bac(){
    Matrix4x3_FromRotationY(&this->mAnmModel.mat4x3, this->mAngleY);
    this->mAnmModel.mat4x3.t.x = this->mPosX >> 3;
    this->mAnmModel.mat4x3.t.y = (this->mPosY + tail().mModelYOffset) >> 3;
    this->mAnmModel.mat4x3.t.z = this->mPosZ >> 3;
    int d = this->mPosY - tail().mFloorY; /* height above the floor, at least 1 unit */
    if (d <= 0x1000)
        d = 0x1000;
    int rad = FX_MUL(d, 0x180); /* how much the shadow shrinks */
    int h = tail().mShadowRadius - rad;
    if (h < 0xa000)
        h = 0xa000; /* shadow radius, at least 10 units */
    Matrix4x3_FromRotationY(&tail().mShadowMtx, this->mAngleY);
    tail().mShadowMtx.t.x = this->mPosX >> 3;
    tail().mShadowMtx.t.y = this->mPosY >> 3;
    tail().mShadowMtx.t.z = this->mPosZ >> 3;
    DropShadowRadHeight(this->mShadowModel, tail().mShadowMtx, h, d + 0x28000, 0xf);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_02116754Ev
/* The hit reaction: what the actor does when its collider is touched. Does
 * nothing in STATE_KNOCKED_BACK, or unless the other collider's owner is the
 * player. The cases, in the order tried:
 *   - hit flags 0x7c0 (punch, kick, breakdance, slide kick, dive -- see the bit
 *     table in dCc_c.h) or a metal player: mPrevAngleY takes the player's
 *     mAngleY and mHorzSpeed becomes 50 units a frame (40 when the player's
 *     param1 is not 2);
 *   - flag 0x2000 (the egg bit): mPrevAngleY takes the player's mPrevAngleY,
 *     mHorzSpeed 40 units a frame;
 *   - not the Big Bully and flag 0x10: killed outright (KillByInvincibleChar);
 *   - flag 0x40000: mPrevAngleY takes the player's mAngleY, mHorzSpeed 57.5
 *     units a frame;
 *   - stomped (JumpedOnByPlayer): as the first case;
 *   - a player on a shell: the player's mHorzSpeed is set to minus the bully's,
 *     and the bully turns about (mPrevAngleY = mAngleY + half a turn) at 40 units
 *     a frame;
 *   - a player with mIsVanish set is left alone; any other player is hurt
 *     (Player::Hurt) and the bully turns about at 20 units a frame (10 for the
 *     Big Bully).
 * Every case except the outright kill and the mIsVanish exit leaves the actor in
 * STATE_KNOCKED_BACK with the hop counter cleared. PlayHitSound plays in the kill
 * and in every case that reaches the knocked-back state; only the mIsVanish exit
 * is silent. */
void daOts_c::func_ov064_02116754()
{
    Player *hitPlayer;
    s32 hitFlags;
    u32 id;

    if (tail().mState == STATE_KNOCKED_BACK)
        return;
    id = this->mdCcAc_c.otherOwner;     /* unique ID of the other collider's owner */
    if (id == 0)
        return;

    hitPlayer = static_cast<Player *>(dActor_c::FindWithID(id));
    if (!hitPlayer)
        return;

    {
        int isBf = (int)(hitPlayer->actorID == kPlayerActorId);
        if (!isBf)
            return;
    }

    hitFlags = (s32)this->mdCcAc_c.hitFlags;
    if ((hitFlags & kHitAttackMask) || hitPlayer->mIsMetal != 0) {
        this->mPrevAngleY = hitPlayer->mAngleY;
        if (hitPlayer->param1 == 2)
            this->mHorzSpeed = kHitSpeedHeavy;
        else
            this->mHorzSpeed = kHitSpeedLight;
        this->mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimHit)), 0, kAnimSpeedNormal, 0);
        func_ov064_02115f98((char*)hitPlayer);
        tail().mState = STATE_KNOCKED_BACK;
        tail().mHopCount = 0;
        this->PlayHitSound();
        return;
    }

    if (hitFlags & kHitEgg) {
        this->mPrevAngleY = hitPlayer->mPrevAngleY;
        this->mHorzSpeed = kHitSpeedLight;
        this->mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimHit)), 0, kAnimSpeedNormal, 0);
        func_ov064_02115f98((char*)hitPlayer);
        tail().mState = STATE_KNOCKED_BACK;
        tail().mHopCount = 0;
        this->PlayHitSound();
        return;
    }

    {
        int isD8 = (int)(this->actorID == kBigBullyActorId);
        if (!isD8 && (hitFlags & kHitMegaChar)) {
            s16 v[3];
            v[0] = 0x2000;
            v[1] = 0;
            v[2] = 0;
            KillByInvincibleChar(*(Vector3_16 *)v, *hitPlayer, CONFIG(this)->eggAimHeight);
            this->PlayHitSound();
            return;
        }
    }

    if (hitFlags & kHitBit40000) {
        this->mPrevAngleY = hitPlayer->mAngleY;
        this->mHorzSpeed = kHitSpeedBit40000;
        this->mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimHit)), 0, kAnimSpeedNormal, 0);
        func_ov064_02115f98((char*)hitPlayer);
        tail().mState = STATE_KNOCKED_BACK;
        tail().mHopCount = 0;
        this->PlayHitSound();
        return;
    }

    if (this->JumpedOnByPlayer(this->mdCcAc_c, *hitPlayer) != 0) {
        this->mPrevAngleY = hitPlayer->mAngleY;
        if (hitPlayer->param1 == 2)
            this->mHorzSpeed = kHitSpeedHeavy;
        else
            this->mHorzSpeed = kHitSpeedLight;
        this->mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimHit)), 0, kAnimSpeedNormal, 0);
        func_ov064_02115f98((char*)hitPlayer);
        tail().mState = STATE_KNOCKED_BACK;
        tail().mHopCount = 0;
        this->PlayHitSound();
        return;
    }

    if (hitPlayer->IsOnShell() != 0) {
        hitPlayer->mHorzSpeed = -this->mHorzSpeed;
        this->mPrevAngleY = (s16)(this->mAngleY + 0x8000);
        this->mHorzSpeed = kHitSpeedLight;
        this->mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimWalk)), 0, kAnimSpeedNormal, 0);
        this->PlayShellHitSound();
        func_ov064_02115f98((char*)hitPlayer);
        tail().mState = STATE_KNOCKED_BACK;
        tail().mHopCount = 0;
        this->PlayHitSound();
        return;
    }

    if (hitPlayer->mIsVanish != 0)
        return;

    tail().mState = STATE_KNOCKED_BACK;
    tail().mHopCount = 0;
    this->PlayHitSound();

    {
        Vector3 v;
        v.x = this->mPosX;
        v.y = this->mPosY;
        v.z = this->mPosZ;
        if (hitPlayer->Hurt(v, 0, 0x14000, 1, 0, 1) != 0) {
            func_ov064_02115f98((char*)hitPlayer);
        }
    }

    this->mPrevAngleY = (s16)(this->mAngleY + 0x8000);
    {
        int isD8 = (int)(this->actorID == kBigBullyActorId);
        if (!isD8)
            this->mHorzSpeed = kHurtRecoilSpeed;
        else
            this->mHorzSpeed = kHurtRecoilSpeedBigBully;
    }
    this->mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimWalk)), 0, kAnimSpeedNormal, 0);
    this->PlayShellHitSound();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_021166f0Ev
/* STATE_RETURN_HOME's steering: records the angle to mHomePos in Tail::mHomeAngle
 * and turns mPrevAngleY toward it by up to 0x140 a frame (about 1.8 degrees).
 * Returns whether the player is within 800 units of mHomePos. */
int daOts_c::func_ov064_021166f0()
{
    Vector3 v;
    tail().mHomeAngle = Vec3_HorzAngle(&this->mPos, (Vector3 *)&tail().mHomePosX);
    ApproachLinear(this->mPrevAngleY, tail().mHomeAngle, 0x140);
    v.x = tail().mHomePosX;
    v.y = tail().mHomePosY;
    v.z = tail().mHomePosZ;
    return this->IsPlayerInRange(v, kStartChaseRange);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_021165d8Ev
/* The movement step shared by the states that move (all but STATE_DYING and
 * STATE_REMOVE). Moves the actor (UpdatePos, no collider). If IsGoingOffCliff
 * fires (down = config->cliffDown, slope limit 0x2888 = about 57 degrees, water
 * not detected, pipe check skipped, up = 50 units) and the actor is not already in
 * STATE_KNOCKED_BACK or STATE_LEDGE_TURN, puts x and z back to
 * Tail::mPosBeforeMove, switches to STATE_LEDGE_TURN and starts the ledge-turn
 * animation. Then resolves the
 * with-mesh collision, sets mFlags bit 0 (the clip test) while the actor is on the
 * ground and IsTooFarAwayFromPlayer(1500 units) is nonzero, clears it otherwise,
 * plays the footstep sound and runs the floor probe (func_ov064_02116220). */
void daOts_c::func_ov064_021165d8()
{
    this->UpdatePos(0);

    if (this->IsGoingOffCliff(
            this->mWithMeshClsn,
            CONFIG(this)->cliffDown,
            0x2888, 0, 1, 0x32000) != 0
        && tail().mState != STATE_KNOCKED_BACK
        && tail().mState != STATE_LEDGE_TURN) {
        this->mPosX = tail().mPosBeforeMoveX;
        this->mPosZ = tail().mPosBeforeMoveZ;
        tail().mState = STATE_LEDGE_TURN;
        this->mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimLedgeTurn)), 0, kAnimSpeedNormal,
                                0);
    }

    this->UpdateWMClsn(this->mWithMeshClsn, 0);

    if (this->mWithMeshClsn.IsOnGround() != 0 && IsTooFarAwayFromPlayer(kFarFromPlayer) != 0) {
        this->mFlags |= 1;
    } else {
        this->mFlags &= ~1;
    }

    this->PlayStepSound();

    func_ov064_02116220();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c14UpdateRunStateEv
/* Slot 31, called from STATE_CHASE. Empty here; every child overrides it. */
int daOts_c::UpdateRunState()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_02116560Ev
/* STATE_CHASE's body: runs the child's UpdateRunState, then, unless the player is
 * within 1000 units of mHomePos (measured at the actor's own height), drops back
 * to STATE_RETURN_HOME and restarts the walk animation. The caller ignores the
 * result, so this is void; the ROM's tail call to SetAnim needs nothing more. */
void daOts_c::func_ov064_02116560() {
    this->UpdateRunState();
    if (IsPlayerInRange(tail().mHomePosX, this->mPosY, tail().mHomePosZ, kKeepChaseRange))
        return;
    tail().mState = STATE_RETURN_HOME;
    mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimWalk)), 0, kAnimSpeedNormal, 0);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_02116460Ev
/* STATE_KNOCKED_BACK's body. While mHorzSpeed is still being slowed to zero
 * (by Tail::mHorzDecel a frame) the state timer is held at 0. Once it has
 * stopped the actor hops: while fewer than two hops are done, each frame on the
 * ground it launches upward at 15 units a frame and counts a hop. After the
 * second hop it waits for the state timer to reach 10 frames (20 for the Big
 * Bully), then goes back to STATE_CHASE with mPrevAngleY = mAngleY and the walk
 * animation at double speed. */
void daOts_c::func_ov064_02116460()
{
    if (ApproachLinear(this->mHorzSpeed, 0, tail().mHorzDecel) != 0) {
        int b = (this->actorID == kBigBullyActorId);
        int lim = b ? 0x14 : 0xa;
        if (tail().mHopCount < 2) {
            if (this->mWithMeshClsn.IsOnGround()) {
                unsigned char *p = &tail().mHopCount;
                this->mVertSpeed = kHopSpeed;
                *p = *p + 1;
            }
            mStateTimer = 0;
            return;
        }
        if (*(unsigned short *)&mStateTimer < (unsigned int)lim)
            return;
        tail().mState = STATE_CHASE;
        this->mPrevAngleY = this->mAngleY;
        {
            BCA_File *anim = CONFIG_BCA(this, kAnimWalk);
            this->mAnmModel.SetAnim((BCA_File *)anim, 0, kAnimSpeedDouble, 0);
        }
        return;
    }
    mStateTimer = 0;
}

/* -------------------------------------------------------------------------- */
/* STATE_LEDGE_TURN's body. On its first frame (state timer 0), if the actor is on
 * the ground, it flips mPrevAngleY by half a turn (0x8000). Every frame it sets
 * mHorzSpeed to 5 units a frame and turns mAngleY toward mPrevAngleY by up to
 * 0x200 (2.8 degrees). After 15 frames it snaps mPrevAngleY to mAngleY, goes back
 * to STATE_RETURN_HOME and restarts the walk animation. An unmangled extern "C"
 * helper that takes the actor as a plain pointer. */
extern "C" void func_ov064_021163c0(daOts_c *c)
{
    if (*(unsigned short *)&c->mStateTimer == 0) {
        if (c->mWithMeshClsn.IsOnGround()) {
            c->mPrevAngleY += 0x8000;
        }
    }
    c->mHorzSpeed = kWalkSpeed;
    ApproachLinear(c->mAngleY, c->mPrevAngleY, 0x200);
    if (*(unsigned short *)&c->mStateTimer < 0xf)
        return;
    c->mPrevAngleY = c->mAngleY;
    c->tail().mState = daOts_c::STATE_RETURN_HOME;
    BCA_File *f = CONFIG_BCA(c, kAnimWalk);
    c->mAnmModel.SetAnim((BCA_File *)f, 0, kAnimSpeedNormal, 0);
    return;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c16UpdateDeathStateEv
/* Slot 32, called from STATE_DYING. Empty here; every child overrides it. */
void daOts_c::UpdateDeathState()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c13PlayStepSoundEv
/* Slot 33. Plays the footstep sound on frames 4 and 7 of the animation. */
void daOts_c::PlayStepSound()
{
    if (mAnmModel.WillHitFrame(4) == 0) {
        if (mAnmModel.WillHitFrame(7) == 0)
            return;
    }
    func_0201267c(kSfxStep, &mCamSpacePos);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c12PlayHitSoundEv
/* Slot 34. Called when the actor is struck or enters STATE_KNOCKED_BACK. */
void daOts_c::PlayHitSound()
{
    func_0201267c(kSfxHit, &mCamSpacePos);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c17PlayShellHitSoundEv
/* Slot 35, called after the shell and player-hurt reactions. Empty here. */
void daOts_c::PlayShellHitSound()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c14PlayDeathSoundEv
/* Slot 36. Called when the actor enters STATE_DYING. */
void daOts_c::PlayDeathSound()
{
    func_0201267c(kSfxDeath, &mCamSpacePos);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_02116220Ev
/* The floor probe, run at the end of every movement step. Casts a ground
 * collision query from 150 units above the actor's (mPos + model Y offset), and
 * if it finds a floor records its height in Tail::mFloorY. When the actor is no
 * more than 20 units above that floor it looks at the floor's surface type
 * (CLPS::GetSurfaceType): types 4, 5 and 0x13 send it to STATE_REMOVE; type 1 sends it to
 * STATE_DYING -- death animation, clip test off, PlayDeathSound, and mHomePos
 * overwritten with the spot it died at, 5 units above the floor. */
void daOts_c::func_ov064_02116220() {
    dBgCh_Gnd rg;
    Vector3 v;
    int y = this->mPosY;
    int yoff = tail().mModelYOffset;
    int z = this->mPosZ;
    int x = this->mPosX;
    int sum = y + yoff;
    int yv = sum + 0x96000;
    v.x = x;
    v.y = yv;
    v.z = z;

    rg.SetObjAndPos(v, this);
    if (rg.DetectClsn() != 0) {
        tail().mFloorY = rg.clsnY;
        if (this->mPosY <= rg.clsnY + 0x14000) {
            int r = rg.surface.clps.GetSurfaceType();
            if (r == 4 || r == 5 || r == 0x13) {
                tail().mState = STATE_REMOVE;
            } else if (r == 1) {
                tail().mState = STATE_DYING;
                this->mAnmModel.SetAnim((BCA_File *)(CONFIG_BCA(this, kAnimDeath)), 0,
                                        kAnimSpeedNormal, 0);
                this->mFlags &= ~1u;
                this->PlayDeathSound();
                tail().mHomePosX = this->mPosX;
                tail().mHomePosY = this->mPosY;
                tail().mHomePosZ = this->mPosZ;
                tail().mHomePosY = rg.clsnY + 0x5000;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_0211616cEv
/* The death animation's tick, called by the children's UpdateDeathState. Once the
 * state timer exceeds 30 it marks the actor for destruction and returns 1. Until
 * then it sinks mPosY by 5 units a frame and keeps two particle systems alive
 * at mHomePos (the death spot): config->particleId and particleId + 1, their
 * handles kept in mParticle0 / mParticle1. Returns 0. */
int daOts_c::func_ov064_0211616c() {
    /* mStateTimer is s16; the ROM compares ldrh (unsigned). Named signed > DIFFs. */
    if (*(unsigned short *)&mStateTimer > 0x1e) {
        this->MarkForDestruction();
        return 1;
    }
    this->mPosY = this->mPosY - 0x5000;
    this->mParticle0 = (u32)Particle::System::New(
        this->mParticle0, CONFIG(this)->particleId, tail().mHomePosX, tail().mHomePosY,
        tail().mHomePosZ, (const Vector3_16f *)0, (Particle::Callback *)((struct Callback *)0));
    this->mParticle1 = (u32)Particle::System::New(
        this->mParticle1, CONFIG(this)->particleId + 1, tail().mHomePosX, tail().mHomePosY,
        tail().mHomePosZ, (const Vector3_16f *)0, (Particle::Callback *)((struct Callback *)0));
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_02116110Es
/* Turns mPrevAngleY toward the closest player by up to `step` a call. Returns 1
 * once it has reached that angle, else 0 (also 0 when there is no player). The
 * children's UpdateRunState use it to face the player. */
int daOts_c::func_ov064_02116110(short step){
    dActor_c *p = (dActor_c *)this->ClosestPlayer();
    if(p != 0){
        short ang = Vec3_HorzAngle(&this->mPos, &p->mPos);
        if(ApproachLinear(this->mPrevAngleY, ang, step)) return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c19func_ov064_02115f98EPc
/* Spawns particle effect kParticleHit where a hit reaction is aimed. `a1` is the
 * player. Aims from this actor's position (raised by the model Y offset) at the
 * player's position plus 70 units up, and places the effect that far along the
 * direction: config->cylinderRadius from the actor. The direction is split into
 * elevation (angV) and heading (angH) with atan2, and the sine/cosine come from
 * data_02082214 (s16 pairs, one per 16 angle units, 12-bit fixed point; element
 * 0 of a pair is the sine and element 1 the cosine, which is the assignment the
 * geometry here needs: the heading's sine scales the x offset, its cosine the z
 * offset, and the elevation's sine the y offset). */
void daOts_c::func_ov064_02115f98(char* a1)
{
    int scratch[3];
    int pos0[3];
    int pos1[3];
    int diff[3];
    int scale;
    int angH;
    int angV;
    int idxV, idxH;
    s16 cosV, sinV, cosH, sinH;
    int lenfix, ycomp, t0, t1;
    int y0;
    int* p1;
    int tmpy;

    p1 = (int*)&((dActor_c *)a1)->mPosX;
    pos0[0] = this->mPosX;
    y0 = this->mPosY;
    pos0[1] = y0;
    pos0[2] = this->mPosZ;
    pos1[0] = *p1;
    tmpy = p1[1];
    pos1[1] = tmpy;
    pos1[2] = p1[2];
    tmpy = tmpy + 0x46000;

    scale = CONFIG(this)->cylinderRadius;
    pos0[1] = y0 + tail().mModelYOffset;
    pos1[1] = tmpy;

    Vec3_Sub(diff, pos1, pos0);

    scratch[0] = diff[0];
    scratch[1] = diff[1];
    scratch[2] = diff[2];
    angH = cstd::atan2(scratch[0], scratch[2]);
    {
        int hl = Vec3_HorzLen(scratch);
        angV = cstd::atan2(scratch[1], hl);
    }

    idxV = (int)((u16)angV >> 4);
    idxH = (int)((u16)angH >> 4);

    cosV = data_02082214[idxV * 2 + 1];
    sinV = data_02082214[idxV * 2];
    cosH = data_02082214[idxH * 2 + 1];
    sinH = data_02082214[idxH * 2];

    lenfix = FX_MUL(scale, cosV);
    ycomp = FX_MUL(scale, sinV);
    t0 = FX_MUL(lenfix, sinH);
    t1 = FX_MUL(lenfix, cosH);

    pos0[0] = pos0[0] + t0;
    pos0[1] = pos0[1] + ycomp;
    pos0[2] = pos0[2] + t1;

    Particle::System::NewSimple(kParticleHit, pos0[0], pos0[1], pos0[2]);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_c16OnAimedAtWithEggEv
/* Slot 29. The height (a fix12 amount added to pos.y) the egg aims at:
 * config->eggAimHeight, or 20 units when there is no config block. */
int daOts_c::OnAimedAtWithEgg()
{
    BullyResourceConfig *config = (BullyResourceConfig *)mFileTable;
    Fix12i aimHeight = 0x14000;
    if (config != 0)
        aimHeight = config->eggAimHeight;

    return aimHeight;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_cD0Ev
/* recovered: real C++ deleting destructor, defined inline in the header
 *
 * ~daOts_c is defined in the class body -- the three classes derived from it
 * inline its vptr store rather than calling it, which the compiler can only do
 * from a visible body. So this file cannot define it, and the one-function
 * shard needed a stand-in CleanupResources to make the compiler emit the
 * vtable. Nothing forces D0 here: this TU defines the real CleanupResources
 * (slot 3), the class's key function, so the vtable is emitted here and drags
 * both destructor variants with it.
 *
 * D0 is the deleting half: destroy through daOts_c's four members and
 * dEnemyBase_c, then hand the object back through Memory::Deallocate.
 */

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daOts_cD1Ev
/* recovered: real C++ destructor, defined inline in the header
 *
 * The body the key function forces out is the class's own layout evidence: the
 * ROM destroys a dExtShadowModel_c at 0x370, a dCcAc_c at 0x33c, a dBgCh_Actr at
 * 0x174 and a dExtAnmModel_c at 0x110, then chains to _ZN12dEnemyBase_cD2Ev, and
 * every one of those offsets is where the members' asserted sizes put them.
 * Nothing forces D1 here either -- see the D0 note above.
 */
