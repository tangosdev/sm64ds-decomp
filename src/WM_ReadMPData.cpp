//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "private/wm_views.h"

extern "C" {

extern int WMi_CheckInitialized(void);

/* NitroSDK wm_sync.c WM_ReadMPData(header, aid), by body against pret's
 * pokediamond WM_sync.s: the received MP data block from child aid (1..15, and
 * connected), found by walking the header's count blocks of length bytes from
 * +0xa; 0 when there is none. */
char *WM_ReadMPData(char *a, unsigned int idx) {
    WMArm9Buf *sys;
    int n;
    char* local[16];
    int i;
    sys = WM_GetSystemWork();
    if (WMi_CheckInitialized() != 0)
        return 0;
    if (idx < 1 || idx > 0xf)
        return 0;
    DC_InvalidateRange(&sys->status->child_bitmap, 2);
    if ((sys->status->child_bitmap & (1u << idx)) == 0)
        return 0;
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
