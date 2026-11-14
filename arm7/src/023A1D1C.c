extern int fn_0239A98C(unsigned int index, unsigned int value);
extern int fn_0239B6F0(void *src, unsigned short val);
extern void fn_023954C4(void);
extern void fn_0239477C(int a1, int a2);
extern unsigned short *fn_0239E9C0(short *a1, int a2);
extern void fn_0239F778(unsigned short *arg);
extern int fn_023A132C(unsigned short *packet);
extern void fn_023A13CC(int index, char *packet);
extern void fn_023969F4(int a1);
extern unsigned int fn_02395334(unsigned int addr);
extern void fn_02382C98(void *a1, void *a2, int a3);

void fn_023A1D1C(unsigned short *a1) {
    unsigned short *v3 = a1 + 22;
    int v8;
    int v6;
    unsigned short v7;
    int v1;
    {
        unsigned short v4;
        if (*(unsigned short *)(*(unsigned char **)(void *)0x0380FFF4 + 1564) & 8) {
            unsigned char *q = *(unsigned char **)(void *)0x0380FFF4 + 996;
            v4 = *(unsigned short *)0x048080B0;
            if (((v4 & 1) == 0 || *(unsigned short *)q == 0)
                && ((v4 & 4) == 0 || *(unsigned short *)(q + 20) == 0)
                && ((v4 & 8) == 0 || *(unsigned short *)(q + 40) == 0)
                && ((*(unsigned short *)0x0480819C & 1) == 0)) {
                *(unsigned short *)0x04808032 = 0;
                *(unsigned short *)0x04808032 = 0x8000;
                *(unsigned short *)(*(unsigned char **)(void *)0x0380FFF4 + 1178) = 0;
            }
        }
    }
    v6 = 0;
    v7 = v3[1] + 1;
    v8 = a1[1];
    if (v8 == 0) {
        v1 = 19;
        v6 = 1;
        goto tail;
    }
    {
        unsigned char *base = *(unsigned char **)(void *)0x0380FFF4;
        int v9;
        if (*(unsigned short *)(base + 788) == 1) {
            v9 = 28 * v8;
            if (*(unsigned short *)(*(unsigned char **)(base + 736) + v9) > 0x20) {
                fn_0239A98C((unsigned short)v8, 0x20);
                fn_0239B6F0(a1 + 15, 1);
            }
            if (a1[4] & 0x400) {
                unsigned char *v23 = *(unsigned char **)((*(unsigned char **)(void *)0x0380FFF4) + 736);
                if (*(unsigned short *)(v23 + v9 + 14) != 0) {
                    v6 = 1;
                    *v3 = v6;
                    v1 = 15;
                    v7 = 4;
                    *(unsigned short *)(*(unsigned char **)((*(unsigned char **)(void *)0x0380FFF4) + 736) + v9 + 14) = 0;
                    goto tail;
                }
            }
        }
    }
    switch (*v3) {
    case 0:
        goto zero;
    case 1:
        goto one;
    default:
        goto not1;
    }
zero:
        if (*(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 788) != 1)
            goto recheck;
        if (*(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 758) == 1) {
            v1 = 13;
            v6 = 1;
            goto tail;
        }
recheck:
        if (*(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 788) != 1)
            goto gate13;
        if (v3[1] == 1) {
            v1 = 0;
            v6 = 1;
        } else {
            v1 = 14;
            v7 = 2;
            v6 = 1;
        }
        goto tail;
gate13:
        if (*(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 788) == 1)
            goto tail;
        if (v3[1] != 2)
            goto tail;
        {
            unsigned short *v13 = *(unsigned short **)((*(unsigned char **)(void *)0x0380FFF4) + 980);
            if (v13[11] != 0)
                goto tail;
            if (v13[10] != a1[17])
                goto tail;
            if (v13[9] != a1[16])
                goto tail;
            if (v13[8] != a1[15])
                goto tail;
            if (*(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 956) != 49)
                goto tail;
            fn_023954C4();
            if (v3[2] == 0) {
                fn_023969F4(48);
                *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 4) = 0;
                *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 6) = 0;
            } else {
                *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 4) = 12;
                *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 6) = v3[2];
            }
            *(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 956) = 53;
            fn_0239477C(2, 2);
            goto tail;
        }
