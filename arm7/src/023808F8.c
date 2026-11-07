/* storage 0x023808F8 runtime 0x037F8790 size 0x2C + 8 pool wram ARM.
 * Two constant pools (0x04000208, 0x04000210), no calls. */

int fn_023808F8(int a1) {
    volatile unsigned short *r1 = (volatile unsigned short *)0x4000208;
    volatile unsigned int *r2 = (volatile unsigned int *)0x4000210;
    unsigned short save = *r1;
    int v;
    *r1 = 0;
    v = *r2;
    *r2 = a1;
    (void)*r1;
    *r1 = save;
    return v;
}
