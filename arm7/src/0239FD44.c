/* storage 0x0239FD44 runtime 0x027D1CBC linked 0x108 main ARM.
 * Stream/channel setup: bump the seq counter, clear a flag, init MMIO unless
 * the busy bit is set, then dispatch on state. */
extern void fn_0239D638(void *, int);
extern int fn_0239A650(int);
extern int fn_023A07BC(int);
void fn_0239FD44(int a1, int a2) {
    ++*(int *)(*(char **)0x0380FFF4 + 0x508);
    fn_0239D638(*(char **)0x0380FFF4 + 0x170, a1 - 16);
    *(unsigned short *)(*(char **)0x0380FFF4 + 0x40C) = 0;
    if ((((unsigned)*(unsigned short *)((char *)*(void **)(*(char **)0x0380FFF4 + 0x414) + 0xC) << 18) >> 31) == 0) {
        *(volatile unsigned short *)0x48080AC = 8;
        *(volatile unsigned short *)0x48080AE = 5;
        if (a2 != 0) {
            if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x16C) != 0)
                fn_023A07BC(1);
            if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x160) != 0)
                fn_023A07BC(0);
        }
    }
    if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x178) != 0) {
        if (a2 != 0)
            fn_023A07BC(2);
    } else {
        fn_0239A650(0);
    }
}
