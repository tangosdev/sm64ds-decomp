//cpp
// @symbol _ZN10RockPillar13InitResourcesEv
#include "decl_common.h"
#include "daObjKi_Hasira_c.h"
#include "RockPillar.h"

/* RockPillar::InitResources -- vtable slot 0, ov016 0x02112e1c.
 *
 * Minimal-diff member-function migration: the pre-migration recovery's
 * `char *c` self pointer becomes `this`, unchanged otherwise -- the shadow
 * struct daObjKi_Hasira_c (RTTI class name, include/daObjKi_Hasira_c.h)
 * still supplies field offsets/names exactly as the pre-migration body used
 * them, since RockPillar.h itself only declares the class's own 0x320..
 * 0x327 span as opaque padding. The two inherited fields read here, mAngleY
 * and mAreaId, are dActor_c's and now say so in the shadow too -- this
 * comment used to be where that was written down. */
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void func_ov016_02112a9c(char* t);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, void*, void*, int, int);
}

int RockPillar::InitResources()
{
    char *c = (char *)this;
    struct daObjKi_Hasira_c *self = (struct daObjKi_Hasira_c *)(void *)c;
    void* mdl;
    void* kcl;
    mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_02114e24);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, mdl, 1, -1);
    func_ov016_02112a9c(c);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov016_02114e1c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, kcl, c+0x2ec, 0x199, self->mAngleY, data_ov016_02113cac);
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x3b, 0, c+0x5c, 0, self->mAreaId, -1);
    return 1;
}
