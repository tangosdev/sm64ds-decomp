extern int func_020124c4(int a, int b, int c, int d);
extern short data_02082214[];

#pragma opt_common_subs off

void func_ov006_021019e0(char *c, int i)
{
    {
        unsigned short angle = *(unsigned short *)(c + (i << 6) + 0x5200 + 0x90);
        long long t0 = (long long)data_02082214[(angle >> 4) * 2 + 1] * 0x50000 + 0x800;
        *(int *)(c + (i << 6) + 0x5000 + 0x260) = (int)(t0 >> 12) + 0x80000;
    }

    {
        unsigned short angle = *(unsigned short *)(c + (i << 6) + 0x5200 + 0x90);
        long long t1 = (long long)data_02082214[(angle >> 4) * 2] * 0x50000 + 0x800;
        *(int *)(c + (i << 6) + 0x5000 + 0x264) = (int)(t1 >> 12) - 0x80000;
    }

    if (*(unsigned char *)(c + (i << 6) + 0x5000 + 0x29b) != 0) {
        *(unsigned short *)(c + 0x5290 + (i << 6)) -= 0x40;
    } else {
        *(unsigned short *)(c + 0x5290 + (i << 6)) += 0x40;
    }

    *(int *)(c + (i << 6) + 0x5000 + 0x28c) = func_020124c4(*(int *)(c + (i << 6) + 0x5000 + 0x28c), 2, 0x198, 0);
}
