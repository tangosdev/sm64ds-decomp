/* storage 0x0239D670 runtime 0x027CF5E8 linked 0x4C main ARM.
 * Command 1. Bits 0, 1 and 2 of the halfword at +0x10 each call out. */

extern void fn_0239FC90(void);
extern void fn_0238FC40(void);
extern void fn_0238FB90(void);

int fn_0239D670(unsigned short *rec, unsigned short *out) {
    out[1] = 1;
    if (rec[8] & 1)
        fn_0239FC90();
    if (rec[8] & 2)
        fn_0238FC40();
    if (rec[8] & 4)
        fn_0238FB90();
    return 0;
}
