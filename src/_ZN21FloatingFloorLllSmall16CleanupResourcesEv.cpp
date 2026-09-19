//cpp
// @symbol _ZN21FloatingFloorLllSmall16CleanupResourcesEv
/* recovered: named members + real C++ method */
/* FloatingFloorLllSmall::CleanupResources() -- indexes the ov022 parameter
 * table with mVariant, the one field this class adds. */
#include "FloatingFloorLllSmall.h"

struct SharedFilePtr;
struct CLPS_Block;

/* One row of the 0xc-stride ov022 table: the model/collision/CLPS descriptor
 * the shared helper reads out of its second argument, spelled the way
 * src/actors/daObjUkiyuka_c.cpp -- the file that DEFINES func_ov002_020b6424
 * -- spells it. */
struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

/* The helper takes the BASE, daObjUkiyuka_c, not this leaf: this class's
 * __si_class_type_info in ov022 records exactly one base, daObjUkiyuka_c at
 * ov002 0x02109104, at subobject offset 0, and daObjKm2_Ukishima_c (ov045)
 * reaches the same helper through the same base. */
extern "C" {
int func_ov002_020b6424(daObjUkiyuka_c *t, ResourceDescriptor *f);
extern char data_ov022_021140d4[];
}

int FloatingFloorLllSmall::CleanupResources()
{
    return func_ov002_020b6424(
        this, (ResourceDescriptor *)(data_ov022_021140d4 + mVariant * 0xc));
}
