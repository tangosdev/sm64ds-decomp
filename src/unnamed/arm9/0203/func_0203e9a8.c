#include "types.h"
extern void CpuCopy8(int a, void *b, int n);
extern char data_020a0fb8[];
extern int data_020a0f70;

void func_0203e9a8(u32 a0, int sel, int a2)
{
    switch (sel) {
    case 0: case 1:
        break;
    case 2:
        if (a0 >= 4) return;
        if (a2 == 0) return;
        CpuCopy8(a2 + 0xa, data_020a0fb8 + a0 * 6, 6);
        break;
    case 3: case 4: case 5: case 6: case 7:
    case 8: case 9: case 10: case 11:
        break;
    case 12:
        data_020a0f70 = 1;
        break;
    case 13: case 14:
        break;
    }
}
