#include "nitro/wm/wm_api.h"

extern void func_02040aa4(void);
extern void func_020402a0(void *arg); /* the WM_Reset callback */
extern int data_020a0f94;

int Wireless_Reset(void)
{
    int r = WM_Reset(func_020402a0);
    func_02040aa4();
    if (r != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
