extern int fn_02381600(int a1, int a2, int a3);
extern int *dat_27D7A84[];
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

void fn_0238E5A8(int a1, unsigned short *a2, int a3) {
    unsigned short *v5;
    if (a3 == 0) {
        if (fn_02381600(0x27D6BE0, (int)a2, 0) == 0 && dat_27D7A84[388] != 0) {
            v5 = fn_0238DE4C();
            v5[0] = *a2;
            v5[1] = 8;
            v5[2] = 0;
            v5[3] = 0;
            fn_0238DEF4((int)v5);
        }
    }
}
