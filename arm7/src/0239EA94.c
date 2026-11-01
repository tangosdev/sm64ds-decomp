/* storage 0x0239EA94 runtime 0x027D0A0C size 0xF8 main. */

extern char *fn_02383C7C(char *a1, int a2);
extern void fn_02383DC4(int a1);
extern void fn_023833C8(char *dst, char *src, int len);
extern void fn_023857D8(char *a1, short *a2);
extern int fn_0239E83C(char *a1);
extern int fn_0239E784(char *a1);
extern int fn_0239E73C(char *a1);
extern int fn_0239D3F8(char *a1);

unsigned short *fn_0239EA94(short *a1) {
    char *base;
    char *mem;
    unsigned short *v4;
    int v6;
    int v7;
    int v8;

    base = *(char **)0x0380FFF4;
    mem = fn_02383C7C(base + 0x14C, *(unsigned short *)(base + 0x300 + 0x9C) + 120);
    if (mem == 0) {
        fn_02383DC4(2);
        return (unsigned short *)mem;
    }
    *(unsigned short *)(mem + 12) = 0xFFFF;
    v4 = (unsigned short *)(mem + 16);
    fn_023833C8(*(char **)0x0380FFF4 + 0x64C, (char *)v4, 44);
    v4[26] = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x6A);
    v4[27] = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x78);
    v6 = fn_0239E83C((char *)(v4 + 28));
    v7 = v6 + fn_0239E784((char *)(v4 + 28) + v6);
    v8 = v7 + fn_0239E73C((char *)(v4 + 28) + v7);
    v4[3] = (unsigned short)(v8 + fn_0239D3F8((char *)(v4 + 28) + v8) + 12);
    v4[9] = (unsigned short)(v4[3] + 28);
    v4[10] = 80;
    fn_023857D8((char *)v4 + 24, a1);
    return v4;
}
