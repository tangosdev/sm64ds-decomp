#include "types.h"
#include "nitro/wm/wm_api.h"
#include "private/arm9_a1fc0_file_share.h"
extern void func_020418f0(void *arg);
/* NitroSDK WBT_MpParentSendHook and WBT_MpChildSendHook: both return the
   packet length as an int, which this call site narrows to u16 itself */
extern int WBT_MpParentSendHook(int buf, int size);
extern int WBT_MpChildSendHook(int buf, int size);

/* Send the pending file-sharing packet on mPort: WBT_MpParentSendHook (parent) or
 * WBT_MpChildSendHook (child), by mIsParent, builds it in the buffer at +0x40 with
 * that side's packet size as the limit, then WM_SetMPDataToPort sends it to AIDs
 * 0xffff with func_020418f0 as the callback. */
void func_02041930(void)
{
    FileShareWork *work = &data_020a1fc0;
    int f0 = work->mIsParent;
    u16 a;
    u16 r;
    if (f0 != 0)
        a = (u16)work->mParentPacketSize;
    else
        a = (u16)work->mChildPacketSize;
    if (work->mSendRequest == 0)
        return;
    {
        unsigned char *sub = work->mSendBuf;
        if (f0 != 0)
            r = (u16)WBT_MpParentSendHook((int)sub, a);
        else
            r = (u16)WBT_MpChildSendHook((int)sub, a);
        work->mSendBusy = (WM_SetMPDataToPort(func_020418f0, (u16 *)sub, r, 0xffff, work->mPort,
                                              WM_PRIORITY_LOW) == WM_ERRCODE_OPERATING)
                              ? 1
                              : 0;
    }
    /* re-read through the symbol: `work->mSendBusy` changes the .text bytes */
    work->mSendRequest = (data_020a1fc0.mSendBusy == 0) ? 1 : 0;
}
