//cpp
/* daObjRotateUpdownLift_c -- the rotating up-down lift, ov091.
 *
 * UPDOWN_LIFT (actor 0x1d) and HS_UPDOWN_LIFT (actor 0x1e) share this
 * class. param1's low nibble is the node the lift starts on. A 10-node
 * path in data_ov091_02134cdc (one 0x78-byte path per variant, each node
 * a 12-byte point) is rotated by mAngleY and added to the spawn position.
 * Variant 0 is the default path, variant 1 when gCurrentSublevel (the level
 * id) is 7, variant 2 when the actor is HS_UPDOWN_LIFT. The step along
 * the path is data_ov091_021344e8[variant] (0x5000 for all three).
 *
 * param1 0xffff is not a riding lift. InitResources spawns one of those
 * at node 2, at the path's first point plus the Y of node 5
 * (data_ov091_02134d1c). Behavior of that watcher finds the two nearby
 * UPDOWN_LIFT actors, holds sound 0x8d until both are dead, and once the
 * player is far (mFlags bit 0x8 and DistToCPlayer > 0x7d0000) puts them
 * back on their spawn node through func_ov091_02130fac.
 *
 * Nodes 4..7 set mPitchStep to 0x2000; every other node clears it. The
 * shadow bobs with sin(mAngleX) and scales with the height above mGroundY.
 *
 * #pragma defer_codegen off lays .text down in source order. The out-of-line
 * destructor emits D1 then D0. The two registry factories abut the rest of
 * the run and come last, HS_UPDOWN_LIFT's first; both profiles stay out of
 * this TU. The "// address (size)" line above each definition is its ROM
 * location.
 *
 * leftovers (member spellings measured and rejected on size):
 * - Kill: Particle::System::NewSimple(Fix12<int> x, y, z) is +0x24
 *   (0x8c -> 0xb0). The scalar extern passes the three positions in r1-r3.
 * - func_ov091_02131160: a Matrix4x3 * local for mShadowMat is +4
 *   (0x1e0 -> 0x1e4); the translation row is stored through `this`.
 * - Behavior: one this+0x3a2 halfword for the arrived pitch is
 *   -0xc (0x488 -> 0x47c); &mPitchBase is a pool add of 0x3a2 and the
 *   step stays this+0x300+0xa4.
 * - InitResources: paths[variant].node[index] swaps the mla registers (3 words, same
 *   size); the char* stride multiply stays.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjRotateUpdownLift_c_AssetId {
    ASSET_OBJ_UPDNLIFT_BMD = 0x0493,
    ASSET_OBJ_UPDNLIFT_KCL = 0x0494,
    ASSET_HS_UPDOWN_LIFT_BMD = 0x061f,
    ASSET_HS_UPDOWN_LIFT_KCL = 0x0620
};

#pragma defer_codegen off

#include "daObjRotateUpdownLift_c.h"
#include "daPly_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"
#include "decl_Platform.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_cp.h"

/* Plain 12-byte point. Vector3's destructor is empty but the type is not
   POD, and the copies below are three-word ldm/stm with no cleanup. */

/* Ten nodes, 0x78 bytes. Three of these sit at data_ov091_02134cdc. */
struct UpdownPath {
    VecFx32 node[10];
};

/* Node 5's Y, one word per path, stride 0x78. The symbol is 0x40 bytes
   into the path table (node 5.y), which is what InitResources adds when
   it spawns the watcher. */
struct UpdownTopY {
    s32 y;
    u8 pad[0x74];
};

/* The file table at 0x02134c30 is three records of
   { SharedFilePtr *model, SharedFilePtr *collision, CLPS_Block *clps }.
   Each symbol below is one field of that record, so [variant] on that
   symbol is that field and the stride stays 0xc. */
struct UpdownFileSlot {
    SharedFilePtr *file;
    u32 pad4;
    u32 pad8;
};

struct UpdownClpsSlot {
    CLPS_Block *clps;
    u32 pad4;
    u32 pad8;
};

