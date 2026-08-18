//cpp
// @symbol _ZN11RickshawBdw16CleanupResourcesEv
#include "RickshawBdw.h"

/* RickshawBdw::CleanupResources -- vtable slot 3, ov043 0x021114b0.
 *
 * A tail call into daObjKurumajiku_c's shared cleanup helper
 * func_ov002_020b6ac8(this, filetable) -- out of this task's scope, kept
 * under its existing name. Same-overlay call (both this class and the
 * helper's data live in ov043's own address range via daObjKurumajiku_c's
 * base in ov002, but the call itself is same-module -- func_ov002_020b6ac8
 * is reached the ordinary way, matching the pre-migration recovery, which
 * used no long_calls pragma here). */
extern "C" {
extern int func_ov002_020b6ac8(void *self, void *arg);
extern void *data_ov043_02112344;
}

int RickshawBdw::CleanupResources()
{
    return func_ov002_020b6ac8(this, &data_ov043_02112344);
}
