//cpp
// @symbol _ZN10Scuttlebug13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Scuttlebug.h"
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
struct BMD_File; struct BCA_File; struct Vector3_16;
/* ModelBase, ShadowModel and dBgCh_Actr are the real classes now, through
   this actor's header (Scuttlebug.h). */

/* Declared by final name, not as members: both take Fix12<int> where these calls pass
   int literals, and Fix12<int> is an aggregate with no converting constructor from int.
   dBgCh_Actr::Init's last parameter is a Vector3_16* as well -- the S5_ in
   ...P10Vector3_16S5_ back-references the pointer type, it is not an int. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, dActor_c *a, int b, int c, unsigned int d, unsigned int e);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, dActor_c *a, int b, int c, Vector3_16 *d, Vector3_16 *e);
extern "C" char data_ov071_02122f88;
struct Blob48 { int w[12]; };
extern "C" Blob48 IDENTITY_MATRIX4X3;
extern "C" void Scuttlebug_SetState(dActor_c *self, int type);
extern "C" int func_ov071_0211f524(char *c);

int Scuttlebug::InitResources()
{
    char *s = (char*)((dActor_c *)this);
    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov071_02122f80);
    ((ModelBase*)(s + 0xd4))->SetFile((BMD_File*)mf, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov071_02122f88);
    if (((ShadowModel*)(s + 0x138))->InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        s + 0x160, ((dActor_c *)this), 0x46000, 0x64000, 0x200000, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        s + 0x194, ((dActor_c *)this), 0x50000, 0x50000, (Vector3_16*)0, (Vector3_16*)0);
    ((dBgCh_Actr*)(s + 0x194))->StartDetectingWater();
    *(int*)(s + 0x384) = *(int*)(s + 0x5c);
    *(int*)(s + 0x388) = *(int*)(s + 0x60);
    *(int*)(s + 0x38c) = *(int*)(s + 0x64);
    *(short*)(s + 0x3a6) = *(short*)(s + 0x8e);
    *(int*)(s + 0x390) = *(int*)(s + 0x5c);
    *(int*)(s + 0x394) = *(int*)(s + 0x60);
    *(int*)(s + 0x398) = *(int*)(s + 0x64);
    if (*(int*)(s + 8) != 0)
        Scuttlebug_SetState(((dActor_c *)this), 0);
    else
        Scuttlebug_SetState(((dActor_c *)this), 2);
    *(char*)(s + 0x3aa) = 3;
    *(int*)(s + 0x9c) = -0x2000;
    *(int*)(s + 0xa0) = -0x3c000;
    *(int*)(s + 0x80) = 0x1000;
    *(int*)(s + 0x84) = 0x1000;
    *(int*)(s + 0x88) = 0x1000;
    *(int*)(s + 0xd0) = 0;
    *(short*)(s + 0x3a8) = 0x3c;
    *(Blob48*)(s + 0x350) = IDENTITY_MATRIX4X3;
    func_ov071_0211f524(s);
    return 1;
}
