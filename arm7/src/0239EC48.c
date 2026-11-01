/* storage 0x0239EC48 runtime 0x027D0BC0 size 0x198 main. */

extern char *fn_02383C7C(char *a1, int a2);
extern void fn_02383DC4(int a1);
extern void fn_023833C8(char *dst, char *src, int len);
extern int fn_02399630(int a1);
extern void fn_023857D8(char *a1, short *a2);
extern int fn_02395334(char *a1);
extern void fn_02395354(int addr, int val);
extern int fn_0239E784(int a1);

int fn_0239EC48(int a1, int a2, char *a3) {
    int v6;
    char *v8;
    int v16;
    unsigned int i;

    v6 = (int)fn_02383C7C(*(char **)0x0380FFF4 + 332, 96);
    if (v6 == 0) {
        fn_02383DC4(2);
        return v6;
    }
    *(unsigned short *)(v6 + 12) = 0xFFFF;
    v8 = (char *)v6 + 16;
    if (a2 == 0) {
        v6 = fn_02399630(a1);
        if (v6 == 0)
            a2 = 19;
    } else {
        v6 = 0;
    }
    fn_023833C8(*(char **)0x0380FFF4 + 0x64C, v8, 44);
    *(unsigned short *)(v8 + 44) = *(unsigned short *)(*(char **)0x0380FFF4 + 888);
    *(unsigned short *)(v8 + 46) = (unsigned short)a2;
    *(unsigned short *)(v8 + 48) = (unsigned short)(v6 | 0xC000);
    *(unsigned short *)(v8 + 6) = (unsigned short)(fn_0239E784((int)(v8 + 50)) + 6);
    *(unsigned short *)(v8 + 18) = (unsigned short)(*(unsigned short *)(v8 + 6) + 28);
    *(unsigned short *)(v8 + 20) = 48;
    fn_023857D8(v8 + 24, (short *)(*(int *)(*(char **)0x0380FFF4 + 736) + 28 * a1 + 4));
    a1 = (int)v8 + 20 + *(unsigned short *)(v8 + 18);
    if (a3 != 0) {
        a2 = fn_02395334(a3 + 1);
        v16 = fn_02395334(a3);
        fn_02395354(a1, v16);
        fn_02395354(a1 + 1, a2 & 0xFF);
        a1 = a1 + 2;
        i = 0;
        while (i < (unsigned int)a2) {
            v16 = fn_02395334((char *)(a3 + 2) + i);
            fn_02395354(a1, v16);
            ++a1;
            ++i;
        }
    } else {
        fn_02395354(a1, 0);
        fn_02395354(a1 + 1, 0);
    }
    return (int)v8;
}
