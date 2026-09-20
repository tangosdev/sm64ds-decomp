//cpp
// @symbol _ZN5Spiny13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Spiny.h"
struct SharedFilePtr;
struct BMD_File;
struct BCA_File;
struct dActor_c;
struct Vector3_16;

/* The scalar definition takes a 16-bit start frame; the shared native
   declaration still takes u32. Keep its measured ABI bridge here. */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *self, BCA_File *f, int a, int b, u16 startFrame);
/* A native call with aggregate Fix12 arguments grows this function by 16 bytes. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c *self, dActor_c *a, int b, int c, unsigned int d, unsigned int e);
/* The production mesh initializer takes actor/rotation addresses as int.
   Its native header spells a different symbol; retain that scalar contract. */
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(dBgCh_Actr *self, int actor, int radius, int height, int first, int second);
extern "C" void func_ov077_02125e94(char *c, int state);

extern SharedFilePtr data_ov077_02127b48;
extern SharedFilePtr data_ov077_02127b38;
extern SharedFilePtr data_ov077_02127c14;
extern char IDENTITY_MATRIX4X3;


int Spiny::InitResources()
{
    BMD_File *bmd;
    bmd = (BMD_File *)Model::LoadFile(data_ov077_02127b48);
    mModel.SetFile(bmd, 1, -1);
    bmd = (BMD_File *)Model::LoadFile(data_ov077_02127b38);
    mModelAnim.SetFile(bmd, 1, -1);
    Animation::LoadFile(data_ov077_02127c14);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(BCA_File **)((char *)&data_ov077_02127c14 + 4), 0, 0x1000, 0);
    if (!mShadowModel.InitCylinder())
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x2d000, 0x3c000, 0x200000, 0x4a3d0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (int)this, 0x2d000, 0, (int)&mPrevAngleX, (int)&mAngleX);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mDespawnTimer = 0x2c;
    func_ov077_02125e94((char *)this, 0);
    *(Matrix4x3 *)((char *)&mMatrix) = *(Matrix4x3 *)&IDENTITY_MATRIX4X3;
    func_ov077_02125304(((char *)this));
    return 1;
}
