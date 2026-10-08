//cpp
/* daObjRotateUpdownLift_c -- the rotating up-down lift, ov091.
 *
 * UPDOWN_LIFT (actor 0x1d) and HS_UPDOWN_LIFT (actor 0x1e) share this
 * class. param1's low nibble is the node the lift starts on. A 10-node
 * path in data_ov091_02134cdc (one 0x78-byte path per variant, each node
 * a 12-byte point) is rotated by mAngleY and added to the spawn position.
 * Variant 0 is the default path, variant 1 when data_0209f2f8 (the level
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
 * pitch base at 0x3a2 is a halfword (the header's u8 mPitchBase plus the
 * following pad byte). The shadow bobs with sin(mAngleX) and scales with
 * the height above mGroundY.
 *
 * #pragma defer_codegen off lays .text down in source order. The out-of-line
 * destructor emits D1 then D0. The two registry factories abut the rest of
 * the run and come last, HS_UPDOWN_LIFT's first; both profiles stay out of
 * this TU. The "// address (size)" line above each definition is its ROM
 * location.
 *
 * Known limits:
 * - Kill: Particle::System::NewSimple(Fix12<int> x, y, z) is +0x24
 *   (0x8c -> 0xb0). The scalar extern passes the three positions in r1-r3.
 * - func_ov091_02131160: dActor_c::DropShadowScaleXYZ as a method is +0x1c
 *   (0x1e0 -> 0x1fc). A Matrix4x3 * at pad_348 is +4 (0x1e0 -> 0x1e4);
 *   the translation row stays this+0x36c / 0x370 / 0x374.
 * - Behavior: dBgActor_c::UpdateKillByMegaChar as a method is +0xc
 *   (0x488 -> 0x494). One this+0x3a2 halfword for the arrived pitch is
 *   -0xc (0x488 -> 0x47c); &mPitchBase is a pool add of 0x3a2 and the
 *   step stays this+0x300+0xa4.
 * - InitResources: dBgW_KcMbg::SetFile as a method is +4 (0x2b4 -> 0x2b8).
 *   paths[variant].node[index] swaps the mla registers (3 words, same
 *   size); the char* stride multiply stays.
 */

#pragma defer_codegen off

#include "daObjRotateUpdownLift_c.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"
#include "decl_Platform.h"

/* Plain 12-byte point. Vector3's destructor is empty but the type is not
   POD, and the copies below are three-word ldm/stm with no cleanup. */
struct PodVec3 {
    s32 x, y, z;
};

/* Ten nodes, 0x78 bytes. Three of these sit at data_ov091_02134cdc. */
struct UpdownPath {
    PodVec3 node[10];
};

/* Node 5's Y, one word per path, stride 0x78. The symbol is 0x40 bytes
   into the path table (node 5.y), which is what InitResources adds when
   it spawns the watcher. */
struct UpdownTopY {
    s32 y;
    u8 pad[0x74];
};

/* Three words, one per variant. Copied onto the stack and indexed by
   mVariant. */
struct VariantWords {
    s32 v[3];
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

extern UpdownPath data_ov091_02134cdc[3];
extern UpdownTopY data_ov091_02134d1c[3];
extern s32 data_ov091_021344e8[];
extern VariantWords data_ov091_02134bac;
extern VariantWords data_ov091_02134bd0;
extern VariantWords data_ov091_02134bb8;
extern VariantWords data_ov091_02134ba0;
extern UpdownFileSlot data_ov091_02134c30[];
extern UpdownFileSlot data_ov091_02134c34[];
extern UpdownClpsSlot data_ov091_02134c38[];
extern Matrix4x3 data_020a0e68;
extern s16 data_02082214[];
extern s8 data_0209f2f8;

namespace cstd {
int fdiv(int numerator, int denominator);
}

extern "C" {
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 ang);
void Matrix4x3_FromRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
void MulVec3Mat4x3(void *src, void *m, void *dst);
void Vec3_Add(void *out, void *a, void *b);
void Vec3_Sub(void *out, void *a, void *b);
void Vec3_MulScalar(void *out, void *in, int scale);
void SubVec3(void *a, void *b, void *c);
int LenVec3(void *v);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void func_02012694(unsigned int id, const Vector3 *pos);
/* dBgW range/callback setters. Their C definitions take int *; the calls
   cast &mMeshCollider. */
void func_020393a4(int *collider, int range);
void func_02039394(int *collider, int range);
void func_020393d4(int *collider, void *callback);

void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b, int c, int d);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
int _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mat, int sx, int sy, int sz, unsigned int opacity);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    int scale, s16 angY, CLPS_Block *clps);
}

