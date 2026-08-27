struct S {
    int f0;       /* 0x00 */
    int f4;       /* 0x04 */
    int f8;       /* 0x08 */
    short fc;     /* 0x0c */
    short fe;     /* 0x0e */
    short f10;    /* 0x10 */
    short f12;    /* 0x12 */
    int f14;      /* 0x14 */
    int f18;      /* 0x18 */
};
void func_ov007_020c9214(struct S *s, short a1, int a2, short a3, int a5, int a6) {
    s->f0 = 0;
    s->f4 = a2;
    s->fe = s->fc;
    s->fc = a1;
    s->f10 = (short)a5;
    s->f12 = a3;
    s->f14 = a5 << 12;
    s->f18 = a6;
    s->f8 |= 1;
}
