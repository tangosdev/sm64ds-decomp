typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct Mtx43 { Vec3 r0, r1, r2, r3; } Mtx43;

struct Part {
    int count;
    unsigned char* ids;
    int f8;
    int fc;
};
struct Entry {
    int count;
    struct Part* part;
};

extern void func_020527e8(Mtx43* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(Mtx43* a, Mtx43* b, Mtx43* dst);
extern void NormalizeVec3(Vec3* a, Vec3* b);
extern void func_020458a8(void* src, void* dst);
extern void Geometry_MatrixMultiply3x3(void* m);
extern void func_020553a4(void* m);
extern void func_0205a358(int a0, int a1, int a2, int a3, int a4);
extern void func_02059fa8(int ch);
extern Vec3 data_02099f88;

#define FIXMUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void func_02044534(char* self, int index, Mtx43* mtx, Vec3* light)
{
    int i;
    struct Entry* e;
    int off;
    int j0;
    int z14;
    int z18;
    int z1c;
    int z20;
    int z24;
    int two;
    int z2c;
    int z30;
    int three;
    Vec3 dirv;
    Mtx43 mw;
    Mtx43 m;
    int out[9];
    Vec3* dir;
    struct Part* part;
    int j;
    Mtx43* mp;
    struct Entry* base;
    int cnt;
    int t0;
    char* obj;

    dir = &dirv;
    dirv = data_02099f88;
    if (light)
        dir = light;
    obj = *(char**)self;
    i = 0;
    base = *(struct Entry**)(obj + 0x10);
    cnt = base[index].count;
    e = base + index;
    if (cnt <= 0)
        return;

    t0 = i;
    z14 = t0;
    z20 = t0;
    z24 = t0;
    z18 = t0;
    z1c = t0;
    two = 2;
    t0 = i;
    z2c = t0;
    z30 = t0;
    three = 3;
    j0 = i;

    do {
        part = e->part;
        j = j0;
        if (part->count > 0) {
            do {
                unsigned char b = *(volatile unsigned char*)(part->ids + j);
                int t;
                int ix = *(volatile unsigned short*)((*(unsigned short**)(*(char**)self + 0x2c)) + b);
                mp = *(Mtx43* volatile*)(self + 0xc) + ix;
                off = ix * 0x30;
                if (mtx) {
                    func_020527e8(&mw, dir->x, dir->y, dir->z);
                    MulMat4x3Mat4x3(&mw, mtx, &mw);
                    MulMat4x3Mat4x3(mp, &mw, &mw);
                    mp = &mw;
                }
                m.r2.y = m.r2.x = z14;
                m.r2.z = dir->z;
                m.r3.x = mp->r3.x;
                m.r3.y = mp->r3.y;
                m.r3.z = mp->r3.z;
                if (mtx) {
                    MulMat4x3Mat4x3((Mtx43*)(*(char**)(self + 0xc) + off), mtx, &mw);
                    mp = &mw;
                }
                if (mp->r1.x * mp->r1.x + mp->r1.y * mp->r1.y > 0x4000) {
                    m.r1.x = mp->r1.x;
                    m.r1.y = mp->r1.y;
                    m.r1.z = z18;
                    NormalizeVec3(&m.r1, &m.r1);
                    t = FIXMUL(m.r1.x, dir->x);
                    m.r1.x = t;
                    { int dy = dir->y; int ry = m.r1.y;
                    m.r0.y = -t;
                    t = FIXMUL(ry, dy); }
                    { int zz = z1c;
                    m.r1.y = t;
                    m.r0.x = t;
                    m.r0.z = zz; }
                } else {
                    m.r0.x = mp->r0.x;
                    m.r0.y = mp->r0.y;
                    m.r0.z = z20;
                    NormalizeVec3(&m.r0, &m.r0);
                    m.r0.x = FIXMUL(m.r0.x, dir->x);
                    t = FIXMUL(m.r0.y, dir->y);
                    m.r1.y = m.r0.x;
                    m.r1.x = -t;
                    { int zz = z24;
                    m.r0.y = t;
                    m.r1.z = zz; }
                }
                func_020458a8(&m, out);
                *(int*)0x4000440 = two;
                *(int*)0x4000454 = z2c;
                Geometry_MatrixMultiply3x3(out);
                *(int*)0x4000440 = 1;
                func_020553a4(&m);
                {
                    int v = 1 << (*(unsigned char*)*(char**)self + 12);
                    *(int*)0x400046c = v;
                    *(int*)0x400046c = v;
                    *(int*)0x400046c = v;
                }
                *(int*)0x400044c = j;
                j++;
            } while (j < part->count);
        }
        { int a5 = z30; int a4 = z30;
        func_0205a358(three, part->fc, part->f8, a4, a5); }
        func_02059fa8(three);
        i = i + 1;
        cnt = e->count;
    } while (i < cnt);
}
