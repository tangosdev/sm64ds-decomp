#include "types.h"
extern u8 *data_ov007_0210342c;

extern void *func_ov007_020c9388(void *self);
extern void *func_ov007_020c946c(void *r5);

#pragma opt_strength_reduction off
void func_ov007_020b69c4(void)
{
    int i;
    int arg;

    *(int *)(data_ov007_0210342c + 0xf4) = 0;
    *(int *)(data_ov007_0210342c + 0x100) = 0;
    *(int *)(data_ov007_0210342c + 0x104) = 0;
    *(int *)(data_ov007_0210342c + 0xf0) = 0;
    *(int *)(data_ov007_0210342c + 0xec) = *(int *)(data_ov007_0210342c + 0xf0);

    i = 0;
    do {
        (data_ov007_0210342c + i)[0xfc] = (u8)0;
        i++;
    } while (i < 2);

    *(void **)(data_ov007_0210342c + 0x40) = func_ov007_020c9388((void *)0);

    arg = 0;
    i = 0;
    do {
        *(void **)(data_ov007_0210342c + (i << 2) + 0x44) = func_ov007_020c946c((void *)arg);
        i++;
    } while (i < 3);

    *(int **)(*(int ***)(data_ov007_0210342c + 0x44)) =
        (int *)((char *)*(int **)(data_ov007_0210342c + 0x4c) + 4);
    *(int **)(*(int ***)(data_ov007_0210342c + 0x48)) =
        (int *)((char *)*(int **)(data_ov007_0210342c + 0x44) + 4);
}
