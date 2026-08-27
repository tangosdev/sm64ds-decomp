#include "types.h"
extern void _Z14ApproachLinearRiii(int* v, int target, int step);

void func_ov002_020d9aac(char* c)
{
    int v;

    if ((*(u8*)(c + 0x6e3) & 7) >= 2) {
        if (*(int*)(c + 0x640) < 0) {
            *(int*)(c + 0xa8) = -(*(int*)(c + 0x640) >> 2);
            v = *(int*)(c + 0xa8);
            if (v <= 0x1000) {
                *(int*)(c + 0xa8) = 0;
            } else if (v >= 0xa000) {
                *(int*)(c + 0xa8) = 0xa000;
            }
            if (*(int*)(c + 0x98) >= 0xa000) *(int*)(c + 0x98) = 0xa000;
        }
    } else {
        if (*(int*)(c + 0x98) >= 0x12000) *(int*)(c + 0x98) = 0x12000;
    }

    if ((*(u8*)(c + 0x6e3) & 7) < 2) {
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x800);
    } else {
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x1800);
    }
}
