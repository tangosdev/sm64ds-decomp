//cpp
// @symbol _ZN9PushBlock13InitResourcesEv
/* recovered: vtable identified, shared common types, declarations from a shared header
 *
 * PushBlock::InitResources -- vtable slot 0, ov002 0x020b8fe0. Attributed by
 * the vtable: _ZTV9PushBlock (ov002 0x021096b0) carries this address at slot
 * 0, where _ZTV10dBgActor_c carries fBase_c's generic 0x02043c80.
 *
 * A real method over the shared header. It used to be an extern "C" free
 * function over a raw `char *self` reading every field by hand offset; the
 * conversion is byte-exact under the pinned 2004/b56. */
#include "PushBlock.h"
#include "decl_common.h"
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern "C" KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *k, void *m, int fix, short s, void *clps);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *a, int b, int c, void *d, int e);

struct V3 { int x, y, z; };
struct dBgCh_Gnd { char buf[0x44]; int f44; char rest[8]; };
extern "C" void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd *self);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *self, V3 *v, void *a);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);
extern "C" void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *self);

extern SharedFilePtr data_ov002_0210df9c;
extern SharedFilePtr data_ov002_0210df94;

int PushBlock::InitResources()
{
    dBgCh_Gnd rg;
    V3 v;
    BMD_File *bmd;
    KCL_File *kcl;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210df9c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, bmd, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov002_0210df94);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, &data_ov002_0210d7b4);

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x96000, 0x96000, 0, 0);

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y + 0x14000;
    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, 0);
    mGroundY = v.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg))
        mGroundY = rg.f44;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    _ZN9dBgCh_GndD1Ev(&rg);
    return 1;
}