/* File-scope objects at the end of this file construct the four resource
 * handles (model files 0x493/0x61f, collision files 0x494/0x620). mwcc
 * emits __sinit_d_a_obj_rotate_updown_lift.cpp from those definitions. The
 * wrapper names are local; the handle constructors and destructors are
 * the ROM resource-family functions, aliased in the manifest. Nothing in
 * this TU names the handles directly; the UpdownFileSlot rows above
 * point at them. */
struct RotateUpdownLiftModelFile : SharedFilePtr {
    RotateUpdownLiftModelFile(u32 fileID);
    ~RotateUpdownLiftModelFile();
};

struct RotateUpdownLiftCollisionFilePtr : SharedFilePtr {
    RotateUpdownLiftCollisionFilePtr(u32 fileID);
    ~RotateUpdownLiftCollisionFilePtr();
};

extern UpdownPath data_ov091_02134cdc[3];
extern UpdownTopY data_ov091_02134d1c[3];
extern s32 data_ov091_021344e8[];
/* Per-variant word triples copied onto the stack. The shadow-width
   triple lives in its own file: it sits between this class's typeinfo
   and its name. */
VariantWords data_ov091_02134ba0 = {{0x200000, 0x200000, 0x500000}};
VariantWords data_ov091_02134bac = {{0x3c000, 0x3c000, 0x78000}};
VariantWords data_ov091_02134bb8 = {{0x32000, 0x32000, 0x78000}};
extern VariantWords data_ov091_02134bd0;
extern UpdownFileSlot data_ov091_02134c30[];
extern UpdownFileSlot data_ov091_02134c34[];
extern UpdownClpsSlot data_ov091_02134c38[];
extern s8 gCurrentSublevel;

extern "C" void Matrix4x3_FromRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
/* local extern: no shared math header declares MTX_MultVec43. */
extern "C" void MTX_MultVec43(Vector3 *in, Matrix4x3 *m, Vector3 *out);
extern "C" void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern "C" void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern "C" void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
extern "C" void VEC_Subtract(void *a, void *b, void *c);
extern "C" int VEC_Mag(void *v);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void func_02012694(unsigned int id, const Vector3 *pos);
extern "C" void hold_d_a_obj_rotate_updown_lift(dBgW_KcMbg *self, KCL_File *file,
                                                const Matrix4x3 *mat, int scale, s16 angY,
                                                CLPS_Block *clps);

// 0x02130f00 (0x4c)
daObjRotateUpdownLift_c::~daObjRotateUpdownLift_c()
{
}

// 0x02130fac (0xc4)
/* Put a killed lift back on its spawn node. The watcher calls this. */
void daObjRotateUpdownLift_c::func_ov091_02130fac()
{
    VecFx32 tmp[2];

    mIsDead = 0;
    mAngleX = mSpawnAngleX;
    mAngleY = mSpawnAngleY;
    mAngleZ = mSpawnAngleZ;
    mPosX = mBasePosX;
    mPosY = mBasePosY;
    mPosZ = mBasePosZ;
    mWaypointIndex = (u8)(param1 & 0xf);
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MTX_MultVec43((Vector3 *)&data_ov091_02134cdc[mVariant].node[mWaypointIndex], &data_020a0e68,
                  (Vector3 *)&tmp[0]);
    Vec3_Add((Vector3 *)&tmp[1], (const Vector3 *)&mBasePosX, (const Vector3 *)&tmp[0]);
    mPosX = tmp[1].x;
    mPosY = tmp[1].y;
    mPosZ = tmp[1].z;
}

// 0x02131070 (0x8c)
void daObjRotateUpdownLift_c::Kill()
{
    Particle::System::NewSimple(kPtclUpdownLiftBreak, mPosX, mPosY, mPosZ);
    /* memberwise stores: the (x,y,z) ctor form batches the loads */
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    PoofDustAt(v);
    Sound::PlayBank3(0xf, mCamSpacePos);
    mIsDead = 1;
    mMegaKnockback = 0;
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
}

// 0x021310fc (0x64)
/* HS_UPDOWN_LIFT ignores the hit. The others count it, play 0x1e, and
   snap yaw back to the previous angle. */
void daObjRotateUpdownLift_c::OnHitByMegaChar(daPly_c &player) {
    unsigned short id = actorID;
    int isHs = (id == 0x1e);
    if (isHs)
        return;
    player.IncMegaKillCount();
    func_02012694(0x1e, &mCamSpacePos);
    KillByMegaChar(player);
    mAngleY = mPrevAngleY;
}

