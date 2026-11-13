//cpp
/* Klepto the condor (ov062/daJango_c; the cartridge's asset directory, profile
 * name and RTTI all say "jango"), 24 functions, enrolled.
 *
 * daJango_c_classInit (0x0211ce80..0x0211ced8, historical alias
 * Klepto_Spawn) is a reconstructed name (RTTI daJango_c, the sole JANGO
 * registry profile); retail does not store it. It hand-called
 * fBase_c::operator new(1168) + the inherited dEnemyBase_c ctor + this
 * class's vtable store + the five member subobjects in field order
 * (two dCcAc_c at +0x110/+0x144, dBgCh_Actr at +0x178, dExtBlendAnmModel_c at
 * +0x334, dExtShadowModel_c at +0x3a4). daJango_c has no user-declared
 * constructor, so `new daJango_c()` reproduces the identical sequence.
 *
 * How it behaves: a condor that flies a loop (CIRCLE), swoops (SWOOP) at a
 * nearby player to steal the cap, flies a four-node path (FLY_PATH) once it
 * carries or holds something, and is knocked into HURT by an attack. The five states are
 * records of two pointers-to-member (enter, update) built by
 * __sinit_ov062_0211d6fc; the JANGO_STATE_* macros below name them and the
 * per-state functions say which enter/update pair they are.
 *
 * Source runs REVERSE of ROM (highest address first). Do not reorder; the
 * classInit factory, now the highest address in the TU, is written first.
 * decl_common.h is deliberately NOT included; its remaining needed names
 * are declared locally, and the shared vector and matrix functions come from
 * math/CalcLib.h and math/MtxStack.h. SWOOP uses an array-backed position copy to retain
 * the ROM block move in C++; see its site.
 *
 * deslop leftovers:
 *   - the helper and state-handler names are not the cartridge's: the
 *     state handlers take the Enter<X>State / Update<X>State spelling, and
 *     ChangeTarget is N64's klepto_change_target. The N64 klepto.inc.c
 *     counterpart is noted at each one that has one.
 *   - Local resource wrappers retain their existing lifecycle aliases.
 *     The four handles use file ID/reference bytes and typed loaded-file
 *     pointers; animation reads use those pointer members directly.
 *   - NewSimple keeps its scalar-ABI spelling (wall 6az).
 *   - The held-cap byte uses daObjMarioCap_c::mTaken after the star branch.
 *   - the two matrix members are 12-word arrays, not Matrix4x3; see the
 *     header.
 *   - mStateParam, mLeaveDelay, mRiseToFarNode and mPathPickTick are named
 *     from the behaviour described at the header; sound ids 0xa / 0xee / 0xef and particle 0x7e
 *     are named only by where they are played; daPly_c::Hurt's last three
 *     arguments are not decoded; the level ids 0x10, 0x18 and 0x19 are not
 *     named.
 *   - the state record at data_ov062_0211e16c (RISE: b880 / b800) is not
 *     selected by any function in this file.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daJango_c_AssetId {
    ASSET_JANGO_BMD = 0x0348,
    ASSET_JANGO_ATTACK_BCA = 0x0349,
    ASSET_JANGO_DAMAGE_BCA = 0x034a,
    ASSET_JANGO_FLY_BCA = 0x034b
};

#include "daJango_c.h"
#include "types.h"
#include "common.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "math/Approach.h"
#include "Particle__System.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
/* decl_common.h deliberately NOT included; AngleDiff is declared locally. */
extern "C" int AngleDiff(int a, int b);
#include "PathPtr.h"
#include "SharedFilePtr.h"
#include "decl_SaveData.h"
#include "SaveData.h"
#include "daPly_c.h"
#include "dExtBlendAnmModel_c.h"
#include "dExtFrameCtrl_c.h"
#include "daObjMarioCap_c.h"
#include "Sound.h"
#include "private/mtx43.h"

/* Static-init ownership (was the handwritten __sinit_ov062_0211d6fc shard).
 * The file-scope objects at the end of this file construct the four
 * resource handles (model file 0x348, animation files 0x349, 0x34a, 0x34b)
 * and fill the five two-entry state records. mwcc emits
 * __sinit_d_a_jango.cpp from those definitions. */
typedef int (daJango_c::*JangoPMF)();

/* Local handle payloads: construction writes fileID at +0, numRefs at +2
 * and the loaded-file pointer at +4. CleanupResources releases references;
 * the registered wrapper destructors only publish the file ID. */
struct JangoModelFile : SharedFilePtr {
    JangoModelFile(u32 fileID);
    ~JangoModelFile();
};

struct JangoAnimationFilePtr : SharedFilePtr {
    JangoAnimationFilePtr(u32 fileID);
    ~JangoAnimationFilePtr();
};

struct JangoSwoopRec {
    JangoPMF enter;
    JangoPMF update;
    /* Section-alignment fill: retail BSS runs 4 bytes past the update slot,
     * to the section end at 0x0211e1a0. The constructor never stores here. */
    u8 tailFill[4];

    JangoSwoopRec()
    {
        enter = &daJango_c::EnterSwoopState;
        update = &daJango_c::UpdateSwoopState;
    }
};

extern JangoModelFile data_ov062_0211e0fc;
extern JangoAnimationFilePtr data_ov062_0211e114;
extern JangoAnimationFilePtr data_ov062_0211e10c;
extern JangoAnimationFilePtr data_ov062_0211e104;
extern JangoPMF data_ov062_0211e15c[2];
extern JangoPMF data_ov062_0211e17c[2];
extern JangoPMF data_ov062_0211e14c[2];
extern JangoPMF data_ov062_0211e16c[2];
extern JangoSwoopRec data_ov062_0211e18c;


/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow enum 'Bool' */
enum Bool { FALSE, TRUE };

/* shadow struct 'BCA_File' */
struct BCA_File;

/* The matrix copies go through Mtx43: assigning this TU's nested
   Matrix4x3 instead grows .text by 0x38 bytes. */

/* Array-backed position snapshot: mwccarm 2004/b56 copies these three
   words with the retail ldm/stm sequence in C++. A three-scalar-member
   assignment selects separate loads/stores here (notes/mwccarm-codegen.md
   6ay, the struct-copy lever). */
struct SwoopPositionWords { int w[3]; };


extern signed char gCurrentSublevel;
extern "C" void func_02012694(unsigned int, void *);
extern "C" s16 Vec3_VertAngle(const Vector3 *v0, const Vector3 *v1);
extern "C" void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern "C" void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int s);
/* Parameter names keep this TU's @NNN. */
extern "C" {
typedef void BgchInitAt_(void *self, void *a, Fix12i b, Fix12i c, void *d, void *e);
}
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
extern daPly_c *gPlayers[];

