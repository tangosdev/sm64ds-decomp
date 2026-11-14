/* storage 0x0239E73C runtime 0x027D06B4 size 0x44 main. */

extern void fn_02395354(int addr, int val);

int fn_0239E73C(int a1) {
    char *base;

    fn_02395354(a1, 3);
    fn_02395354(a1 + 1, 1);
    base = *(char **)0x0380FFF4;
    fn_02395354(a1 + 2, *(unsigned short *)(base + 0x300 + 0x76) & 0xFF);
    return 3;
}
