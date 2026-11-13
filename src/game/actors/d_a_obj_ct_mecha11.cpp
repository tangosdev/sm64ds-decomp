//cpp
/* Production translation unit for ov035/daObjCtMecha11_c.
 * 9 function(s), .text 0x021116ec..0x02111bd0. Tick Tock Clock's spinning
 * platform (profile CT_MECHA11).
 *
 * Behavior adds a clock-setting step to mAngleX. Setting 2 rolls a sign
 * and, for the first five frames of the burst, adds nothing. InitResources
 * loads the model and the moving mesh, then probes a short way down for the
 * floor the cuboid shadow sits on. UpdateShadow widens the clip as the
 * platform rises and scales the shadow's depth with how flat it is.
 *
 * NAME: _ZTS16daObjCtMecha11_c is "16daObjCtMecha11_c" at ov035 0x02112b94;
 * _ZTI at 0x02112b88 reads [__si_class_type_info, that string,
 * _ZTI10dBgActor_c]. The tree previously called the class SpinningPlatform
 * (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x021116ec), D0
 * (0x02111738), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 * The registry factory daObjCtMecha11_c_classInit (0x02111b98) closes the
 * unit and ov035's .text.
 */

// Notes: dBgW::SetClipRadius / dBgW::SetClipOffsetY / dBgW::SetBeforeClsnCallback / dBgW_Kc::SetEdgeToleranceShift are
// the out-of-line stores into the collider's range fields and callback slot;
// d_bg_w.cpp defines them.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjCtMecha11_c_AssetId {
    ASSET_CT_MECHA_OBJ11_BMD = 0x05d0,
    ASSET_CT_MECHA_OBJ11_KCL = 0x05d1
};

#pragma defer_codegen off

#include "common.h"
#include "daObjCtMecha11_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "LevelRng.h"
#include "nitro/fx/fx_trig.h"

/* Resource handles constructed by this TU's static initializer. The
 * constructor and destructor bodies stay out of line; mwcc registers them. */
struct Mecha11ModelFilePtr : SharedFilePtr {
    Mecha11ModelFilePtr(u32 fileID);
    ~Mecha11ModelFilePtr();
};

struct Mecha11ClsnFilePtr : SharedFilePtr {
    Mecha11ClsnFilePtr(u32 fileID);
    ~Mecha11ClsnFilePtr();
};

extern "C" {
/* local extern: these matching CalcLib signatures keep their names to avoid
 * renumbering this TU's compiler-generated static-initializer aliases. */
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern int RandomIntInternal(int *p);
}

extern Mecha11ClsnFilePtr RotatingClockHand_ClsnFile;
extern Mecha11ModelFilePtr RotatingClockHand_ModelFile;
extern unsigned char data_0209f2c0;
extern short data_ov035_02112b80[];
extern CLPS_Block data_ov035_02112238;

daObjCtMecha11_c::~daObjCtMecha11_c() {}

void daObjCtMecha11_c::UpdateShadow() {
    int d1 = mPosY - mFloorPosY;
    int ad = d1 < 0 ? -d1 : d1;
    if (ad > 0x7d0000)
        return;
    int clipD = ad + 0x350000;
    int half = ad / 2;
    if (clipD < 0x350000)
        clipD = 0x350000;
    mClipOffsetY = -half;
    mClipRadius = (int)(clipD + ((unsigned)clipD >> 31)) >> 4;
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = mFloorPosY >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    int s = FX_CosIdx((u16)mAngleX >> 4);
    int sa = s < 0 ? -s : s;
    int scaled = FX_MUL(sa, 0x26c000);
    DropShadowScaleXYZ(mShadowModel, mShadowMat, 0x12c000, 0x32000, scaled + 0x50000, 0xf);
}

void daObjCtMecha11_c::UpdateModel() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

int daObjCtMecha11_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    RotatingClockHand_ModelFile.Release();
    RotatingClockHand_ClsnFile.Release();
    return 1;
}

int daObjCtMecha11_c::Render() {
    mModel.Render(0);
    return 1;
}

int daObjCtMecha11_c::Behavior() {
    unsigned char idx = data_0209f2c0;
    mPrevAngleX = data_ov035_02112b80[idx];
    if (idx == 2) {
        if (DecIfAbove0_Short(&mRandTimer) == 0) {
            int r = (unsigned short)((unsigned)RandomIntInternal(&data_0209e650.value) >> 16);
            if ((unsigned)r >= 0x7fff)
                mRandDirection = 1;
            else
                mRandDirection = -1;
            mRandTimer = ((r % 4 + 1) * 0x1e);
            mRandFrames = mRandTimer;
        } else {
            if (mRandTimer < mRandFrames - 5) {
                /* The temporary keeps the 0x92 address in its own register
                 * beside the this+0x300 base the timer already computed. */
                short *angleStep = &mPrevAngleX;
                *angleStep = (*angleStep * mRandDirection);
            } else {
                mPrevAngleX = 0;
            }
        }
    }
    mMeshCollider.SetClipRadius(0x180000);
    mMeshCollider.SetClipOffsetY(0x1000);
    short *angle = &mAngleX;
    *angle = (*angle + mPrevAngleX);
    UpdateModel();
    UpdateShadow();
    if (IsClsnInRange(0x180000, 0x1000))
        UpdateClsnPosAndRot();
    return 1;
}

int daObjCtMecha11_c::InitResources() {
    BMD_File *mf = dExtSimpleModel_c::LoadFile(RotatingClockHand_ModelFile);
    mModel.SetFile(mf, 1, -1);
    mShadowModel.InitCuboid();
    UpdateModel();
    UpdateClsnPosAndRot();
    KCL_File *kf = dBgW_Kc::LoadFile(RotatingClockHand_ClsnFile);
    mMeshCollider.SetFile(kf, mClsnMat, 0x1000, mAngleY, data_ov035_02112238);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    mMeshCollider.SetEdgeToleranceShift(0);
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y - 0xa000;
    dBgCh_Gnd rg;
    rg.SetObjAndPos(v, 0);
    mFloorPosY = v.y;
    if (rg.DetectClsn() != 0)
        mFloorPosY = rg.clsnY;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daObjCtMecha11_c through
 * RTTI, allocation size, vtable identity, and the CT_MECHA11 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SpinningPlatform_Spawn. */
extern "C" daObjCtMecha11_c *daObjCtMecha11_c_classInit() {
    return new daObjCtMecha11_c();
}

Mecha11ModelFilePtr RotatingClockHand_ModelFile(ASSET_CT_MECHA_OBJ11_BMD);
Mecha11ClsnFilePtr RotatingClockHand_ClsnFile(ASSET_CT_MECHA_OBJ11_KCL);
