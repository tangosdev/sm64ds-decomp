#include "types.h"
#include "nitro/mb/mb.h"
#include "private/wm_views.h"
// local extern: MI_Memory.c defines MI_CpuCopy8 as an assembly stub.
extern void MI_CpuCopy8(const void *src, void *dst, unsigned int size);
extern u8 data_020a0fb8[];
extern int data_020a0f70;

/* The parent state callback func_0203e20c installs
   (MB_CommSetParentStateCallback). When child aid (0..3) connects it keeps
   the child's MAC address from the WM record in data_020a0fb8; on
   MB_COMM_PSTATE_END it raises data_020a0f70, which func_0203e20c waits on.
   The other states are ignored. */
void func_0203e9a8(u16 aid, u32 status, void *arg) {
    switch (status) {
    case MB_COMM_PSTATE_NONE:
    case MB_COMM_PSTATE_INIT_COMPLETE:
        break;
    case MB_COMM_PSTATE_CONNECTED:
        if (aid >= 4)
            return;
        if (arg == 0)
            return;
        MI_CpuCopy8(((WmStartParentCallback *)arg)->macAddress, data_020a0fb8 + aid * 6, 6);
        break;
    case MB_COMM_PSTATE_DISCONNECTED:
    case MB_COMM_PSTATE_KICKED:
    case MB_COMM_PSTATE_REQ_ACCEPTED:
    case MB_COMM_PSTATE_SEND_PROCEED:
    case MB_COMM_PSTATE_SEND_COMPLETE:
    case MB_COMM_PSTATE_BOOT_REQUEST:
    case MB_COMM_PSTATE_BOOT_STARTABLE:
    case MB_COMM_PSTATE_REQUESTED:
    case MB_COMM_PSTATE_MEMBER_FULL:
        break;
    case MB_COMM_PSTATE_END:
        data_020a0f70 = 1;
        break;
    case MB_COMM_PSTATE_ERROR:
    case MB_COMM_PSTATE_WAIT_TO_SEND:
        break;
    }
}
