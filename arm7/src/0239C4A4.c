/* storage 0x0239C4A4 runtime 0x027CE41C size 0xE8 main. */

extern void fn_023969D8(void *a1, void *a2);
extern int fn_0239E91C(void *a1, int a2);
extern void fn_0239F948(void *a1);
extern int fn_0239F778(int a1);

int fn_0239C4A4(int a1, int a2) {
    unsigned char *base;
    unsigned short v4;
    int v6;

    *(unsigned short *)(a2 + 2) = 4;
    base = *(unsigned char **)0x0380FFF4;
    v4 = *(unsigned short *)(base + 754);
    if (v4 != 3 && v4 != 2 && v4 != 1)
        return 11;
    if (*(unsigned short *)(base + 784) < 0x30)
        return 1;
    if ((unsigned short)(v4 + 0xFFFE) <= 1 && (*(unsigned short *)(a1 + 16) & 1) != 0)
        return 5;
    fn_023969D8((void *)(a2 + 6), (void *)(a1 + 16));
    v6 = fn_0239E91C((void *)(a2 + 6), *(unsigned short *)(a1 + 22));
    if (v6 == 0)
        return 8;
    fn_0239F948((void *)(a1 + 16));
    *(int *)(*(char **)0x0380FFF4 + 980) = a1;
    *(int *)(*(char **)0x0380FFF4 + 984) = a2;
    *(unsigned short *)(*(char **)0x0380FFF4 + 956) = 65;
    fn_0239F778(v6);
    return 128;
}
