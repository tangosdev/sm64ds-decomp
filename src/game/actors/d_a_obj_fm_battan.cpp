//cpp
/* daObjFm_Battan_c -- the slamming slab (FM_BATTAN), ov023.
 *
 * It waits until the player is within 1000.0 (sound 0x43), then tips over
 * about X, accelerating until it lies flat (mAngleX -0x4000). On impact it
 * shakes the ground, plays 0x44 and raises dust 400.0 behind its heading
 * (position - 400.0 * facing). After 60 frames it swings back upright
 * (0x45), rests 60 frames, and the cycle starts over. The shadow stretches with the tilt (UpdateShadow).
 *
 * daObjFm_Battan_c_classInit and g_profile_FM_BATTAN are reconstructed
 * source-style names. SM64DS proves the RTTI class, FM_BATTAN registry ID,
 * descriptor/factory relationship, and object shape; later EAD lineage
 * supplies the spelling prior. Historical project aliases: Squasher_Spawn
 * and Squasher_SpawnInfo. Private helper spellings are likewise inferred;
 * their class ownership, bodies, calls, and ordering are byte/relocation
 * proven.
 *
 * _ZTS16daObjFm_Battan_c 0x02111fac. _ZTI16daObjFm_Battan_c 0x02111fa0.
 * _ZTV16daObjFm_Battan_c 0x02111fe4. InitResources is the key function;
 * with the inline destructor in the real header this emits retail D1
 * (0x021111a0) then D0 (0x021111ec).
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *   Source is reverse ROM order: keep the ROM-high factory first and the
 *   ROM-low helper last. InitResources is the key function; with the inline
 *   destructor in the real header this emits retail D1 then D0 and the
 *   class RTTI/vtable without a forcing object.
 *   BattanVector3: a local Vector3 would emit vague-linkage ~Vector3.
 *   common.h first (via dBgActor_c.h): Matrix4x3 is s32 m[12], so the
 *   translation stays m[9]/m[10]/m[11].
 *
 * WHY SOME CALLS ARE SPELLED AS MANGLED SYMBOLS (Fix12<int> by value, see
 * notes/mwccarm-codegen.md 6az; the header method form changes the code
 * size): dBgW_KcMbg::SetFile
 * (InitResources), dActor_c::DropShadowScaleXYZ (UpdateShadow) and
 * dActor_c::Earthquake (Behavior).
 *
 * Known limits:
 *   func_020393d4: InitResources stores dBgW::UpdatePosWithTransform on
 *   mMeshCollider; dBgW.h has no setter.
 *   func_0200fa04 is Behavior's impact dust; no named method in this TU.
 *   AddVec3 / Matrix4x3_FromRotationY / Matrix4x3_FromRotationXYZExt: no
 *   shared header this TU can take without a wider change.
 *   data_ov023_02112088 / 02112080 are resource handles this TU's sinit
 *   owns; data_ov064_0211ba4c (CLPS) is overlay .data; data_02082214 is the
 *   sine table.
 *   No Player.h / dCamera_c.h. The leaf operator new stays until #2570.
 */

// leftovers:
// - func_020393d4: InitResources stores dBgW::UpdatePosWithTransform on
//   mMeshCollider; dBgW.h has no setter.
// - func_0200fa04 is Behavior's impact dust; no named method in this TU.
// - AddVec3 / Matrix4x3_FromRotationY / Matrix4x3_FromRotationXYZExt: no
//   shared header this TU can take without a wider change.
// - data_ov023_02112088 / 02112080 are resource handles this TU's sinit
//   owns; data_ov064_0211ba4c (CLPS) is overlay .data; data_02082214 is the
//   sine table.
// - No Player.h / dCamera_c.h here. The leaf operator new stays until the
//   fBase_c size_t overload work lands.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_fm_battan_AssetId {
    ASSET_FM_BATTAN_BMD = 0x0616,
    ASSET_FM_BATTAN_KCL = 0x0617
};

#include "daObjFm_Battan_c.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "nitro/fx/fx.h"

/* A local Vector3 value would make mwccarm emit its vague-linkage destructor.
 * These calls only require the three-word ABI value, so keep that value POD. */
struct BattanVector3 { s32 x, y, z; };

extern "C" {
extern char data_ov064_0211ba4c[];
extern s16 data_02082214[];

extern void Matrix4x3_FromRotationY(void *, s16);
extern void Matrix4x3_FromRotationXYZExt(void *, int, int, int);
/* dActor_c::DropShadowScaleXYZ / Earthquake -- reached through the mangled
   name because the by-value Fix12<int> parameters
   (notes/mwccarm-codegen.md 6az) break the method form. */
extern int _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *, dExtShadowModel_c *, Matrix4x3 *, Fix12i, Fix12i, Fix12i, u32);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
    dActor_c *, BattanVector3 *, Fix12i);
extern void AddVec3(BattanVector3 *, BattanVector3 *, BattanVector3 *);
extern void func_0200fa04(dActor_c *, BattanVector3 *, int);

