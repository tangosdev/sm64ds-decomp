#include "types.h"
#include "WmWork.h"
extern char *data_020a9db4;

void func_02069918(char *arg0)
{
    u16 v;
    if (*(u16 *)(arg0 + 2) != 0) return;
    v = *(u16 *)(arg0 + 4);
    if (v == 7) return;
    if (v == 9) return;
    if (v != 0x15) return;
    WM_WORK->userCallback(9, arg0);
}
