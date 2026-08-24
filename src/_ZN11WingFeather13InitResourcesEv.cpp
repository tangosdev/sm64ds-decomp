//cpp
// @symbol _ZN11WingFeather13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WingFeather.h"
struct SharedFilePtr;
struct BMD_File;
struct dActor_c;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *a, int b, int c, unsigned int d, unsigned int e);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *a, int b, int c, void *d, int e);

extern SharedFilePtr data_ov002_0210da58;
extern unsigned char data_0209f2d8;

struct Sub { char pad[0x7c]; short f7c; };
extern char *data_0209f318;

int WingFeather::InitResources()
{
    BMD_File *bmd;
    int b;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da58);
    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, bmd, 1, 1))
        return 0;
    if (!_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel))
        return 0;

    mVertAccel = -0x199;
    mTerminalVelocity = -0x28000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x124, (dActor_c *)((char *)this), 0x6e000, 0x6e000, 0x100002, 0);
    func_ov002_020b2c44(((char *)this));
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x158, (dActor_c *)((char *)this), 0x28000, 0xa000, 0, 0);

    mAngleX = 0x4000;
    mAngleZ = -0x4000;
    b = (data_0209f2d8 == 1);
    if (!b) {
        mAngleY = ((Sub *)(data_0209f318 + 0x100))->f7c + 0x4000;
    }
    unk_384 = 0xb4;
    mVertSpeed = 0x3000;
    return 1;
}
