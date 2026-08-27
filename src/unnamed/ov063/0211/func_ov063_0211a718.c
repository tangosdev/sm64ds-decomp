#include "types.h"
extern s16 data_02082214[];

void func_ov063_0211a718(char* o) {
    int a;
    s16 v;
    s16* p;
    a = (u16)(s16)((*(u16*)(o + 0x100) - 0x1f) << 13) >> 4;
    p = (s16*)(o + 0x8e);
    v = *p;
    *p = v + (data_02082214[a * 2 + 1] << 10) / 4096;
}