// 0x02131160 (0x1e0)
/* Aim the cuboid shadow and the clip volume. Called every frame except
   for HS_UPDOWN_LIFT. */
void daObjRotateUpdownLift_c::func_ov091_02131160() {
    /* Holding mShadowMat in a Matrix4x3 * local steals r4 from `this` and
       the function grows. The three translation words are stored from
       `this`, not from that pointer. */
    char *c = (char *)this;
    VariantWords shadowBaseY = data_ov091_02134bac;
    VariantWords shadowSizeX = data_ov091_02134bd0;
    VariantWords shadowSizeZ = data_ov091_02134bb8;
    VariantWords clipCap = data_ov091_02134ba0;

    Matrix4x3_FromRotationY((Matrix4x3 *)mShadowMat, mAngleY);
    ((Matrix4x3 *)mShadowMat)->t.x = mPosX >> 3;

    int sine = FX_SinIdx((u16)mAngleX >> 4);
    int sineAbs = sine < 0 ? -sine : sine;
    int scaled = FX_MUL(sineAbs, 0xa0000);
    int variantA = mVariant;
    int base = shadowBaseY.v[variantA];
    int sum = base + scaled;
    int py = mPosY;
    ((Matrix4x3 *)mShadowMat)->t.y = (py - sum) >> 3;
    ((Matrix4x3 *)mShadowMat)->t.z = mPosZ >> 3;

    int variantB = mVariant;
    int h = mPosY - mGroundY;
    if (h <= 0x1000)
        h = 0x1000;
    int cap = clipCap.v[variantB];
    if (h + 0x100000 >= cap)
        cap = h + 0x100000;
    mClipOffsetY = -((int)(h + ((unsigned)h >> 31)) >> 1);
    mClipRadius = (int)(cap + ((unsigned)cap >> 31)) >> 4;

    int shr = FX_MUL(h, 32);
    int variantC = mVariant;
    int sx = shadowSizeX.v[variantC] - shr;
    int cosine = FX_CosIdx((u16)mAngleX >> 4);
    int fac = 0xa0000 - shr;
    if (cosine < 0)
        cosine = -cosine;
    DropShadowScaleXYZ(mShadowModel, *(Matrix4x3 *)mShadowMat, sx, h,
                       shadowSizeZ.v[variantC] + FX_MUL(fac, cosine), 0xf);
}

// 0x02131340 (0x48)
/* Write the model matrix: full rotation, translation at 1/8 of position. */
void daObjRotateUpdownLift_c::func_ov091_02131340()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3,
                                 mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

// 0x02131388 (0x80)
int daObjRotateUpdownLift_c::CleanupResources()
{
    if (param1 == 0xffff)
        return 1;
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov091_02134c30[mVariant].file->Release();
    data_ov091_02134c34[mVariant].file->Release();
    return 1;
}

// 0x02131408 (0x60)
int daObjRotateUpdownLift_c::Render()
{
    if (mIsDead)
        return 1;
    if (param1 == 0xffff)
        return 1;
    mModel.Render(0);
    return 1;
}

