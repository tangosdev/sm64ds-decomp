/* The 0x70-byte record func_ov007_020c2304 allocates: what Ov007Work's pad
 * points at (func_ov007_020b6a84). */
struct S {
    unsigned short f0;   /* 0x00 */
    unsigned short f2;   /* 0x02 */
    int a[12];           /* 0x04 .. 0x33 */
    int b[12];           /* 0x34 .. 0x63 */
    int f64;             /* 0x64 */
    unsigned char pad68; /* 0x68 */
    char f69;            /* 0x69 */
    unsigned char pad6a; /* 0x6a */
    char f6b;            /* 0x6b */
    unsigned char pad6c; /* 0x6c */
    char f6d;            /* 0x6d */
};

extern int data_ov007_0210324c[];
extern char *func_ov007_020c3df4(int, int);
extern void func_ov007_020c22a0(struct S *s);
extern int func_ov007_020c3d1c(void);

char *func_ov007_020c2304(void) {
    char *p = func_ov007_020c3df4((int)data_ov007_0210324c, 0x70);
    func_ov007_020c22a0((struct S *)p);
    return p;
}

/* Long-branch veneer: long_calls emits the pooled `ldr ip,[pc]; bx ip`
 * absolute tail-call; bracketed because the pragma is positional. */
#pragma long_calls on
int func_ov007_020c22f8(void) {
    return func_ov007_020c3d1c();
}
#pragma long_calls off

void func_ov007_020c22a0(struct S *s) {
    int i;
    s->f2 = 0;
    s->f0 = s->f2;
    s->f69 = 0;
    *(signed char *)&s->pad68 = s->f69;
    s->f6b = 0;
    *(signed char *)&s->pad6a = s->f6b;
    s->f6d = 0;
    *(signed char *)&s->pad6c = s->f6d;
    for (i = 0; i < 12; i++) {
        s->a[i] = 0;
        s->b[i] = 0;
    }
    s->f64 = 0;
}