/* Known by-value Fix12 compiler wall: the typed declaration is present in
 * dBgW_KcMbg.h, but this narrow caller seam preserves retail's register ABI. */
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *, KCL_File *, Matrix4x3 *, Fix12i, s16, void *);
extern void func_020393d4(int *, int);
}

/* The retail static initializer constructs these two 8-byte resource handles
 * in source order and lets the C++ runtime register their destructors.  The
 * family spellings are reconstructed; the constructor/destructor addresses,
 * file IDs, object widths, BSS order, and registration topology are direct ROM
 * evidence.  The intact-TU manifest maps their compiler-generated undefined
 * member imports to the existing evidence-bounded ROM symbols without claiming
 * that those project names were the original source spellings. */
struct BattanModelFilePtr : SharedFilePtr {
    u32 words[2];

    BattanModelFilePtr(u32 fileID);
    ~BattanModelFilePtr();
};

struct BattanCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    BattanCollisionFilePtr(u32 fileID);
    ~BattanCollisionFilePtr();
};

extern "C" BattanModelFilePtr data_ov023_02112088;
extern "C" BattanCollisionFilePtr data_ov023_02112080;

BattanModelFilePtr data_ov023_02112088(ASSET_FM_BATTAN_BMD);
BattanCollisionFilePtr data_ov023_02112080(ASSET_FM_BATTAN_KCL);


// @symbol daObjFm_Battan_c_classInit
/* Every instruction the cartridge has here falls out of the one `new`.
 * sizeof(daObjFm_Battan_c) is 0x37c; the leaf operator new forwards to
 * fBase_c::operator new; the implicit constructor calls dBgActor_c's C2,
 * stores this class's vptr, and constructs mShadowModel. The null check is
 * the one `new` itself emits. */
extern "C" daObjFm_Battan_c *daObjFm_Battan_c_classInit()
{
    return new daObjFm_Battan_c();
}

extern "C" SpawnInfo<daObjFm_Battan_c> g_profile_FM_BATTAN = {
    daObjFm_Battan_c_classInit,
    0x0054,
    0x0124,
    0x02000000,
    0,
    0x00500000,
    0x01000000,
    0x01000000
};


// @symbol _ZN16daObjFm_Battan_c13InitResourcesEv
int daObjFm_Battan_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov023_02112088), 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelTransform();
    UpdateClsnPosAndRot();

    KCL_File *file = (KCL_File *)dBgW_Kc::LoadFile(data_ov023_02112080);
    /* MEASURED: by-value Fix12<int> scale; see notes/mwccarm-codegen.md 6az. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, file, &mClsnMat, 0x1000, mAngleY,
        data_ov064_0211ba4c);
    /* Stores dBgW::UpdatePosWithTransform at mMeshCollider+0x18; dBgW.h has
       no setter, so this stays the 8-byte store helper. */
    func_020393d4(
        (int *)&mMeshCollider,
        (int)&dBgW::UpdatePosWithTransform);
    mMeshCollider.Enable(this);

    mAngVelX = 0;
    mStateTimer = 0;
    mState = 0;
    return 1;
}


// @symbol _ZN16daObjFm_Battan_c8BehaviorEv
int daObjFm_Battan_c::Behavior()
{
    switch (mState) {
    case 0:
        if (DistToCPlayer() < 0x3e8000) {
            mState++;
            Sound::PlayBank3(0x43, mCamSpacePos);
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
            /* MEASURED: by-value Fix12<int> (notes/mwccarm-codegen.md 6az). */
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
                this, &pos, 0x04000000);
            Sound::PlayBank3(0x44, mCamSpacePos);

            sinY = data_02082214[((u16)mAngleY >> 4) * 2];
            offset.x = FX_MUL(sinY, -0x190000);
            cosY = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            offset.z = FX_MUL(cosY, -0x190000);
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
            Sound::PlayBank3(0x45, mCamSpacePos);
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


// @symbol _ZN16daObjFm_Battan_c6RenderEv
int daObjFm_Battan_c::Render()
{
    mModel.Render(0);
    return 1;
}


// @symbol _ZN16daObjFm_Battan_c16CleanupResourcesEv
int daObjFm_Battan_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov023_02112088.Release();
    data_ov023_02112080.Release();
    return 1;
}

// @symbol _ZN16daObjFm_Battan_c20UpdateModelTransformEv
void daObjFm_Battan_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationXYZExt(
        &mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN16daObjFm_Battan_c12UpdateShadowEv
int daObjFm_Battan_c::UpdateShadow()
{
    int tableIndex = (u16)mAngleX >> 4;
    int sinX = data_02082214[tableIndex * 2];
    int depth = FX_MUL(sinX, 0x49c000);

    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = mPosY >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    /* MEASURED: three by-value Fix12<int> scales; see notes/mwccarm-codegen.md
       6az. */
    return _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &mShadowMat,
        0x258000, 0x32000, -depth, 0xf);
}
