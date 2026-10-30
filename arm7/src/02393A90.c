extern void *fn_0238E688(void);
extern int fn_0239355C(int a1, int a2, unsigned short *a3, unsigned short *a4, unsigned short *a5, unsigned short *a6);
extern int fn_02392E58(int a1, int a2, int a3);
extern int fn_0238FE74(void *a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, void *a10);

int fn_02393A90(int a1) {
    unsigned short *v2;
    unsigned short *v4;
    int z9;
    unsigned short v7;
    int v8;
    int v3;
    int v5;
    int w9;
    int v10;
    int res;
    unsigned short v11;
    unsigned short v12;
    unsigned short v13;
    int v14[128];

    {
        int *p = (int *)((char *)fn_0238E688() + 0x1000);
        v2 = *(unsigned short **)(p[0x184]);
    }
    v3 = 0;
    v4 = *(unsigned short **)((char *)v2 + 80);
    v5 = v2[199];
    if (v2[68] == 1) {
        a1 = 0xFFFF;
        v2[68] = v3;
    }
    if (v2[77] == 1) {
        a1 = 0xFFFF;
        v3 = 1;
        v5 = 0;
        v2[77] = v5;
    }
    res = fn_0239355C(a1, v5, &v13, v4, &v11, &v12);
    if (res == 1)
        return res;
    v7 = (unsigned short)(v2[200] + 2);
    v8 = *(unsigned short *)0x48080F8;
    if (v2[31] == 1) {
        z9 = 0;
        w9 = fn_02392E58(v12, v7, a1);
        v13 |= 0x8000;
    } else {
        z9 = 0;
        w9 = 0;
    }
    if (v2[73] == 1) {
        if (v3 == 0) {
            v10 = (unsigned short)(v11 & v2[191]);
        } else {
            v10 = v2[191];
            v7 = 0;
            w9 = v7;
            v2[31]++;
            v13 &= ~0x8000;
        }
    } else {
        v10 = (unsigned short)(a1 & v2[191]);
    }
    return fn_0238FE74(v14, 0, z9, v7, v10, w9, v8, v12, v13, v4);
}
