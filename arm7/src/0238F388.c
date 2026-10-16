/* storage 0x0238F388 runtime 0x027C1300 size 0x270 main. */

extern int *fn_0238E688(void);
extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);
extern void fn_0238DEA8(void);
extern void fn_02382DC0(void *a1, void *a2, int a3);
extern int fn_0238F2F8(int a1, int a2, int a3);
extern int fn_02392F18(int a1, int a2, void *a3, int a4, void *a5);
extern int fn_02393128(int a1, int a2);
extern int fn_02381D00(void *a1, void *a2, int a3);
extern int fn_02382AC4(void);
extern int fn_02382AD8(int a1);
extern int unk_27D80B8;
extern int unk_27D80C0;

void fn_0238F388(int a1) {
    unsigned int v6;
    int v2;
    unsigned short *v4;
    unsigned short *v5;
    int v3;
    unsigned int v7;
    int *slot;
    int idx;

    v2 = *(int *)*(int **)((char *)fn_0238E688() + 0x1610);
    v3 = 0;
    if (*(int *)(v2 + 16) != 0) {
        idx = ((unsigned short *)v2)[33];
        slot = &((int *)v2)[idx];
        v4 = *(unsigned short **)((char *)slot + 72);
        fn_02382DC0((char *)a1 + 16, v4, ((unsigned short *)v2)[35]);
        fn_0238F2F8(a1, *v4, ((unsigned short *)v2)[73]);
        v5 = v4 + 5;
        v6 = v3;
        while (v6 < v4[2]) {
            v7 = *v5;
            if (v7 >= 2 && v7 != 0xFFFF) {
                *v5 = (unsigned short)(v7 - 2);
                *(unsigned short *)(v2 + 134) |= 1 << v5[2];
                if (*v5 != 0)
                    fn_02392F18(v5[2], v5[4], v5 + 5, *v5, v4);
            }
            v6 = (unsigned short)(v6 + 1);
            v5 = (unsigned short *)((char *)v5 + v4[3]);
        }
        fn_02393128(0, *v4);
        if (*v4 != 0 && ((unsigned short *)v2)[75] == 0 && ((unsigned short *)v2)[30] != 0)
            v3 = 1;
        {
            short *m = fn_0238DE4C();
            m[0] = 14;
            m[1] = 0;
            m[2] = 11;
            ((int *)m)[2] = (int)v4;
            fn_0238DEF4(m);
        }
        fn_0238DEA8();
        if (((unsigned short *)a1)[9] != 0 && ((unsigned short *)v2)[109] == 0) {
            unk_27D80C0 = 36;
            (&unk_27D80C0)[1] = ((unsigned short *)a1)[9];
            if (fn_02381D00((char *)fn_0238E688() + 0x15C, &unk_27D80C0, 0) == 0) {
                short *m2 = fn_0238DE4C();
                m2[0] = 128;
                m2[1] = 8;
                m2[2] = 22;
                m2[3] = 36;
                fn_0238DEF4(m2);
            }
        }
        *(unsigned short *)(v2 + 66) ^= 1;
        {
            int v12 = fn_02382AC4();
            int v13;
            if (v3 == 0)
                --*(unsigned short *)(v2 + 62);
            v13 = *(unsigned short *)(v2 + 62) != 0;
            fn_02382AD8(v12);
            if (v13) {
                int *v15;
                if (v3 == 1)
                    v3 = ((unsigned short *)a1)[8];
                else
                    v3 = 0xFFFF;
                v15 = fn_0238E688();
                unk_27D80B8 = 37;
                (&unk_27D80B8)[1] = v3;
                if (fn_02381D00((char *)v15 + 0x15C, &unk_27D80B8, 0) == 0 && v15[1412] != 0) {
                    short *m3 = fn_0238DE4C();
                    m3[0] = 128;
                    m3[1] = 8;
                    m3[2] = 22;
                    m3[3] = 37;
                    fn_0238DEF4(m3);
                }
            }
        }
    }
}


