/* storage 0x023978A4 runtime 0x027C981C linked 0xDC main ARM. */

extern void fn_02396FE8(void);

int fn_023978A4(unsigned int a1) {
    int *base;
    unsigned short *slot;
    unsigned int cur;
    unsigned char *blk;

    if (a1 > 1)
        return 5;
    base = *(int **)0x0380FFF4;
    slot = (unsigned short *)((char *)base + 0x2FE);
    cur = *slot;
    cur &= ~4u;
    *slot = cur | ((a1 & 1) << 2);
    if (a1 == 0) {
        slot = (unsigned short *)((char *)*(int **)0x0380FFF4 + 0x378);
        *slot &= ~0x20u;
    } else {
        slot = (unsigned short *)((char *)*(int **)0x0380FFF4 + 0x378);
        *slot |= 0x20;
    }
    base = *(int **)0x0380FFF4;
    blk = (unsigned char *)base + 0x300;
    if (*(unsigned short *)(blk + 0x10) == 0x40 &&
        *(unsigned short *)((char *)base + 0x2F2) == 1) {
        *(unsigned short *)(*(int *)((char *)base + 0x464) + 0x2E) =
            *(unsigned short *)(blk + 0x78);
    }
    if (a1 == 0)
        *(unsigned short *)0x048080BC &= ~6u;
    else
        *(unsigned short *)0x048080BC |= 6;
    fn_02396FE8();
    return 0;
}
