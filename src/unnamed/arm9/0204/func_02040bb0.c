#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
extern int data_020a0f38;
extern char data_020a2400[];

extern int WM_ReadStatus(int *arg);
extern void func_02065a84(int x);
extern void func_020657d0(u16 id, void *obj);
extern void func_02041930(void);
extern void func_02040f30(void *arg);

void func_02040bb0(void) {
    int old;
    data_020a0f38 = 1;
    old = data_020a1fc0.mIsParent;
    data_020a1fc0.mSendRequest = 1;
    if (old == 0) {
        /* The child area past its sixteen block infos (0xc0 + 0x10 * 0x28):
           WM_ReadStatus copies the WM status block, 0x930 bytes, to +0x340,
           which ends at mRecvWork (0xc70); WM_ReadStatus has that shape
           (6 on a NULL buffer, then a cache-invalidated copy). The halfword
           at the copy's +0x184 goes to func_02065a84. The header keeps all
           of it in pad_0c0. */
        u32 base = (u32)data_020a2400;
        WM_ReadStatus((int *)(base + 0x340));
        func_02065a84(*(u16 *)(base + 0x4c4));
        func_020657d0(1, (void *)func_02040f30);
    } else {
        void (*fn)(int);
        data_020a1fc0.mState = 2;
        fn = (void (*)(int))data_020a1fc0.mStateCallback;
        if (fn != 0) {
            fn(0);
        }
    }
    func_02041930();
}
