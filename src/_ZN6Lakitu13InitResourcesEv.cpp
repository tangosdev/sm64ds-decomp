//cpp
// @symbol _ZN6Lakitu13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Lakitu.h"
#include "TextureSequence.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *a, void *v, int b, int c, unsigned int d, unsigned int e);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *a, int b, int c, void *d, void *e);
extern void func_ov077_02123d40(void *c);
extern int data_ov077_02127b38[];
extern int data_ov077_02127b48[];
extern int data_ov077_02127b88[];
struct M48 { int w[12]; };
extern M48 data_02082128;
}

int Lakitu::InitResources()
{
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b50), 1, 1);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x138, _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b48), 1, 1);
    for (int i = 0; i < 2; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void *)data_ov077_02127238[i]);
    for (int i = 0; i < 2; i++) {
        void *t = (void *)data_ov077_02127230[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(t);
        TextureSequence::Prepare(*(BMD_File *)data_ov077_02127b50[1], *(BTP_File *)((int *)t)[1]);
    }
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(((char *)this) + 0x1c4, ((char *)this), data_ov077_02127b88, 0x41000, 0x78000, 0x200002, 0x6eff0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x204, ((char *)this), 0x2d000, 0x2d000, 0, 0);

    *(int *)(((int)((char *)&unk_09c)) & 0xFFFFFFFFFFFFFFFFLL) = 0;
    *(int *)(((int)((char *)&unk_0a0)) & 0xFFFFFFFFFFFFFFFFLL) = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    *(int *)(((int)((char *)&unk_3f8)) & 0xFFFFFFFFFFFFFFFFLL) = *(int *)(((int)((char *)&unk_05c)) & 0xFFFFFFFFFFFFFFFFLL);
    *(int *)(((int)((char *)&unk_3fc)) & 0xFFFFFFFFFFFFFFFFLL) = *(int *)(((int)((char *)&unk_060)) & 0xFFFFFFFFFFFFFFFFLL);
    unk_400 = *(int *)(((int)((char *)&unk_064)) & 0xFFFFFFFFFFFFFFFFLL);
    unk_410 = 0;

    func_ov077_0212478c(((char *)this));
    *(M48 *)((char *)&unk_3c0) = data_02082128;
    func_ov077_02123d40(((char *)this));
    return 1;
}