/* Actor ids (symbols/actor_debug_names.tsv). */
enum {
    ACTOR_STAR = 178,
    ACTOR_SILVER_STAR = 179,
    ACTOR_PLAYER = 191,
    ACTOR_OBJ_MARIO_CAP = 269
};

/* Item field of the spawn parameter (bits 8..11), and mCarriedItem after
   InitResources has folded the silver star into 1 plus mSilverStarFlag = 2. */
enum { SPAWN_ITEM_SILVER_STAR = 2 };
enum { CARRIES_NO_STAR = 0, CARRIES_STAR = 1 };

/* Sound ids, named by where they are played (the sounds themselves are not
   identified): func_02012790(id) plays an id without a position, func_02012694(id,
   &camSpacePos) plays it at the actor's camera-space position. */
enum {
    SND_HELD_RELEASED = 0xa,      /* the held actor was let go */
    SND_AT_SWOOP_ENTER = 0xee,
    SND_AT_HURT_ENTER = 0xef
};
enum { PARTICLE_AT_HURT_ENTER = 0x7e };

/* The five state records (two pointers-to-member each: enter, update), filled by
   __sinit_ov062_0211d6fc. Selected with ChangeState(record); the
   record's address is stored in mState; only HURT (in Behavior) and SWOOP
   (in HandleCylinderHit) are ever compared against it by address, the others
   are just passed to ChangeState.
     HURT      enter b930  update b8d8   knocked by an attack: damage animation
     FLY_PATH  enter c594  update c2f4   flying the four-node path
     RISE      enter b880  update b800   climbs (name inferred from b800 alone;
                                        never selected from this TU)
     CIRCLE    enter c218  update bd10   the default loop
     SWOOP     enter bc54  update ba84   dive at the player */
#define JANGO_STATE_HURT      data_ov062_0211e14c
#define JANGO_STATE_FLY_PATH  (&data_ov062_0211e15c)
#define JANGO_STATE_RISE      data_ov062_0211e16c
#define JANGO_STATE_CIRCLE    data_ov062_0211e17c
#define JANGO_STATE_SWOOP     (&data_ov062_0211e18c)

/* dActor_c::mFlags bit 0x08 is set by the framework while the actor is off
   screen. Int-returning, as the ROM materializes the 0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

static inline int IsNoControl(daPly_c *player) {
    return player->mIsNoControl != 0;
}

 daJango_c *daJango_c_classInit()
{
    return new daJango_c();
}

// recovered name: Klepto_OnAimedAtWithEgg
/* daJango_c::OnAimedAtWithEgg - recovered from vtable slot identity.
   Returns the constant 458752 (0x70000, which is 112.0 if read as fix12) and
   does nothing else. */
s32 daJango_c::OnAimedAtWithEgg() {
    return 458752;
}

/* Loads the model and the three animations, decodes the spawn parameter, sets
   up the two cylinders and the wall collider, then picks the first state.

   Spawn parameter (param1):
     bits  0..7   path id (mPathId)
     bits  8..11  item: 0 = nothing, 1 = star, 2 = silver star (stored as
                  mCarriedItem = 1 with mSilverStarFlag = 2); other values
                  are not handled specially
     bits 12..15  mHeldItemParam, ORed into the star's own spawn parameter

   With a star the actor starts on its path node holding the star and flies the
   path (JANGO_STATE_FLY_PATH). Without one it circles (JANGO_STATE_CIRCLE), and
   if the player has already lost the cap it starts out holding it.

   The four handle wrappers are SharedFilePtrs; the loaders take them by
   reference and the payloads come back through filePtr. */
int daJango_c::InitResources()
{
    void *bmd;
    dActor_c *spawned;
    daPly_c *pl;

    bmd = dExtSimpleModel_c::LoadFile(data_ov062_0211e0fc); /* jango.bmd */
    mBlendAnmModel.SetFile((BMD_File *)bmd, 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(data_ov062_0211e114);                   /* jango_attack.bca */
    dExtFrameCtrl_c::LoadFile(data_ov062_0211e10c);                   /* jango_damage.bca */
    dExtFrameCtrl_c::LoadFile(data_ov062_0211e104);                   /* jango_fly.bca */
    dExtSimpleModel_c::LoadFile(data_ov002_0210da40);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9a0);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9c0);

    mPathId = param1 & 0xff;
    mCarriedItem = (param1 >> 8) & 0xf;
    mHeldItemParam = (param1 >> 0xc) & 0xf;
    if (mPathId < 0)
        mPathId = 0;
    if (mCarriedItem == 0xff)
        mCarriedItem = 0;
    if (mCarriedItem == SPAWN_ITEM_SILVER_STAR) {
        mSilverStarFlag = 2;
        mCarriedItem = CARRIES_STAR;
    }

    PathPtr path1;
    path1.FromID(mPathId);
    mPathNodeCount = 4;
    mTerminalVelocity = -0x1e000;               /* fall speed capped at 30 units/frame */
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    /* Init(radius, height, flags, vulnFlags): radii 100 and 60 units, both 160 high. */
    mdCcAc_c1.Init(this, 0x64000, 0xa0000, 0x200002, 0x3eff0);
    mdCcAc_c2.Init(this, 0x3c000, 0xa0000, 0x200000, 0);
    mWithMeshClsn.Init(this, 0x1e000, 0x1e000, 0, 0);

    PathPtr path2;
    path2.FromID(mPathId);
    path2.GetNode(mPathNodePos, mPathNodeIndex);
    mBlendAnmModel.speed = 0x1000; /* 1.0x */
    mHeldActorID = 0;

    if (mCarriedItem == CARRIES_STAR) {
        if (mSilverStarFlag != 2) {
            spawned = dActor_c::Spawn(ACTOR_STAR, mHeldItemParam | 0x50, mPos, 0, mAreaId, -1);
        } else {
            spawned = dActor_c::Spawn(ACTOR_SILVER_STAR, 0x50, mPos, 0, mAreaId, -1);
        }
        if (spawned != 0) {
            mHeldActorID = spawned->uniqueID;
            /* SetRanges(100, 600, 8000, 8000) units: the four clip-volume fields of
               dActor_c.h. The cap below gets the same four numbers. */
            spawned->SetRanges(0x64000, 0x258000, 0x1f40000, 0x1f40000);
        }
        /* Start on the path node, already holding the star. */
        mPosX = mPathNodePosX;
        mPosY = mPathNodePosY;
        mPosZ = mPathNodePosZ;
        ChangeState(JANGO_STATE_FLY_PATH);
    } else {
        pl = gPlayers[0];
        /* No star: if the player has already lost the cap (and param1 is not 3),
           start out holding a cap object, spawned with the player's mCharacter
           shifted left 8. (HandleCylinderHit builds the same shape from
           param1 when it takes the cap off the player.) */
        if (pl != 0 && pl->param1 != 3 && SaveData::HasPlayerLostCap() != 0) {
            {
                unsigned int hat = pl->mCharacter;
                int area = mAreaId;
                unsigned int param = 0;
                param = param | (hat << 8);
                spawned = dActor_c::Spawn(ACTOR_OBJ_MARIO_CAP, param, mPos, 0, area, -1);
            }
            if (spawned != 0) {
                spawned->SetRanges(0x64000, 0x258000, 0x1f40000, 0x1f40000);
                mHeldActorID = spawned->uniqueID;
            }
        }
        /* Taken while mPos still equals mSpawnPos, so the two vectors are equal. */
        mTargetAngleY = Vec3_HorzAngle(&mPos, &mSpawnPos);
        ChangeState(JANGO_STATE_CIRCLE);
    }
    return 1;
}

