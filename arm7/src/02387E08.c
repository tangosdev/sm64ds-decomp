extern int dat_038087A0[];
extern void fn_02387F00(unsigned int, unsigned int);
extern int fn_02387FA0(int);

int fn_02387E08(int *a1)
{
    int *v2 = dat_038087A0;
    int v8;
    int v3 = (int)(a1 + 8);
    volatile int *v4 = (volatile int *)0x4100010;
    volatile int *v5 = (volatile int *)0x40001A4;
    unsigned int mask = ~0x1FF;
    unsigned int v9;
    int v10;
    unsigned int v6;
    unsigned int v7;
    do {
        while (1) {
            int t;
            int result;
            v6 = v2[7];
            v7 = v6 & mask;
            if (v7 != v6 || ((v8 = v2[8]) & 3) != 0 || (unsigned int)v2[9] < 0x200) {
                v8 = v3;
                a1[2] = v7;
            }
            fn_02387F00((v7 >> 8) | 0xB7000000, v7 << 24);
            v9 = 0;
            *v5 = a1[1];
            do {
                v10 = *v5;
                if ((v10 & 0x800000) != 0) {
                    t = *v4;
                    if (v9 < 0x200)
                        *(int *)(v8 + 4 * v9++) = t;
                }
            } while ((v10 & 0x80000000) != 0);
            if (v8 != v2[8])
                break;
            dat_038087A0[7] += 512;
            dat_038087A0[8] += 512;
            result = dat_038087A0[9] - 512;
            dat_038087A0[9] = result;
            if (result == 0)
                return result;
        }
        v8 = fn_02387FA0((int)a1);
    } while (v8 != 0);
    return v8;
}
