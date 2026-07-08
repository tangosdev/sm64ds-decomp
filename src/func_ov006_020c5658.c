//cpp
struct Vec3 { int x, y, z; };

struct Actor {
    char _00[0x10];
    int f10, f14, f18, f1c;   /* 0x10,0x14,0x18,0x1c */
    char _20[0x9c - 0x20];
    int f9c, fa0, fa4;        /* 0x9c,0xa0,0xa4 */
    int fa8, fac, fb0;        /* 0xa8,0xac,0xb0 */
    int fb4, fb8, fbc;        /* 0xb4,0xb8,0xbc */
    char _c0[0xdc - 0xc0];
    int fdc;                  /* 0xdc */
    char _e0[0xe6 - 0xe0];
    short fe6;                /* 0xe6 */
    short _e8;                /* 0xe8 */
    short fea;                /* 0xea */
};

int ApproachLinear(int&, int, int);
short ApproachLinear(short&, short, short);
void Vec3_Sub(Vec3*, Vec3*, Vec3*);
void func_0203ce80(Vec3*, Vec3*);
void Vec3_MulScalarInPlace(int*, int);
void func_ov006_020c49d8(void*);

extern "C" void func_ov006_020c5658(void* arg) {
    struct Actor* s = (struct Actor*)arg;
    Vec3 v0, v1, v2, v3;
    int r5, r0b;
    int t;

    t = s->fa8; if (t < 0) t = -t;
    r5 = ApproachLinear(s->f9c, s->fb4, t);
    t = s->fac; if (t < 0) t = -t;
    r0b = ApproachLinear(s->fa0, s->fb8, t);

    if (r5 != 0 && r0b != 0 && s->fb8 == s->f18) {
        s->fb4 = s->f14;
        s->fb8 = s->f1c;
        Vec3_Sub(&v0, (Vec3*)&s->fb4, (Vec3*)&s->f9c);
        s->fa8 = v0.x;
        s->fac = v0.y;
        s->fb0 = v0.z;
        func_0203ce80(&v1, (Vec3*)&s->fa8);
        Vec3_MulScalarInPlace(&s->fa8, s->fdc);
        s->fea = 0;
    } else if (r5 != 0 && r0b != 0 && s->fb8 == s->f1c) {
        s->fb4 = s->f10;
        s->fb8 = s->f18;
        Vec3_Sub(&v2, (Vec3*)&s->fb4, (Vec3*)&s->f9c);
        s->fa8 = v2.x;
        s->fac = v2.y;
        s->fb0 = v2.z;
        func_0203ce80(&v3, (Vec3*)&s->fa8);
        Vec3_MulScalarInPlace(&s->fa8, s->fdc);
        s->fea = 0;
    }

    if (s->fa8 > 0) {
        ApproachLinear(s->fe6, (short)0x3000, (short)0x200);
    } else {
        ApproachLinear(s->fe6, (short)-0x3000, (short)0x200);
    }
    func_ov006_020c49d8(s);
}
