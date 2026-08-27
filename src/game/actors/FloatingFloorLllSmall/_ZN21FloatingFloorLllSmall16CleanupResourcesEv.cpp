//cpp
// @symbol _ZN21FloatingFloorLllSmall16CleanupResourcesEv
/* recovered: named members + real C++ method */
/* FloatingFloorLllSmall::CleanupResources() -- indexes the ov022 parameter
 * table with mVariant, the one field this class adds. */
#include "FloatingFloorLllSmall.h"

extern "C" {
int func_ov002_020b6424(char *t, void **f);
extern char data_ov022_021140d4[];
}

int FloatingFloorLllSmall::CleanupResources()
{
    return func_ov002_020b6424((char *)this,
                               (void **)(data_ov022_021140d4 + mVariant * 0xc));
}
