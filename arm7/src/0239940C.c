#pragma exceptions off

/* storage 0x0239940C runtime 0x027CB384 size 0x30 main ARM.
 * Pools: =0xFFFF, =0x48081AC, =0x4808010. Callee fn_023965A8. */

extern void fn_023965A8(void);

unsigned short *fn_0239940C(void) {
    unsigned short *p;
    fn_023965A8();
    *(unsigned short *)0x48081AC = 0xFFFF;
    p = (unsigned short *)0x4808010;
    *p = 0x10;
    return p;
}
