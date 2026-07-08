// NONMATCHING: base materialization / addressing (div=25). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int __aeabi_idiv(int a, int b);

extern u32 PARTICLE_RNG_STATE;

struct Dst {
    char pad0[0x40];
    u16 u40;       /* 0x40 */
};

struct Src {
    char pad0[0xc];
    void *p0c;     /* 0xc */
};

void func_0204d1b4(struct Dst *dst, struct Src *src, int t) {
    u16 *e = (u16*)src->p0c;
    u8 *be = (u8*)e;
    int lo = be[4];
    int hi = be[5];
    int r1;
    if (t < lo) {
        u32 h = e[0];
        u32 lo5 = (h << 0x1b) >> 0x1b;
        int c = (int)((h << 0x16) >> 0x1b) - (int)lo5;
        r1 = __aeabi_idiv(t * c, lo) + (int)lo5;
    } else if (t < hi) {
        u32 h = e[0];
        r1 = (int)((h << 0x16) >> 0x1b);
    } else {
        u32 h = e[0];
        int d = (int)((h << 0x11) >> 0x1b) - (int)((h << 0x16) >> 0x1b);
        r1 = __aeabi_idiv((t - 0xff) * d, 0xff - hi) + (int)((h << 0x11) >> 0x1b);
    }
    PARTICLE_RNG_STATE = PARTICLE_RNG_STATE * 0x5eedf715 + 0x1b0cb173;
    int rnd = PARTICLE_RNG_STATE >> 0x18;
    int v = be[2] * rnd;
    v >>= 8;
    v = 0xff - v;
    v = r1 * v;
    v = (v << 8) >> 0x10;
    v &= 0x1f;
    dst->u40 = (u16)((dst->u40 & ~0x3e0) | (v << 5));
}
