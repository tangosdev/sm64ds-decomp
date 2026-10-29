#include "types.h"
#include "WmWork.h"
extern char *data_020a9db4;
extern int func_02062200(int x);

int func_02068a74(void)
{
    WmWork *p = WM_WORK;
    if (p->resetting == 0) {
        int r;
        p->scanContinue = 0;
        /* through the global again: the ROM reloads data_020a9db4 here */
        WM_WORK->resetting = 1;
        r = func_02062200((int)WM_WORK->stateCallback);
        if (r == 2)
            return 0;
        return r;
    }
    return 1;
}
