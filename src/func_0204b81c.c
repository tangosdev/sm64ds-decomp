typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;
typedef long long s64;

typedef struct { int x, y, z; } Vec3;

typedef struct {
    u32 start : 6;
    u32 limit : 6;
    u32 count : 6;
    u32 : 14;
} Cnt74;

typedef struct {
    char _pad[0x10];
    u32 tx : 2;
    u32 ty : 2;
    u32 : 28;
} Bdat;

typedef struct {
    char _pad[0x20];
    s16 q20;
    char _pad2[0x12];
    u32 : 8;
    u32 val : 16;
    u32 fa : 2;
    u32 fb : 2;
    u32 : 4;
} P0;

typedef struct {
    P0 *p0;
    char _pad[0x10];
    Bdat *p14;
} Adat;

typedef struct {
    char _pad[0x18];
    Adat *p18;
    char _pad2[0x74 - 0x1c];
    Cnt74 cnt;
} Ctx;

typedef struct {
    u16 lo : 5;
    u16 hi : 5;
    u16 : 6;
} M40;

typedef struct {
    char _pad0[8];
    int p8;
    int pc;
    int p10;
    int p14;
    int p18;
    int p1c;
    Vec3 v20;
    char _pad2c[4];
    int p30;
    s16 p34;
    u16 p36;
    char _pad38[2];
    u16 p3a;
    char _pad3c[4];
    M40 p40;
} Node;

typedef struct {
    char _pad[0x30];
    int p30;
    Ctx *p34;
    int *p38;
} Self;

extern void CrossVec3(Vec3 *a, Vec3 *b, Vec3 *out);
extern void NormalizeVec3(Vec3 *a, Vec3 *b);
extern void Copy36Bytes(int *src, int *dst);
extern void MulVec3Mat3x3(int *in, int *m, int *out);
extern void MulVec3Mat4x3(Vec3 *v, int *m, Vec3 *dst);
extern void func_02055388(int *m);
extern void func_0204c24c(u8 a, u8 b);

#define FX(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

void func_0204b81c(Self *self, Node *node)
{
    Ctx *c = self->p34;
    s16 q = c->p18->p0->q20;
    int scale = (node->p40.lo * (node->p40.hi + 1)) >> 5;
    int *mtx = self->p38;
    Vec3 nv;
    Vec3 trans;
    Vec3 cr;
    Vec3 col;
    int tmp[9];
    int mat[12];
    u32 f2;
    u32 v;
    int v2;
    s64 v1w;

    v = *(u32 *)((char *)c + 0x74) << 14;
    f2 = v >> 26;
    v = self->p30 | 0xc0;
    v |= f2 << 24;
    v |= scale << 16;
    *(volatile u32 *)0x40004a4 = v;
    *(volatile u32 *)0x40004a4;
    {
        Cnt74 *qq = (Cnt74 *)(u32)(((s64)(int)((char *)c + 0x74)) & 0xFFFFFFFFFFFFFFFFLL);
        qq->count++;
        if (c->cnt.count > c->cnt.limit)
            qq->count = c->cnt.start;
    }

    if (scale == 0)
        return;

    v1w = (int)(((s64)node->p30 * node->p34 + 0x800) >> 12);
    trans.x = node->p14 + node->p8;
    trans.y = node->p18 + node->pc;
    trans.z = node->p1c + node->p10;
    v2 = FX((int)v1w, q);
    cr = node->v20;
    col.x = mtx[2];
    col.y = mtx[5];
    col.z = mtx[8];
    CrossVec3(&cr, &col, &cr);

    if (cr.x == 0 && cr.y == 0 && cr.z == 0)
        return;

    NormalizeVec3(&cr, &cr);
    Copy36Bytes(mtx, tmp);
    MulVec3Mat3x3((int *)&cr, tmp, (int *)&cr);
    MulVec3Mat4x3(&trans, mtx, &trans);
    nv = node->v20;
    NormalizeVec3(&nv, &nv);

    {
        int d = FX(nv.x, -mtx[2]) + FX(nv.y, -mtx[5]) + FX(nv.z, -mtx[8]);
        int w;
        int s;
        int t;
        if (d < 0)
            d = -d;
        w = 0x1000 - d;
        s = FX(w, (int)self->p34->p18->p0->val);
        t = (int)((v1w * (s + 0x1000) + 0x800) >> 12);

        mat[0] = FX(cr.x, v2);
        mat[8] = 0x1000;
        mat[9] = trans.x;
        mat[10] = trans.y;
        mat[11] = trans.z;
        mat[3] = FX(-cr.y, t);
        mat[1] = FX(cr.y, v2);
        mat[6] = 0;
        mat[4] = FX(cr.x, t);
        mat[7] = 0;
        mat[2] = 0;
        mat[5] = 0;
        *(volatile u32 *)0x4000454 = 0;
        func_02055388(mat);
    }

    *(volatile u32 *)0x4000480 = node->p3a;
    {
        P0 *p = self->p34->p18->p0;
        func_0204c24c(p->fa, p->fb);
    }
}
