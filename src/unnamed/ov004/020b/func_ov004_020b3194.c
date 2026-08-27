#include "types.h"
extern u8 data_ov004_020bf3e8[];
void func_ov004_020b3194(void *c)
{
    *(int*)((char*)c+0x20) = 0x1d;
    s16 idx = *(s16*)((char*)c+0x30);
    data_ov004_020bf3e8[idx] = 0;
}
