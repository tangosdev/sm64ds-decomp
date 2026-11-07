#pragma exceptions off

/* storage 0x023999D0 runtime 0x027CB948 size 0x70 main ARM.
 * Pools: =0x380FFF4. Callees fn_02394E7C, fn_02395434. */

extern int fn_02394E7C(unsigned short *, int);
extern void fn_02395434(unsigned short *, unsigned short *, int);

unsigned short *fn_023999D0(unsigned short *a1, int a2) {
    int v4;
    unsigned short *v6;
    v4 = fn_02394E7C((unsigned short *)(*(int *)0x380FFF4 + 0x14C), a2 + 0x22);
    if (v4 == 0)
        return 0;
    v6 = (unsigned short *)(v4 + 0x10);
    fn_02395434(v6 + 4, a1, a2 + 0xC);
    v6[3] = a2 - 0x18;
    v6[7] = ((unsigned char)v6[7] | (unsigned int)((unsigned char)v6[9] << 16) >> 8);
    return v6;
}
