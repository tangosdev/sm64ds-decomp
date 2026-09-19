//cpp
// @symbol _ZN19RotatingPlatformLll13InitResourcesEv
#include "RotatingPlatformLll.h"

/* RotatingPlatformLll::InitResources -- vtable slot 0, ov022 0x02111670.
 *
 * A tail call into daObjKaitendai_c's shared cross-overlay helper
 * func_ov002_020b676c(this, filetable, angle) -- out of this task's scope,
 * kept under its existing name, same idiom as
 * src/_ZN17daObjBk_Ukisima_c13InitResourcesEv.cpp (ov015). The pre-migration
 * recovery discarded the helper's return value (void call, despite the
 * header's `int` slot signature), so this keeps that shape rather than
 * adding a `return` the ROM does not have.
 *
 * The helper takes the BASE, daObjKaitendai_c, not this leaf: this class's
 * __si_class_type_info in ov022 records exactly one base, daObjKaitendai_c at
 * ov002 0x021091ac, at subobject offset 0, and the four other leaves reach the
 * same helper through the same base. */
struct SharedFilePtr;
struct CLPS_Block;

/* The second argument, which this file used to call `Arg`: the three-word
 * model/collision/CLPS descriptor, spelled the way
 * src/actors/daObjKaitendai_c.cpp -- the file that DEFINES
 * func_ov002_020b676c -- spells it. */
struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
extern int func_ov002_020b676c(daObjKaitendai_c *self, ResourceDescriptor *a,
                               short arg2);
extern ResourceDescriptor data_ov022_02113da4;
}

int RotatingPlatformLll::InitResources()
{
    func_ov002_020b676c(this, &data_ov022_02113da4, 0x100);
}
