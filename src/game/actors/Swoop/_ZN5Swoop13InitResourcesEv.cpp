//cpp
// @symbol _ZN5Swoop13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * The stand-in typedefs this file used to carry (`typedef int Fix12;`, a
 * two-word SharedFilePtr, a three-short Vector3_16, opaque dActor_c/BMD_File)
 * all name real types now that Swoop.h pulls in the dEnemyBase_c chain, so they are
 * gone. Both ModelAnims are loaded here, which is the other half of the
 * evidence that 0x300 and 0x364 are two members and not one and a gap.
 *
 * dCcAc_c::Init and dBgCh_Actr::Init are still reached through
 * their mangled names because neither is declared as a method yet.
 */
#include "Swoop.h"
#include "SharedFilePtr.h"

struct PMF;

extern SharedFilePtr data_ov065_0211d698;
extern SharedFilePtr data_ov065_0211d6a8;
extern SharedFilePtr data_ov065_0211d690;
extern SharedFilePtr data_ov065_0211d6a0;
extern PMF data_ov065_0211d700;

extern "C" {
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, Vector3_16* q);
extern int func_ov065_02117944(void* c, PMF* p);
}

int Swoop::InitResources()
{
    mModelAnim1.SetFile((BMD_File *)Model::LoadFile(data_ov065_0211d698), 1, -1);
    mModelAnim2.SetFile((BMD_File *)Model::LoadFile(data_ov065_0211d6a8), 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(data_ov065_0211d690);
    Animation::LoadFile(data_ov065_0211d6a0);
    mTerminalVelocity = -0xa000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x28000, 0x28000, 0x200000, 0x7eff0);
    mAngleY = mPrevAngleY;
    mPrevAngleX = -0x8000;
    mAngleX = mPrevAngleX;
    unk_43c = 0;
    mModelAnim1.speed = 0x1000;
    mModelAnim2.speed = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x28000, 0x1e000, 0, 0);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_108 = 1;
    func_ov065_02117944(this, &data_ov065_0211d700);
    return 1;
}
