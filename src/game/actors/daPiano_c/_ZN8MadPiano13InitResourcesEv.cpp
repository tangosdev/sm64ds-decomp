//cpp
// @symbol _ZN8MadPiano13InitResourcesEv
/* Load the piano's three shared assets, initialize its typed render/collision
 * members, and snapshot its home position. Fix12-by-value APIs keep their
 * explicit ABI declarations because 2004/b56 homes the member form differently. */
#include "MadPiano.h"
#include "MadPianoResources.h"
#include "SharedFilePtr.h"

extern SharedFilePtr gPianoModelFile;
extern SharedFilePtr gPianoCollisionFile;
extern SharedFilePtr gPianoAttackAnimationFile;
extern char data_ov063_0211ecb8;

extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *, void *, int, int, void *, int);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *, void *, void *, int, int, unsigned int, unsigned int);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, void *, int, short, void *);
extern void func_ov063_0211d88c(MadPiano *self);
extern void func_ov063_0211d828(MadPiano *self);
extern void func_ov063_0211d5f4(MadPiano *self);
}


int MadPiano::InitResources()
{
    int i;
    dCcAcPos_c *cylinder;
    void *f;

    f = Model::LoadFile(gPianoModelFile);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    mShadowModel1.InitCuboid();
    mShadowModel2.InitCuboid();
    mShadowModel3.InitCylinder();
    f = Animation::LoadFile(gPianoAttackAnimationFile);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, f, 0, 0x1000, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x159000, 0x159000, 0, 0);
    for (i = 0, cylinder = mCylinderClsn; i < 2; i++) {
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            cylinder, this, &mPosX, 0x9b000, 0xfa000, 0x200004, 0);
        cylinder++;
    }
    f = dBgW_Kc::LoadFile(gPianoCollisionFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, f, &mClsnMat, 0x199, mAngleY, &data_ov063_0211ecb8);
    func_ov063_0211d88c(this);
    func_ov063_0211d828(this);
    func_ov063_0211d5f4(this);
    return 1;
}
