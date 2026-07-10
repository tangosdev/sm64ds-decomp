typedef unsigned char u8;
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern void func_ov006_02102864(char *base, int arg);

void func_ov006_02102f3c(char *base, int arg1)
{
    u8 idx;
    int off;
    int has;
    char *e;
    int a, b;

    idx = data_020a0e40[0];
    off = idx * 4;
    has = 0;
    if (data_020a0de8[off])
    {
        if (data_020a0de9[off])
            has = 1;
    }
    if (has != 0)
    {
        e = base + arg1 * 0x40 + 0x4000;
        a = (*(int *)(e + 0x660) >> 12) - data_020a0dea[idx * 4];
        b = (*(int *)(e + 0x664) >> 12) - data_020a0deb[idx * 4];
        *(u8 *)(e + 0x699) = 1;
        *(int *)(e + 0x670) = a << 12;
        *(int *)(e + 0x674) = b << 12;
        *(u8 *)(e + 0x69b) = 0;
        arg1 = (int)e;
    }
    func_ov006_02102864(base, arg1);
}
