/* storage 0x023A2344 runtime 0x027D42BC size 0x398 main. */

extern void fn_02382C80(int val, void *dst, int n);
extern void fn_02382CB4(int val, void *dst, int n);
extern void fn_023A17C8(void *a1);
extern unsigned int fn_02395334(const void *a1);
extern void fn_02395354(void *a1, unsigned int a2);
extern void fn_023969D8(unsigned short *dst, const unsigned short *src);
extern void fn_023954C4(unsigned int a1, unsigned int a2, int a3, int a4);
extern void fn_0239477C(int a1, int a2);

extern unsigned short unk_023A458C[];

void fn_023A2344(unsigned short *a1, int *a2) {
    unsigned short *v4;
    unsigned short *v5;
    unsigned short *state;
    unsigned int v7;
    unsigned short *v6;
    unsigned int v8;
    unsigned short *v9;
    char *v9b, *v9c;
    unsigned short *v4a, *v4b;
    unsigned short *ctr;
    unsigned int i, j, k, v18;
    unsigned int v11;
    unsigned int v23, v24;
    int v12;
    int v15, v19, v20;
    volatile unsigned short v25;
    volatile int v26;
    int req[10];

    state = (unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x3BC);
    if (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x300 + 0xBC) == 0x13) {
        v4 = *(unsigned short **)(*(char **)0x0380FFF4 + 0x3D8);
        v5 = a1 + 22;
        v6 = (unsigned short *)((char *)*(unsigned char **)((char *)state + 0x18) + 0x4E);
        v7 = 0;
        v8 = *(unsigned short *)((char *)*(unsigned char **)((char *)state + 0x18) + 0x4C);
        while (v7 < v8) {
            if (v6[2] == a1[20] && v6[1] == a1[19] && *v6 == a1[18]) {
                *(unsigned short *)((char *)*(unsigned char **)((char *)state + 0x1C) + 6) |= 1 << v7;
                return;
            }
            v6 += 3;
            v7++;
        }
        v9 = v4 + 5;
        for (v8 = 0; v8 < v4[4]; v8++) {
            if (a1[20] == v9[4] && a1[19] == v9[3] && a1[18] == v9[2])
                return;
            v9 += *v9;
        }
        v25 = 0;
        fn_02382C80(v25, v9, 68);
        v11 = a1[3];
        if (v11 > 0xC) {
            if (a2 == 0) {
                v26 = 0;
                fn_02382CB4(v26, req, 40);
                req[0] = (int)(v5 + 6);
                ((unsigned short *)req)[4] = (unsigned short)(v11 - 12);
                ((unsigned short *)req)[5] = 3;
                ((unsigned short *)req)[6] = 56;
                ((unsigned short *)req)[2] = a1[4];
                ((unsigned short *)req)[3] = v5[5];
                fn_023A17C8(req);
                a2 = req;
            }
            v12 = a2[9];
            if (v12 != 0)
                v9[30] = fn_02395334((char *)v12 + 1) - 8;
            if ((((unsigned short *)a2)[5] & 1) == 1
                && *(unsigned short *)(*(char **)0x0380FFF4 + 0x3C0) >= (((unsigned int)v9[30] + 0x41) >> 1)) {
                v9[22] = v5[5];
                fn_023969D8(v9 + 2, a1 + 18);
                v9[25] = v5[4];
                v9[1] = (unsigned char)a1[9];
                if (a2[9] != 0) {
                    for (j = 0, v9b = (char *)v9 + 64; j < v9[30]; j++)
                        fn_02395354(v9b + j,
                                    fn_02395334((char *)a2[9] + 10 + j));
                }
                v15 = a2[6];
                if (v15 != 0) {
                    v9[5] = fn_02395334((char *)v15 + 1);
                    for (k = 0, v9c = (char *)v9 + 12; k < v9[5]; k++)
                        fn_02395354(v9c + k,
                                    fn_02395334((char *)a2[6] + 2 + k));
                } else {
                    v18 = 0;
                    v9[5] = 0;
                    v9c = (char *)v9 + 12;
                    do {
                        fn_02395354(v9c + v18, 0);
                    } while (++v18 < 0x20);
                }
                v9[23] = ((unsigned short *)a2)[10];
                v9[24] = ((unsigned short *)a2)[11];
                v9[27] = ((unsigned short *)a2)[9];
                v19 = a2[7];
                if (v19 != 0)
                    v9[28] = fn_02395334((char *)v19 + 3);
                v20 = a2[8];
                if (v20 != 0)
                    v9[26] = fn_02395334((char *)v20 + 3);
                *v9 = (unsigned short)(((unsigned int)v9[30] + 0x41) >> 1);
                v4a = v4 + 1;
                *v4a = (unsigned short)(*v4a + *v9);
                v4b = v4 + 4;
                ++*v4b;
                ctr = (unsigned short *)(*(char **)0x0380FFF4 + 0x3C0);
                v23 = *ctr;
                *ctr = (unsigned short)(v23 - *v9);
                v24 = *(unsigned short *)(*(char **)0x0380FFF4 + 0x3C0);
                if (v24 < 0x20) {
                    fn_023954C4(v24, v23, 0x380FFF4, (int)ctr);
                    *(unsigned short *)(*(char **)0x0380FFF4 + 0x3BC) = 21;
                    fn_0239477C(2, 0);
                }
            } else {
                fn_023969D8(v9 + 2, unk_023A458C);
            }
        }
    }
}
