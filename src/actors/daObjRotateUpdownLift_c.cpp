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
 * destructor emits D1 then D0. Factories and both profiles stay out of
 * this TU.
 *
 * deslop leftovers:
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
void func_ov091_02130fac(daObjRotateUpdownLift_c *lift);
int func_ov091_02131160(daObjRotateUpdownLift_c *lift);
void func_ov091_02131340(daObjRotateUpdownLift_c *lift);

void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b, int c, int d);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
int _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mat, int sx, int sy, int sz, unsigned int opacity);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    int scale, s16 angY, CLPS_Block *clps);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN23daObjRotateUpdownLift_cD1Ev, 0x02130f00, size 0x4c */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov091_02130fac, 0x02130fac, size 0xc4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02130fac
/* Put a killed lift back on its spawn node. The watcher calls this. */
extern "C" void func_ov091_02130fac(daObjRotateUpdownLift_c *lift)
{
    PodVec3 tmp[2];

    lift->mIsDead = 0;
    lift->mAngleX = lift->mSpawnAngleX;
    lift->mAngleY = lift->mSpawnAngleY;
    lift->mAngleZ = lift->mSpawnAngleZ;
    lift->mPosX = lift->mBasePosX;
    lift->mPosY = lift->mBasePosY;
    lift->mPosZ = lift->mBasePosZ;
    lift->mWaypointIndex = (u8)(lift->param1 & 0xf);
    Matrix4x3_FromRotationY(&data_020a0e68, lift->mAngleY);
    MulVec3Mat4x3(&data_ov091_02134cdc[lift->mVariant].node[lift->mWaypointIndex],
                  &data_020a0e68, &tmp[0]);
    Vec3_Add(&tmp[1], &lift->mBasePosX, &tmp[0]);
    lift->mPosX = tmp[1].x;
    lift->mPosY = tmp[1].y;
    lift->mPosZ = tmp[1].z;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN23daObjRotateUpdownLift_c4KillEv, 0x02131070, size 0x8c */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN23daObjRotateUpdownLift_c15OnHitByMegaCharER6Player, 0x021310fc, size 0x64 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov091_02131160, 0x02131160, size 0x1e0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02131160
/* Aim the cuboid shadow and the clip volume. Called every frame except
   for HS_UPDOWN_LIFT. */
extern "C" int func_ov091_02131160(daObjRotateUpdownLift_c *lift)
{
    /* The shadow matrix is the 0x30 bytes at 0x348 (pad_348). Holding that
       as a Matrix4x3 * steals r4 from `this` and the function grows. The
       three translation words are stored from `this`, not from that pointer. */
    char *c = (char *)lift;
    VariantWords v0 = data_ov091_02134bac;
    VariantWords v1 = data_ov091_02134bd0;
    VariantWords v2 = data_ov091_02134bb8;
    VariantWords v3 = data_ov091_02134ba0;

    Matrix4x3_FromRotationY((Matrix4x3 *)(c + 0x348), lift->mAngleY);
    *(int *)(c + 0x36c) = lift->mPosX >> 3;

    int idx = (u16)lift->mAngleX >> 4;
    int s = data_02082214[idx << 1];
    int sa = s < 0 ? -s : s;
    int scaled = (int)(((long long)sa * 0xa0000 + 0x800) >> 12);
    int b5 = lift->mVariant;
    int base = v0.v[b5];
    int sum = base + scaled;
    int py = lift->mPosY;
    *(int *)(c + 0x370) = (py - sum) >> 3;
    *(int *)(c + 0x374) = lift->mPosZ >> 3;

    int b5b = lift->mVariant;
    int h = lift->mPosY - lift->mGroundY;
    if (h <= 0x1000)
        h = 0x1000;
    int cap = v3.v[b5b];
    if (h + 0x100000 >= cap)
        cap = h + 0x100000;
    lift->mClipOffsetY = -((int)(h + ((unsigned)h >> 31)) >> 1);
    lift->mClipRadius = (int)(cap + ((unsigned)cap >> 31)) >> 4;

    int shr = (int)(((long long)h * 32 + 0x800) >> 12);
    int idx2 = (u16)lift->mAngleX >> 4;
    int b5c = lift->mVariant;
    int sx = v1.v[b5c] - shr;
    int cosine = data_02082214[(idx2 << 1) + 1];
    int fac = 0xa0000 - shr;
    if (cosine < 0)
        cosine = -cosine;
    return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x320, c + 0x348, sx, h,
        v2.v[b5c] + (int)(((long long)fac * cosine + 0x800) >> 12), 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov091_02131340, 0x02131340, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02131340
/* Write the model matrix: full rotation, translation at 1/8 of position. */
extern "C" void func_ov091_02131340(daObjRotateUpdownLift_c *lift)
{
    Matrix4x3_FromRotationXYZExt(&lift->mModel.mat4x3,
                                 lift->mAngleX, lift->mAngleY, lift->mAngleZ);
    lift->mModel.mat4x3.t.x = lift->mPosX >> 3;
    lift->mModel.mat4x3.t.y = lift->mPosY >> 3;
    lift->mModel.mat4x3.t.z = lift->mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN23daObjRotateUpdownLift_c16CleanupResourcesEv, 0x02131388, size 0x80 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN23daObjRotateUpdownLift_c6RenderEv, 0x02131408, size 0x60 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN23daObjRotateUpdownLift_c8BehaviorEv, 0x02131468, size 0x488 */
/* -------------------------------------------------------------------------- */
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
            daObjRotateUpdownLift_c *a;
            daObjRotateUpdownLift_c *b;
            if ((daObjRotateUpdownLift_c *)mPlatform0 == 0
                || (daObjRotateUpdownLift_c *)mPlatform1 == 0) {
                daObjRotateUpdownLift_c *found;
                found = (daObjRotateUpdownLift_c *)FindWithActorID(0x1d, 0);
                if (found != 0) {
                    do {
                        if (found != this
                            && Vec3_HorzDist((Vector3 *)&mPosX,
                                             (Vector3 *)&found->mPosX) < 0xa0000) {
                            if ((daObjRotateUpdownLift_c *)mPlatform0 == 0)
                                mPlatform0 = (s32)found;
                            else if ((daObjRotateUpdownLift_c *)mPlatform1 == 0)
                                mPlatform1 = (s32)found;
                        }
                        found = (daObjRotateUpdownLift_c *)FindWithActorID(0x1d, found);
                    } while (found != 0);
                }
            }
            a = (daObjRotateUpdownLift_c *)mPlatform0;
            if (a != 0) {
                b = (daObjRotateUpdownLift_c *)mPlatform1;
                if (b != 0) {
                    int offScreen;
                    if (a->mIsDead != 0 && b->mIsDead != 0)
                        keepSound = 0;
                    offScreen = (mFlags & 8) ? 1 : 0;
                    if (offScreen != 0) {
                        if (DistToCPlayer() > 0x7d0000) {
                            a = (daObjRotateUpdownLift_c *)mPlatform0;
                            if (a->mIsDead != 0)
                                func_ov091_02130fac(a);
                            b = (daObjRotateUpdownLift_c *)mPlatform1;
                            if (b->mIsDead != 0)
                                func_ov091_02130fac(b);
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

    func_ov091_02131340(this);

    {
        int isHs = actorID;
        isHs = (isHs == 0x1e);
        if (isHs == 0) {
            func_ov091_02131160(this);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN23daObjRotateUpdownLift_c13InitResourcesEv, 0x021318f0, size 0x2b4 */
/* -------------------------------------------------------------------------- */
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

    func_ov091_02131340(this);
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
