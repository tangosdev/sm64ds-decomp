#pragma exceptions off

/* storage 0x023833CC runtime 0x037FB264 size 0x40 wram ARM.
 * Pool: =0x4000304. Calls fn_02383420, fn_02383410 (asm veneer),
 * fn_02382B28, fn_02389848. */

extern int fn_02383420(void);
extern void fn_02383410(int);
extern void fn_02382B28(int);
extern int fn_02389848(int);

int fn_023833CC(void) {
    unsigned short *p;
    int result;
    fn_02383420();
    fn_02383410(0x80);
    fn_02382B28(0x40000);
    fn_02389848(1);
    p = (unsigned short *)0x4000304;
    result = *p & ~1;
    *p = result;
    return result;
}
