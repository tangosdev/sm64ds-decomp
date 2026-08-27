// func_ov007_020c9d90 at 0x020c9d90 (ov007)
typedef struct Vec3 { int x, y, z; } Vec3;

typedef struct Obj { int f0; int f4; int f8; } Obj;
typedef struct Cell { Obj* obj; } Cell;
typedef struct Row { Cell** cells; } Row;
typedef struct Grid { int w; int h; Row** rows; } Grid;

typedef struct Ctx {
    Vec3* arr;      // 0x00
    Grid* grid;     // 0x04
    int pad08[6];   // 0x08
    int f20;        // 0x20
} Ctx;

typedef struct Self {
    Ctx* ctx;       // 0x00
    int pad04;      // 0x04
    int f8;         // 0x08
    Vec3 vC;        // 0x0c
    int pad18[2];   // 0x18
    Vec3 v20[2];    // 0x20
    Vec3 v38[2];    // 0x38
    int f50[2];     // 0x50
    int f58[2];     // 0x58
    int f60[2];     // 0x60
    int f68[2];     // 0x68
} Self;

extern int func_ov007_020c30ac(Vec3* arr, int count, Vec3* target, int arg4, int* out);
extern int _ZN4cstd3modEii(int a, int b);
extern int _ZN4cstd3divEii(int a, int b);
extern void func_ov007_020c8010(int a, int b, Cell* c);

void func_ov007_020c9d90(Self* self, Ctx* ctx)
{
    int count;
    int i;
    int a;
    int b;
    int m;
    int d;
    Grid* g;
    Cell* cell;
    Obj* obj;
    int old;

    count = ctx->grid->w * ctx->grid->h;
    self->ctx = ctx;
    self->f8 = func_ov007_020c30ac(ctx->arr, count, &self->vC, -1, 0);

    for (i = 0; i < 2; i++) {
        a = func_ov007_020c30ac(ctx->arr, count, &self->v20[i], -1, 0);
        b = func_ov007_020c30ac(ctx->arr, count, &self->v38[i], -1, 0);

        g = ctx->grid;
        m = _ZN4cstd3modEii(a, g->w);
        d = _ZN4cstd3divEii(a, g->h);
        cell = g->rows[d]->cells[m];
        obj = cell->obj;
        old = obj->f8;
        obj->f8 = ctx->f20;
        func_ov007_020c8010(self->f60[i], self->f50[i], cell);
        cell->obj->f8 = old;

        g = ctx->grid;
        m = _ZN4cstd3modEii(b, g->w);
        d = _ZN4cstd3divEii(b, g->h);
        cell = g->rows[d]->cells[m];
        obj = cell->obj;
        old = obj->f8;
        obj->f8 = ctx->f20;
        func_ov007_020c8010(self->f68[i], self->f58[i], cell);
        cell->obj->f8 = old;
    }
}
