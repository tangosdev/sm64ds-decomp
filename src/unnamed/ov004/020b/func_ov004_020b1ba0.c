#include "types.h"
void func_ov004_020b1ba0(void *c, s32 delta)
{
    s32 v = *(s32*)((char*)c + 0xa8) + delta;
    if (v < 0) v = 0;
    else if (v > 0x270f) v = 0x270f;
    *(s32*)((char*)c + 0xa8) = v;
}
