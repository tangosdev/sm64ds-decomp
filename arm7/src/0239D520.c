/* storage 0x0239D520 runtime 0x027CF498 size 0x64 main ARM. */
extern unsigned int unk_380FFF4 /* =0x0380FFF4 */;
void fn_023969F4(int a1) /* =0x027C896C */;

int fn_0239D520(int a1, int a2) {
    unsigned int v2;
    *(unsigned short *)(a2 + 2) = 1;
    v2 = *(unsigned short *)(unk_380FFF4 + 784);
    if (v2 != 0x10) {
        if (v2 != 0x20)
            goto ret1;
        /* bit 2 of the +790 halfword; LSL/LSR pair, no stack temp. */
        if ((unsigned int)(*(unsigned short *)(unk_380FFF4 + 790) << 29) >> 31)
            goto ret1;
    }
    fn_023969F4(32);
    return 0;
ret1:
    /* Single shared return block: a second `return 1` keeps R0 pinned
       and spills the base pointer into R1. */
    return 1;
}
