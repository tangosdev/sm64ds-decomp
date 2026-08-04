// NONMATCHING: different op / idiom (div=24). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Quad { unsigned char b0, b1, b2, b3; };
extern struct Quad data_020a0de8[];
extern unsigned char data_020a0e40[];
extern void *data_ov006_021428c8;
extern int func_ov006_02108650(void);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

struct Obj {
    int x0;
    int x4;
    int x8;
    int xc;
    char pad10[0x1c];
    short x2c;
    char pad2e[4];
    unsigned char x32;
};

void func_ov006_02108d28(struct Obj *o)
{
    int flag;
    int dx, dy;
    int idx;

    if (data_ov006_021428c8 != 0) return;

    idx = data_020a0e40[0];
    flag = 0;
    if (data_020a0de8[idx].b0 != 0) {
        if (data_020a0de8[idx].b1 != 0) flag = 1;
    }
    if (flag == 0) return;

    if (o->x32 != 1) return;

    dx = (o->x0 >> 12) - data_020a0de8[idx].b2;
    dy = (o->x4 >> 12) - data_020a0de8[idx].b3;
    if (o->x2c != 0x25) return;

    if (func_ov006_02108650() == 0x25) return;

    _ZN5Sound12PlayBank2_2DEj(0x15d);
    data_ov006_021428c8 = o;
    o->x32 = 2;
    o->x8 = dx << 12;
    o->xc = dy << 12;
}
