typedef short s16;

typedef struct Q {
    int p0;
    int f4;
    int f8;
} Q;

typedef struct PtrChain {
    int p0;
    Q *q;
} PtrChain;

typedef struct Obj {
    int p0;
    int f4;
    int f8;
} Obj;

typedef struct P0 {
    s16 s0;
    s16 p2;
    int p4;
    int p8;
    int sel;
} P0;

typedef struct S {
    P0 *p0;
    Obj *obj;
    int mode;
    s16 c;
    s16 e;
} S;

typedef struct G2 {
    char pad0[0x34];
    void *f34;
    char pad38[0xdc];
    PtrChain *arr114[1];
} G2;

extern S *data_ov007_02103360;
extern G2 *data_ov007_0210342c;
extern int func_ov007_020b79e4(void);
extern void func_ov007_020b3e04(int mode, s16 *p1, s16 *p2);
extern int _ZN4cstd3divEii(int, int);
extern void func_ov007_020c4388(Obj *o, void *arg);
extern void func_ov007_020c421c(Obj *o);

void func_ov007_020b2cf0(void) {
    int sel;
    int factor;
    int prod_c;
    int prod_e;
    int x;
    int y;
    s16 loc0;
    s16 loc2;

    if (data_ov007_02103360->p0->s0 == 0xff) return;
    if (data_ov007_02103360->mode == 2) return;
    if (data_ov007_02103360->mode != 5) {
        if (func_ov007_020b79e4() != 0) return;
    }
    prod_c = 0;
    prod_e = 0;
    sel = data_ov007_02103360->p0->sel;
    func_ov007_020b3e04(data_ov007_02103360->mode, &loc0, &loc2);

    {
        Q *q = data_ov007_0210342c->arr114[data_ov007_02103360->p0->s0]->q;
        x = loc0 + (q->f4 >> 12);
        y = loc2 + (q->f8 >> 12);
    }
    if (sel <= 4) {
        if (sel <= 0) {
            factor = 0;
        } else if (sel >= 4) {
            factor = 0x1000;
        } else {
            factor = _ZN4cstd3divEii(sel << 12, 4);
        }
        factor = 0x1000 - factor;
        prod_c = -data_ov007_02103360->c * factor * 6;
        prod_e = -data_ov007_02103360->e * factor * 6;
    }
    data_ov007_02103360->obj->f4 = prod_c + (x << 12);
    data_ov007_02103360->obj->f8 = prod_e + (y << 12);
    func_ov007_020c4388(data_ov007_02103360->obj, data_ov007_0210342c->f34);
    func_ov007_020c421c(data_ov007_02103360->obj);
}
