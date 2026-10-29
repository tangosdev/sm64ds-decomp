#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
extern char data_020a1fc0[];
extern int data_020a0f38;
extern char data_020a2400[];

extern int func_02061c20(int *arg);
extern void func_02065a84(int x);
extern void func_020657d0(u16 id, void *obj);
extern void func_02041930(void);
extern void func_02040f30(int arg);

void func_02040bb0(void)
{
    int old;
    data_020a0f38 = 1;
    old = FILE_SHARE_WORK->mIsParent;
    FILE_SHARE_WORK->mSendRequest = 1;
    if (old == 0) {
        /* The child area's words past its block-info entries: the header
           leaves them in pad_0c0 and no reader here names them. */
        u32 base = (u32)data_020a2400;
        func_02061c20((int *)(base + 0x340));
        func_02065a84(*(u16 *)(base + 0x4c4));
        func_020657d0(1, (void *)func_02040f30);
    } else {
        void (*fn)(int);
        FILE_SHARE_WORK->mState = 2;
        fn = (void (*)(int))FILE_SHARE_WORK->mStateCallback;
        if (fn != 0) {
            fn(0);
        }
    }
    func_02041930();
}
