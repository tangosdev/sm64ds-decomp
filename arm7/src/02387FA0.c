extern int dat_038087A0[];
extern void fn_02382DC0(void *, void *, unsigned int);

int fn_02387FA0(int a1)
{
    int *v1 = dat_038087A0;
    unsigned int v2 = (unsigned int)v1[7] & ~0x1FF;
    if (v2 == *(unsigned int *)(a1 + 8)) {
        unsigned int v3 = (unsigned int)v1[7] - v2;
        unsigned int v4 = 0x200 - v3;
        if (v4 > (unsigned int)v1[9])
            v4 = v1[9];
        fn_02382DC0((void *)(a1 + 0x20 + v3), (void *)v1[8], v4);
        v1[7] += v4;
        v1[8] += v4;
        v1[9] -= v4;
    }
    return v1[9] != 0;
}
