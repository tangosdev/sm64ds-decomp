/* storage 0x0239D720 runtime 0x027CF698 linked 0x504 main ARM. */
extern void fn_02386958(void *a, void *b, void *c, void *d);
extern void fn_02385400(void *dst, void *src, unsigned int len);
extern int fn_02394DF8(int a1, int a2);
extern int fn_02394E7C(char *a1, int a2);
extern int fn_02382AC4(void);
extern int fn_02382AD8(int a1);
extern int fn_0238D7A0(int a1, int a2);

int fn_0239D720(unsigned char *a1, int a2) {
    unsigned char *base;
    unsigned int v7;
    unsigned char *v4;
    int v5;
    int v14;
    int v9;
    int v10;
    unsigned int v11;
    int v13;
    unsigned short *v15;
    unsigned short *v16;
    int v17;
    int v18;
    unsigned short v19;
    int v20;
    int v22;
    int v23;
    int v24;
    int v25;
    unsigned int v26;
    unsigned int prod;
    unsigned int v12;

    base = *(unsigned char **)0x0380FFF4;
    v4 = base + 0x3E4;
    v5 = 1;
    *(unsigned short *)(a2 + 2) = 1;
    base = *(unsigned char **)0x0380FFF4;
    if (*(unsigned short *)(base + 0x2F2) != 1)
        return 11;
    if (*(unsigned short *)(v4 + 60) != 0)
        return 8;
    v7 = *(unsigned int *)(v4 + 68);
    if ((*(unsigned short *)(a1 + 16) & 0x8000) != 0) {
        if ((*(unsigned short *)(a1 + 16) & 2) == 0)
            *(unsigned short *)(a1 + 20) = *(unsigned short *)(v4 + 148);
        if ((*(unsigned short *)(a1 + 16) & 4) == 0)
            *(unsigned short *)(a1 + 22) = *(unsigned short *)(v4 + 160);
        if ((*(unsigned short *)(a1 + 16) & 8) == 0)
            *(unsigned short *)(a1 + 24) = *(unsigned short *)(v4 + 150);
        if ((*(unsigned short *)(a1 + 16) & 0x10) == 0) {
            *(unsigned short *)(a1 + 28) = *(unsigned short *)(v4 + 156);
            v5 = 0;
        }
    }
    if (*(unsigned short *)(a1 + 28) > 0x204)
        return 5;
    v9 = 2;
    v10 = 0;
    while (v9 != 0) {
        if ((*(unsigned short *)(a1 + 22) & v9) != 0)
            ++v10;
        v9 = (unsigned short)(2 * v9);
    }
    *(unsigned short *)(v4 + 148) = *(unsigned short *)(a1 + 20);
    v11 = *(unsigned short *)(a1 + 20);
    if ((v11 & 0x8000) != 0) {
        *(unsigned short *)(a1 + 20) &= 0x7FFF;
        v11 = (unsigned)(*(unsigned short *)(a1 + 20) - 208) >> 2;
        *(unsigned short *)(v7 + 14) = 234;
        if (v11 > 0x10000)
            return 5;
    } else if ((*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 1564) & 2) != 0) {
        if (++v11 > 0x207)
            return 5;
        *(unsigned short *)(a1 + 20) = 4 * v11 + 210;
        *(unsigned short *)(v7 + 14) = 240;
    } else {
        if (v11 > 0x206)
            return 5;
        *(unsigned short *)(a1 + 20) = 4 * v11 + 208;
        *(unsigned short *)(v7 + 14) = 234;
    }
    v12 = (v11 + 9) & 0xFFFFFFFE;
    prod = v12 * v10;
    v13 = fn_02394E7C(*(char **)0x0380FFF4 + 332, prod + 26);
    *(int *)(v4 + 144) = v13;
    if (*(int *)(v4 + 144) == 0)
        return 8;
    *(unsigned short *)(v4 + 60) = 1;
    ++*(unsigned short *)(v4 + 62);
    *(unsigned short *)(v4 + 150) = *(unsigned short *)(a1 + 24);
    *(unsigned short *)(v4 + 152) = *(unsigned short *)(a1 + 22);
    *(unsigned short *)(v4 + 154) = 0;
    *(unsigned short *)(v4 + 156) = *(unsigned short *)(a1 + 28);
    *(unsigned short *)(v4 + 158) = *(unsigned short *)(a1 + 18);
    *(unsigned short *)v7 = 0;
    *(unsigned short *)(v7 + 2) = *(unsigned short *)(a1 + 22);
    *(unsigned short *)(v7 + 4) = 0;
    *(unsigned short *)(v7 + 8) = 20;
    *(unsigned short *)(v7 + 10) = *(unsigned short *)(a1 + 28) + 34;
    *(unsigned short *)(v7 + 12) = 552;
    *(unsigned short *)(v7 + 14) += (*(unsigned short *)(a1 + 20) + 10) * v10;
    fn_02386958((void *)(v7 + 16), (void *)0x027D650C, (void *)(*(unsigned char **)0x0380FFF4 + 864), (void *)(*(unsigned char **)0x0380FFF4 + 744));
    if (((*(unsigned short *)(a1 + 16)) & 0x8000) != 0 && *(unsigned short *)(v7 + 34) != 0xFFFF)
        v14 = 0x4000;
    else {
        v14 = 0;
        *(unsigned short *)(v7 + 34) = 0xFFFF;
    }
    *(unsigned short *)(v7 + 36) = *(unsigned short *)(a1 + 20);
    *(unsigned short *)(v7 + 38) = *(unsigned short *)(a1 + 22);
    *(unsigned short *)(v7 + 40) = *(unsigned short *)(a1 + 30);
    if (v5 != 0 && *(unsigned short *)(a1 + 28) != 0)
        fn_02385400((void *)(v7 + 42), *(void **)(a1 + 32), *(unsigned short *)(a1 + 28));
    if ((*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 1564) & 4) != 0) {
        v15 = (unsigned short *)((v7 + 40) + (*(unsigned short *)(a1 + 28) + 2));
        v15 = (unsigned short *)(((int)v15 + 3) & 0xFFFFFFFC);
        *v15 = 0xB6B8;
        v15[1] = 0x1D46;
    }
    *(unsigned short *)(*(int *)(v4 + 144) + 12) = 388;
    *(unsigned short *)(*(int *)(v4 + 144) + 14) = (prod + 11) >> 1;
    *(unsigned short *)(*(int *)(v4 + 144) + 16) = *(unsigned short *)(a1 + 22);
    *(unsigned short *)(*(int *)(v4 + 144) + 20) = v10;
    *(unsigned short *)(*(int *)(v4 + 144) + 22) = v12;
    *(unsigned short *)(*(int *)(v4 + 144) + 24) = 0;
    v16 = (unsigned short *)(*(int *)(v4 + 144) + 26);
    v17 = 1;
    v18 = 2;
    v19 = 0xFFFF;
    while (v18 != 0) {
        if ((*(unsigned short *)(a1 + 22) & v18) != 0) {
            *v16 = v19;
            v16[1] = 0;
            v16[3] = 0;
            v16[2] = v17;
            v16 = (unsigned short *)((char *)v16 + v12);
        }
        v18 = (unsigned short)(2 * v18);
        ++v17;
    }
    *(unsigned short *)0x048080C4 = *(unsigned short *)(a1 + 20);
    *(unsigned short *)0x048080C0 = *(unsigned short *)(v7 + 14);
    v20 = 0x10000 - *(unsigned short *)(a1 + 26);
    if (*(unsigned short *)(a1 + 24) == 0) {
        v22 = ((*(unsigned short *)(a1 + 28) + 34) * 4) + 96;
        v25 = *(unsigned short *)(a1 + 20) * v10 + 904;
        v25 = v22 + v25 + 50;
        v22 = fn_0238D7A0(v25, 10);
        v23 = fn_02382AC4();
        *(unsigned short *)0x04808118 = v22;
        *(unsigned short *)0x04808090 = ((v7 & 0x3FFF) >> 1) | 0x8000 | v14;
        fn_02382AD8(v23);
    } else {
        v24 = fn_02382AC4();
        v25 = fn_0238D7A0((v20 + *(unsigned short *)0x048080F8) & 0xFFFF, 10);
        v26 = *(unsigned short *)(a1 + 24);
        if (v25 + 3 < v26) {
            *(unsigned short *)0x04808118 = v26 - v25 - 1;
            *(unsigned short *)0x04808090 = ((v7 & 0x3FFF) >> 1) | 0x8000 | v14;
            fn_02382AD8(v24);
        } else {
            fn_02382AD8(v24);
            fn_02394DF8(*(int *)0x0380FFF4 + 332, *(int *)(v4 + 144));
            *(unsigned short *)(v4 + 60) = 0;
            --*(unsigned short *)(v4 + 62);
            return 5;
        }
    }
    return 0;
}
