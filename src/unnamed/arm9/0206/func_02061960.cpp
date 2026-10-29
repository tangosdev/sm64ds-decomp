//cpp
#include "CP15.h"

extern "C" {

extern void *WM_GetSystemWork(void);
extern int func_020614d0(void);

unsigned short func_02061960(void)
{
    unsigned char *o = (unsigned char *)WM_GetSystemWork();
    if (func_020614d0() != 0) return 0;
    {
        unsigned char *p = *(unsigned char **)(o + 4);
        CP15::InvalidateDataCache((unsigned)(p + 0x1f2), 2);
        return *(unsigned short *)(*(unsigned char **)(o + 4) + 0x1f2);
    }
}
}
