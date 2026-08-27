struct Poly {
    int f0;
    int f4;
    unsigned short n;
    unsigned short fa;
    int fc[6];
    int *xs;
    int *ys;
};

struct Ctx {
    int f0;
    int f4;
    int count;
    int fc[11];
    struct Poly **polys;
    int ****g;
    char **h40;
    char **h44;
    short *i48;
    short *i4c;
    short *i50;
    short *i54;
};

extern int func_ov007_020c31e0(int *xa, int *xb, int n, int cx, int qy, int maxr, int *out);
extern void func_ov007_020c8010(char* r4, int* r1, int* r2);

void func_ov007_020c6b58(struct Ctx *c)
{
    int i;
    struct Poly *self;
    int j;
    int best1;
    int best2;
    struct Poly *other;
    int bj1;
    int bj2;
    int d0;
    int d1;
    int res;

    for (i = 0; i < c->count; i++) {
        self = c->polys[i];
        best1 = -1;
        d0 = -1;
        d1 = -1;
        best2 = -1;
        for (j = 0; j < c->count; j++) {
            other = c->polys[j];
            if (i != j) {
                res = func_ov007_020c31e0(other->xs, other->ys, other->n,
                                          self->xs[0], self->ys[0], d0, &d0);
                if (res != -1) {
                    best1 = res;
                    bj1 = j;
                }
                res = func_ov007_020c31e0(other->xs, other->ys, other->n,
                                          self->xs[self->n - 1],
                                          self->ys[self->n - 1], d1, &d1);
                if (res != -1) {
                    best2 = res;
                    bj2 = j;
                }
            }
        }
        if (best1 != -1) {
            c->i48[i] = (short)bj1;
            c->i50[i] = (short)best1;
            func_ov007_020c8010(c->h40[i], c->g[i][0][0], c->g[bj1][0][best1]);
        }
        if (best2 != -1) {
            c->i4c[i] = (short)bj2;
            c->i54[i] = (short)best2;
            func_ov007_020c8010(c->h44[i], c->g[i][0][self->n - 1],
                                c->g[bj2][0][best2]);
        }
    }
}