/* A state record, as __sinit_ov062_0211d6fc fills the five JANGO_STATE_*
   records: two pointers-to-member, `enter` (called once by
   ChangeState when the state is selected) and `update` (called here
   every frame; the +0x08 member). */
typedef int (daJango_c::*PMF)();
struct StateRecord { PMF enter; PMF update; };
struct dCc_c;
struct dBgCh_Actr;

/* One frame: tick the two timers, run the current state's update, apply gravity
   and move, copy the steering angles onto the model angles, rebuild the
   matrices, carry the held actor, then (outside the hurt state) look for
   contacts and refresh both cylinders. */
int daJango_c::Behavior()
{
    StateRecord *m;

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Short(&mTimer);

    m = (StateRecord *)mState;
    if (m->update != 0)
        (this->*(m->update))();

    /* Gravity: mVertSpeed = max(mVertSpeed + mVertAccel, mTerminalVelocity).
       (mVertAccel is negative and mTerminalVelocity is the floor.) mSpeedZ is
       loaded and stored back unchanged. */
    {
        int accum = mVertSpeed;
        int a0 = mVertAccel;
        int lim = mTerminalVelocity;
        int sum = accum + a0;
        if (sum >= lim)
            lim = sum;
        int t = mSpeedZ;
        mVertSpeed = lim;
        mSpeedZ = t;
    }
    UpdatePosWithOnlySpeed((dCc_c *)&mdCcAc_c1);
    UpdateWMClsn(mWithMeshClsn, 0);

    /* The states steer with mPrevAngle*; the model is turned by mAngle*. */
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdateModelAndShadow();

    /* Carry the held actor along: it is moved to mHeldPos every frame. If the
       actor no longer exists, or (when mCarriedItem is not CARRIES_STAR) its
       mTaken byte is non-zero, the cargo is let go: mHeldActorID is cleared,
       the release sound plays, mTimer is set to 30 frames and the state goes back to
       CIRCLE. A star that still exists is never let go here. */
    unsigned int actorId = mHeldActorID;
    if (actorId != 0) {
        dActor_c *p = dActor_c::FindWithID(actorId);
        if (p != 0) {
            if (mCarriedItem == CARRIES_STAR) {
                p->mPosX = mHeldPosX;
                p->mPosY = mHeldPosY;
                p->mPosZ = mHeldPosZ;
            } else if (static_cast<daObjMarioCap_c *>(p)->mTaken == 0) {
                /* daObjMarioCap_c::mTaken: the cap sets it to 1 in
                   InitTaken, the same function that clears the +0xc8
                   matrix pointer stored below. */
                p->mPosX = mHeldPosX;
                p->mPosY = mHeldPosY;
                p->mPosZ = mHeldPosZ;
            } else {
                mHeldActorID = 0;
                ((void (*)(int, int))func_02012790)(SND_HELD_RELEASED, 0);
                mTimer = 0x1e;
                ChangeState(JANGO_STATE_CIRCLE);
            }
        } else {
            mHeldActorID = 0;
            ((void (*)(int, int))func_02012790)(SND_HELD_RELEASED, 0);
            mTimer = 0x1e;
            ChangeState(JANGO_STATE_CIRCLE);
        }
    } else if (mCarriedItem == CARRIES_STAR && mSilverStarFlag != 2) {
        /* Holding nothing: a star carrier (not the silver-star variant)
           that is off screen removes itself. */
        if (IsOffScreen(this))
            MarkForDestruction();
    }

    mBlendAnmModel.Advance();
    /* Hurt state excepted, test the cylinders for a hit or a player contact. */
    if (mState != (void *)JANGO_STATE_HURT) {
        HandleCylinderHit();
    }

    mdCcAc_c1.Clear();
    mdCcAc_c1.Update();
    mdCcAc_c2.Clear();
    mdCcAc_c2.Update();

    return 1;
}

/* Draws the model through dExtBlendAnmModel_c::Render with a null scale. */
int daJango_c::Render()
{
    mBlendAnmModel.Render(0);
    return 1;
}

/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daJango_c::OnPendingDestroy()
{
}

/* recovered: shared header, real C++ method
 *
 * Releases the seven shared files InitResources claimed -- three of them from
 * ov002, which is where daJango_c's shared Mario-cap assets live.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
int daJango_c::CleanupResources()
{
    data_ov002_0210da40.Release();
    data_ov002_0210d9a0.Release();
    data_ov002_0210d9c0.Release();
    data_ov062_0211e0fc.Release();
    data_ov062_0211e114.Release();
    data_ov062_0211e10c.Release();
    data_ov062_0211e104.Release();
    return 1;
}

/* Per-frame matrices (called from Behavior).
   1. dExtSimpleModel_c matrix: translation = mPos >> 3, rotation = mAngle X/Y/Z
      (Matrix4x3_ApplyInPlaceToRotationZXYExt), stored in mBlendAnmModel.mat4x3.
   2. If something is held: multiply bone 6 of the model by the model matrix into
      mHeldMat, set mHeldPos to the translation row of data_020a0e68 shifted
      back up by 3, then place the held actor with a fixed offset and rotation
      (one pair for a star, one for the cap) and hand it a pointer to mHeldMat.
   3. Shadow matrix: mPos >> 3 with Y lowered by 0x18000 (24 units). */
