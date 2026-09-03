//cpp
// @symbol _ZN18RotatingPlatformRr13InitResourcesEv
#include "RotatingPlatformRr.h"

/* RotatingPlatformRr::InitResources -- vtable slot 0, ov036 0x0211150c.
 *
 * A tail call into daObjKaitendai_c's shared cross-overlay helper
 * func_ov002_020b676c(this, filetable, angle) -- out of this task's
 * scope, kept under its existing name, same idiom as
 * daObjBk_Ukisima_c/Lll's slot 0. Unlike those two, the angle argument
 * here is picked between two per-instance statics based on param1
 * (fBase_c's own field, include/fBase_c.h) -- param1 == 1 selects the
 * alternate angle. */
extern "C" {
extern int func_ov002_020b676c(void *self, void *arg, short val);
extern short data_ov036_02113b18;
extern short data_ov036_02113b1c;
extern void *data_ov036_02113b2c;
}

int RotatingPlatformRr::InitResources()
{
    short v = data_ov036_02113b18;
    if ((param1 & 0xff) == 1) {
        v = data_ov036_02113b1c;
    }
    return func_ov002_020b676c(this, &data_ov036_02113b2c, v);
}
