//cpp
/* daKpa2Bg_c: KOOPA2BG, ov060. Adds mAngleX/Y/ZSpeed to its angles each
 * frame. The model matrix follows X and Z; the moving mesh follows all three.
 *
 * Class name is the ROM RTTI string (include/daKpa2Bg_c.h). .text
 * 0x02117980..0x02117d1c: D1 through InitResources, then
 * daKpa2Bg_c_classInit, ROM-ascending under defer_codegen off.
 */
/* What stays, and why:
 * - dBgW::SetBeforeClsnCallback stores dBgW::UpdatePosAndAngs on the collider.
 * - data_ov046_021115bc is the CLPS block SetFile takes. gScratchMtx is
 *   the scratch matrix both matrix helpers write.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daKpa2Bg_c_AssetId { ASSET_KB2_STAGE_BMD = 0x0623, ASSET_KB2_STAGE_KCL = 0x0624 };

#pragma defer_codegen off

#include "common.h"
#include "daKpa2Bg_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"

struct CLPS_Block;

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (ModelFile::ModelFile / ModelFile::~ModelFile for the model, ClsnFile::ClsnFile /
 * ClsnFile::~ClsnFile for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct Kpa2BgModelFilePtr : SharedFilePtr {
    Kpa2BgModelFilePtr(u32 fileID);
    ~Kpa2BgModelFilePtr();
};
struct Kpa2BgCollisionFilePtr : SharedFilePtr {
    Kpa2BgCollisionFilePtr(u32 fileID);
    ~Kpa2BgCollisionFilePtr();
};

extern Kpa2BgModelFilePtr daKpa2Bg_c_ModelFile;
extern Kpa2BgCollisionFilePtr daKpa2Bg_c_ClsnFile;
extern CLPS_Block data_ov046_021115bc;

int Sound_PlayIfNotActive(int handle, int a, int b, short c);

daKpa2Bg_c::~daKpa2Bg_c() {}

/* Plays sound 0x95 through the handle kept in mSoundHandle. Bowser calls it
   on odd frames while he tilts the arena. */
void daKpa2Bg_c::PlayTiltSound() {
    mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 3, 0x95, 0);
}

/* Rebuilds the collision matrix from the position and the X and Z angles,
   then moves the moving mesh collider to it with the Y angle. */
void daKpa2Bg_c::UpdateClsnMatrix() {
    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x, mPos.y, mPos.z);
    Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, mAngle.x);
    Matrix4x3_ApplyInPlaceToRotationZ(&gScratchMtx, mAngle.z);
    mClsnMat = gScratchMtx;
    mMovingMeshCollider2.Transform(mClsnMat, mAngle.y);
}

/* Rebuilds the model matrix the same way from the position shifted down by 3
   bits. The Y angle is not part of this matrix build. */
void daKpa2Bg_c::UpdateModelMatrix() {
    Vector3 pos;
    Vec3_Asr(&pos, &mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, pos.x, pos.y, pos.z);
    Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, mAngle.x);
    Matrix4x3_ApplyInPlaceToRotationZ(&gScratchMtx, mAngle.z);
    mModel2.mat4x3 = gScratchMtx;
}

s32 daKpa2Bg_c::CleanupResources() {
    if (mMovingMeshCollider2.IsEnabled()) {
        mMovingMeshCollider2.Disable();
    }
    daKpa2Bg_c_ModelFile.Release();
    daKpa2Bg_c_ClsnFile.Release();
    return 1;
}

s32 daKpa2Bg_c::Render() {
    mModel2.Render(0);
    return 1;
}

s32 daKpa2Bg_c::Behavior() {
    mAngle.x += mAngleXSpeed;
    mAngle.y += mAngleYSpeed;
    mAngle.z += mAngleZSpeed;

    UpdateModelMatrix();
    UpdateClsnMatrix();
    return 1;
}

int daKpa2Bg_c::InitResources() {
    mModel2.SetFile(dExtSimpleModel_c::LoadFile(daKpa2Bg_c_ModelFile), 1, -1);
    UpdateClsnMatrix();
    mMovingMeshCollider2.SetFile(dBgW_Kc::LoadFile(daKpa2Bg_c_ClsnFile), mClsnMat, 0x1000, mAngle.y,
                                 *(&data_ov046_021115bc));
    mMovingMeshCollider2.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);
    mMovingMeshCollider2.Enable(this);
    mAngleXSpeed = 0;
    mAngleYSpeed = 0;
    mAngleZSpeed = 0;
    mSoundHandle = 0;
    return 1;
}

/* `new daKpa2Bg_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x570), dBgActor_c's base constructor, the vptr store,
 * then the dExtSimpleModel_c and dBgW_KcMbg member constructors. */
daKpa2Bg_c *daKpa2Bg_c_classInit(void) {
    return new daKpa2Bg_c;
}

/* The KOOPA2BG model and collision file handles; the extern block above
 * declares them for CleanupResources / InitResources. */
Kpa2BgModelFilePtr daKpa2Bg_c_ModelFile(ASSET_KB2_STAGE_BMD);
Kpa2BgCollisionFilePtr daKpa2Bg_c_ClsnFile(ASSET_KB2_STAGE_KCL);
