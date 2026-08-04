// func_ov006_02106bc0 at 0x02106bc0 (ov006)
struct Pmf { int off; int adj; };
typedef void (*PmfFn)(void *, int);

extern struct Pmf data_ov006_02142840[];
extern void func_ov004_020b0a54(int arg);
extern void func_ov006_02104ea8(char *c);

void func_ov006_02106bc0(char *c) {
    int i;
    for (i = 0; i < *(int *)(c + 0x4cb8); i++) {
        unsigned char idx = *(unsigned char *)(c + i + 0x4efa);
        struct Pmf *e = &data_ov006_02142840[idx];
        int adj = e->adj;
        char *thisp = c + (adj >> 1);
        PmfFn fn;
        if (adj & 1) {
            fn = *(PmfFn *)(*(char **)thisp + e->off);
        } else {
            fn = (PmfFn)e->off;
        }
        fn(thisp, i);
    }
    if (*(unsigned short *)(c + 0x4ec0) == 0) return;
    *(unsigned short *)(((long long)(int)(c + 0x4ec0))) -= 1;
    if (*(short *)(c + 0x4ec0) > 0) return;
    *(unsigned short *)(c + 0x4ec0) = 0;
    func_ov004_020b0a54(0x12);
    *(unsigned char *)(c + 0xc3) = 0;
    *(unsigned char *)(c + 0x4fe3) = 0;
    func_ov006_02104ea8(c);
}
