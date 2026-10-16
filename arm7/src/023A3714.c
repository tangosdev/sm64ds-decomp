#pragma opt_propagation off

extern void fn_0239AE84(int a, int b);

int fn_023A3714(unsigned short *a1) {
    unsigned char *base;
    unsigned short *p;
    unsigned short *rec;
    int ok1;
    int ok2;

    base = *(unsigned char **)0x0380FFF4;
    p = (unsigned short *)(base + 0x308);
    if (*(unsigned short *)(base + 0x310) != 64)
        return 1;
    if (a1[17] == p[46] && a1[16] == p[45] && a1[15] == p[44])
        ok1 = 1;
    else
        ok1 = 0;
    if (ok1 == 0)
        goto fail;
    if (a1[20] == p[61] && a1[19] == p[60] && a1[18] == p[59])
        ok2 = 1;
    else
        ok2 = 0;
    if (ok2 == 0)
        goto fail;
    goto body;
fail:
    return 1;
body:
    a1[3] = a1[8] - 28;
    rec = a1 - 8;
    rec[6] = 0x185;
    rec[7] = 24;
    {
        unsigned short *slot = (unsigned short *)((char *)rec + 0x18);
        unsigned short cur = *slot;
        int bit94 = *(unsigned short *)0x04808094 & 0x8000;
        int *gp = (int *)0x0380FFF4;
        unsigned char *b = (unsigned char *)(*gp + 0x400);
        unsigned short mid = *(unsigned short *)(b + 0x9E);
        int bit98 = *(unsigned short *)0x04808098 & 0x8000;
        int mixed = mid | (bit98 >> 4);
        mixed = mixed | (bit94 >> 3);
        *slot = (unsigned short)(cur | mixed);
        fn_0239AE84(*gp + 0x14C, (int)rec);
    }
    return 0;
}
