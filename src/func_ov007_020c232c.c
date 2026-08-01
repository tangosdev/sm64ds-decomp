#include "types.h"
void func_ov007_020c232c(char* o) {
    int a, b, c;
    if (*(int*)(o + 0x24) == 0) return;
    a = (*(s16*)(o + 0x1c) >> 3) & 0x3ff;
    b = (*(s16*)(o + 0x1e) >> 3) & 0x3ff;
    c = (*(s16*)(o + 0x20) >> 3) & 0x3ff;
    *(volatile unsigned*)0x40004c8 = a | (b << 10) | (c << 20) | (*(int*)o << 30);
    *(volatile unsigned*)0x40004cc = (*(int*)o << 30) | *(u16*)(o + 0x22);
}
