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

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" BCA_File *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *f, int a, int b, unsigned int c);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *a, int b, int c, unsigned int d, unsigned int e);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *a, int b, int c, Vector3_16 *d, Vector3_16 *e);
extern "C" int func_ov077_02125e94(void *c, int a, int b);

extern SharedFilePtr data_ov077_02127b48;
extern SharedFilePtr data_ov077_02127b38;
extern SharedFilePtr data_ov077_02127c14;
extern char IDENTITY_MATRIX4X3;


int Spiny::InitResources()
{
    BMD_File *bmd;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b48);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, bmd, 1, -1);
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x124, bmd, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov077_02127c14);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char *)this) + 0x124, *(BCA_File **)((char *)&data_ov077_02127c14 + 4), 0, 0x1000, 0);
    if (!_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel))
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x1b0, (dActor_c *)((char *)this), 0x2d000, 0x3c000, 0x200000, 0x4a3d0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x1e4, (dActor_c *)((char *)this), 0x2d000, 0, (Vector3_16 *)((char *)&mPrevAngleX), (Vector3_16 *)((char *)&mAngleX));
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mDespawnTimer = 0x2c;
    func_ov077_02125e94(((char *)this), 0, 0x2c);
    *(Matrix4x3 *)((char *)&mMatrix) = *(Matrix4x3 *)&IDENTITY_MATRIX4X3;
    func_ov077_02125304(((char *)this));
    return 1;
}
