/* storage 0x0238DEA8 runtime 0x03805D40 linked 0x4C wram ARM.
 * Take the lock, wait until bit 0 clears, then drop the lock. */

extern void fn_023817E0(int arg);
extern void fn_02388CD8(int arg);
extern int fn_0238176C(int arg);

int fn_0238DEA8(void) {
    fn_023817E0(0x027D8074);
    while (*(volatile unsigned short *)0x027FFF96 & 1)
        fn_02388CD8(0x100);
    return fn_0238176C(0x027D8074);
}
