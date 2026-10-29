#include "common.h"
#include "nitro/fx/fx.h"

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
    u32 : 8;
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
    Vector3 v20;
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

/* No header declares these; each is spelled as its definition spells it
   (CrossVec3's vector is a Vec3_Fix12, NormalizeVec3 and MulVec3Mat3x3 take
   int*), so the calls below cast. */
typedef struct {
    Fix12i x, y, z;
} Vec3_Fix12;
extern void CrossVec3(const Vec3_Fix12 *a, const Vec3_Fix12 *b, Vec3_Fix12 *out);
extern void NormalizeVec3(int *v, int *out);
extern void Copy36Bytes(int *src, int *dst);
extern void MulVec3Mat3x3(const int *in, const int *m, int *out);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern void func_02055388(int *m);
extern void func_0204c24c(u8 a, u8 b);

void func_0204b460(Self *self, Node *node)
{
    Ctx *c = self->p34;
    s16 q = c->p18->p0->q20;
    int scale = (node->p40.lo * (node->p40.hi + 1)) >> 5;
    int *mtx = self->p38;
    Vector3 nv;
    Vector3 trans;
    Vector3 cr;
    Vector3 col;
    int tmp[9];
    int mat[12];
    u32 f2;
    u32 v;
    int v2;
    s64 v1w;

    v = *(u32 *)&c->cnt << 14;
    f2 = v >> 26;
    v = self->p30 | 0xc0;
    v |= f2 << 24;
    v |= scale << 16;
    *(volatile u32 *)0x40004a4 = v;
    *(volatile u32 *)0x40004a4;
    {
        Cnt74 *qq = &c->cnt;
        qq->count++;
        if (c->cnt.count > c->cnt.limit)
            qq->count = c->cnt.start;
    }

    if (scale == 0)
        return;

    v1w = FX_MUL(node->p30, node->p34);
    trans.x = node->p14 + node->p8;
    trans.y = node->p18 + node->pc;
    trans.z = node->p1c + node->p10;
    v2 = FX_MUL((int)v1w, q);
    cr = node->v20;
    col.x = mtx[2];
    col.y = mtx[5];
    col.z = mtx[8];
    CrossVec3((Vec3_Fix12 *)&cr, (Vec3_Fix12 *)&col, (Vec3_Fix12 *)&cr);

    if (cr.x == 0 && cr.y == 0 && cr.z == 0)
        return;

    NormalizeVec3((int *)&cr, (int *)&cr);
    Copy36Bytes(mtx, tmp);
    MulVec3Mat3x3((int *)&cr, tmp, (int *)&cr);
    MulVec3Mat4x3(&trans, (Matrix4x3 *)mtx, &trans);
    nv = node->v20;
    NormalizeVec3((int *)&nv, (int *)&nv);

    {
        int d = FX_MUL(nv.x, -mtx[2]) + FX_MUL(nv.y, -mtx[5]) + FX_MUL(nv.z, -mtx[8]);
        int w;
        int s;
        int t;
        if (d < 0)
            d = -d;
        w = 0x1000 - d;
        s = FX_MUL(w, (int)self->p34->p18->p0->val);
        t = FX_MUL(v1w, (s + 0x1000));

        mat[0] = FX_MUL(cr.x, v2);
        mat[8] = 0x1000;
        mat[9] = trans.x;
        mat[10] = trans.y;
        mat[11] = trans.z;
        mat[3] = FX_MUL(-cr.y, t);
        mat[1] = FX_MUL(cr.y, v2);
        mat[6] = 0;
        mat[4] = FX_MUL(cr.x, t);
        mat[7] = 0;
        mat[2] = 0;
        mat[5] = 0;
        *(volatile u32 *)0x4000454 = 0;
        func_02055388(mat);
    }

    *(volatile u32 *)0x4000480 = node->p3a;
    {
        Bdat *B = self->p34->p18->p14;
        func_0204c24c(B->tx, B->ty);
    }
}
