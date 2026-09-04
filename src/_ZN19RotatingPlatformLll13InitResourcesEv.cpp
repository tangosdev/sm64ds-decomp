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
 * adding a `return` the ROM does not have. */
struct Arg { void *m[3]; };
extern "C" {
extern int func_ov002_020b676c(void *self, Arg *a, short arg2);
extern Arg data_ov022_02113da4;
}

int RotatingPlatformLll::InitResources()
{
    func_ov002_020b676c(this, &data_ov022_02113da4, 0x100);
}
