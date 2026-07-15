typedef struct { int x, y, z; } Vec3;
typedef struct { int a, b; } W2;
extern int LenVec3(Vec3 *v);
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern void Vec3_MulScalarInPlace(int *v, int s);

struct Obj {
    int pad0;
    int f4, f8, fc;
    int f10, f14, f18, f1c;
    char gap20[0x9c - 0x20];
    Vec3 f9c;
    Vec3 fa8;
    Vec3 fb4;
    char gapc0[0xdc - 0xc0];
    int fdc;
    char gape0[0xe6 - 0xe0];
    short fe6;
};

extern W2 data_ov006_0213af50;

void func_ov006_020c57d4(struct Obj *o)
{
    Vec3 a, b, da, db, t;
    int la;
    int ay, ax, by, bx;
    o->fa8.x = o->f4;
    o->fa8.y = o->f8;
    o->fa8.z = o->fc;
    o->fdc = LenVec3(&o->fa8);
    o->fe6 = 0;
    ay = o->f1c;
    ax = o->f14;
    a.x = ax;
    a.y = ay;
    a.z = 0;
    by = o->f18;
    bx = o->f10;
    b.x = bx;
    b.y = by;
    b.z = 0;
    Vec3_Sub(&da, &a, &o->f9c);
    la = LenVec3(&da);
    Vec3_Sub(&db, &b, &o->f9c);
    if (LenVec3(&db) < la) {
        o->fb4.x = a.x;
        o->fb4.y = a.y;
        o->fb4.z = a.z;
    } else {
        o->fb4.x = b.x;
        o->fb4.y = b.y;
        o->fb4.z = b.z;
    }
    Vec3_Sub(&t, &o->fb4, &o->f9c);
    o->fa8.x = t.x;
    o->fa8.y = t.y;
    o->fa8.z = t.z;
    if (NormalizeVec3IfNonZero(&o->fa8) == 0) {
        o->fa8.x = o->f4;
        o->fa8.y = o->f8;
        o->fa8.z = o->fc;
    } else {
        Vec3_MulScalarInPlace((int *)&o->fa8, o->fdc);
    }
    *(W2 *)((char *)o + 0x30) = data_ov006_0213af50;
}