// 0x02130f00 (0x4c)
// @symbol _ZN23daObjRotateUpdownLift_cD1Ev
// @symbol _ZN23daObjRotateUpdownLift_cD0Ev
daObjRotateUpdownLift_c::~daObjRotateUpdownLift_c()
{
}

#ifdef _MSC_VER
extern "C" daObjRotateUpdownLift_c *_ZN23daObjRotateUpdownLift_cD0Ev(daObjRotateUpdownLift_c *thiz)
{
    thiz->daObjRotateUpdownLift_c::~daObjRotateUpdownLift_c();
    daObjRotateUpdownLift_c::operator delete(thiz);
    return thiz;
}
#endif

// 0x02130fac (0xc4)
// @symbol func_ov091_02130fac
/* Put a killed lift back on its spawn node. The watcher calls this. */
void daObjRotateUpdownLift_c::func_ov091_02130fac()
{
    PodVec3 tmp[2];

    mIsDead = 0;
    mAngleX = mSpawnAngleX;
    mAngleY = mSpawnAngleY;
    mAngleZ = mSpawnAngleZ;
    mPosX = mBasePosX;
    mPosY = mBasePosY;
    mPosZ = mBasePosZ;
    mWaypointIndex = (u8)(param1 & 0xf);
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&data_ov091_02134cdc[mVariant].node[mWaypointIndex],
                  &data_020a0e68, &tmp[0]);
    Vec3_Add(&tmp[1], &mBasePosX, &tmp[0]);
    mPosX = tmp[1].x;
    mPosY = tmp[1].y;
    mPosZ = tmp[1].z;
}

// 0x02131070 (0x8c)
// @symbol _ZN23daObjRotateUpdownLift_c4KillEv
void daObjRotateUpdownLift_c::Kill()
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xbb, mPosX, mPosY, mPosZ);
    Vector3 v = { mPosX, mPosY, mPosZ };
    PoofDustAt(v);
    Sound::PlayBank3(0xf, *(Vector3 *)&mCamSpacePosX);
    mIsDead = 1;
    unk_31c = 0;
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
}

// 0x021310fc (0x64)
// @symbol _ZN23daObjRotateUpdownLift_c15OnHitByMegaCharER6Player
/* HS_UPDOWN_LIFT ignores the hit. The others count it, play 0x1e, and
   snap yaw back to the previous angle. */
void daObjRotateUpdownLift_c::OnHitByMegaChar(Player &player)
{
    unsigned short id = actorID;
    int isHs = (id == 0x1e);
    if (isHs)
        return;
    player.IncMegaKillCount();
    func_02012694(0x1e, (const Vector3 *)&mCamSpacePosX);
    KillByMegaChar(player);
    mAngleY = mPrevAngleY;
}

// 0x02131160 (0x1e0)
// @symbol func_ov091_02131160
/* Aim the cuboid shadow and the clip volume. Called every frame except
   for HS_UPDOWN_LIFT. */
