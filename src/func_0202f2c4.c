#include "types.h"
extern int data_023c0000;
extern u8 data_0209f5fc;
extern volatile u32 data_0209f608;
extern u8 data_0209f648[];

extern void func_0202f3a4(void);
extern void MultiCopy_Int(void *src, void *dest, u32 size);

void func_0202f2c4(void)
{
    int line;

    *(int *)((char *)&data_023c0000 + 0x3ff8) |= 2;

    line = *(volatile u16 *)0x4000006 + 1;

    if (line >= 0xc0) {
        if (data_0209f5fc != 1) {
            func_0202f3a4();
        }
        return;
    }

    if ((*(volatile u16 *)0x4000004 & 2) == 0) {
        return;
    }

    MultiCopy_Int(data_0209f648 + data_0209f608 * 0x300u + line * 4, (void *)0x4000040u, 4);
    MultiCopy_Int(data_0209f648 + data_0209f608 * 0x300u + line * 4, (void *)0x4001040u, 4);

    data_0209f5fc = 0;
}