one:
        {
        int v19;
        if (*(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 788) == 1) {
            fn_0239A98C((unsigned short)v8, 0x20);
            v19 = v3[1];
            if (v19 == 1) {
                unsigned short *v20 = fn_0239E9C0((short *)(a1 + 15), 128);
                unsigned short *v21 = v20;
                if (v21 != 0) {
                    v20[22] = *v3;
                    v20[23] = v7;
                    v20[24] = 0;
                    fn_023A13CC(v8, (char *)v20);
                    fn_0239F778(v21);
                }
            } else if (v19 == 3) {
                int t28 = 28 * v8;
                unsigned char *v23 = *(unsigned char **)((*(unsigned char **)(void *)0x0380FFF4) + 736);
                if (*(unsigned short *)(v23 + t28) != 32)
                    goto else3;
                {
                    unsigned v22 = t28;
                    if (*(unsigned short *)(v23 + v22 + 14) == 0)
                        goto else3;
                goto true3;
else3:
                v1 = 1;
                v6 = v1;
                goto tail;
true3:
                if (fn_023A132C(a1) != 0)
                    goto true3in;
                v1 = 15;
                v6 = 1;
                *(unsigned short *)(*(unsigned char **)((*(unsigned char **)(void *)0x0380FFF4) + 736) + v22 + 14) = 0;
                goto tail;
true3in:
                v1 = 0;
                *(unsigned short *)(*(unsigned char **)((*(unsigned char **)(void *)0x0380FFF4) + 736) + v22 + 14) = 0;
                v6 = 1;
                goto tail;
                }
            } else {
                *(unsigned short *)(*(unsigned char **)((*(unsigned char **)(void *)0x0380FFF4) + 736) + 28 * v8 + 14) = 0;
                v1 = 14;
                v7 = 2;
                v6 = 1;
            }
            goto tail;
        }
        {
            unsigned short *v24 = *(unsigned short **)((*(unsigned char **)(void *)0x0380FFF4) + 980);
            int v26;
            int v27;
            unsigned int v30;
            unsigned short *v32;
            if (v24[11] != 1)
                goto tail;
            v26 = (v24[10] == a1[17] && v24[9] == a1[16] && v24[8] == a1[15]);
            if (v26 == 0)
                goto tail;
            v27 = v3[1];
                if (v27 == 2) {
                if (*(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 956) != 49)
                    goto tail;
                if (v3[2] != 0) {
                    fn_023954C4();
                    *(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 956) = 53;
                    *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 4) = 12;
                    *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 6) = v3[2];
                    fn_0239477C(2, 2);
                    fn_023969F4(32);
                } else {
                    *(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 956) = 51;
                    v30 = fn_02395334((unsigned int)((char *)a1 + 51));
                    v32 = fn_0239E9C0((short *)(a1 + 15), v30);
                    if (v32 == 0)
                        goto tail;
                    v32[10] |= 0x4000;
                    fn_02382C98(a1 + 22, v32 + 22, a1[3] + 1);
                    v32[22] = *v3;
                    v32[23] = 3;
                    v32[24] = 0;
                    fn_0239F778(v32);
                }
            } else if (v27 == 4) {
                if (*(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 956) != 51)
                    goto tail;
                fn_023954C4();
                if (v3[2] == 0) {
                    fn_023969F4(48);
                    *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 4) = 0;
                    *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 6) = 0;
                } else {
                    *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 4) = 12;
                    *(unsigned short *)(*(unsigned int *)((*(unsigned char **)(void *)0x0380FFF4) + 984) + 6) = v3[2];
                }
                *(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 956) = 53;
                fn_0239477C(2, 2);
            }
            goto tail;
        }
    }
not1:
    {
        if (*(unsigned short *)((*(unsigned char **)(void *)0x0380FFF4) + 788) == 1) {
            v1 = 13;
            v6 = 1;
        }
    }
tail:
    if (v6 != 0) {
        unsigned short *v34 = fn_0239E9C0((short *)(a1 + 15), 0);
        if (v34 != 0) {
            v34[22] = *v3;
            v34[23] = v7;
            v34[24] = v1;
            fn_0239F778(v34);
        }
    }
}
