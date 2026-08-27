#pragma opt_common_subs off

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_02100f7c(char *c, int i)
{
    int d;
    *(int *)(c + 0x5264 + (i << 6)) += *(int *)(c + 0x5000 + (i << 6) + 0x26c);
    d = *(int *)(c + 0x5000 + (i << 6) + 0x264) >> 12;
    if (*(int *)(c + 0x5000 + (i << 6) + 0x26c) <= 0x4000)
        *(int *)(c + 0x526c + (i << 6)) += 0x200;
    if (d < 0xa0)
        return;
    *(int *)(c + 0x5000 + (i << 6) + 0x264) = 0xa0000;
    *(unsigned char *)(c + 0x5000 + (i << 6) + 0x296) = 0xa;
    *(int *)(c + 0x5000 + (i << 6) + 0x26c) = 0;
    *(unsigned short *)(c + 0x5200 + (i << 6) + 0x92) = 0x40;
    if ((((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) << 1) >> 15) {
        *(int *)(c + 0x5000 + (i << 6) + 0x268) = 0x1000;
        *(unsigned char *)(c + 0x5000 + (i << 6) + 0x29a) = 0;
    } else {
        *(int *)(c + 0x5000 + (i << 6) + 0x268) = -0x1000;
        *(unsigned char *)(c + 0x5000 + (i << 6) + 0x29a) = 1;
    }
}
