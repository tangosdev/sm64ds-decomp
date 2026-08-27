//cpp
// @symbol _ZN16FloatingFloorBfs13InitResourcesEv
#include "FloatingFloorBfs.h"

/* FloatingFloorBfs::InitResources -- vtable slot 0, ov045 0x02111bdc.
 *
 * A tail call into daObjUkiyuka_c's shared cross-overlay file-load helper
 * func_ov002_020b6584(this, filetable, angle) -- out of this task's
 * scope, kept under its existing name, same cross-overlay-veneer idiom as
 * the other abstract-base leaves in this worklist (ov015/ov022/ov036's
 * daObjKaitendai_c leaves, ov043's daObjKurumajiku_c leaf). */
extern "C" {
extern int func_ov002_020b6584(void *self, void *filetable, int angle);
extern int data_ov045_02112f08[];
}

int FloatingFloorBfs::InitResources()
{
    return func_ov002_020b6584(this, data_ov045_02112f08, 0xf50);
}
