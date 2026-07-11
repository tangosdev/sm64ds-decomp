extern int func_020124c4(int a, int b, int c, int d);

#pragma opt_common_subs off

void func_ov006_02101224(char *c, int i)
{
    *(int *)(c + (i << 6) + 0x5000 + 0x28c) = func_020124c4(*(int *)(c + (i << 6) + 0x5000 + 0x28c), 2, 0x198, 0);

    if (*(unsigned short *)(c + (i << 6) + 0x5200 + 0x92) != 0) {
        *(unsigned short *)(c + 0x5292 + (i << 6)) -= 1;
        if (*(short *)(c + (i << 6) + 0x5200 + 0x92) < 0)
            *(short *)(c + (i << 6) + 0x5200 + 0x92) = 0;
        return;
    }

    if (*(int *)(c + 0x5000 + 0x668) == 3)
        *(unsigned char *)(c + (i << 6) + 0x5000 + 0x296) = 3;
    else
        *(unsigned char *)(c + (i << 6) + 0x5000 + 0x296) = 2;
}
