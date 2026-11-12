#include "nitro/fs/overlay.h"
#include "nitro/os/system.h"
extern int func_02018b64(FSOverlayInfo *info);
/* Only its address is used: 0x020aa420, where the overlay region starts
 * (ov000 loads there too). */
extern void CapIconMgr_DispatchVs(void);

int FS_LoadOverlay(MIProcessor target, FSOverlayID id) {
    FSOverlayInfo info;
    unsigned int ram;
    unsigned int sum;
    if (!func_02018c00(&info, target, id) || !func_02018b64(&info))
        return 0;
    ram = (unsigned int)info.header.ram_address;
    /* goto kept: an || guard around OS_Terminate() colours ram/sum differently (4 forms tried) */
    if (ram >= (unsigned int)CapIconMgr_DispatchVs) {
        sum = ram + info.header.ram_size;
        if (sum + info.header.bss_size <= 0x214eaa0u)
            goto start;
    }
    OS_Terminate();
start:
    FS_StartOverlay(&info);
    return 1;
}
