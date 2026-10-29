//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

extern int WM_GetSystemWork(void);
extern int func_0206152c(void);
extern void MultiCopy32Bytes(int *src, int *dst, int len);

int func_02061c20(int *arg)
{
    char *x = (char *)WM_GetSystemWork();
    int r = func_0206152c();
    if (r != 0) return r;
    if (arg == 0) return 6;
    CP15::InvalidateDataCache(*(u32 *)(x + 4), 0x930);
    MultiCopy32Bytes(*(int **)(x + 4), arg, 0x930);
    return 0;
}
}
