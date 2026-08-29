//cpp
// @symbol _ZN10daPgMthr_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daPgMthr_c.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *act, int a, int b, unsigned int c2, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, int a, int b, void *c2, void *d);
extern void func_ov018_02111d28(char *c, int r1);
extern int data_ov018_02113c00[];
extern int data_ov018_02112c04[];
}

int daPgMthr_c::InitResources()
{
    void *m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov018_02113c00);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, m, 1, 1);
    for (int i = 0; i < 2; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov018_02112c0c[i]);
    for (int i = 0; i < 2; i++) {
        void *t = (void*)data_ov018_02112c04[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(t);
        TextureSequence::Prepare(*(BMD_File *)data_ov018_02113c00[1], *(BTP_File *)((int*)t)[1]);
    }
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0) return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x104000, 0x12c000, 0x4800004, 0x900000);
    func_ov018_021123d0((char *)this, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_374 = 0;
    func_ov018_02111d28((char *)this, 0);
    return 1;
}
