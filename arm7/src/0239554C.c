/* storage 0x0239554C runtime 0x027C74C4 size 0x4C main.
 * Pool +0x4C: 0380FFF4, 000005EC, 000082EA. */

extern void fn_023954C4(void);
extern int fn_023822A8(int, unsigned int, int, int, int);

int fn_0239554C(unsigned int scale, int arg) {
    int base;

    fn_023954C4();
    base = *(int *)0x0380FFF4;
    return fn_023822A8(base + 0x5EC, (scale * 0x82EAu) >> 6, 0, arg, 0);
}