int daObjRotateUpdownLift_c::func_ov091_02131160()
{
    /* The shadow matrix is the 0x30 bytes at 0x348 (pad_348). Holding that
       as a Matrix4x3 * steals r4 from `this` and the function grows. The
       three translation words are stored from `this`, not from that pointer. */
    char *c = (char *)this;
    VariantWords shadowBaseY = data_ov091_02134bac;
    VariantWords shadowSizeX = data_ov091_02134bd0;
    VariantWords shadowSizeZ = data_ov091_02134bb8;
    VariantWords clipCap = data_ov091_02134ba0;

    Matrix4x3_FromRotationY((Matrix4x3 *)(c + 0x348), mAngleY);
    *(int *)(c + 0x36c) = mPosX >> 3;

    int sinIdx = (u16)mAngleX >> 4;
    int sine = data_02082214[sinIdx << 1];
    int sineAbs = sine < 0 ? -sine : sine;
    int scaled = (int)(((long long)sineAbs * 0xa0000 + 0x800) >> 12);
    int variantA = mVariant;
    int base = shadowBaseY.v[variantA];
    int sum = base + scaled;
    int py = mPosY;
    *(int *)(c + 0x370) = (py - sum) >> 3;
    *(int *)(c + 0x374) = mPosZ >> 3;

    int variantB = mVariant;
    int h = mPosY - mGroundY;
    if (h <= 0x1000)
        h = 0x1000;
    int cap = clipCap.v[variantB];
    if (h + 0x100000 >= cap)
        cap = h + 0x100000;
    mClipOffsetY = -((int)(h + ((unsigned)h >> 31)) >> 1);
    mClipRadius = (int)(cap + ((unsigned)cap >> 31)) >> 4;

    int shr = (int)(((long long)h * 32 + 0x800) >> 12);
    int cosIdx = (u16)mAngleX >> 4;
    int variantC = mVariant;
    int sx = shadowSizeX.v[variantC] - shr;
    int cosine = data_02082214[(cosIdx << 1) + 1];
    int fac = 0xa0000 - shr;
    if (cosine < 0)
        cosine = -cosine;
    return _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x320, c + 0x348, sx, h,
        shadowSizeZ.v[variantC] + (int)(((long long)fac * cosine + 0x800) >> 12), 0xf);
}

// 0x02131340 (0x48)
// @symbol func_ov091_02131340
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
// @symbol _ZN23daObjRotateUpdownLift_c16CleanupResourcesEv
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
// @symbol _ZN23daObjRotateUpdownLift_c6RenderEv
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
// @symbol _ZN23daObjRotateUpdownLift_c8BehaviorEv
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
            if ((daObjRotateUpdownLift_c *)mPlatform0 == 0
                || (daObjRotateUpdownLift_c *)mPlatform1 == 0) {
                daObjRotateUpdownLift_c *nearby;
                nearby = (daObjRotateUpdownLift_c *)FindWithActorID(0x1d, 0);
                if (nearby != 0) {
                    do {
                        if (nearby != this
                            && Vec3_HorzDist((Vector3 *)&mPosX,
                                             (Vector3 *)&nearby->mPosX) < 0xa0000) {
                            if ((daObjRotateUpdownLift_c *)mPlatform0 == 0)
                                mPlatform0 = (s32)nearby;
                            else if ((daObjRotateUpdownLift_c *)mPlatform1 == 0)
                                mPlatform1 = (s32)nearby;
                        }
                        nearby = (daObjRotateUpdownLift_c *)FindWithActorID(0x1d, nearby);
                    } while (nearby != 0);
                }
            }
            platform0 = (daObjRotateUpdownLift_c *)mPlatform0;
            if (platform0 != 0) {
                platform1 = (daObjRotateUpdownLift_c *)mPlatform1;
                if (platform1 != 0) {
                    int offScreen;
                    if (platform0->mIsDead != 0 && platform1->mIsDead != 0)
                        keepSound = 0;
                    offScreen = (mFlags & 8) ? 1 : 0;
                    if (offScreen != 0) {
                        if (DistToCPlayer() > 0x7d0000) {
                            platform0 = (daObjRotateUpdownLift_c *)mPlatform0;
                            if (platform0->mIsDead != 0)
                                platform0->func_ov091_02130fac();
                            platform1 = (daObjRotateUpdownLift_c *)mPlatform1;
                            if (platform1->mIsDead != 0)
                                platform1->func_ov091_02130fac();
                        }
                    }
                }
            }
        }
        if (keepSound != 0) {
            mSoundHandle = Sound::PlayLong(
                mSoundHandle, 3, 0x8d, *(Vector3 *)&mCamSpacePosX, 0);
        }
        return 1;
    }

    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(this, 0x2000, 0, 0, 0) != 0)
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
    MulVec3Mat4x3(&data_ov091_02134cdc[mVariant].node[mWaypointIndex],
                  &data_020a0e68, &cur);
    MulVec3Mat4x3(&data_ov091_02134cdc[mVariant].node[prevIndex],
                  &data_020a0e68, &prev);
    Vec3_Add(&target, &mBasePosX, &cur);
    Vec3_Sub(&remain, &mPosX, &target);
    Vec3_Sub(&edge, &cur, &prev);
    span = LenVec3(&edge);
    dist = LenVec3(&remain);
    along = cstd::fdiv(span - dist, span);
    /* 0x3a2 is a halfword. mPitchBase in the header is only the low byte. */
    mAngleX = (s16)(*(s16 *)((char *)this + 0x3a2)
                    + (s16)(((long long)mPitchStep * along + 0x800) >> 12));

    if (dist == 0 || dist <= stepLen) {
        Vec3_Add(&snapped, &mBasePosX, &cur);
        mPosX = snapped.x;
        mPosY = snapped.y;
        mPosZ = snapped.z;
        /* &mPitchBase is offsetof 0x3a2, which is not an ARM immediate, so this
           is a pool add. The step is the halfword at this+0x300+0xa4. One
           base for both grows the function. */
        {
            s16 *pitch = (s16 *)&mPitchBase;
            s16 *stepAt = (s16 *)((char *)this + 0x300);
            arrived = 1;
            *pitch = (s16)(*pitch + *(s16 *)((char *)stepAt + 0xa4));
        }
    } else {
        Vec3_MulScalar(&stepVec, &remain, cstd::fdiv(stepLen, dist));
        SubVec3(&mPosX, &stepVec, &mPosX);
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
            func_020393a4((int *)&mMeshCollider, 0x150000);
            func_02039394((int *)&mMeshCollider, 0x1000);
            if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x150000, 0x1000) != 0)
                UpdateClsnPosAndRot();
        } else {
            if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
                UpdateClsnPosAndRot();
        }
    }

    return 1;
}