void daJango_c::UpdateModelAndShadow() {
    Vector3 v;
    dActor_c* actor;

    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    *(Mtx43 *)&mBlendAnmModel.mat4x3 = *(Mtx43 *)&data_020a0e68;

    if (mHeldActorID != 0)
    {
        actor = dActor_c::FindWithID(mHeldActorID);
        if (actor != 0)
        {
            mHeldPosX = 0;
            mHeldPosY = 0;
            mHeldPosZ = 0;
            MTX_Concat43(&mBlendAnmModel.data.transforms[6], &mBlendAnmModel.mat4x3, &mHeldMat);
            mHeldPosX = data_020a0e68.t.x;
            mHeldPosY = data_020a0e68.t.y;
            mHeldPosZ = data_020a0e68.t.z;
            mHeldPosX <<= 3;
            mHeldPosY <<= 3;
            mHeldPosZ <<= 3;
            if (mCarriedItem == CARRIES_STAR)
            {
                /* Offset (-13.0, 3.0, -4.0), rotation X/Y/Z = 0, -180, -95.6 degrees. */
                Matrix4x3_FromTranslation(&data_020a0e68, -0xd000, 0x3000, -0x4000);
                Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0, -0x8000, -0x4400);
            }
            else
            {
                /* Offset (-7.0, 1.5, 0.0), rotation X/Y/Z = -67.5, -180, -90 degrees. */
                Matrix4x3_FromTranslation(&data_020a0e68, -0x7000, 0x1800, 0);
                Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, -0x3000, -0x8000, -0x4000);
            }
            MTX_Concat43(&data_020a0e68, &mHeldMat, &mHeldMat);
            actor->mCarryMatrix = &mHeldMat;
        }
    }

    Matrix4x3_FromTranslation(&data_020a0e68, mPosX >> 3, (mPosY - 0x18000) >> 3, mPosZ >> 3);
    *(Mtx43 *)&mShadowMat = *(Mtx43 *)&data_020a0e68;
    DropShadowRadHeight(mShadowModel, mShadowMat, 0x40000, 0x258000, 0xf);
}

/* Select a state: store the record (its address) in mState, then call the
   record's first member, `enter`, on the actor if it has one and return its
   result (1 if it has none). */
int daJango_c::ChangeState(void *p) {
    StateRecord *qq = (StateRecord *)p;
    mState = qq;
    StateRecord *q = (StateRecord *)mState;
    if (*(int *)q == 0)
        return 1;
    return (this->*q->enter)();
}

/* FLY_PATH enter. Clears the three velocity words (mSpeedX, mVertSpeed, mSpeedZ).
   If mRiseToFarNode is 1 -- ChangeTarget set it because the node it
   picked is index ^ 2 (assumed to be the far side of the square) -- the vector (0, 0, 20.0) is turned by mAngleY and
   then by a pitch of -0x4000 (a quarter turn), and its Y becomes mVertSpeed
   (magnitude 20 units per frame), with mStateTimer = 60 frames. Then starts
   jango_fly.bca (4 blend frames, flags 0, speed 1.0, from frame 0). */
int daJango_c::EnterFlyPathState() {
    Vector3 in;
    Vector3 out;

    mSpeedX = 0;
    mVertSpeed = 0;
    mSpeedZ = 0;

    if (mRiseToFarNode == 1) {
        in.x = 0; in.y = 0; in.z = 0;
        out.x = 0; out.y = 0; out.z = 0;
        in.z = 0x14000;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, -0x4000);
        MTX_MultVec43(&in, &data_020a0e68, &out);
        mVertSpeed = out.y;
        mStateTimer = 0x3c;
    }

    mBlendAnmModel.SetAnim(*(BCA_File *)data_ov062_0211e104.filePtr, 4, 0, 0x1000, 0);
    return 1;
}

/* FLY_PATH update: follow the path through its four nodes.
   Each frame it steers mPrevAngleY toward the node at mPathNodeIndex and
   levels the roll. While more than 20 units from the node it is pulled toward
   it by 20 units per frame (diff * (20.0 / len) is subtracted from mPos), but
   only once mAngleY is within 0x2000 (45 degrees) of the node's direction.
   While mRiseToFarNode is 1 the pull leaves mPosY alone (the vertical motion comes
   from mVertSpeed, set by c594), and when mStateTimer has run out the vertical
   speed is cleared and the flag dropped. Within 20 units (or exactly on it) it
   has arrived: mPathPickTick = 7 so the next b3ac call runs at once, the node index advances (the next node in order
   for the silver-star variant and level ids 0x18/0x19, else b3ac's choice, else a
   random one that differs from the current), mPos is snapped to the node and
   copied to mPathNodePos, the velocity words (mSpeedX, mVertSpeed, mSpeedZ) are
   set to (0, 0, 20.0) turned by mAngleY, and the actor re-enters CIRCLE. */
int daJango_c::UpdateFlyPathState() {
    /* Real PathPtr object (as legacy had it via PathPtr.h): its implicit
       ctor call is genuine ROM bytes. Do NOT byte-buffer this one. */
    PathPtr path;
    int zero[6];
    Vector3 node;
    Vector3 node2;
    Vector3 diff;
    Vector3 scaled;
    int len;
    u32 idx;

    path.FromID(mPathId);
    zero[0] = 0; zero[1] = 0; zero[2] = 0; zero[3] = 0; zero[4] = 0; zero[5] = 0;
    path.GetNode(node, mPathNodeIndex);
    ApproachAngle(&mPrevAngleY, Vec3_HorzAngle(&mPos, &node), 0xa, 0x200, 0x100);
    ApproachAngle(&mPrevAngleZ, 0, 0xa, 0x100, 0x50);

    idx = mPathNodeIndex - 1;
    if ((int)idx < 0)
        idx = mPathNodeCount - 1;
    path.GetNode(node2, idx);
    Vec3_Sub(&diff, &mPos, &node);
    len = VEC_Mag(&diff);
    /* len == 0 is already covered by len <= 0x14000, but the ROM keeps
       it as a test of its own. */
    if (len == 0 || len <= 0x14000) {
        mPathPickTick = 7;
        if (mSilverStarFlag == 2 || (u8)(s8)(gCurrentSublevel - 0x18) <= 1) {
            mPathNodeIndex++;
            if (mPathNodeIndex >= 4)
                mPathNodeIndex = 0;
        } else if (ChangeTarget() == 0) {
            u32 r = (u32)RandomIntInternal(&data_0209e650.value) >> 8 & 3;
            if (mPathNodeIndex != r) {
                mPathNodeIndex = r;
            } else {
                int *p = &mPathNodeIndex;
                (*p)++;
                *p &= 3;
            }
        }
        mPosX = node.x;
        mPosY = node.y;
        mPosZ = node.z;
        mPathNodePosX = mPosX;
        mPathNodePosY = mPosY;
        mPathNodePosZ = mPosZ;
        zero[2] = 0;
        zero[0] = 0;
        zero[1] = 0;
        zero[2] = 0x14000;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MTX_MultVec43((Vector3 *)zero, &data_020a0e68, (Vector3 *)&mSpeedX);
        ChangeState(JANGO_STATE_CIRCLE);
        return 1;
    }

    if (AngleDiff(Vec3_HorzAngle(&mPos, &node), mAngleY) < 0x2000) {
        int s;
        zero[1] = mPosY;
        s = FX_Div(0x14000, len);
        Vec3_MulScalar(&scaled, &diff, s);
        VEC_Subtract(&mPos, &scaled, &mPos);
        if (mRiseToFarNode == 1) {
            mPosY = zero[1];
            if (*(u16 *)&mStateTimer == 0) {
                mVertSpeed = 0;
                mRiseToFarNode = 0;
            }
        }
    }
    return 1;
}

