#pragma opt_propagation off
extern unsigned char unk_3807B04[];
extern unsigned char unk_3807D44[];
extern int dword_38075C0;
extern void fn_0238592C(unsigned char *);
extern void fn_02385B24(unsigned char *, int);
extern int fn_02384B94(void);
extern void fn_02385BDC(unsigned char *);
extern void fn_02385BC8(unsigned char *, int, int);
extern void fn_02385EF0(int);
extern int fn_02385E78(unsigned char *);
extern int fn_02385E20(unsigned char *);

int fn_023862C0(int a1, int a2, int a3, int a4) {
    unsigned char *v4 = unk_3807B04;
    unsigned char *e;
    int v13;
    unsigned int v12;
    int ch;
    int ch2;
    int result;
    e = &v4[a1 * 36];
    if (v4[a1 * 36] != 0)
        fn_0238592C(e);
    fn_02385B24(e, a4);
    ch = fn_02384B94();
    if (ch >= 0) {
        unsigned char *v = &unk_3807D44[ch << 6];
        fn_02385BDC(v);
        fn_02385BC8(v, a2, a3);
        e[8] = ch;
        fn_02385EF0(*(int *)(v + 0x28));
        if (fn_02385E78(v) != 0xFE) {
            --*(int *)(v + 0x28);
        } else {
            v12 = (unsigned)(fn_02385E20(v) << 15) >> 16;
            v13 = 1;
            while (v12 != 0) {
                if ((v12 & 1) != 0) {
                    ch2 = fn_02384B94();
                    if (ch2 < 0)
                        break;
                    fn_02385BDC(&unk_3807D44[ch2 << 6]);
                    *(e + v13 + 8) = ch2;
                }
                ++v13;
                v12 = (v12 << 15) >> 16;
            }
        }
        e[0] = 1;
        result = dword_38075C0;
        if (result != 0) {
            int *p = (int *)(result + 4);
            result = *p | (1 << a1);
            *p = result;
        }
        return result;
    }
    return ch;
}
