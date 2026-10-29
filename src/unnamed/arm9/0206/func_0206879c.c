#include "types.h"
#include "WmWork.h"
extern char *data_020a9db4;

struct Pair { s16 a; s16 b; };

void func_0206879c(int x, int kind)
{
    struct Pair p;
    if (kind == 2)
        return;
    if (kind == 0)
        return;
    p.a = (s16)x;
    p.b = (s16)kind;
    WM_WORK->userCallback(0xff, &p);
}
