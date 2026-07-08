// NONMATCHING: register allocation (div=23). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern void func_02012718(void *a, int b);
extern u8 data_020a0e40;
extern u8 TOUCH_INPUT_ARR[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

void func_ov006_020e1b54(char *c)
{
    int idx = data_020a0e40;
    int flag = 0;
    int x, y;

    if (TOUCH_INPUT_ARR[idx * 4] != 0) {
        if (data_020a0de9[idx * 4] != 0) flag = 1;
    }
    if (flag == 0) return;

    x = (*(int *)(c + 0x4eb0) >> 0xc) - data_020a0dea[idx * 4];
    y = (*(int *)(c + 0x4eb4) >> 0xc) - data_020a0deb[idx * 4];
    *(int *)(c + 0x4ec0) = x << 0xc;
    *(int *)(c + 0x4ec4) = y << 0xc;
    *(u8 *)(c + 0x4ee4) = 1;
    *(u16 *)(c + 0x4ede) = 0xc000;

    if (*(u8 *)(c + 0x4ee9) == 0)
        func_02012718((void *)0x1d2, *(int *)(c + 0x4eb0));

    *(u8 *)(c + 0x4ee9) = 6;
    *(int *)(c + 0x4ecc) = 0;
    *(int *)(c + 0x4ed0) = 0;
    *(int *)(c + 0x4eb8) = *(int *)(c + 0x4eb0) + *(int *)(c + 0x4ec0);
    *(int *)(c + 0x4ebc) = *(int *)(c + 0x4eb4) + *(int *)(c + 0x4ec4);
    *(int *)(c + 0x4ed4) = 0xff;
    *(u8 *)(c + 0x4eea) = 0;
}
