/* storage 0x0239A284  runtime 0x027CC1FC  linked 0xF8  main ARM.
 * Slot-claim helper over the state block at dat_0380FFF4: index a1 picks a
 * 0x1C-byte entry in the u16 table at +0x2E0, bumps the +0x4E8 count while
 * the slot is free, clears 0x1A bytes, sets bit a1 in the +0x4EC mask,
 * copies a2 into +4, then mirrors the session fields at +0x300. */

typedef struct {
    char _000[0x5E];
    unsigned short f_35E;
    char _060[0x14];
    unsigned short f_374;
} Dat300;

extern char *dat_0380FFF4;
extern int fn_02382C80(int, void *, int);
extern int fn_0239A8F8(unsigned short, int);
extern void fn_023969D8(void *, void *);
extern int fn_0239A98C(unsigned short, int);

void fn_0239A284(int a1, void *a2) {
    volatile unsigned short zero;
    unsigned short *v4 =
        (unsigned short *)(*(char **)(dat_0380FFF4 + 0x2E0) + a1 * 0x1C);
    if (*v4 == 0)
        ++*(unsigned short *)(dat_0380FFF4 + 0x4E8);
    zero = 0;
    fn_02382C80(zero, v4, 0x1A);
    fn_0239A8F8(a1, 0);
    *(unsigned short *)(dat_0380FFF4 + 0x4EC) |= 1 << a1;
    fn_023969D8(v4 + 2, a2);
    v4[10] = 0xFFFF;
    v4[8] = ((Dat300 *)(dat_0380FFF4 + 0x300))->f_35E;
    v4[9] = ((Dat300 *)(dat_0380FFF4 + 0x300))->f_374;
    v4[12] = v4[13];
    fn_0239A98C(a1, 0x20);
}
