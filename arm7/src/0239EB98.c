/* storage 0x0239EB98 runtime 0x027D0B10 size 0xA4 main. */

extern char *fn_02383C7C(char *a1, int a2);
extern void fn_02383DC4(int a1);
extern void fn_023833C8(char *dst, char *src, int len);
extern void fn_023857D8(char *a1, short *a2);
extern int fn_0239E83C(char *a1);
extern int fn_0239E784(char *a1);

unsigned short *fn_0239EB98(short *a1) {
    char *base;
    char *mem;
    unsigned short *v4;
    int v5;
    int v6;

    base = *(char **)0x0380FFF4;
    mem = fn_02383C7C(base + 0x14C, 90);
    if (mem == 0) {
        fn_02383DC4(2);
        return (unsigned short *)mem;
    }
    *(unsigned short *)(mem + 12) = 0xFFFF;
    v4 = (unsigned short *)(mem + 16);
    fn_023833C8(*(char **)0x0380FFF4 + 0x64C, (char *)v4, 44);
    v5 = fn_0239E83C((char *)(v4 + 22));
    v6 = v5 + fn_0239E784((char *)(v4 + 22) + v5);
    v4[3] = (unsigned short)v6;
    v4[9] = (unsigned short)(v6 + 28);
    v4[10] = 64;
    fn_023857D8((char *)v4 + 24, a1);
    return v4;
}
