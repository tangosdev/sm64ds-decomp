#include "types.h"
void func_ov002_020e4374(char* c, int* p1, int* p2)
{
    int v;
    int m;
    *p1 = (*(int*)(c+0x690) + (*(int*)(c+0x60) - *(int*)(c+0x68c)) + 0xf000) - *(int*)(c+0x644);
    if (*(u8*)(c+0x6fd) != 0) *p1 -= 0x70000;
    if (*p1 <= 0x1000) *p1 = 0x1000;
    m = 0x64000;
    if (*(u8*)(c+0x703) != 0) {
        m = *(int*)(c+0x84) * 0x64;
    } else if (*(u8*)(c+0x6fd) != 0) {
        m = 0xc8000;
    }
    v = (int)(((s64)*p1 * 0x180 + 0x800) >> 12);
    *p2 = m - v;
    if (*p2 < 0xa000) *p2 = 0xa000;
    *p1 += 0x28000;
}
