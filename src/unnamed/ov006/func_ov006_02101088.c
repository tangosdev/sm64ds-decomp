
#pragma opt_common_subs off
void func_ov006_02101088(char *p, int i)
{
    int d;
    *(int *)(p + 0x5264 + (i << 6)) += *(int *)(p + 0x5000 + (i << 6) + 0x26c);
    d = (*(int *)(p + 0x5000 + (i << 6) + 0x264) - *(int *)(p + 0x5000 + (i << 6) + 0x280)) >> 12;
    if (d >= 0x40) {
        if (*(int *)(p + 0x5000 + (i << 6) + 0x26c) >= 0x1000)
            *(int *)(p + 0x526c + (i << 6)) -= 0x100;
    } else if (*(int *)(p + 0x5000 + (i << 6) + 0x26c) <= 0x4000) {
        *(int *)(p + 0x526c + (i << 6)) += 0x200;
    }
    if (d < 0x60) return;
    *(unsigned short *)(p + 0x5200 + (i << 6) + 0x92) = 0x10;
    *(int *)(p + 0x5000 + (i << 6) + 0x26c) = 0;
    *(unsigned char *)(p + 0x5000 + (i << 6) + 0x296) = 0xb;
}
