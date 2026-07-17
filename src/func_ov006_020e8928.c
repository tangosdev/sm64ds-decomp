typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#pragma opt_common_subs off

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020e8928(char *o, int idx)
{
    if (*(u16 *)(o + idx * 0x20 + 0x52d2) != 0) {
        *(u16 *)((char *)(((int)o + 0x52d2) & 0xFFFFFFFFFFFFFFFF) + idx * 0x20) -= 1;
        if (*(short *)(o + idx * 0x20 + 0x52d2) < 0)
            *(u16 *)(o + idx * 0x20 + 0x52d2) = 0;
    } else {
        int rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        rnd = (int)((((u32)rnd) << 5) >> 15);
        *(u16 *)(o + idx * 0x20 + 0x52cc) = rnd + 0x40;
        *(u8 *)(o + idx * 0x20 + 0x52d9) = 1;
        *(u8 *)(o + idx * 0x20 + 0x52da) = 0;
        *(u8 *)(o + idx * 0x20 + 0x52d8) = 0;
        rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        *(int *)(o + idx * 0x20 + 0x52bc) = (int)((((u32)rnd) << 5) >> 15) << 15;
        rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        *(int *)(o + idx * 0x20 + 0x52c0) = ((int)((((u32)rnd) << 4) >> 15) << 3) + 0x40 << 12;
        *(int *)(o + idx * 0x20 + 0x52c8) = -0x200;
        *(u8 *)(o + idx * 0x20 + 0x52d7) = 0;
    }
}
