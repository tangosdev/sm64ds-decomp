//cpp
/**
 * Whomp's Fortress spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers, plus a starting angle at data_ov015_02114794.
 * func_ov002_020b676c types the slots as SharedFilePtr *m[2]
 * and CLPS_Block *clps.
 *
 * daObjBk_Ukisima_c_classInit is reconstructed (RTTI daObjBk_Ukisima_c,
 * BK_UKISIMA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b676c / func_ov002_020b66a8 are still the
 *   linker names of daObjKaitendai_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov002.
 * Leftover: the start-angle halfword is still data_ov015_02114794.
 * Leftover: g_profile_BK_UKISIMA lives outside this TU (S14).
 */

#include "daObjBk_Ukisima_c.h"
#include "SharedFilePtr.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4. The collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. The descriptor stays ROM data. */
struct UkisimaModelFilePtr : SharedFilePtr {
    unsigned int words[2];
    UkisimaModelFilePtr(unsigned int fileId);
    ~UkisimaModelFilePtr();
};
struct UkisimaClsnFileHandle : SharedFilePtr {
    unsigned int words[2];
    UkisimaClsnFileHandle(unsigned int fileId);
    ~UkisimaClsnFileHandle();
};
typedef char UkisimaModelFilePtr_size_must_be_8[
    sizeof(UkisimaModelFilePtr) == 8 ? 1 : -1];
typedef char UkisimaClsnFileHandle_size_must_be_8[
    sizeof(UkisimaClsnFileHandle) == 8 ? 1 : -1];

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
int func_ov002_020b676c(daObjKaitendai_c *self, ResourceDescriptor *descriptor,
                        s16 angle);
int func_ov002_020b66a8(daObjKaitendai_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov015_021147a4;
extern s16 data_ov015_02114794;
}

// @symbol daObjBk_Ukisima_c_classInit
extern "C" daObjBk_Ukisima_c *daObjBk_Ukisima_c_classInit()
{
    return new daObjBk_Ukisima_c();
}

// @symbol _ZN17daObjBk_Ukisima_c13InitResourcesEv
s32 daObjBk_Ukisima_c::InitResources()
{
    return func_ov002_020b676c(this, &data_ov015_021147a4, data_ov015_02114794);
}

// @symbol _ZN17daObjBk_Ukisima_c16CleanupResourcesEv
s32 daObjBk_Ukisima_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov015_021147a4);
}

/* Retail construction order. mwcc emits __sinit_d_a_obj_bk_ukisima.cpp. */
UkisimaModelFilePtr data_ov015_02114aac(1441);
UkisimaClsnFileHandle data_ov015_02114ab4(1442);
