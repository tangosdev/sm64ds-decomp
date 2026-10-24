/* storage 0x023808C0 runtime 0x037F8758 size 0x30 + 8 pool wram ARM.
 * Two constant pools (0x04000208, 0x04000210), no calls. Same notes as
 * fn_02380884 (explicit `v | a1`, r2 declared first). */

int fn_023808C0(int a1) {
    volatile unsigned int *r2 = (volatile unsigned int *)0x4000210;
    volatile unsigned short *r1 = (volatile unsigned short *)0x4000208;
    unsigned short save = *r1;
    int v;
    *r1 = 0;
    v = *r2;
    *r2 = v | a1;
    (void)*r1;
    *r1 = save;
    return v;
}
