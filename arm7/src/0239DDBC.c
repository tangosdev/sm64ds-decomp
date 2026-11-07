/* storage 0x0239DDBC runtime 0x027CFD34 linked 0x2B0 main ARM. */
extern int fn_0239AD50(unsigned short *a1);
extern void fn_023969D8(unsigned short *dst, unsigned short *src);
extern void fn_0239AB20(unsigned short *rec);
extern void fn_02394D84(void *a, void *b, int c);
extern void fn_023A07BC(int arg);
extern void fn_0239ABA0(void *a, void *b);
extern void fn_023A06F0(void);

#define BASE (*(unsigned char **)0x0380FFF4)

int fn_0239DDBC(unsigned char *a1) {
    unsigned short *v3;
    unsigned short *v4;
    int v8;
    v3 = (unsigned short *)(BASE + 776);
    v4 = (unsigned short *)(a1 + 16);
    if (*(unsigned short *)(a1 + 22) > 0x5E4)
        return 5;
    switch (*(unsigned short *)(BASE + 754)) {
    case 1:
        v4[2] = *(unsigned int *)(BASE + 932);
        v8 = fn_0239AD50(v4 + 12);
        if (v8 == 255 || *(unsigned short *)(*(unsigned char **)(BASE + 736) + 28 * v8) != 64)
            return 10;
        v4[1] = (unsigned short)v8;
        if ((unsigned short)(v4[7] & 0xFF) == 0) {
            v4[8] = *(unsigned short *)(*(unsigned char **)(BASE + 736) + 28 * v8 + 18);
        } else {
            v4[8] = v4[7];
            v4[7] = 0;
        }
        if (v4[3] == 0) {
            v4[10] = (unsigned short)((v3[63] | 0x40) & ~0x4000);
            v4[9] = 28;
        } else {
            v4[10] = v3[63];
            if (*(unsigned short *)(BASE + 760) == 0)
                v4[9] = (unsigned short)(v4[3] + 28);
            else
                v4[9] = (unsigned short)(v4[3] + 36);
        }
        fn_023969D8(v4 + 18, v4 + 15);
        fn_023969D8(v4 + 15, v3 + 44);
        if (v8 == 0) {
            fn_0239ABA0(BASE + 452, a1);
            if ((*(unsigned short *)(BASE + 1258) & ~*(unsigned short *)(BASE + 1262)) == 0)
                fn_023A06F0();
        } else {
            fn_0239AB20(v4);
            fn_02394D84(BASE + 452, BASE + 344, (int)a1);
            fn_023A07BC(0);
        }
        break;
    case 2:
    case 3:
        v4[2] = *(unsigned int *)(BASE + 932);
        v4[1] = v3[62];
        if ((unsigned short)(v4[7] & 0xFF) != 0)
            v4[8] = v4[7];
        else
            v4[8] = *(unsigned short *)(*(unsigned char **)(BASE + 736) + 28 * (unsigned short)v3[62] + 18);
        if (v4[3] == 0) {
            v4[10] = (unsigned short)((v3[63] | 0x40) & ~0x4000);
            v4[9] = 28;
        } else {
            v4[10] = v3[63];
            if (*(unsigned short *)(BASE + 760) == 0)
                v4[9] = (unsigned short)(v4[3] + 28);
            else
                v4[9] = (unsigned short)(v4[3] + 36);
        }
        fn_023969D8(v4 + 18, v4 + 12);
        fn_023969D8(v4 + 12, v3 + 44);
        fn_0239AB20(v4);
        fn_02394D84(BASE + 452, BASE + 344, (int)a1);
        fn_023A07BC(0);
        break;
    }
    return 129;
}
