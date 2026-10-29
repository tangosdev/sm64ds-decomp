#include "types.h"
#include "WmWork.h"
extern char *data_020a9db4;
extern int data_0209a088;
extern void func_020690b0(void);
extern int func_02068a28(int a, int b, int c, int d, int e);

int func_020688fc(int arg0, int arg1, int arg2) {
    WmWork *hdr = WM_WORK;
    int a = (u16)arg1;
    int b = (u16)arg2;
    char *st;
    int ret;
    int u;
    if ((hdr->mpStarted == 0) || (hdr->resetting == 1)) {
        return 1;
    }
    switch (hdr->mode) {
    case 1:
        if (a < data_0209a088) {
            a = (u16)data_0209a088;
        }
        if (hdr->mpContinuous == 0) {
            u = 1000;
        } else {
            u = 0;
        }
        ret = func_02068a28((int)hdr->stateCallback, arg0, a, (u16)u, b);
        if (ret == 2) {
            WM_WORK->mpSending = 1;
        }
        if (ret == 2) {
            ret = 0;
        }
        return ret;

    case 2:
        ret = func_02068a28((int)func_020690b0, arg0, a, 0, b);
        if (ret == 2) {
            WM_WORK->mpSending = 1;
        }
        if (ret == 2) {
            ret = 0;
        }
        return ret;

    default:
        return 1;
    }
}
