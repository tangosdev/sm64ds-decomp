//cpp
// @symbol _ZN18RotatingPlatformWf13InitResourcesEv
#include "RotatingPlatformWf.h"

/* RotatingPlatformWf::InitResources -- vtable slot 0, ov015 0x02112c98.
 *
 * A tail call into daObjKaitendai_c's shared cross-overlay helper
 * func_ov002_020b676c(this, filetable, angle) -- out of this task's scope,
 * kept under its existing name (mwccarm-codegen.md's cross-overlay veneer
 * idiom, same as src/_ZN18RotatingPlatformWf16CleanupResourcesEv.cpp for
 * slot 3). data_ov015_021147a4/02114794 are this class's own file-table/
 * angle statics, referenced under their existing decl_common.h externs. */
struct Arg { void *m[3]; };
extern "C" {
extern int func_ov002_020b676c(void *self, Arg *a, short arg2);
extern Arg data_ov015_021147a4;
extern short data_ov015_02114794;
}

int RotatingPlatformWf::InitResources()
{
    return func_ov002_020b676c(this, &data_ov015_021147a4, data_ov015_02114794);
}
