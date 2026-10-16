/* storage 0x0239E9C0 runtime 0x027D0938 size 0xC4 main. */

extern char *fn_02383C7C(char *a1, int a2);
extern void fn_02383DC4(int a1);
extern void fn_023833C8(char *dst, char *src, int len);
extern void fn_02395354(int addr, int val);
extern void fn_023857D8(char *a1, short *a2);

unsigned short *fn_0239E9C0(short *a1, int a2) {
    char *base;
    char *mem;
    unsigned short *v6;
    int v3 = a2;

    base = *(char **)0x0380FFF4;
    mem = fn_02383C7C(base + 0x14C, v3 + 0x13A);
    if (mem == 0) {
        fn_02383DC4(2);
        return (unsigned short *)mem;
    }
    *(unsigned short *)(mem + 12) = 0xFFFF;
    v6 = (unsigned short *)(mem + 16);
    base = *(char **)0x0380FFF4;
    fn_023833C8(base + 0x64C, (char *)v6, 44);
    if (v3 != 0) {
        fn_02395354((int)((char *)v6 + 50), 16);
        fn_02395354((int)((char *)v6 + 51), v3 & 0xFF);
        v3 = (unsigned short)(v3 + 2);
    }
    v6[3] = (unsigned short)(v3 + 6);
    v6[9] = (unsigned short)(v6[3] + 28);
    v6[10] = 176;
    fn_023857D8((char *)v6 + 24, a1);
    return v6;
}
