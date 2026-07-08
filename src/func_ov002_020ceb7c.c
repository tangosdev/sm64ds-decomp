// NONMATCHING: different op / idiom (div=17). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Camera {
    char pad60[0x60];
    int f60;              // 0x60
    char pad_to6d8[0x6d8 - 0x64];
    unsigned char f6d8;   // 0x6d8
    char pad_to6e5[0x6e5 - 0x6d9];
    unsigned char f6e5;   // 0x6e5
};

extern volatile int CAMERA;
extern int WATER_HEIGHT;

extern void func_0200d768(struct Camera *thiz, unsigned char playerID);
extern void func_0200d72c(struct Camera *thiz, unsigned char playerID);

void func_ov002_020ceb7c(struct Camera *c)
{
    (void)CAMERA;
    if (c->f6e5 & 2) {
        if (WATER_HEIGHT - 0x50000 - c->f60 >= 0x64000)
            return;
        func_0200d768(c, c->f6d8);
        *(unsigned char *)((char *)c + 0x6e5) ^= 2;
    } else {
        if (WATER_HEIGHT - 0x50000 - c->f60 <= 0x12c000)
            return;
        func_0200d72c(c, c->f6d8);
        *(unsigned char *)((char *)c + 0x6e5) |= 2;
    }
}
