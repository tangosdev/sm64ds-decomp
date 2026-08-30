//cpp
// @symbol _ZN16daObjPushblock_c13InitResourcesEv
/* recovered: vtable identified, shared common types, declarations from a shared header
 *
 * daObjPushblock_c::InitResources -- vtable slot 0, ov002 0x020b8fe0. Attributed by
 * the vtable: _ZTV16daObjPushblock_c (ov002 0x021096b0) carries this address at slot
 * 0, where _ZTV10dBgActor_c carries fBase_c's generic 0x02043c80.
 *
 * A real method over the shared header. It used to be an extern "C" free
 * function over a raw `char *self` reading every field by hand offset; the
 * conversion is byte-exact under the pinned 2004/b56. */
#include "daObjPushblock_c.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"
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

extern SharedFilePtr data_ov002_0210df9c;
extern SharedFilePtr data_ov002_0210df94;

int daObjPushblock_c::InitResources()
{
    Vector3 v;
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
    dBgCh_Gnd ground;
    ground.SetObjAndPos(v, 0);
    mGroundY = v.y;
    if (ground.DetectClsn())
        mGroundY = ground.clsnY;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    return 1;
}