/* CIRCLE enter. Takes the current yaw as the steering target, restarts the
   phase counter (mStateParam = 0) and sets mStateTimer to 300, 400, 500 or 600 frames
   (random, 100 apart) before the actor may leave along the path. If the last thing that happened was a hit on the player
   (mHitPlayer) the timer is zeroed instead and the flag cleared. mLeaveDelay
   starts at 0, or at 2 with a zero timer for the silver-star variant and on
   level ids 0x18 / 0x19 (gCurrentSublevel is LEVEL_ID in symbols/verified.tsv). Plays
   jango_fly.bca (4 blend frames, flags 0, speed 1.0). */
int daJango_c::EnterCircleState() {
    mTargetAngleY = mAngleY;
    mStateParam = 0;
    mStateTimer = (((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) & 3) * 0x64 + 0x12c;
    if (mHitPlayer == 1) {
        mStateTimer = 0;
        mHitPlayer = 0;
    }
    mLeaveDelay = 0;
    mBlendAnmModel.SetAnim(*(BCA_File *)data_ov062_0211e104.filePtr, 4, 0, 0x1000, 0);
    if (mSilverStarFlag == 2 ||
        (unsigned char)(signed char)((signed char)gCurrentSublevel - 0x18) <= 1) {
        mLeaveDelay = 2;
        mStateTimer = 0;
    }
    return 1;
}

/* CIRCLE update (Klepto flying a turning loop: constant yaw turn, pitch steered
   toward mSpawnPos when empty-handed or toward mPathNodePos otherwise). gCurrentSublevel is LEVEL_ID in symbols/verified.tsv; this code
   special-cases level 0x10 and levels 0x18 / 0x19.

   1. mTimer != 0 (it has just lost its cargo or its target): head back to the
      spawn point -- mTargetAngleY is the direction to mSpawnPos (or
      mPrevAngleY + 0x4000 when the wall collider reports a wall), and pitch,
      yaw and roll are stepped toward it at 0x500 / 0x1000 / 0x500 per frame.
      mTimer drops to 0 once within 500 units of mSpawnPos, else it is held at 30.
   2. Otherwise steer the pitch toward mSpawnPos (empty-handed) or toward
      mPathNodePos (carrying or holding something); the roll follows a third of
      the yaw error; the yaw turns by a constant 0x200 per frame (level 0x10 and the
      silver-star variant) or 0x400.
   3. Empty-handed and the player still has the cap: when the player is within
      1000 units horizontally (700 outside level 0x10), not in a no-control
      state and not vanished, enter SWOOP. Outside level 0x10 it does not attack
      a player who is more than 40 units above it or who is underwater.
   4. Carrying or holding something: mStateParam is the animation phase (see the
      header): a shallow turn (yaw error / 3 below 0x300) returns to jango_fly.bca,
      a sharp one starts jango_attack.bca.
   5. If the node picked by ChangeTarget differs (and the actor is not the
      silver-star variant or in level 0x18 / 0x19) the timer is cancelled; once
      mStateTimer and mLeaveDelay are both 0, it carries or holds something and mAngleY is within 0x2000 (45 degrees) of the
      path node's direction, it enters FLY_PATH.

   mStateTimer is declared s16, but the ROM tests it with ldrh in this function
   and in c2f4; the declared type compiles those tests to ldrsh, so they read it
   through a u16 cast (measured: both functions stop matching without it). */
int daJango_c::UpdateCircleState() {
    s16 angV;
    s16 angD;
    daPly_c *player;
    Vector3 ppos;
    Vector3 node;
    int thr;
    s8 stage;
    int *p43c;
    s16 *p94;
    int *pp;
    int tmp;

    if (mTimer != 0) {
        if (mWithMeshClsn.IsOnWall() != 0) {
            mTargetAngleY = (s16)(mPrevAngleY + 0x4000);
        } else {
            mTargetAngleY = Vec3_HorzAngle(&mPos, &mSpawnPos);
        }
        ApproachLinear(mPrevAngleY, mTargetAngleY, 0x1000);
        ApproachLinear(mPrevAngleX, Vec3_VertAngle(&mPos, &mSpawnPos), 0x500);
        ApproachLinear(mPrevAngleZ, 0, 0x500);
        if (Vec3_Dist(&mPos, &mSpawnPos) < 0x1f4000) {
            mTimer = 0;
        } else {
            mTimer = 0x1e;
        }
        UpdateCircleVelocity();
        return 1;
    }

    if (mCarriedItem == CARRIES_NO_STAR && mHeldActorID == 0) {
        angV = Vec3_VertAngle(&mPos, &mSpawnPos);
        angD = (s16)(AngleDiff(mTargetAngleY, mPrevAngleY) / 3);
    } else {
        angV = Vec3_VertAngle(&mPos, &mPathNodePos);
        angD = (s16)(AngleDiff(mTargetAngleY, mPrevAngleY) / 3);
    }

    if (gCurrentSublevel == 0x10) {
        ApproachAngle(&mPrevAngleX, angV, 0xa, 0x400, 0x200);
    } else {
        ApproachLinear(mPrevAngleX, angV, 0x300);
    }
    ApproachAngle(&mPrevAngleZ, angD, 0xa, 0x200, 0x100);

    if (gCurrentSublevel == 0x10 || mSilverStarFlag == 2) {
        p94 = &mPrevAngleY;
        *p94 = (s16)(*p94 - 0x200);
    } else {
        p94 = &mPrevAngleY;
        *p94 = (s16)(*p94 - 0x400);
    }

    if (mHeldActorID == 0 && mCarriedItem == CARRIES_NO_STAR) {
        if (SaveData::HasPlayerLostCap() == 0) {
            player = ClosestPlayer();
            if (player != 0) {
                pp = &player->mPosX;
                tmp = pp[0];
                ppos.x = tmp;
                tmp = pp[1];
                stage = gCurrentSublevel;
                ppos.y = tmp;
                tmp = pp[2];
                ppos.z = tmp;
                if (stage != 0x10 && (mPosY + 0x28000 < ppos.y || player->mIsUnderwater != 0)) {
                    UpdateCircleVelocity();
                    return 1;
                }
                thr = 0x3e8000;
                ppos.y = mPosY;
                if (stage != 0x10) {
                    thr = 0x2bc000;
                }
                if (Vec3_Dist(&mPos, &ppos) < thr) {
                    if (!IsNoControl(player) && player->mIsVanish == 0)
                        ChangeState(JANGO_STATE_SWOOP);
                }
                /* fall through to shared b2fc + return */
            }
        }
        UpdateCircleVelocity();
        return 1;
    }

    if (angD < 0x300) {
        if (mStateParam == 1) {
            mBlendAnmModel.SetAnim(*(BCA_File *)data_ov062_0211e104.filePtr, 4, 0, 0x1000, 0);
            if (mSilverStarFlag == 2 || (unsigned)(u8)(s8)((s8)gCurrentSublevel - 0x18) <= 1u) {
                p43c = &mLeaveDelay;
                *p43c = *p43c - 1;
                if (mLeaveDelay <= 0) {
                    mLeaveDelay = 0;
                }
            }
            mStateParam = 2;
        }
    } else if (mStateParam == 0) {
        mBlendAnmModel.SetAnim(*(BCA_File *)data_ov062_0211e114.filePtr, 4, 0, 0x1000, 0);
        p43c = &mStateParam;
        *p43c = *p43c + 1;
    }

    if (mStateParam >= 2) {
        p43c = &mStateParam;
        *p43c = *p43c + 1;
        if (mStateParam > 0x18) {
            mStateParam = 0;
        }
    }

    if (mSilverStarFlag != 2 && gCurrentSublevel != 0x18 && gCurrentSublevel != 0x19 &&
        (mCarriedItem != CARRIES_NO_STAR || mHeldActorID != 0) && ChangeTarget() != 0 &&
        *(u16 *)&mStateTimer != 0) {
        mStateTimer = 0;
    }

    UpdateCircleVelocity();

    if (*(u16 *)&mStateTimer == 0 && mLeaveDelay == 0) {
        PathPtr path;
        path.FromID(mPathId);
        path.GetNode(node, mPathNodeIndex);
        if (AngleDiff(Vec3_HorzAngle(&mPos, &node), mAngleY) < 0x2000) {
            ChangeState(JANGO_STATE_FLY_PATH);
        }
    }

    return 1;
}

/* SWOOP enter. mStateTimer is the swoop length: 50 frames in level 0x10 (the
   LEVEL_ID in gCurrentSublevel), else a random 20 to 35. Clears the velocity words,
   plays sound 0xee at the actor's camera-space position, picks the turn rate in
   mStateParam (0x300, 0x400, 0x500 or 0x600, random) that UpdateSwoopState uses as
   the ApproachLinear step, and starts jango_attack.bca (4 blend frames, flags 0,
   speed 1.0). */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
int daJango_c::EnterSwoopState() {
    if (gCurrentSublevel == 0x10) {
        mStateTimer = 0x32;
    } else {
        mStateTimer = (((unsigned)RandomIntInternal(&data_0209e650.value) >> 8) & 0xf) + 0x14;
    }
    mSpeedX = 0;
    mVertSpeed = 0;
    mSpeedZ = 0;
    func_02012694(SND_AT_SWOOP_ENTER, &mCamSpacePosX);
    mStateParam = ((((unsigned)RandomIntInternal(&data_0209e650.value) >> 8) & 3) << 8) + 0x300;
    mBlendAnmModel.SetAnim(*(BCA_File *)data_ov062_0211e114.filePtr, 4, 0, 0x1000, 0);
    return 1;
}

/* SWOOP update: dive at the nearest player. The position snapshot keeps
   the retail three-word block copy; field access and calls use the actor and
   player interfaces.

   If a player exists: a vanished player ends the swoop (mTimer = 30, back to
   CIRCLE). Otherwise the target is the player's position with Y = mGroundY + 40
   units; mTargetAngleY and the pitch are the direction to it, and mPrevAngleY /
   mPrevAngleX step toward them by mStateParam (level 0x10) or mStateParam + 0x500. The velocity is
   (0, 0, 30.0) turned by mAngleY then mAngleX. After HandleCylinderHit (the hit
   and cap-steal tests) the swoop ends (mTimer = 30, CIRCLE) when mStateTimer
   has run out or the wall collider reports a wall. */
int daJango_c::UpdateSwoopState() {
    Vector3 v;
    SwoopPositionWords t;
    Vector3 hv;
    Vector3 vv;
    s16 pitch;
    daPly_c *player;
    int tx, ty, tz;

    pitch = 0;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    player = ClosestPlayer();
    if (player != 0) {
        if (player->mIsVanish == 1) {
            mTimer = 0x1e;
            ChangeState(JANGO_STATE_CIRCLE);
            return 1;
        }

        t = *(SwoopPositionWords *)&player->mPosX;
        tx = t.w[0];
        {
            int y = player->mGroundY;
            t.w[1] = y;
            t.w[1] = y + 0x28000;
        }
        tz = t.w[2];
        ty = t.w[1];
        hv.x = tx; hv.y = ty; hv.z = tz;
        mTargetAngleY = Vec3_HorzAngle(&mPos, &hv);
        vv.x = tx; vv.y = ty; vv.z = tz;
        pitch = Vec3_VertAngle(&mPos, &vv);
    }
    if (gCurrentSublevel == 0x10) {
        ApproachLinear(mPrevAngleY, mTargetAngleY, (s16)mStateParam);
        ApproachLinear(mPrevAngleX, pitch, (s16)mStateParam);
    } else {
        ApproachLinear(mPrevAngleY, mTargetAngleY, (s16)(mStateParam + 0x500));
        ApproachLinear(mPrevAngleX, pitch, (s16)(mStateParam + 0x500));
    }
    v.z = 0x1e000;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    MTX_MultVec43(&v, &data_020a0e68, (Vector3 *)&mSpeedX);
    HandleCylinderHit();
    /* Retail reads the timer as unsigned here; the inherited field is s16. */
    if ((u16)mStateTimer == 0 || mWithMeshClsn.IsOnWall() != 0) {
        mTimer = 0x1e;
        ChangeState(JANGO_STATE_CIRCLE);
    }
    return 1;
}

/* HURT enter. Starts jango_damage.bca (4 blend frames, flags 0x40000000, speed
   1.0, from frame 0; the animations that run continuously pass flags 0,
   this one passes 0x40000000 and UpdateHurtState waits for its last frame). mStateParam = 1.
   If a held actor still exists it is let go: its gravity is set to -2 units per
   frame squared, its fall speed cap to -50 units per frame, and its word at +0xc8
   (the matrix pointer c6a8 gave it) is cleared. A star is then destroyed and a
   loose star spawned in its place (ACTOR_SILVER_STAR with parameter 0x10, and
   mCarriedItem cleared, for the silver-star variant; ACTOR_STAR with
   mHeldItemParam | 0x40 otherwise); anything else (the cap) just plays the
   release sound. Either way mHeldActorID = 0, mStateParam = 0 and mTimer = 30. Finally plays sound 0xef
   at the actor's camera-space position, spawns particle 0x7e at mPos and zeroes
   the velocity words and mVertAccel. */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
int daJango_c::EnterHurtState() {
    dActor_c* found;
    ((dExtBlendAnmModel_c *)&(mBlendAnmModel))
        ->SetAnim(*(BCA_File *)data_ov062_0211e10c.filePtr, 4, 0x40000000, 0x1000, 0);
    mStateParam = 1;
    if (mHeldActorID != 0 && (found = dActor_c::FindWithID(mHeldActorID)) != 0) {
        found->mVertAccel = -0x2000;
        found->mTerminalVelocity = -0x32000;
        found->mCarryMatrix = 0;
        if (mCarriedItem == CARRIES_STAR) {
            ((fBase_c*)found)->MarkForDestruction();
            if (mSilverStarFlag != 2) {
                dActor_c::Spawn(ACTOR_STAR, mHeldItemParam | 0x40, mPos, &mAngle, mAreaId, -1);
            } else {
                dActor_c::Spawn(ACTOR_SILVER_STAR, 0x10, mPos, &mAngle, mAreaId, -1);
                mCarriedItem = CARRIES_NO_STAR;
            }
        } else {
            func_02012790(SND_HELD_RELEASED);
        }
        mStateParam = 0;
        mHeldActorID = 0;
        mTimer = 0x1e;
    }
    func_02012694(SND_AT_HURT_ENTER, &mCamSpacePosX);
    Particle::System::NewSimple(PARTICLE_AT_HURT_ENTER, mPosX, mPosY, mPosZ);
    mSpeedX = 0;
    mVertSpeed = 0;
    mSpeedZ = 0;
    mVertAccel = 0;
    return 1;
}

/* HURT update: when jango_damage.bca has reached its last frame (the integer
   part of the 20.12 frame counter, with its top four bits masked off, against
   the frame count minus 1), set mTimer to 30 and go back to CIRCLE. */
int daJango_c::UpdateHurtState() {
    int f = mBlendAnmModel.currFrame;
    int fc = mBlendAnmModel.GetFrameCount();
    if ((int)((unsigned int)(f << 4) >> 16) >= fc - 1) {
        mTimer = 0x1e;
        ChangeState(JANGO_STATE_CIRCLE);
    }
    return 1;
}

/* RISE enter (state record data_ov062_0211e16c, which nothing in this file
   selects): doubles the animation speed (0x2000 = 2.0), zeroes the three
   velocity words and mVertAccel, and starts jango_fly.bca (4 blend frames,
   flags 0, speed 1.0). */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
int daJango_c::EnterRiseState() {
    mBlendAnmModel.speed = 0x2000;
    mSpeedX = 0;
    mVertSpeed = 0;
    mSpeedZ = 0;
    mVertAccel = 0;
    mBlendAnmModel.SetAnim(*(BCA_File *)data_ov062_0211e104.filePtr, 4, 0, 0x1000, 0);
    return 1;
}

/* RISE update: climbs at 20 units per frame while pitch (mPrevAngleX) and roll
   (mPrevAngleZ) level off by 0x300 per frame. Once mPosY is within 2 units
   below mPathNodePosY or above it, it stops, snaps to that height, restores
   the animation speed to 1.0 and enters CIRCLE. */
int daJango_c::UpdateRiseState() {
    mVertSpeed = 0x14000;
    ApproachLinear(mPrevAngleX, 0, 0x300);
    ApproachLinear(mPrevAngleZ, 0, 0x300);
    if (mPosY >= mPathNodePosY - 0x2000) {
        mSpeedX = 0;
        mVertSpeed = 0;
        mSpeedZ = 0;
        mPosY = mPathNodePosY;
        mBlendAnmModel.speed = 0x1000;
        ChangeState(JANGO_STATE_CIRCLE);
    }
    return 1;
}

/* Contact tests, run by Behavior every frame outside HURT and by the SWOOP
   update every frame. The return value is incidental: the early outs
   return whatever value they just tested (0, a nonzero query result, a held
   actor ID, or the SWOOP record's address) and no caller uses it.
   Cylinder 1 (mdCcAc_c1) is tested first, then cylinder 2 (mdCcAc_c2).
   dCc_c.h's bit table: 0x4000 explosion; 0x27f0 is the set (names per that table, best effort; only egg
   and explosion are proven) 0x10 mega
   character, 0x20 spin/ground-pound, 0x40 punch, 0x80 kick, 0x100 breakdance,
   0x200 slide kick, 0x400 dive and 0x2000 egg.
   1. Whatever touched cylinder 1 (otherOwner) still exists and hit it with an
      explosion or with any of the 0x27f0 hits: enter HURT. If it was the player
      (actor 191), also enter HURT when the player bumped it from underneath, is
      metal, or is riding a shell.
   2. Whatever touched cylinder 2 is the player and Klepto is in SWOOP, the player
      is not collecting a cap and Klepto holds nothing: unless this is the
      silver-star variant, or the player has wings, a balloon or the vanish
      cap, is metal, or has param1 == 3, take the cap (SetNewHatCharacter when
      mCharacter differs from param1, else SaveData::PlayerLoseCap, returning if
      the cap was already lost), spawn a cap object holding param1 << 8,
      hold it and enter CIRCLE. Then, whether or not a cap was taken, set
      mHitPlayer, call daPly_c::Hurt (pass-through arguments: the first flag is
      set for the silver-star variant or param1 == 3, 0xc000 is 12.0 as fix12; the
      other three are not decoded here) and, if that took effect, set
      mTimer = 30 and enter CIRCLE. */
int daJango_c::HandleCylinderHit() {
    daPly_c *pl;
    dActor_c *sp;
    Vector3 pos;
    unsigned int flag;
    int fl;
    int id;
    enum Bool isPlayer;
    void *state;
    int r;
    u8 hc;
    u8 st;
    int newchar;
    int zero;

    id = mdCcAc_c1.otherOwner;
    if (id != 0) {
        pl = (daPly_c *)dActor_c::FindWithID((unsigned int)id);
        if (pl == 0)
            return (int)pl;
        fl = mdCcAc_c1.hitFlags;
        if ((fl & 0x4000) != 0)
            return ChangeState(JANGO_STATE_HURT);
        if ((fl & 0x27f0) != 0)
            return ChangeState(JANGO_STATE_HURT);
        isPlayer = (enum Bool)(pl->actorID == ACTOR_PLAYER);
        if (isPlayer) {
            if (BumpedUnderneathByPlayer(*pl) == 1 ||
                pl->mIsMetal == 1 ||
                pl->IsOnShell() == 1)
                return ChangeState(JANGO_STATE_HURT);
        }
    }

    id = mdCcAc_c2.otherOwner;
    if (id == 0)
        return id;
    pl = (daPly_c *)dActor_c::FindWithID((unsigned int)id);
    if (pl == 0)
        return (int)pl;
    isPlayer = (enum Bool)(pl->actorID == ACTOR_PLAYER);
    if (!isPlayer)
        return (int)isPlayer;
    state = JANGO_STATE_SWOOP;
    if (mState != state)
        return (int)state;
    r = pl->IsCollectingCap();
    if (r != 0)
        return r;
    r = mHeldActorID;
    if (r != 0)
        return r;

    st = mSilverStarFlag;
    hc = pl->mCharacter;
    if (st != 2 &&
        pl->mHasWings == 0 &&
        pl->mIsBalloon == 0 &&
        pl->mIsVanish == 0 &&
        pl->mIsMetal == 0 &&
        (int)pl->param1 != 3) {
        newchar = (int)pl->param1;
        if (hc != newchar) {
            pl->SetNewHatCharacter(hc, 0, 0);
        } else {
            r = SaveData::HasPlayerLostCap();
            if (r != 0)
                return r;
            SaveData::PlayerLoseCap();
        }
        {
            int area = mAreaId;
            unsigned int ch = pl->param1;
            unsigned int param = 0;
            param = param | (ch << 8);
            sp = dActor_c::Spawn(ACTOR_OBJ_MARIO_CAP, param, mPos, 0, area, -1);
        }
        if (sp != 0) {
            sp->SetRanges(0x64000, 0x258000, 0x1f40000, 0x1f40000);
            mHeldActorID = sp->uniqueID;
            ChangeState(JANGO_STATE_CIRCLE);
        }
    }

    mHitPlayer = 1;
    flag = 0;
    if (mSilverStarFlag == 2 || (int)pl->param1 == 3)
        flag = 1;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    r = pl->Hurt(pos, flag, 0xc000, 1, 0, 1);
    if (r == 0)
        return r;
    mTimer = 0x1e;
    return ChangeState(JANGO_STATE_CIRCLE);
}

/* Pick which of the four path nodes to fly to next, based on where the player is.
   Called from the circling and path states; does its work on every eighth call
   (mPathPickTick counts 1..7 and wraps; c2f4 sets it to 7 so the next call runs).
   Finds the node nearest the player horizontally; returns 0, changing nothing,
   if there is no change (that node is where mPathNodePos already is). Otherwise,
   in level 0x10 (LEVEL_ID in gCurrentSublevel), sets mRiseToFarNode when the node
   chosen is index ^ 2 of the current one, stores the chosen index in
   mPathNodeIndex and returns 1. */
int daJango_c::ChangeTarget() {
    daPly_c *player;
    Vector3 node;
    Vector3 best;
    Vector3 ppos;
    int i;
    int bestIdx;
    int bestDist;

    {
        int *ctr = &mPathPickTick;
        *ctr = *ctr + 1;
        *ctr = *ctr & 7;
    }
    if (mPathPickTick != 0)
        return 0;

    player = ClosestPlayer();
    PathPtr path;
    path.FromID(mPathId);

    bestIdx = 0;
    node.x = bestIdx; node.y = bestIdx; node.z = bestIdx;
    best.x = bestIdx; best.y = bestIdx; best.z = bestIdx;

    if (player != 0) {
        int *pp = &player->mPosX;
        ppos.x = *pp;
        i = bestIdx;
        ppos.y = pp[1];
        bestDist = 0x10000000;
        ppos.z = pp[2];
        while (i < 4) {
            path.GetNode(node, (u32)i);
            if (bestDist > Vec3_HorzDist(&node, &ppos)) {
                bestIdx = i;
                bestDist = Vec3_HorzDist(&node, &ppos);
                best = node;
            }
            i = i + 1;
        }
    }

    if (Vec3_HorzDist(&best, &mPathNodePos) == 0)
        return 0;

    if (gCurrentSublevel == 0x10) {
        mRiseToFarNode = 0;
        if (mPathNodeIndex == (bestIdx ^ 2))
            mRiseToFarNode = 1;
    }
    mPathNodeIndex = bestIdx;
    return 1;
}

/* Sets the velocity words (mSpeedX, mVertSpeed, mSpeedZ) for the circling
   state: the vector (0, 0, 20.0) turned by mAngleY then mAngleX. Empty-handed
   (nothing carried or held) it is (0, 0, 40.0) instead, and while mTimer is
   0 mPrevAngleY also drops by 0x100 (about 1.4 degrees) per call. */
void daJango_c::UpdateCircleVelocity() {
    Vector3 v[2];
    v[0].z = 0;
    v[0].x = 0;
    v[0].y = 0;
    v[0].z = 0;
    v[1].x = 0;
    v[1].y = 0;
    v[1].z = 0;
    v[0].x = 0;
    v[0].y = 0;
    v[0].z = 0x14000;
    if (mCarriedItem == CARRIES_NO_STAR && mHeldActorID == 0) {
        if (mTimer == 0) {
            short *a = &mPrevAngleY;
            *a = *a - 0x100;
        }
        v[0].z = 0x28000;
    }
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    MTX_MultVec43(&v[0], &data_020a0e68, (Vector3 *)&mSpeedX);
}

/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. Nobody writes that; declaring `~daJango_c()` is
 * enough, because mwcc emits D2, D0 and D1 together and objisolate keeps the
 * one this file is bound to.
 *
 * The deallocation is an inline operator delete -- dEnemyBase_c's, reachable because
 * dEnemyBase_c is this class's IMMEDIATE base.
 *
 * (No out-of-line definition here: the header carries the single inline
 * definition for the merged TU; the compiler emits D0 from the visible body.)
 */

/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and six destructor calls, every one a consequence of
 * `struct daJango_c : dEnemyBase_c` and the members that declaration types, destroyed in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header: each member's size closes exactly
 * on the next one's offset.
 *
 * (No out-of-line definition here either, for the same single-definition reason.)
 */

/* Static-init globals (was the handwritten __sinit_ov062_0211d6fc shard).
 * Definition order is the retail initializer's construction order: the model
 * handle, then the three animation handles, then the five state records in
 * FLY_PATH, CIRCLE, SWOOP, HURT, RISE order. */
JangoModelFile data_ov062_0211e0fc(ASSET_JANGO_BMD);
JangoAnimationFilePtr data_ov062_0211e114(ASSET_JANGO_ATTACK_BCA);
JangoAnimationFilePtr data_ov062_0211e10c(ASSET_JANGO_DAMAGE_BCA);
JangoAnimationFilePtr data_ov062_0211e104(ASSET_JANGO_FLY_BCA);

JangoPMF data_ov062_0211e15c[2] = {
    &daJango_c::EnterFlyPathState,
    &daJango_c::UpdateFlyPathState,
};

JangoPMF data_ov062_0211e17c[2] = {
    &daJango_c::EnterCircleState,
    &daJango_c::UpdateCircleState,
};

JangoSwoopRec data_ov062_0211e18c;

JangoPMF data_ov062_0211e14c[2] = {
    &daJango_c::EnterHurtState,
    &daJango_c::UpdateHurtState,
};

JangoPMF data_ov062_0211e16c[2] = {
    &daJango_c::EnterRiseState,
    &daJango_c::UpdateRiseState,
};
