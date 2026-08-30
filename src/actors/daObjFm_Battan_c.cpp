//cpp
/* Genuine production translation unit for ov023/daObjFm_Battan_c.
 *
 * The class identity is carried by retail RTTI. Squasher_Spawn and
 * Squasher_SpawnInfo remain evidence-bounded C ABI aliases because the ROM
 * does not preserve their original source spellings. Private helper spellings
 * are likewise inferred; their class ownership, bodies, calls, and ordering
 * are byte/relocation proven.
 *
 * mwccarm emits function sections in reverse source order. Keep the ROM-high
 * factory first and the ROM-low helper last. InitResources is the key function;
 * together with the inline destructor in the real header this naturally emits
 * retail D1 then D0 and the class RTTI/vtable without a forcing object.
 */

#include "daObjFm_Battan_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"

/* A local Vector3 value would make mwccarm emit its vague-linkage destructor.
 * These calls only require the three-word ABI value, so keep that value POD. */
struct BattanVector3 { s32 x, y, z; };

struct BattanSpawnInfo {
    daObjFm_Battan_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char BattanSpawnInfo_size_must_be_0x1c[
    sizeof(BattanSpawnInfo) == 0x1c ? 1 : -1];

extern "C" {
extern int _ZTV16daObjFm_Battan_c[];
extern void *_ZN7fBase_cnwEj(unsigned int);
extern void _ZN10dBgActor_cC2Ev(void *);
extern void _ZN11ShadowModelC1Ev(ShadowModel *);

extern SharedFilePtr data_ov023_02112080;
extern SharedFilePtr data_ov023_02112088;
extern char data_ov064_0211ba4c[];
extern s16 data_02082214[];

extern void Matrix4x3_FromRotationY(void *, s16);
extern void Matrix4x3_FromRotationXYZExt(void *, int, int, int);
extern int _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *, ShadowModel *, Matrix4x3 *, Fix12i, Fix12i, Fix12i, u32);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
    dActor_c *, BattanVector3 *, Fix12i);
extern void AddVec3(BattanVector3 *, BattanVector3 *, BattanVector3 *);
extern void func_0200fa04(dActor_c *, BattanVector3 *, int);

/* Known by-value Fix12 compiler wall: the typed declaration is present in
 * dBgW_KcMbg.h, but this narrow caller seam preserves retail's register ABI. */
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *, KCL_File *, Matrix4x3 *, Fix12i, s16, void *);
extern void func_020393d4(int *, int);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
}

extern "C" daObjFm_Battan_c *Squasher_Spawn();

/* ROM ordinal 8 -- Squasher_Spawn, 0x02111728, size 0x38. */
// @symbol Squasher_Spawn
extern "C" daObjFm_Battan_c *Squasher_Spawn()
{
    daObjFm_Battan_c *actor =
        (daObjFm_Battan_c *)_ZN7fBase_cnwEj(sizeof(daObjFm_Battan_c));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV16daObjFm_Battan_c[2];
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
    }
    return actor;
}

extern "C" BattanSpawnInfo Squasher_SpawnInfo = {
    Squasher_Spawn,
    0x0054,
    0x0124,
    0x02000000,
    0,
    0x00500000,
    0x01000000,
    0x01000000
};

/* ROM ordinal 7 -- InitResources, 0x02111670, size 0xb8. */
// @symbol _ZN16daObjFm_Battan_c13InitResourcesEv
int daObjFm_Battan_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov023_02112088), 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelTransform();
    UpdateClsnPosAndRot();

    KCL_File *file = (KCL_File *)dBgW_Kc::LoadFile(data_ov023_02112080);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, file, &mClsnMat, 0x1000, mAngleY,
        data_ov064_0211ba4c);
    func_020393d4(
        (int *)&mMeshCollider,
        (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mMeshCollider.Enable(this);

    mAngVelX = 0;
    mStateTimer = 0;
    mState = 0;
    return 1;
}

/* ROM ordinal 6 -- Behavior, 0x021113b0, size 0x2c0. */
// @symbol _ZN16daObjFm_Battan_c8BehaviorEv
int daObjFm_Battan_c::Behavior()
{
    switch (mState) {
    case 0:
        if (DistToCPlayer() < 0x3e8000) {
            mState++;
            Sound::PlayBank3(0x43, *(Vector3 *)&mCamSpacePosX);
        }
        break;

    case 1:
        mAngVelX -= 0x40;
        if (mAngVelX <= -0x1000)
            mAngVelX = -0x1000;
        mAngleX += mAngVelX;
        if (mAngleX <= -0x4000) {
            BattanVector3 offset;
            BattanVector3 pos;
            BattanVector3 dustPos;
            int sinY;
            int cosY;

            mAngleX = -0x4000;
            mAngVelX = 0;
            mStateTimer = 0;

            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
                this, &pos, 0x04000000);
            Sound::PlayBank3(0x44, *(Vector3 *)&mCamSpacePosX);

            sinY = data_02082214[((u16)mAngleY >> 4) * 2];
            offset.x = (s32)(((s64)sinY * -0x190000 + 0x800) >> 12);
            cosY = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            offset.z = (s32)(((s64)cosY * -0x190000 + 0x800) >> 12);
            AddVec3(&offset, (BattanVector3 *)&mPosX, &offset);

            dustPos.x = offset.x;
            dustPos.y = offset.y;
            dustPos.z = offset.z;
            func_0200fa04(this, &dustPos, 1);
            mState++;
        }
        break;

    case 2:
        if (mStateTimer >= 0x3c) {
            mAngVelX = 0x80;
            mState++;
            Sound::PlayBank3(0x45, *(Vector3 *)&mCamSpacePosX);
        } else {
            mStateTimer++;
        }
        break;

    case 3:
        mAngleX += mAngVelX;
        if (mAngleX >= 0) {
            mAngVelX = 0;
            mAngleX = 0;
            mStateTimer = 0;
            mState++;
        }
        break;

    case 4:
        if (mStateTimer >= 0x3c)
            mState = 0;
        else
            mStateTimer++;
        break;
    }

    UpdateModelTransform();
    UpdateClsnPosAndRot();
    UpdateShadow();
    return 1;
}

/* ROM ordinal 5 -- Render, 0x02111388, size 0x28. */
// @symbol _ZN16daObjFm_Battan_c6RenderEv
int daObjFm_Battan_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 4 -- CleanupResources, 0x02111350, size 0x38. */
// @symbol _ZN16daObjFm_Battan_c16CleanupResourcesEv
int daObjFm_Battan_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov023_02112088.Release();
    data_ov023_02112080.Release();
    return 1;
}

/* ROM ordinal 3 -- inferred daObjFm_Battan_c::UpdateModelTransform,
 * 0x02111308, size 0x48. */
// @symbol _ZN16daObjFm_Battan_c20UpdateModelTransformEv
void daObjFm_Battan_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationXYZExt(
        &mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* ROM ordinal 2 -- inferred daObjFm_Battan_c::UpdateShadow,
 * 0x0211124c, size 0xbc. */
// @symbol _ZN16daObjFm_Battan_c12UpdateShadowEv
int daObjFm_Battan_c::UpdateShadow()
{
    int tableIndex = (u16)mAngleX >> 4;
    int sinX = data_02082214[tableIndex * 2];
    int depth = (s32)(((s64)sinX * 0x49c000 + 0x800) >> 12);

    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = mPosY >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &mShadowMat,
        0x258000, 0x32000, -depth, 0xf);
}
