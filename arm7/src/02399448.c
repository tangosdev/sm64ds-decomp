#pragma exceptions off

/* storage 0x02399448 runtime 0x027CB3C0 size 0x30 main ARM.
 * Pools: =0xFFFF, =0x48081AC, =0x4808010. Callee fn_023965A8. */

extern void fn_023965A8(void);

unsigned short *fn_02399448(void) {
    unsigned short *p;
    fn_023965A8();
    *(unsigned short *)0x48081AC = 0xFFFF;
    p = (unsigned short *)0x4808010;
    *p = 0x20;
    return p;
}
