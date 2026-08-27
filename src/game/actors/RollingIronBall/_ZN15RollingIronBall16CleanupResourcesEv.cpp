//cpp
// @symbol _ZN15RollingIronBall16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "RollingIronBall.h"
#include "SharedFilePtr.h"
extern char data_ov100_02148668;

int RollingIronBall::CleanupResources()
{
    char *file = *(char **)((char *)&unk_3a8);

    if (file != 0) {
        (*(unsigned char *)(((int)file + 0x3d2)))--;
    }

    ((SharedFilePtr *)(&data_ov100_02148668))->Release();
    return 1;
}
