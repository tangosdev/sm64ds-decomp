//cpp
#include "types.h"
#include "WmWork.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "IRQ.h"

extern "C" {

#pragma opt_strength_reduction off

extern char *data_020a9db4;
extern char *data_020a9db8;

/* Typed through a cast: the other declarations of data_020a9db8 spell it as
 * a byte pointer (func_02066a94.c's MB_WORK). */
#define MB_WORK ((MbParentWork *)data_020a9db8)

extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void func_020672a4(u32 val);
extern void func_0205a588(void *dst, int val, u32 size);
extern void func_020684a8(void);
extern int func_02068d08(void);

extern void func_02066a94(int arg0, char *arg1);
extern void func_02069994(char *arg0);

/* NitroSDK MB_StartParent-style setup of the multiboot parent: record the
 * channel, clear the parent work from mChildUser on (keeping the parent
 * callback across the clear), size the blocks and the child request pieces
 * from the MP packet sizes, mark every child as in no file, clear the file
 * table and the pending answers, then hand WM the parent callbacks and
 * buffer sizes and start. */
int func_02068ae8(int param0)
{
    u32 saved;
    volatile u16 sp0;
    volatile u16 sp2;
    u32 cb;
    int i;
    int result;

    saved = IRQ::Disable();

    WM_WORK->parentParam.channel = param0;

    cb = MB_WORK->mParentCallback;

    sp0 = 0;
    MultiStore16(sp0, (char *)MB_WORK->mChildUser, 0x60a0);

    func_020672a4(cb);

    MB_WORK->mBlockSize = WM_WORK->parentMaxSize - 6;
    MB_WORK->mReqPieceSize = WM_WORK->childMaxSize - 2;
    MB_WORK->mReqPieceCount = 30 / (WM_WORK->childMaxSize - 2);

    for (i = 0; i < 0xf; i++) {
        MB_WORK->mChildState[i] = 0;
        MB_WORK->mChildFile[i] = -1;
    }
    MB_WORK->mFileCount = 0;

    sp2 = 0;
    MultiStore16(sp2, (char *)MB_WORK->mFiles, 0x5c40);

    func_0205a588(MB_WORK->mChildResponse, 0, 0x1e);

    WM_WORK->mode = 1;
    WM_WORK->userCallback = (void (*)(int, void *))func_02066a94;
    WM_WORK->stateCallback = func_02069994;

    WM_WORK->parentParam.parentMaxSize = WM_WORK->parentMaxSize;
    WM_WORK->sendBufSize = (WM_WORK->parentParam.parentMaxSize + 0x23) & ~0x1f;

    WM_WORK->parentParam.childMaxSize = WM_WORK->childMaxSize;
    WM_WORK->recvBufSize = (((WM_WORK->parentParam.childMaxSize + 0xe) * 0xf + 0x29) & ~0x1f) << 1;

    func_020684a8();
    result = func_02068d08();

    IRQ::Restore(saved);

    return result;
}
}
