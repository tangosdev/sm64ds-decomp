struct S {
    unsigned short f0;        /* 0x00 */
    unsigned short f2;        /* 0x02 */
    int a[12];       /* 0x04 .. 0x33 */
    int b[12];       /* 0x34 .. 0x63 */
    int f64;         /* 0x64 */
    unsigned char pad68;  /* 0x68 */
    char f69;        /* 0x69 */
    unsigned char pad6a;  /* 0x6a */
    char f6b;        /* 0x6b */
    unsigned char pad6c;  /* 0x6c */
    char f6d;        /* 0x6d */
};
void func_ov007_020c22a0(struct S *s) {
    int i;
    s->f2 = 0;
    s->f0 = s->f2;
    s->f69 = 0;
    *(signed char*)&s->pad68 = s->f69;
    s->f6b = 0;
    *(signed char*)&s->pad6a = s->f6b;
    s->f6d = 0;
    *(signed char*)&s->pad6c = s->f6d;
    for (i = 0; i < 12; i++) {
        s->a[i] = 0;
        s->b[i] = 0;
    }
    s->f64 = 0;
}
