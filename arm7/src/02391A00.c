/* storage 0x02391A00 runtime 0x027C3978 size 0x1B8 (code 0x1B4 + 1-word pool) main ARM.
 * State init from an 11-word param block: conditional alloc-fill-send (14,6,10)
 * when mode==2 with bit (n-1) of the 0x1F2 flags clear, bulk field init where
 * h is a load-adjacent (ushort) narrow of p[2] (kept eager in r3 with the
 * shifted form live for the w1+h add), f3C default 16, flag bytes from !=0
 * folds, pool 0xFFFF, two void calls, f0 8->10 / 7->9 remap,
 * final alloc-fill-send (14,0,10). */

extern int *fn_0238E688(void); /* =0x027C0600 */
extern short *fn_0238DE4C(void); /* =0x03805CE4 alloc */
extern int fn_0238DEF4(short *); /* =0x03805D8C send */
extern void fn_02393CA8(void); /* =0x027C5C20 */
extern void fn_02392BBC(void); /* =0x027C4B34 */

void fn_02391A00(int *p) {
    char *q = (char *)fn_0238E688() + 0x1000;
    char *s = *(char **)(q + 0x610);
    char *f = *(char **)s;
    unsigned int w1;
    unsigned short h;
    short *m;
    if (*(unsigned short *)(f + 0xE2) == 2) {
        unsigned short flags = *(unsigned short *)(f + 0x1F2);
        unsigned short n = *(unsigned short *)(s + 0x46);
        if ((flags & ((1 << n) >> 1)) == 0) {
            m = fn_0238DE4C();
            m[0] = 14;
            m[1] = 6;
            m[2] = 10;
            fn_0238DEF4(m);
        }
    }
    w1 = p[1];
    h = (unsigned short)p[2];
    *(int *)(f + 0x10) = 1;
    *(int *)(f + 0x0C) = 0;
    *(unsigned short *)(f + 0x84) = 0;
    *(unsigned short *)(f + 0x38) = 0;
    *(unsigned short *)(f + 0x3A) = 1;
    *(unsigned short *)(f + 0x88) = 0;
    *(unsigned short *)(f + 0x9A) = 0;
    *(unsigned short *)(f + 0x9C) = 0x3C;
    *(int *)(f + 0x92C) = 0;
    *(unsigned short *)(f + 0x8A) = 0;
    *(unsigned short *)(f + 0x8C) = 0;
    *(unsigned short *)(f + 0x8E) = 0;
    *(unsigned short *)(f + 0x90) = 0;
    *(int *)(f + 0x48) = w1;
    *(unsigned short *)(f + 0x46) = h;
    *(int *)(f + 0x4C) = w1 + h;
    *(unsigned short *)(f + 0x42) = 0;
    *(int *)(f + 0x50) = p[3];
    *(unsigned short *)(f + 0x54) = p[4];
    *(unsigned short *)(f + 0x3C) = p[5];
    if (*(unsigned short *)(f + 0x3C) == 0)
        *(unsigned short *)(f + 0x3C) = 16;
    *(unsigned short *)(f + 0x98) = p[6];
    *(unsigned short *)(f + 0x92) = (p[7] != 0);
    *(unsigned short *)(f + 0x94) = (p[8] != 0);
    *(unsigned short *)(f + 0x96) = (p[9] != 0);
    *(unsigned short *)(f + 0x3E) = 0;
    *(unsigned short *)(f + 0xBA) = 0xFFFF;
    *(unsigned short *)(f + 0xBC) = 4;
    *(unsigned short *)(f + 0xDA) = (p[10] != 0);
    fn_02393CA8();
    fn_02392BBC();
    if (*(unsigned short *)f == 8)
        *(unsigned short *)f = 10;
    else if (*(unsigned short *)f == 7)
        *(unsigned short *)f = 9;
    m = fn_0238DE4C();
    m[0] = 14;
    m[1] = 0;
    m[2] = 10;
    fn_0238DEF4(m);
}
