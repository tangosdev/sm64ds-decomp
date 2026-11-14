/* storage 0x0238DE4C runtime 0x03805CE4 linked 0x5C wram ARM.
 * Take the lock, wait until bit 0 of 0x027FFF96 is clear, set it,
 * and return the word at the published pointer + 8. */

extern void fn_023817E0(int arg);
extern void fn_02388CD8(int arg);
extern int unk_27D7A84[];

int fn_0238DE4C(void) {
    unsigned short *reg = (unsigned short *)0x027FFF96;
    fn_023817E0(0x027D8074);
    while (*reg & 1)
        fn_02388CD8(0x100);
    *reg |= 1;
    return *(int *)(unk_27D7A84[0x184] + 8);
}
