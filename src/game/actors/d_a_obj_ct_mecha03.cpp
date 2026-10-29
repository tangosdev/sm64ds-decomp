//cpp
/* daObjCtMecha03_c -- TTC swinging pendulum (CT_MECHA03, ov065).
 * It swings under mSwingAccel; in clock mode 2 (data_0209f2c0) each stop
 * rolls a new push strength and an occasional pause. Shadow follows the
 * bob (UpdateShadow); UpdateModelMatrix builds the model matrix.
 *
 * The destructor pair comes off the inline header destructor (D1 then D0,
 * no D2). Source order is reverse ROM order (2004/b56 emits .text back to
 * front); the factory stays first and the inline destructor stays in the
 * header. `return new` emits a homeless _ZN10dBgActor_cD2Ev (compiler-only
 * deadstrip).
 * common.h stays first. Shadow and model matrices are the flat 12-word
 * Matrix4x3; translation is m[9], m[10], m[11].
 *
 * What stays, and why:
 * - dBgW_KcMbg::SetFile, dActor_c::DropShadowScaleXYZ and
 *   dBgActor_c::IsClsnInRange are header member calls whose headers spell
 *   Fix12 as Fix12i, a plain s32. With Fix12<int> locals SetFile sizes
 *   InitResources 0x104->0x110, DropShadowScaleXYZ sizes UpdateShadow
 *   0x12c->0x144, and IsClsnInRange sizes Behavior 0x190->0x1b0.
 * - Assigning mMeshCollider.beforeClsnCallback does not compile: the static
 *   takes references and the field takes pointers. Casting
 *   &dBgW::UpdatePosWithTransform onto the field type sizes InitResources
 *   0x104->0xfc. func_020393d4 is the store that matches. func_020393a4 and
 *   func_02039394 store the mesh range the same way.
 * - UpdateShadow keeps the dead stores. Dropping armOffset.y = 0, the
 *   first pos.y = bobY, or writing armOffset once, sizes it 0x12c->0x128.
 *   Dropping the bobPos zeros sizes it 0x12c->0x120.
 * - UpdateModelMatrix returns the local z. Returning m[11] after the shift
 *   store sizes it 0x48->0x4c.
 * - Behavior's angle store is I16(0x322). mSwingAngle = mSwingAngle +
 *   mSwingSpeed sizes Behavior 0x190->0x184. *accelP += (short)(spd * pos)
 *   sizes it 0x190->0x198. if (vx * vy > 0) stays 0x190 and differs by 4
 *   words. if (mSoundTimer != 0) stays 0x190 and differs by 1 word. Reading
 *   the angle through I16(0x322) sizes it 0x190->0x194.
 *   offsetof(daObjCtMecha03_c, mSwingAngle) does not compile ("( expected").
 * - Matrix4x3.t is not a member of the flat spelling.
 * - The data_ov065_* handles, g_profile_CT_MECHA03, and data_ov035_02112198
 *   are not this TU's data.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjCtMecha03_c_AssetId {
    ASSET_CT_MECHA_OBJ03_BMD = 0x05bc,
    ASSET_CT_MECHA_OBJ03_KCL = 0x05bd
};

#include "common.h"
#include "daObjCtMecha03_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

struct CLPS_Block;

/* Angle accumulation only. Naming that store mSwingAngle shifts Behavior
   from 0x190 to 0x184. The masked absolute address is the store that matches. */
#define I16(off) (*(short *)(((int)this + (off)) & 0xFFFFFFFF))

/* Resource handles constructed by this TU's static initializer. The
 * constructor and destructor bodies stay out of line; mwcc registers them. */
struct Mecha03ModelFilePtr : SharedFilePtr {
    Mecha03ModelFilePtr(u32 fileID);
    ~Mecha03ModelFilePtr();
};

struct Mecha03ClsnFilePtr : SharedFilePtr {
    Mecha03ClsnFilePtr(u32 fileID);
    ~Mecha03ClsnFilePtr();
};

extern u8 data_0209f2c0;
extern int data_0209e650;
extern s16 data_ov065_0211c0b0[];
extern Mecha03ModelFilePtr data_ov065_0211d88c;
extern Mecha03ClsnFilePtr data_ov065_0211d894;
extern CLPS_Block data_ov035_02112198;

/* local extern: arm9 matrix/vector helpers and dBgW stores with no shared
 * header. */
extern "C" {
extern void Matrix4x3_FromRotationZXYExt(Matrix4x3 *m, int x, int y, int z);
extern void MulVec3Mat4x3(Vector3 *in, Matrix4x3 *m, Vector3 *out);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void func_02039394(void *p, int v);
extern void func_020393a4(void *p, int v);
extern void func_020393d4(void *p, void *v);
}

// @symbol daObjCtMecha03_c_classInit
extern "C" daObjCtMecha03_c *daObjCtMecha03_c_classInit() {
    return new daObjCtMecha03_c();
}

