/* storage 0x023955A4 runtime 0x027C751C size 0x98 main. Pool +0x98: 0380FFF4. */

extern void fn_0239477C(int, int);

void fn_023955A4(void) {
    int base;

    base = *(int *)0x0380FFF4;
    *(int *)(base + 0x3A4) += 1;
    fn_0239477C(1, 10);
    fn_0239477C(2, 0x12);
    fn_0239477C(1, 0x11);
    base = *(int *)0x0380FFF4;
    if (*(unsigned short *)((char *)base + 0x100 + 0xC0) != 0)
        fn_0239477C(2, 0x13);
    base = *(int *)0x0380FFF4;
    if (*(unsigned short *)((char *)base + 0x300 + 0xAC) != 0)
        fn_0239477C(2, 0x15);
}
