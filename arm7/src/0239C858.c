/* storage 0x0239C858 runtime 0x027CE7D0 size 0x17C main. */

extern int fn_02395334(void *a1);
extern void fn_0239763C(unsigned short *a1);
extern void fn_02397508(int a1, unsigned short *a2);
extern int fn_0238477C(int a1, int a2);

int fn_0239C858(unsigned short *a1, unsigned short *a2) {
    unsigned char *base;
    unsigned short v7;
    unsigned short v9;
    unsigned int v10;
    int v12;

    *(unsigned short *)(*(char **)0x0380FFF4 + 960) = (unsigned short)(a2[1] - 2);
    a2[1] = 3;
    base = *(unsigned char **)0x0380FFF4;
    v7 = *(unsigned short *)(base + 754);
    if (v7 != 1 && v7 != 3 && v7 != 2)
        return 11;
    if (*(unsigned short *)(base + 784) < 0x20)
        return 1;
    if (a1[11] > 0x20)
        return 5;
    if (a1[28] > 1)
        return 5;
    if (fn_02395334(a1 + 29) == 0)
        return 5;
    v9 = a1[37];
    if (v9 > 0x3E8)
        return 5;
    if (v9 < 0xA)
        return 5;
    if (a1[38] > 0x10)
        return 5;
    v10 = 0;
    while (v10 < 0x10) {
        v12 = fn_02395334((char *)(a1 + 29) + v10);
        if (v12 == 0)
            break;
        if ((v12 & 0xF0) != 0)
            return 5;
        if (((1 << v12) & *(unsigned short *)(*(char **)0x0380FFF4 + 752)) == 0)
            return 5;
        ++v10;
    }
    fn_0239763C(a1 + 8);
    fn_02397508(a1[11], a1 + 12);
    *(int *)(*(char **)0x0380FFF4 + 980) = (int)a1;
    *(int *)(*(char **)0x0380FFF4 + 984) = (int)a2;
    *(unsigned short *)(*(char **)0x0380FFF4 + 956) = 16;
    fn_0238477C(2, 0);
    return 128;
}


