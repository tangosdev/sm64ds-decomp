/* storage 0x0238721C runtime 0x037FF0B4 linked 0x48 wram.
 * Disable IRQs, swap the head at unk_38087A0+0x3C, relink, restore.
 * The +0 on the pool address is what hoists that load above the disable
 * call. The ^ 0 folds away, and it is what keeps the old head in r5 and
 * the disable result in r4. */
extern int fn_02382AC4(void);
extern void fn_02382AD8(int prev);
extern void fn_02380E10(void *p, int v);
extern int unk_38087A0[];

int fn_0238721C(int la) {
    volatile int *base = (volatile int *)((int)unk_38087A0 + 0);
    int prev;
    int old;

    prev = fn_02382AC4();
    old = base[15] ^ 0;
    base[15] = la;
    fn_02380E10((void *)(base + 16), base[15]);
    fn_02382AD8(prev);
    return old;
}
