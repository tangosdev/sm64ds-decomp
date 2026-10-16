#pragma exceptions off

/* storage 0x02383374 runtime 0x037FB20C size 0x40 wram ARM.
 * Pools: =0x4000304, =0x7AB80. Calls fn_02389874, fn_023833BC (asm veneer),
 * fn_02382B28, fn_02383438. */

extern int fn_02389874(int);
extern void fn_023833BC(int);
extern int fn_02382B28(int);
extern int fn_02383438(int);

int fn_02383374(void) {
    *(unsigned short *)0x4000304 |= 1;
    fn_02389874(1);
    fn_023833BC(0x100);
    return fn_02383438(fn_02382B28(0x7AB80));
}
