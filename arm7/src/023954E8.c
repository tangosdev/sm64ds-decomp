/* storage 0x023954E8 runtime 0x027C7460 size 0x58 main.
 * Pool +0x58: 000082EA, 0380FFF4, 000005EC. */

extern void fn_023954C4(void);
extern int fn_023822A8(int, unsigned int, int, int, int);

int fn_023954E8(unsigned int scale, int arg) {
    unsigned int quot;
    int base;

    fn_023954C4();
    quot = (scale * 0x82EAu) >> 6;
    quot = quot / 1000;
    base = *(int *)0x0380FFF4;
    return fn_023822A8(base + 0x5EC, quot, 0, arg, 0);
}
