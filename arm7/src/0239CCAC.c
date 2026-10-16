/* storage 0x0239CCAC runtime 0x027CEC24 size 0x104 main. */

extern int fn_02396D44(int a1);

int fn_0239CCAC(unsigned short *a1, unsigned short *a2) {
    int v3;
    unsigned int v4, v6, v7;
    unsigned short *v9;
    unsigned short *v10;

    v3 = *(int *)(*(char **)0x0380FFF4 + 0x2E0);
    a2[1] = 1;
    v4 = a1[8];
    if (v4 >= *(unsigned short *)(*(char **)0x0380FFF4 + 0x2E6) && v4 != 0xFFFF)
        return 5;
    v6 = a1[10];
    if (v6 > 0x3F && v6 != 0xFFFF)
        return 5;
    if (v4 == 0xFFFF) {
        for (v7 = 1; v7 < *(unsigned short *)(*(char **)0x0380FFF4 + 0x2E6); v7++) {
            v9 = (unsigned short *)(v3 + 28 * v7);
            v9[13] = a1[9];
            if (v9[12] != 0)
                v9[12] = a1[9];
        }
    } else if (a1[8] != 0) {
        *(unsigned short *)(v3 + 28 * v4 + 26) = a1[9];
        v10 = (unsigned short *)(v3 + 24 + 28 * a1[8]);
        if (*v10 != 0)
            *v10 = a1[9];
    }
    if (a1[10] != 0)
        fn_02396D44(a1[10]);
    return 0;
}