// 0x021318f0 (0x2b4)
// @symbol _ZN23daObjRotateUpdownLift_c13InitResourcesEv
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
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0, 0x1000, 0, 0);
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
        else if (data_0209f2f8 == 7)
            mVariant = 1;
    }

    mWaypointIndex = (u8)(param1 & 0xf);

    if (mWaypointIndex == 2) {
        Vec3_Add(&spawnAt, &mBasePosX, &data_ov091_02134cdc[mVariant].node[0]);
        spawnAt.y += data_ov091_02134d1c[mVariant].y;
        Spawn(0x1d, 0xffff, spawnAt, 0, mAreaId, -1);
    }

    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    {
        char *tbl = (char *)data_ov091_02134cdc;
        int stride = 0x78;
        int i = mVariant;
        node = mWaypointIndex;
        MulVec3Mat4x3(tbl + i * stride + node * 0xc, &data_020a0e68, &rotated);
    }
    Vec3_Add(&placed, &mBasePosX, &rotated);
    mPosX = placed.x;
    mPosY = placed.y;
    mPosZ = placed.z;

    variant = mVariant;
    bmd = Model::LoadFile(*data_ov091_02134c30[variant].file);
    mModel.SetFile((BMD_File *)bmd, 1, -1);

    func_ov091_02131340();
    UpdateClsnPosAndRot();

    variant = mVariant;
    kcl = dBgW_Kc::LoadFile(*data_ov091_02134c34[variant].file);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)kcl, &mClsnMat, 0x199, mAngleY,
        data_ov091_02134c38[variant].clps);
    func_020393d4((int *)&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);

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
// @symbol daObjRotateUpdownLift_c_classInit_HS_UPDOWN_LIFT
extern "C" daObjRotateUpdownLift_c *daObjRotateUpdownLift_c_classInit_HS_UPDOWN_LIFT()
{
    return new daObjRotateUpdownLift_c();
}

// 0x02131bdc (0x38)
// @symbol daObjRotateUpdownLift_c_classInit_UPDOWN_LIFT
extern "C" daObjRotateUpdownLift_c *daObjRotateUpdownLift_c_classInit_UPDOWN_LIFT()
{
    return new daObjRotateUpdownLift_c();
}
