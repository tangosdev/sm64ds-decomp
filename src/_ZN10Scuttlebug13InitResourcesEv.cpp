//cpp
// @symbol _ZN10Scuttlebug13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Scuttlebug.h"
struct BMD_File; struct BCA_File; struct Actor; struct Vector3_16;
struct ModelBase { void SetFile(BMD_File *f, int b, int c); };
struct ShadowModel { int InitCylinder(); };
struct MovingCylinderClsn { void Init(Actor *a, int b, int c, unsigned int d, unsigned int e); };
struct WithMeshClsn { void Init(Actor *a, int b, int c, void *d, int e); void StartDetectingWater(); };
extern "C" char data_ov071_02122f88;
struct Blob48 { int w[12]; };
extern "C" Blob48 data_02082128;
extern "C" void Scuttlebug_SetState(Actor *self, int type);
extern "C" int func_ov071_0211f524(char *c);

int Scuttlebug::InitResources()
{
    char *s = (char*)((Actor *)this);
    void *mf = ModelLoadFile(&data_ov071_02122f80);
    ((ModelBase*)(s + 0xd4))->SetFile((BMD_File*)mf, 1, -1);
    AnimLoadFile(&data_ov071_02122f88);
    if (((ShadowModel*)(s + 0x138))->InitCylinder() == 0)
        return 0;
    ((MovingCylinderClsn*)(s + 0x160))->Init(((Actor *)this), 0x46000, 0x64000, 0x200000, 0x6eff0);
    ((WithMeshClsn*)(s + 0x194))->Init(((Actor *)this), 0x50000, 0x50000, 0, 0);
    ((WithMeshClsn*)(s + 0x194))->StartDetectingWater();
    *(int*)(s + 0x384) = *(int*)(s + 0x5c);
    *(int*)(s + 0x388) = *(int*)(s + 0x60);
    *(int*)(s + 0x38c) = *(int*)(s + 0x64);
    *(short*)(s + 0x3a6) = *(short*)(s + 0x8e);
    *(int*)(s + 0x390) = *(int*)(s + 0x5c);
    *(int*)(s + 0x394) = *(int*)(s + 0x60);
    *(int*)(s + 0x398) = *(int*)(s + 0x64);
    if (*(int*)(s + 8) != 0)
        Scuttlebug_SetState(((Actor *)this), 0);
    else
        Scuttlebug_SetState(((Actor *)this), 2);
    *(char*)(s + 0x3aa) = 3;
    *(int*)(s + 0x9c) = -0x2000;
    *(int*)(s + 0xa0) = -0x3c000;
    *(int*)(s + 0x80) = 0x1000;
    *(int*)(s + 0x84) = 0x1000;
    *(int*)(s + 0x88) = 0x1000;
    *(int*)(s + 0xd0) = 0;
    *(short*)(s + 0x3a8) = 0x3c;
    *(Blob48*)(s + 0x350) = data_02082128;
    func_ov071_0211f524(s);
    return 1;
}
