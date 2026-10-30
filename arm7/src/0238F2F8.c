/* storage 0x0238F2F8 runtime 0x027C1270 size 0x90 main. */

extern int *fn_0238E688(void);
extern int fn_0237CF84(int a1);

int fn_0238F2F8(int a1, int a2, int a3) {
    int i;
    unsigned int v5;
    unsigned int result;
    int **v4;
    int v8;
    char *p;

    v4 = (int **)fn_0238E688();
    v5 = *(unsigned short *)((char *)*(v4[1412]) + 186);
    if (a2 != 0) {
        if (v5 > 4)
            v5 = 4;
    } else {
        result = *(unsigned short *)((char *)a1 + 20);
        if (result < 1)
            return (int)result;
        i = 0;
        p = (char *)a1 + 26;
        for (; i < *(unsigned short *)((char *)a1 + 20); ++i) {
            v8 = fn_0237CF84(*(unsigned char *)(p + *(unsigned short *)((char *)a1 + 22) * i + 3));
            if (v8 < v5)
                v5 = v8;
        }
    }
    result = (unsigned int)*(v4[1412]);
    *(unsigned short *)(result + 186) = (unsigned short)v5;
    return (int)result;
}
