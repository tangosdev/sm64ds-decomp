typedef unsigned char u8;

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

extern int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);

void func_ov006_02123938(char *c)
{
    int v;
    unsigned w;

    if (*(short *)(c + 0x7ba4) == 0 || *(u8 *)(c + 0x7bab) != 0) {
        *(u8 *)(c + 0x7baa) = 0;
        return;
    }

    v = 0;
    if (data_020a0de8[data_020a0e40 * 4] != 0) {
        if (data_020a0de9[data_020a0e40 * 4] != 0)
            v = 1;
    }
    if (v != 0) {
        *(short *)(c + 0x7b9c) = data_020a0dea[(unsigned int)data_020a0e40 * 4];
        *(short *)(c + 0x7ba0) = *(short *)(c + 0x7b9c);
        *(short *)(c + 0x7b9e) = data_020a0deb[(unsigned int)data_020a0e40 * 4];
        *(short *)(c + 0x7ba2) = *(short *)(c + 0x7b9e);
        *(u8 *)(c + 0x7baa) = 1;
        *(int *)(c + 0x7b8c) = 0;
    }

    if (*(u8 *)(c + 0x7baa) != 1)
        return;

    if (data_020a0de8[data_020a0e40 * 4] != 0) {
        func_ov004_020ae5c4(c, *(short *)(c + 0x7b9c), *(short *)(c + 0x7b9e),
                            data_020a0dea[data_020a0e40 * 4],
                            data_020a0deb[data_020a0e40 * 4], 2, 4);
        *(short *)(c + 0x7b9c) = data_020a0dea[data_020a0e40 * 4];
        *(short *)(c + 0x7b9e) = data_020a0deb[data_020a0e40 * 4];
        *(int *)(c + 0x7b8c) = func_02012468(*(int *)(c + 0x7b8c), 2, 0x1b0, 2, 0,
                                             func_020126e8(*(short *)(c + 0x7b9c) << 0xc),
                                             0, 0);
        return;
    }
    w = data_020a0de8[data_020a0e40 * 4] == 0
        && data_020a0de9[data_020a0e40 * 4] != 0;
    if (w != 0)
        *(u8 *)(c + 0x7bab) = 1;
}
