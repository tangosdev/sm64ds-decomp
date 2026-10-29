#include "WmWork.h"
extern void func_02068834(int v);
extern int func_02062240(int a, int b, int c);
extern int func_02061ce4(int x);
extern char *data_020a9db4;
extern unsigned short data_020a9db0;

int func_02068d08(void)
{
    WmWork *base;
    int ret;
    base = WM_WORK;
    base->mpStarted = 0;
    WM_WORK->childBitmap = 0;
    WM_WORK->resetting = 0;
    *(short *)(data_020a9db4 + 0x1e48) = 0; /* in WmWork.pad_1e2e: no reader here */
    func_02068834(0xa);
    do {
        ret = func_02062240((int)WM_WORK->wmSysBuf, (int)WM_WORK->stateCallback, data_020a9db0);
    } while (ret == 4);
    if (ret != 2) return 8;
    func_02061ce4((int)WM_WORK->stateCallback);
    WM_WORK->initialized = 1;
    return 0;
}
