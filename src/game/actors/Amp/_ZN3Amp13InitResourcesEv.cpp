//cpp
// @symbol _ZN3Amp13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "common.h"
#include "Amp.h"
#include "TextureSequence.h"
struct SharedFilePtr;
struct BMD_File;
struct BTA_File;
struct dActor_c;
struct Vector3;
struct Vector3_16;

extern "C" void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(dActor_c *self, int a, int b, int c, int d);
extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, dActor_c *a, Vector3 const &b, int c, int d, unsigned int e, unsigned int f);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *a, int b, int c, Vector3_16 *d, Vector3_16 *e);
extern "C" int func_ov070_02120da8(void *c, int a);
extern "C" void func_ov070_02120724(void *c);

extern SharedFilePtr data_ov070_021235fc;
extern SharedFilePtr data_ov070_02123604;
extern SharedFilePtr *data_ov070_021222e0[];
extern SharedFilePtr data_ov070_021235ec;
extern BTA_File data_ov070_021231f4;
extern Vector3 data_ov070_0212365c;
extern char IDENTITY_MATRIX4X3;


int Amp::InitResources()
{
    BMD_File *bmd;
    bmd = (BMD_File *)Model::LoadFile(data_ov070_021235fc);
    mModelAnim.SetFile(bmd, 1, 1);
    bmd = (BMD_File *)Model::LoadFile(data_ov070_02123604);
    mModel.SetFile(bmd, 1, 1);

    int i;
    for (i = 0; i < 2; i++) {
        Animation::LoadFile(*data_ov070_021222e0[i]);
    }

    BMD_File *bmd2 = *(BMD_File **)((char *)&data_ov070_02123604 + 4);
    BTP_File *btp = (BTP_File *)TextureSequence::LoadFile(data_ov070_021235ec);
    TextureSequence::Prepare(*bmd2, *btp);

    bmd2 = *(BMD_File **)((char *)&data_ov070_02123604 + 4);
    TextureTransformer::Prepare(*bmd2, data_ov070_021231f4);

    if (!mShadowModel.InitCylinder())
        return 0;

    if ((unsigned char)((param1 >> 1) & 1)) {
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0, 0x20d000, 0x1000000, 0xa28000);
    } else {
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0, 0x2c1000, 0x1000000, 0xa28000);
    }

    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, data_ov070_0212365c, 0x2d000, 0x50000, 0x200002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x2d000, 0x2d000, 0, 0);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    func_ov070_02120da8(this, 1);

    *(Matrix4x3 *)&mMat4x3 = *(Matrix4x3 *)&IDENTITY_MATRIX4X3;

    func_ov070_02120724(this);
    return 1;
}
