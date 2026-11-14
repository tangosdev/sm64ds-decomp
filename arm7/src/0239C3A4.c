/* storage 0x0239C3A4 runtime 0x027CE31C size 0x100 main. */

extern void fn_023969F4(int a1);
extern void fn_02396A98(void);
extern void fn_0239BBA4(void);

int fn_0239C3A4(unsigned short *a1, int a2) {
    unsigned char *base;
    unsigned short v4;
    unsigned short v;

    *(unsigned short *)(a2 + 2) = 3;
    base = *(unsigned char **)0x0380FFF4;
    v4 = *(unsigned short *)(base + 754);
    if (v4 != 3 && v4 != 2)
        return 11;
    if (*(unsigned short *)(base + 784) < 0x30)
        return 1;
    if ((a1[8] & 1) != 0)
        return 5;
    v = a1[11];
    if (v == 0)
        return 5;
    if (v > 0xFF)
        return 5;
    v = a1[12];
    if (v > 0x7D0)
        return 5;
    if (v < 0xA)
        return 5;
    fn_023969F4(48);
    fn_02396A98();
    *(unsigned short *)(*(char **)0x0380FFF4 + 876) = a1[11];
    *(unsigned short *)(*(char **)0x0380FFF4 + 878) = a1[11];
    *(int *)(*(char **)0x0380FFF4 + 980) = (int)a1;
    *(int *)(*(char **)0x0380FFF4 + 984) = a2;
    *(unsigned short *)(*(char **)0x0380FFF4 + 956) = 80;
    fn_0239BBA4();
    return 128;
}
