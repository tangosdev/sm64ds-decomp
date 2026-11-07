/* storage 0x0239C680 runtime 0x027CE5F8 size 0x1D8 main. */

extern void fn_023969F4(int a1);
extern int fn_023A43D8(int a1);
extern int fn_023978A4(int a1);
extern void fn_0239763C(unsigned short *a1);
extern void fn_02397508(int a1, unsigned short *a2);
extern void fn_023974A8(int a1);
extern void fn_023970C8(int a1, int a2);
extern void fn_0239707C(unsigned short *a1);
extern int fn_0238477C(int a1, int a2);

int fn_0239C680(unsigned short *a1, int a2) {
    unsigned char *base;
    unsigned short v4;
    unsigned short v6;
    unsigned short v7;
    unsigned short v8;
    unsigned short v9;

    *(unsigned short *)(a2 + 2) = 5;
    base = *(unsigned char **)0x0380FFF4;
    v4 = *(unsigned short *)(base + 754);
    if (v4 != 3 && v4 != 2)
        return 11;
    if (*(unsigned short *)(base + 784) < 0x20)
        return 1;
    fn_023969F4(32);
    if ((a1[12] & 1) != 0)
        return 5;
    if (a1[15] == 0)
        return 5;
    if (a1[15] > 0x20)
        return 5;
    v6 = a1[35];
    if (v6 < 0xA)
        return 5;
    if (v6 > 0x3E8)
        return 5;
    if (a1[36] > 0xFF)
        return 5;
    v7 = a1[37];
    if ((v7 & 0xFFF0) != 0)
        return 5;
    if (((1 << v7) & *(unsigned short *)(*(char **)0x0380FFF4 + 752)) == 0)
        return 5;
    v8 = a1[33];
    if ((v8 & ~0xFFF) != 0)
        return 5;
    v9 = a1[34];
    if ((v9 & ~0xFFF) != 0)
        return 5;
    if (v8 == 0)
        return 5;
    if ((v9 | v8) == 0)
        return 5;
    if (a1[8] > 0x7D0)
        return 5;
    if (fn_023A43D8(0) != 0)
        return 14;
    if ((a1[32] & 0x20) == 0)
        fn_023978A4(0);
    fn_0239763C(a1 + 12);
    fn_02397508(a1[15], a1 + 16);
    fn_023974A8(a1[35]);
    fn_023970C8(a1[37], 0);
    fn_0239707C(a1 + 33);
    *(int *)(*(char **)0x0380FFF4 + 980) = (int)a1;
    *(int *)(*(char **)0x0380FFF4 + 984) = a2;
    *(unsigned short *)(*(char **)0x0380FFF4 + 956) = 32;
    fn_0238477C(2, 1);
    return 128;
}