// 0x02131468 (0x488)
int daObjRotateUpdownLift_c::Behavior()
{
    Vector3 pos;
    Vector3 cur;
    Vector3 prev;
    Vector3 remain;
    Vector3 target;
    Vector3 edge;
    Vector3 snapped;
    Vector3 stepVec;
    int keepSound;
    int arrived;
    int prevIndex;
    int stepLen;
    int dist;
    int span;
    int along;

    if (mIsDead != 0)
        return 1;

    if (param1 == 0xffff) {
        int isParent;
        keepSound = 1;
        isParent = actorID;
        isParent = (isParent == 0x1d);
        if (isParent != 0) {
            daObjRotateUpdownLift_c *platform0;
            daObjRotateUpdownLift_c *platform1;
            if (mPlatform0 == 0 || mPlatform1 == 0) {
                daObjRotateUpdownLift_c *nearby;
                nearby = (daObjRotateUpdownLift_c *)FindWithActorID(0x1d, 0);
                if (nearby != 0) {
                    do {
                        if (nearby != this && Vec3_HorzDist(&mPos, &nearby->mPos) < 0xa0000) {
                            if (mPlatform0 == 0)
                                mPlatform0 = nearby;
                            else if (mPlatform1 == 0)
                                mPlatform1 = nearby;
                        }
                        nearby = (daObjRotateUpdownLift_c *)FindWithActorID(0x1d, nearby);
                    } while (nearby != 0);
                }
            }
            platform0 = mPlatform0;
            if (platform0 != 0) {
                platform1 = mPlatform1;
                if (platform1 != 0) {
                    int offScreen;
                    if (platform0->mIsDead != 0 && platform1->mIsDead != 0)
                        keepSound = 0;
                    offScreen = (mFlags & 8) ? 1 : 0;
                    if (offScreen != 0) {
                        if (DistToCPlayer() > 0x7d0000) {
                            platform0 = mPlatform0;
                            if (platform0->mIsDead != 0)
                                platform0->func_ov091_02130fac();
                            platform1 = mPlatform1;
                            if (platform1->mIsDead != 0)
                                platform1->func_ov091_02130fac();
                        }
                    }
                }
            }
        }
        if (keepSound != 0) {
            mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x8d, mCamSpacePos, 0);
        }
        return 1;
    }

    if (UpdateKillByMegaChar(0x2000, 0, 0, 0) != 0)
        return 1;

    {
        int py;
        pos.x = mPosX;
        py = mPosY;
        pos.y = py;
        pos.z = mPosZ;
        pos.y = py - 0x14000;
    }
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    mGroundY = pos.y;
    if (ground.DetectClsn() != 0)
        mGroundY = ground.clsnY;

    arrived = 0;
    stepLen = data_ov091_021344e8[mVariant];
    prevIndex = mWaypointIndex - 1;
    if (prevIndex < 0)
        prevIndex = 9;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MTX_MultVec43((Vector3 *)&data_ov091_02134cdc[mVariant].node[mWaypointIndex], &data_020a0e68,
                  &cur);
    MTX_MultVec43((Vector3 *)&data_ov091_02134cdc[mVariant].node[prevIndex], &data_020a0e68, &prev);
    Vec3_Add(&target, (const Vector3 *)&mBasePosX, &cur);
    Vec3_Sub(&remain, &mPos, &target);
    Vec3_Sub(&edge, &cur, &prev);
    span = VEC_Mag(&edge);
    dist = VEC_Mag(&remain);
    along = FX_Div(span - dist, span);
    mAngleX = (s16)(mPitchBase + (s16)FX_MUL(mPitchStep, along));

    if (dist == 0 || dist <= stepLen) {
        Vec3_Add(&snapped, (const Vector3 *)&mBasePosX, &cur);
        mPosX = snapped.x;
        mPosY = snapped.y;
        mPosZ = snapped.z;
        /* &mPitchBase is offsetof 0x3a2, which is not an ARM immediate, so this
           is a pool add. The step is the halfword at this+0x300+0xa4. One
           base for both grows the function. */
        {
            s16 *pitch = &mPitchBase;
            s16 *stepAt = (s16 *)((char *)this + 0x300);
            arrived = 1;
            *pitch = (s16)(*pitch + *(s16 *)((char *)stepAt + 0xa4));
        }
    } else {
        Vec3_MulScalar(&stepVec, &remain, FX_Div(stepLen, dist));
        VEC_Subtract(&mPosX, &stepVec, &mPosX);
    }

    if (arrived != 0) {
        u8 *waypoint = &mWaypointIndex;
        *waypoint = (u8)(*waypoint + 1);
        if ((u32)mWaypointIndex >= 0xa)
            mWaypointIndex = 0;
        if ((u32)mWaypointIndex > 3 && (u32)mWaypointIndex < 8)
            mPitchStep = 0x2000;
        else
            mPitchStep = 0;
    }

    func_ov091_02131340();

    {
        int isHs = actorID;
        isHs = (isHs == 0x1e);
        if (isHs == 0) {
            func_ov091_02131160();
            mMeshCollider.SetClipRadius(0x150000);
            mMeshCollider.SetClipOffsetY(0x1000);
            if (IsClsnInRange(0x150000, 0x1000) != 0)
                UpdateClsnPosAndRot();
        } else {
            if (IsClsnInRange(0, 0) != 0)
                UpdateClsnPosAndRot();
        }
    }

    return 1;
}

