//cpp
// @symbol _ZN11BabyPenguin13InitResourcesEv
#include "BabyPenguin.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov072_02122cb4;
extern SharedFilePtr *data_ov072_02122004[];

/* The collision Init symbols are measured mwccarm 6az walls. Spelling
 * dCcAc_c::Init with its honest Fix12<int> values homes both wrappers and
 * grows this function by 0x10. dBgCh_Actr's current scalar compatibility
 * declaration instead mangles `ii`, while the ROM destination spells
 * `5Fix12IiES3_`. Keep these two ABI calls until those compiler/signature
 * levers are recovered; the remaining class APIs below are compiler-spelled. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *thiz, void *actor, int r, int h, unsigned int a, unsigned int b);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int r, int h, void *v, int b);
extern "C" void func_ov072_021210c4(void *c);
extern "C" void func_ov072_02121d50(void *c);

int BabyPenguin::InitResources()
{
    BMD_File *f = (BMD_File *)Model::LoadFile(data_ov072_02122cb4);
    mModelAnim.SetFile(f, 1, -1);
    int i;
    for (i = 0; i < 5; i++) {
        Animation::LoadFile(*data_ov072_02122004[i]);
    }
    if (mShadowModel.InitCylinder() == 0) return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x28000, 0x50000, 0x800004, 0x9000);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mScaleX = 0x400;
    mScaleY = 0x400;
    mScaleZ = 0x400;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    mEatingPlayer = 0;
    unk_360 = 0;
    func_ov072_02121d50(((char *)this));
    func_ov072_021210c4(((char *)this));
    return 1;
}
