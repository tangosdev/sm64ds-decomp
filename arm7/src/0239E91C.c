/* storage 0x0239E91C runtime 0x027D0894 size 0x98 main. */

extern char *fn_02383C7C(char *a1, int a2);
extern void fn_02383DC4(int a1);
extern void fn_023833C8(char *dst, char *src, int len);
extern void fn_023857D8(char *a1, short *a2);

unsigned short *fn_0239E91C(short *a1, short a2) {
    char *base;
    char *mem;
    unsigned short *v6;

    base = *(char **)0x0380FFF4;
    mem = fn_02383C7C(base + 0x14C, 54);
    if (mem == 0) {
        fn_02383DC4(2);
        return (unsigned short *)mem;
    }
    *(unsigned short *)(mem + 12) = 0xFFFF;
    v6 = (unsigned short *)(mem + 16);
    base = *(char **)0x0380FFF4;
    fn_023833C8(base + 0x64C, (char *)v6, 44);
    v6[22] = (unsigned short)a2;
    v6[3] = 2;
    v6[9] = (unsigned short)(v6[3] + 28);
    v6[10] = 192;
    fn_023857D8((char *)v6 + 24, a1);
    return v6;
}
