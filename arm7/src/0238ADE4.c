/* storage 0x0238ADE4 runtime 0x03802C7C size 0x2C wram. No pool. */

extern void fn_02383210(unsigned int);
extern int fn_0238A77C(int, int, int, int);

int fn_0238ADE4(int arg) {
    fn_02383210(0x8000);
    return fn_0238A77C(0x86, 0x20, arg, 4);
}
