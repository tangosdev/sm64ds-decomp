//cpp
/**
 * Wet-Dry World's spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers, plus a starting angle at data_ov029_02113fc4.
 * func_ov002_020b676c types the slots as SharedFilePtr *m[2]
 * and CLPS_Block *clps.
 *
 * daObjWc_Obj07_c_classInit is reconstructed (RTTI daObjWc_Obj07_c,
 * WC_UKISIMA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b676c / func_ov002_020b66a8 are still the
 *   linker names of daObjKaitendai_c Init/Cleanup (the base leaves
 *   those slots pure virtual). Naming belongs in ov002.
 * Leftover: the BMD/KCL SharedFilePtrs and CLPS_Block are still
 *   data_ov029_*. The start-angle halfword 02113fc4 sits 4 bytes
 *   before this class's typeinfo at 02113fc8; it is not owned here.
 * Leftover: g_profile_WC_UKISIMA lives outside this TU (S14).
 */

#include "daObjWc_Obj07_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
int func_ov002_020b676c(daObjWc_Obj07_c *self, ResourceDescriptor *descriptor,
                        s16 angle);
int func_ov002_020b66a8(daObjWc_Obj07_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov029_02113fd4;
extern s16 data_ov029_02113fc4;
}

// @symbol daObjWc_Obj07_c_classInit
extern "C" daObjWc_Obj07_c *daObjWc_Obj07_c_classInit()
{
    return new daObjWc_Obj07_c();
}

// @symbol _ZN15daObjWc_Obj07_c13InitResourcesEv
s32 daObjWc_Obj07_c::InitResources()
{
    return func_ov002_020b676c(this, &data_ov029_02113fd4, data_ov029_02113fc4);
}

// @symbol _ZN15daObjWc_Obj07_c16CleanupResourcesEv
s32 daObjWc_Obj07_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov029_02113fd4);
}
