extern int *func_ov007_020c3df4(int, int);
extern void func_ov007_020c0a44(int *);
extern int func_ov007_020c3d1c(void);

/* The 0x38-byte record func_ov007_020c0a44 resets; no class or header names it, so raw. */
int *func_ov007_020c0aa8(int a, int b, int c, short d, unsigned short e) {
    int *o = func_ov007_020c3df4(0, 0x38);
    func_ov007_020c0a44(o);
    o[4] = a;
    *(short *)((char *)o + 0x1c) = d;
    *(short *)((char *)o + 0x1e) = e;
    o[0] = b;
    o[1] = c;
    return o;
}

/* Long-branch veneer: long_calls emits the pooled `ldr ip,[pc]; bx ip`
 * absolute tail-call; bracketed because the pragma is positional. */
#pragma long_calls on
int func_ov007_020c0a9c(void) {
    return func_ov007_020c3d1c();
}
#pragma long_calls off

/* c: the 0x38-byte record func_ov007_020c0aa8 allocates; no class or header names it, so raw. */
void func_ov007_020c0a44(int *c) {
    c[9] = 0;
    c[8] = c[9];
    c[11] = 0;
    c[10] = c[11];
    c[0] = -1;
    c[2] = 1;
    c[4] = 0;
    c[5] = 0;
    *(unsigned short *)((char *)c + 0x1e) = 0;
    *(unsigned short *)((char *)c + 0x1c) = *(unsigned short *)((char *)c + 0x1e);
    c[6] = 0;
    c[13] = 0;
    c[12] = c[13];
    c[3] = 1;
}
