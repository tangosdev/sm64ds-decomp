#include "types.h"
extern void CpuCopy8(const void *src, void *dst, unsigned int size);
extern char data_020a0fb8[];
extern int data_020a0f70;

/* The parent state callback func_0203e20c installs
   (MB_CommSetParentStateCallback). On state 2 for child a0 (0..3) it keeps
   6 bytes from +0xa of the state's record in data_020a0fb8; on state 12 it
   raises data_020a0f70, which func_0203e20c waits on. The other states are
   ignored. */
void func_0203e9a8(u32 a0, int sel, int a2)
{
    switch (sel) {
    case 0: case 1:
        break;
    case 2:
        if (a0 >= 4) return;
        if (a2 == 0) return;
        CpuCopy8((const void *)(a2 + 0xa), data_020a0fb8 + a0 * 6, 6);
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
