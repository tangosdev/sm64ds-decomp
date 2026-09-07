//cpp
// @symbol _ZN13PrincessPeach13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "PrincessPeach.h"
extern "C" char data_ov085_021304f4;
extern "C" void *data_ov085_0212f280[];
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
struct BMD_File; struct dActor_c; struct Vector3_16;
/* ModelBase and ShadowModel are the real classes now, through PrincessPeach.h. */
/* Declared by final name, not as members: both take Fix12<int> where these calls pass
   int literals, and Fix12<int> is an aggregate with no converting constructor from int.
   dBgCh_Actr::Init's last parameter is a Vector3_16* as well -- the S5_ in
   ...P10Vector3_16S5_ back-references the pointer type, it is not an int. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, dActor_c *a, int b, int c, unsigned int d, unsigned int e);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, dActor_c *a, int b, int c, Vector3_16 *d, Vector3_16 *e);

int PrincessPeach::InitResources()
{
    char *s = (char*)((dActor_c *)this);
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_021304f4);
    ((ModelBase*)(s + 0xd4))->SetFile((BMD_File*)f, 1, -1);
    for (int i = 0; i < 7; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov085_0212f280[i]);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        s + 0x160, ((dActor_c *)this), 0x90000, 0xc0000, 0x4800004, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        s + 0x194, ((dActor_c *)this), 0x40000, 0x40000, (Vector3_16*)0, (Vector3_16*)0);
    *(int*)(s + 0x9c) = -0x2000;
    *(int*)(s + 0xa0) = -0x3c000;
    *(int*)(s + 0x80) = 0x1000;
    *(int*)(s + 0x84) = 0x1000;
    *(int*)(s + 0x88) = 0x1000;
    SetState(0);
    UpdateModel();
    return 1;
}
