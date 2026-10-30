/* storage 0x0238C1C8 runtime 0x03804060 size 0xAC wram ARM.
 * Compares 78 halfwords at a1 against slot-2 ROM +4; entries 0x4C/0x4D
 * are masked with ~0x84/~3 before compare. Holds an OS lock over the read.
 * opt_propagation off keeps m^=K un-folded into eor+truncate chains and
 * keeps rom+4 as a split-base add; masks inside the loop let LICM hoist
 * them last, which is what puts m3 in lr and m2 in r8. */

#pragma opt_propagation off
extern int fn_0238104C(void);   /* local extern: OS lock id, owned by another job */
extern void fn_02381174(int);  /* local extern: lock acquire */
extern void fn_02381040(int);  /* local extern: lock release thunk */
extern void fn_023810E4(int);  /* local extern: lock release */

int fn_0238C1C8(int a1) {
    unsigned char *rom = (unsigned char *)0x8000000;
    unsigned short *b = (unsigned short *)(rom + 4);
    int v;
    int ok;
    ok = 1;
    v = (unsigned short)fn_0238104C();
    fn_02381174(v);
    int i;
    for (i = 0; i < 0x4E; ++i) {
        unsigned short m = 0xFFFF;
        if (i == 0x4C)
            m ^= 0x84;
        else if (i == 0x4D)
            m ^= 3;
        if ((m & *(unsigned short *)(a1 + 2 * i)) != b[i]) {
            ok = 0;
            break;
        }
    }
    fn_02381040(v);
    fn_023810E4(v);
    return ok;
}
