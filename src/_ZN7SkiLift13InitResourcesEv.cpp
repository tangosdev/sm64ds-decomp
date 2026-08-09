//cpp
// @symbol _ZN7SkiLift13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SkiLift.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *act, int a, int b, unsigned int c2, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, int a, int b, void *c2, void *d);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *act);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void func_ov018_02111d28(char *c, int r1);
extern void _ZN13RaycastGroundD1Ev(void *self);
extern int data_ov018_02113c00[];
extern int data_ov018_02112c04[];
}

int SkiLift::InitResources()
{
    void *m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov018_02113c00);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this)+0xd4, m, 1, 1);
    for (int i = 0; i < 2; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov018_02112c0c[i]);
    for (int i = 0; i < 2; i++) {
        void *t = (void*)data_ov018_02112c04[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(t);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)data_ov018_02113c00[1], (void*)((int*)t)[1]);
    }
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0) return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this)+0x174, ((char *)this), 0x104000, 0x12c000, 0x4800004, 0x900000);
    func_ov018_021123d0((char *)this, 0);
    unk_09c = -0x2000;
    unk_0a0 = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this)+0x1a8, ((char *)this), 0x32000, 0x32000, 0, 0);
    char rg[0x54];
    int v[3];
    v[0] = mPosX;
    v[1] = mPosY;
    v[2] = mPosZ;
    v[1] += 0x14000;
    _ZN13RaycastGroundC1Ev(rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, v, 0);
    if (_ZN13RaycastGround10DetectClsnEv(rg))
        mPosY = *(int*)(rg+0x44);
    else
        mPosY = v[1];
    unk_364 = mPosX;
    unk_368 = mPosY;
    unk_36c = mPosZ;
    unk_374 = 0;
    func_ov018_02111d28(((char *)this), 0);
    _ZN13RaycastGroundD1Ev(rg);
    return 1;
}
