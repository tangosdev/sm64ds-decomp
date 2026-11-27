#pragma exceptions off

/* storage 0x02399834 runtime 0x027CB7AC size 0xA0 main ARM.
 * Pools: =0x380FFF4, =0x4808010, =0x316.  Calls fn_0239477C. */

extern void fn_0239477C(int, int);

int fn_02399834(void) {
    unsigned short *v0;
    int result;

    v0 = (unsigned short *)(*(int *)0x380FFF4 + 0x308);
    *(unsigned short *)0x4808010 = 0x8000;
    if (v0[4] == 0x40 &&
        v0[57] != 0 &&
        v0[51] == v0[50] &&
        (unsigned short)++v0[58] > (unsigned int)v0[57]) {
        v0[58] = 0;
        fn_0239477C(1, 13);
    }
    result = *(unsigned short *)(*(int *)0x380FFF4 + 0x316) | 2;
    *(unsigned short *)(*(int *)0x380FFF4 + 0x316) = result;
    return result;
}
