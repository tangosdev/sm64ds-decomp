//cpp
// @symbol _ZN11SnowmanHead13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
#include "TextureSequence.h"
/* recovered: named members + shared header, real C++ method */
#include "SnowmanHead.h"
#include "dBgCh_Gnd.h"

struct BMD_File;
struct BTP_File;

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *ref);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *base, void *file, int a, int b);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *ref);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *t, void *a, int b, int c, unsigned int d, unsigned int e);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *t, void *a, int b, int c, void *d, void *e);

}

int SnowmanHead::InitResources()
{
    struct Vector3 pos;
    int i;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov072_02122bc4), 1, -1);

    for (i = 0; i < 2; i++) {
        void *tex = (&data_ov072_02121ffc)[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(tex);
        TextureSequence::Prepare(*(BMD_File *)((void **)&data_ov072_02122bc4)[1],
                                 *(BTP_File *)((void **)tex)[1]);
    }

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x138, ((char *)this), 0x96000, 0x12c000, 0x800004, 0);
    mScaleX = 0x1800;
    mScaleY = 0x1800;
    mScaleZ = 0x1800;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x16c, ((char *)this), 0x96000, 0x96000, 0, 0);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn() != 0)
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;

    SetState(0);
    UpdateModel();
    return 1;
}
