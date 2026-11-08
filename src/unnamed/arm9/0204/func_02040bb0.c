#include "types.h"
#include "private/arm9_a1fc0_file_share.h"
#include "private/wm_views.h"
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
        /* WM_ReadStatus copies the WM status block into the child area's
           mStatus (6 on a NULL buffer, then a cache-invalidated copy); its
           aid goes to func_02065a84. */
        FileShareChildArea *child = (FileShareChildArea *)data_020a2400;
        WM_ReadStatus((int *)child->mStatus);
        func_02065a84(((WMStatus *)child->mStatus)->aid);
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
