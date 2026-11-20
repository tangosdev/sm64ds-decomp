/* storage 0x0238AF68 runtime 0x03802E00 size 0x40 wram. No pool. */

extern void fn_02383210(unsigned int);
extern int fn_0238A77C(int, int, int, int);

int fn_0238AF68(void) {
    unsigned short value;

    fn_02383210(0x8000);
    value = (unsigned short)((value & ~1) | 1);
    return fn_0238A77C(6, 0, (int)&value, 1);
}
