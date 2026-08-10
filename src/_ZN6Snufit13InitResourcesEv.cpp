//cpp
// @symbol _ZN6Snufit13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * The stand-in typedefs this file used to carry (`typedef int Fix12;`, a
 * two-word SharedFilePtr, a three-short Vector3_16, opaque Actor/BMD_File)
 * all name real types now that Snufit.h pulls in the Enemy chain, so they are
 * gone. MovingCylinderClsn::Init and WithMeshClsn::Init are still reached
 * through their mangled names because neither is declared as a method yet.
 */
#include "Snufit.h"
#include "SharedFilePtr.h"

struct PMF;

extern SharedFilePtr data_ov065_0211d618;
extern SharedFilePtr data_ov065_0211d610;
extern SharedFilePtr data_ov065_0211d600;
extern SharedFilePtr data_ov065_0211d608;
extern PMF data_ov065_0211d670;

extern "C" {
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, Actor* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, int r, int h, Vector3_16* p, Vector3_16* q);
extern int func_ov065_0211691c(void* c, PMF* p);
}

int Snufit::InitResources()
{
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov065_0211d618), 1, -1);
    Model::LoadFile(data_ov065_0211d610);
    mShadowModel.InitCylinder();
    Animation::LoadFile(data_ov065_0211d600);
    Animation::LoadFile(data_ov065_0211d608);
    mTerminalVelocity = -0x1e000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(&mMovingCylinderClsn, this, 0x38000, 0x7e000, 0x200000, 0x7eff0);
    mAngleY = mPrevAngleY;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x46000, 0, 0, 0);
    unk_108 = 1;
    unk_10a = 1;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mModelAnim.speed = 0x1000;
    func_ov065_0211691c(this, &data_ov065_0211d670);
    return 1;
}
