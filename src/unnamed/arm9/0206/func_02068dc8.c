#include "types.h"
#include "WmWork.h"
extern char* data_020a9db4;
extern int func_02068e44(int, int, int);

int func_02068dc8(int a, int b, int c)
{
    if (WM_WORK->initialized)
        return 0;
    if (func_02068e44(a, b, c) == 0)
        return 0;
    WM_WORK->parentParam.maxEntry = c;
    WM_WORK->parentMaxSize = a;
    WM_WORK->childMaxSize = b;
    return 1;
}
