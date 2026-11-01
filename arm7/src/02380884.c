/* storage 0x02380884 runtime 0x037F871C size 0x34 + 8 pool wram ARM.
 * Two constant pools (0x04000208, 0x04000210), no calls. The dead re-read of
 * [R12] before the restore is a volatile re-read the compiler keeps. The AND
 * is written out (`v & ~a1`, not `&=`) so b56 keeps MVN+AND instead of BIC,
 * and r2 is declared first so both bases land on R12/R2. */

int fn_02380884(int a1) {
    volatile unsigned int *r2 = (volatile unsigned int *)0x4000210;
    volatile unsigned short *r1 = (volatile unsigned short *)0x4000208;
    unsigned short save = *r1;
    int v;
    *r1 = 0;
    v = *r2;
    *r2 = v & ~a1;
    (void)*r1;
    *r1 = save;
    return v;
}
