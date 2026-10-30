/* storage 0x0239C58C runtime 0x027CE504 size 0xF4 main. */

extern void fn_023969F4(int a1);
extern void fn_023969D8(void *a1, void *a2);
extern void fn_0239BCA0(void);

int fn_0239C58C(unsigned short *a1, unsigned short *a2) {
    unsigned char *base;
    unsigned short v4;
    unsigned short v;

    a2[1] = 6;
    base = *(unsigned char **)0x0380FFF4;
    v4 = *(unsigned short *)(base + 754);
    if (v4 != 3 && v4 != 2)
        return 11;
    if (*(unsigned short *)(base + 784) < 0x20)
        return 1;
    if ((a1[8] & 1) != 0)
        return 5;
    if (a1[11] > 1)
        return 5;
    v = a1[12];
    if (v > 0x7D0)
        return 5;
    if (v < 0xA)
        return 5;
    fn_023969F4(32);
    *(int *)(*(char **)0x0380FFF4 + 980) = (int)a1;
    *(int *)(*(char **)0x0380FFF4 + 984) = (int)a2;
    *(unsigned short *)(*(char **)0x0380FFF4 + 956) = 48;
    a2[7] = *(unsigned short *)(*(int *)(*(char **)0x0380FFF4 + 980) + 22);
    fn_023969D8(a2 + 4, (void *)(*(int *)(*(char **)0x0380FFF4 + 980) + 16));
    fn_0239BCA0();
    return 128;
}
