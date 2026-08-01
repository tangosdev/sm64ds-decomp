#include "types.h"
extern int func_0205fbd0(int v);

int func_0205fab4(int x)
{
    int v;
    switch (x) {
    case 1: v = 1; break;
    case 3: v = 2; break;
    case 2: v = 3; break;
    default: v = 0; break;
    }
    if (v == 0) return 0xffff;
    return func_0205fbd0(v);
}
