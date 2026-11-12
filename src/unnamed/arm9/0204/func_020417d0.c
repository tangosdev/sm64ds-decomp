#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
#include "private/wm_views.h"
extern void func_02041930(void);
extern void WBT_MpParentRecvHook(int data, int length, int aid);
extern void WBT_MpChildRecvHook(int data, int length);

/* The file-sharing port receive callback. Once the work block is up
 * (mState): CONNECTED lets the parent resend (func_02041930) unless a
 * send is in flight; DISCONNECTED drops the child from the parent's
 * mConnectedBitmap; PORT_RECV marks the sender connected and hands
 * its packet to the parent's handler (WBT_MpParentRecvHook), or on a child to
 * WBT_MpChildRecvHook. The argument is a NitroSDK WMPortRecvCallback
 * (private/wm_views.h). */
void func_020417d0(void *arg) {
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;
    FileShareWork *g = FILE_SHARE_WORK;
    FileShareParentArea *base = (FileShareParentArea *)g->mArea;

    if (g->mState == 0)
        return;

    switch (cb->state) {
    case WM_STATECODE_CONNECTED:
        if (g->mIsParent == 0)
            return;
        if (g->mSendBusy != 0)
            return;
        func_02041930();
        return;
    case WM_STATECODE_DISCONNECTED:
        if (g->mIsParent == 0)
            return;
        base->mConnectedBitmap &= ~(1 << cb->aid);
        return;
    case WM_STATECODE_PORT_RECV: {
        s32 data = (s32)cb->data;
        u32 length = cb->length;
        if (g->mIsParent != 0) {
            u32 aid = cb->aid;
            base->mConnectedBitmap |= (1 << aid);
            WBT_MpParentRecvHook(data, length, aid);
        } else {
            WBT_MpChildRecvHook(data, length);
        }
        return;
    }
    }
}