// @symbol _ZN16daObjCtMecha03_c13InitResourcesEv
int daObjCtMecha03_c::InitResources() {
    void *bmd;
    void *kcl;

    bmd = dExtSimpleModel_c::LoadFile(data_ov065_0211d88c);
    mModel.SetFile((BMD_File *)bmd, 1, -1);
    mShadowModel.InitCuboid();

    mSwingDir = 1;
    mSwingAccel = data_ov065_0211c0b0[data_0209f2c0];
    mSwingAngle = 0x1964;
    mAngleZ = mSwingAngle;

    UpdateModelMatrix();
    UpdateClsnPosAndRot();

    kcl = dBgW_Kc::LoadFile(data_ov065_0211d894);
    mMeshCollider.SetFile((KCL_File *)kcl, mClsnMat, 0x1000, mAngleY, data_ov035_02112198);

    func_020393a4(&mMeshCollider, 0x300000);
    func_02039394(&mMeshCollider, -0x200000);

    if (data_0209f2c0 != 3) {
        func_020393d4(&mMeshCollider, &dBgW::UpdatePosWithTransform);
    }
    return 1;
}

// @symbol _ZN16daObjCtMecha03_c8BehaviorEv
int daObjCtMecha03_c::Behavior() {
    if (data_0209f2c0 != 3) {
        if ((u16)mSoundTimer != 0) {
            if (DecIfAbove0_Short((u16 *)&mSoundTimer) == 0) {
                Sound::PlayBank3(0x38, mCamSpacePos);
            }
        }
        if (DecIfAbove0_Short((u16 *)&mPauseTimer) == 0) {
            {
                short vx = mSwingDir;
                short vy = mSwingAngle;
                short *accelP = &mSwingSpeed;
                if (vy * vx > 0) {
                    vx = -vx;
                    mSwingDir = vx;
                }
                {
                    short spd = mSwingAccel;
                    short pos = mSwingDir;
                    short accel = *accelP;
                    *accelP = (short)(spd * pos + accel);
                }
            }
            if (data_0209f2c0 == 2 && mSwingSpeed == 0) {
                int roll = RandomIntInternal(&data_0209e650);
                if ((unsigned)roll % 3 != 0)
                    mSwingAccel = 0xd;
                else
                    mSwingAccel = 0x2a;
                if ((roll & 1) == 0) {
                    mPauseTimer = ((unsigned)roll >> 0x1b) + 3;
                }
            }
            if (mSwingSpeed == 0) {
                mSoundTimer = (u16)mPauseTimer + 0xf;
            }
            I16(0x322) += mSwingSpeed;
        }
        mAngleZ = mSwingAngle;
    }

    UpdateModelMatrix();
    UpdateShadow();
    if (IsClsnInRange(0x300000, -0x200000) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN16daObjCtMecha03_c6RenderEv
int daObjCtMecha03_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjCtMecha03_c16CleanupResourcesEv
int daObjCtMecha03_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d88c.Release();
    data_ov065_0211d894.Release();
    return 1;
}

// @symbol _ZN16daObjCtMecha03_c17UpdateModelMatrixEv
/* Writes mModel.mat4x3 from the actor's Y/Z angles and copies the
   actor position into its translation row. */
int daObjCtMecha03_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, 0, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    int z = mPosZ >> 3;
    mModel.mat4x3.m[11] = z;
    return z;
}

// @symbol _ZN16daObjCtMecha03_c12UpdateShadowEv
/* Drops the pendulum's shadow: swings a fixed offset through the actor's
   orientation, raycasts the ground under the result, then hands the shadow
   matrix to dActor_c::DropShadowScaleXYZ. */
void daObjCtMecha03_c::UpdateShadow() {
    Vector3 armOffset;
    Vector3 bobPos;
    Vector3 pos;
    armOffset.y = 0;
    armOffset.y = -0x320000;
    bobPos.x = 0;
    bobPos.y = 0;
    bobPos.z = 0;
    armOffset.x = 0;
    armOffset.z = 0;
    Matrix4x3_FromRotationZXYExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    MulVec3Mat4x3(&armOffset, &data_020a0e68, &bobPos);
    AddVec3(&bobPos, &mPos, &bobPos);
    {
        int bobY = bobPos.y;
        pos.x = bobPos.x;
        pos.z = bobPos.z;
        pos.y = bobY;
        pos.y = bobY - 0xc8000;
    }
    dBgCh_Gnd rc;
    rc.SetObjAndPos(pos, 0);
    mGroundY = pos.y;
    if (rc.DetectClsn())
        mGroundY = rc.clsnY;
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = bobPos.x >> 3;
    mShadowMat.m[10] = mGroundY >> 3;
    mShadowMat.m[11] = bobPos.z >> 3;
    DropShadowScaleXYZ(mShadowModel, mShadowMat, 0x12c000, 0x12c000, 0x78000, 0xf);
}
// @symbol _ZN16daObjCtMecha03_cD1Ev
// @symbol _ZN16daObjCtMecha03_cD0Ev
/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 order without
 * the otherwise homeless D2 variant. */

Mecha03ModelFilePtr data_ov065_0211d88c(ASSET_CT_MECHA_OBJ03_BMD);
Mecha03ClsnFilePtr data_ov065_0211d894(ASSET_CT_MECHA_OBJ03_KCL);
