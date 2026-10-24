/* storage 0x0238ADB8 runtime 0x03802C50 size 0x2C wram. No pool. */

extern void fn_02383210(unsigned int);
extern int fn_0238A77C(int, int, int, int);

int fn_0238ADB8(int arg) {
    fn_02383210(0x8000);
    return fn_0238A77C(6, 0x20, arg, 4);
}
