//cpp
// @symbol _ZN9Spindrift13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Spindrift.h"
struct BMD_File; struct BCA_File; struct dActor_c; struct Vector3_16;

/* Spell the ROM's own symbols. This file used local stand-in classes whose method
   calls mangled to names that exist nowhere -- _ZN7dCcAc_c4InitEP8dActor_ciijj
   and friends, five of the tree's unresolved references. match.py never noticed
   because it wildcards every relocated word; only check_references sees it. These are
   the real symbols, so the calls now resolve and the bytes are unchanged. */
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *ref);
extern "C" void *_ZN9Animation8LoadFileER13SharedFilePtr(void *ref);
extern "C" int  _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int b, int c);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *f, int b, int c, unsigned int d);
extern "C" int  _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *a, int r, int h, unsigned int d, unsigned int e);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *a, int r, int h, void *v, void *w);
extern "C" void _ZN10dBgCh_Actr19StartDetectingWaterEv(void *self);

int Spindrift::InitResources()
{
    char *s = (char*)((dActor_c *)this);
    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128d60);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(s + 0x110, (BMD_File*)mf, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov081_02128d68);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(s + 0x110, (BCA_File*)data_ov081_02128d68[1], 0, 0x1000, 0);
    if (_ZN11ShadowModel12InitCylinderEv(s + 0x174) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(s + 0x19c, ((dActor_c *)this), 0x3c000, 0x78000, 0x200000, 0xa6efe0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(s + 0x1d0, ((dActor_c *)this), 0x3c000, 0x3c000, 0, 0);
    _ZN10dBgCh_Actr19StartDetectingWaterEv(s + 0x1d0);
    *(int*)(s + 0x38c) = *(int*)(s + 0x5c);
    *(int*)(s + 0x390) = *(int*)(s + 0x60);
    *(int*)(s + 0x394) = *(int*)(s + 0x64);
    *(char*)(s + 0x39a) = 0;
    *(int*)(s + 0x9c) = -0x2000;
    *(int*)(s + 0xa0) = -0x3c000;
    return 1;
}
