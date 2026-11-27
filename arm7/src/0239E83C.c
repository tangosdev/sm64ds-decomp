/* storage 0x0239E83C runtime 0x027D07B4 size 0x88 main. */

extern void fn_02395354(int addr, int val);
extern int fn_02395334(char *a1);

int fn_0239E83C(int a1) {
    int v2 = 0;
    unsigned int v3;
    char **pp;
    char *base;
    int v5;

    fn_02395354(a1, 0);
    base = *(char **)0x0380FFF4;
    fn_02395354(a1 + 1, *(unsigned short *)(base + 0x300 + 0x1A) & 0xFF);
    v2 += 2;
    v3 = 0;
    pp = (char **)0x0380FFF4;
    while (v3 < *(unsigned short *)(*pp + 794)) {
        v5 = fn_02395334(*pp + 796 + v3);
        fn_02395354(a1 + v2, v5);
        ++v2;
        ++v3;
    }
    return v2;
}
