#include "types.h"
extern char *data_ov007_0210342c;
extern int data_ov007_020d7e28[];
extern int data_ov007_020d7e38[];
extern int data_ov007_020d7e50[];
extern int data_ov007_020d7ec8[];
extern u8 data_ov007_020d7a24[];

extern int func_ov007_020b7594(int a, int b);
extern void func_ov007_020b7320(char *c, int a, int b);
extern int func_ov007_020c99d8(int x);
extern char *func_ov007_020c9988(int a);
extern void func_ov007_020b72fc(char *c, int n);
extern int func_ov007_020c9688(int a);
extern void *func_ov007_020ade58(int self);
extern void func_ov007_020b72d8(char *c, int n);
extern int *func_ov007_020c95fc(void *a, int b, int c);
extern int func_ov007_020adc08(int type, char *obj);

#pragma opt_strength_reduction off
#pragma opt_common_subs off

void func_ov007_020b6d40(void)
{
    char *c;
    int i;
    int tmp;

    tmp = func_ov007_020b7594(0x1f, 0x1f);
    *(int *)(data_ov007_0210342c + 0x74) = tmp;
    c = *(char **)(data_ov007_0210342c + 0x74);
    func_ov007_020b7320(c, 4, 6);

    for (i = 0; i < 4; i++) {
        *(int *)(*(char **)(c + 0x24) + i * 4) = func_ov007_020c99d8(data_ov007_020d7e28[i]);
    }

    for (i = 0; i < 6; i++) {
        *(char **)(*(char **)(c + 0x2c) + i * 4) = func_ov007_020c9988(data_ov007_020d7e38[i]);
        if (i >= 2 && i <= 5) {
            *(u16 *)(*(char **)(*(char **)(c + 0x2c) + i * 4) + 4) = 0xd0;
        }
    }

    func_ov007_020b72fc(c, 6);

    for (i = 0; i < 6; i++) {
        *(int *)(*(char **)(c + 0x3c) + i * 4) = func_ov007_020c9688(data_ov007_020d7e50[i]);
    }

    for (i = 0; i < 9; i++) {
        *(void **)(data_ov007_0210342c + i * 4 + 0xa4) = func_ov007_020ade58(i);
    }

    func_ov007_020b72d8(c, 8);

    for (i = 0; i < 8; i++) {
        *(int **)(*(char **)(c + 0x34) + i * 4) =
            func_ov007_020c95fc((void *)data_ov007_020d7ec8[i],
                                (&data_ov007_020d7a24[i * 2])[0],
                                (&data_ov007_020d7a24[i * 2])[1]);
        *(int *)(data_ov007_0210342c + i * 4 + 0xc8) =
            func_ov007_020adc08(i, *(char **)(*(char **)(c + 0x34) + i * 4));
    }
}
