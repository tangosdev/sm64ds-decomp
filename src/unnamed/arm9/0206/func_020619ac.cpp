//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

extern int WM_GetSystemWork(void);
extern int func_0206152c(void);

char* func_020619ac(char* a, unsigned int idx)
{
    int h;
    int n;
    char* local[16];
    int i;
    h = WM_GetSystemWork();
    if (func_0206152c() != 0) return 0;
    if (idx < 1 || idx > 0xf)
        return 0;
    CP15::InvalidateDataCache((u32) * (char **)(h + 4) + 0x17e, 2);
    if ((*(u16*)(*(char**)(h + 4) + 0x100 + 0x7e) & (1u << idx)) == 0) return 0;
    n = *(u16*)(a + 4);
    if (n == 0) return 0;
    local[0] = a + 0xa;
    i = 0;
    for (;;) {
        if (idx == *(u16*)(local[i] + 4)) return local[i];
        i++;
        local[i] = (char*)(*(u16*)(a + 6) + (u32)local[i - 1]);
        if (i >= *(volatile u16*)(a + 4)) break;
    }
    return 0;
}
}