// 0x021318f0 (0x2b4)
int daObjRotateUpdownLift_c::InitResources()
{
    Vector3 spawnAt;
    Vector3 rotated;
    Vector3 probe;
    Vector3 placed;
    unsigned char node;
    unsigned char variant;
    void *bmd;
    void *kcl;

    if (param1 == 0xffff) {
        mFlags &= ~2;
        SetRanges(0, 0x1000, 0, 0);
        return 1;
    }

    mBasePosX = mPosX;
    mBasePosY = mPosY;
    mBasePosZ = mPosZ;
    mShadowModel.InitCuboid();

    mVariant = 0;
    {
        int isHs = (actorID == 0x1e);
        if (isHs)
            mVariant = 2;
        else if (gCurrentSublevel == 7)
            mVariant = 1;
    }

    mWaypointIndex = (u8)(param1 & 0xf);

    if (mWaypointIndex == 2) {
        Vec3_Add(&spawnAt, (const Vector3 *)&mBasePosX,
                 (const Vector3 *)&data_ov091_02134cdc[mVariant].node[0]);
        spawnAt.y += data_ov091_02134d1c[mVariant].y;
        Spawn(0x1d, 0xffff, spawnAt, 0, mAreaId, -1);
    }

    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    {
        char *tbl = (char *)data_ov091_02134cdc;
        int stride = 0x78;
        int i = mVariant;
        node = mWaypointIndex;
        MTX_MultVec43((Vector3 *)(tbl + i * stride + node * 0xc), &data_020a0e68, &rotated);
    }
    Vec3_Add(&placed, (const Vector3 *)&mBasePosX, &rotated);
    mPosX = placed.x;
    mPosY = placed.y;
    mPosZ = placed.z;

    variant = mVariant;
    bmd = dExtSimpleModel_c::LoadFile(*data_ov091_02134c30[variant].file);
    mModel.SetFile((BMD_File *)bmd, 1, -1);

    func_ov091_02131340();
    UpdateClsnPosAndRot();

    variant = mVariant;
    kcl = dBgW_Kc::LoadFile(*data_ov091_02134c34[variant].file);
    mMeshCollider.SetFile((KCL_File *)kcl, mClsnMat, 0x199, mAngleY,
                          *(data_ov091_02134c38[variant].clps));
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);

    {
        int py;
        probe.x = mPosX;
        py = mPosY;
        probe.y = py;
        probe.z = mPosZ;
        probe.y = py - 0x14000;
    }
    dBgCh_Gnd ground;
    ground.SetObjAndPos(probe, 0);
    mGroundY = probe.y;
    if (ground.DetectClsn())
        mGroundY = ground.clsnY;

    mSpawnAngleX = mAngleX;
    mSpawnAngleY = mAngleY;
    mSpawnAngleZ = mAngleZ;
    return 1;
}

// 0x02131ba4 (0x38)
daObjRotateUpdownLift_c *daObjRotateUpdownLift_c_classInit_HS_UPDOWN_LIFT() {
    return new daObjRotateUpdownLift_c();
}

// 0x02131bdc (0x38)
daObjRotateUpdownLift_c *daObjRotateUpdownLift_c_classInit_UPDOWN_LIFT() {
    return new daObjRotateUpdownLift_c();
}

/* Static-init globals (was the handwritten __sinit_ov091_02134524 shard).
 * Definition order is the retail initializer's construction order. */
RotateUpdownLiftModelFile gObjUpdnliftModelFile(ASSET_OBJ_UPDNLIFT_BMD);
RotateUpdownLiftCollisionFilePtr gObjUpdnliftCollisionFile(ASSET_OBJ_UPDNLIFT_KCL);
RotateUpdownLiftModelFile gHsUpdownLiftModelFile(ASSET_HS_UPDOWN_LIFT_BMD);
RotateUpdownLiftCollisionFilePtr gHsUpdownLiftCollisionFile(ASSET_HS_UPDOWN_LIFT_KCL);
