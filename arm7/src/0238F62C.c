/* storage 0x0238F62C runtime 0x027C15A4 size 0x110 main. */

extern int *fn_0238E688(void);
extern short *fn_0238DE4C(void);
extern int fn_0238DEF4(short *);
extern void fn_02382C98(void *, void *, int);
extern int fn_02381D00(void *a1, void *a2, int a3);
extern int unk_27D80B0;

void fn_0238F62C(int a1) {
    int *v2;
    char *a60;
    int v3;
    unsigned short *v7;
    unsigned int v8;
    int r;

    v2 = *(int **)((char *)fn_0238E688() + 0x1610);
    a60 = (char *)a1 + 60;
    v3 = *(unsigned short *)*v2;
    if (v3 == 8 || v3 == 10) {
        if (((unsigned short *)*v2)[91] != *(unsigned short *)(a60 + 8)) {
            unk_27D80B0 = 36;
            (&unk_27D80B0)[1] = 1;
            r = fn_02381D00((char *)fn_0238E688() + 0x15C, &unk_27D80B0, 0);
            if (r == 0) {
                v7 = (unsigned short *)fn_0238DE4C();
                v7[0] = 128;
                v7[1] = 8;
                v7[2] = 22;
                v7[3] = 36;
                fn_0238DEF4((short *)v7);
            }
        } else {
            v7 = (unsigned short *)fn_0238DE4C();
            v7[0] = 128;
            v7[1] = 0;
            v7[2] = 16;
            v7[3] = *(unsigned short *)(a60 + 8);
            v7[4] = *(unsigned short *)*v2;
            v7[5] = *(unsigned short *)(a1 + 22);
            v8 = v7[5];
            if (v8 <= 0x80)
                fn_02382C98(a60, v7 + 6, (v8 + 1) & ~1);
            fn_0238DEF4((short *)v7);
        }
    }
}
