typedef struct { int x, y, z; } Elem;
typedef struct { char pad[8]; Elem ****rows; } Grid;

typedef struct { int v[6]; } Copy6;
typedef struct {
    Elem *buf0; Grid *grid; int *buf8; Copy6 copy;
    short s24; short s26;
    int *buf28; int *buf2c; int *buf30; int *buf34;
} Ctx;
extern void *func_ov007_020c3df4(int a, int size);
extern long long _ZN4cstd3divEii(int num, int den);
extern void *func_ov007_020c73f0(int a, int b, int c, int d);
extern void *func_ov007_020c80a4(void);
extern void func_ov007_020c72d0(void *g);
extern void func_ov007_020c8010(void *node, void *a, void *b);
extern void func_ov007_020cb3dc(void *ctx);

void *func_ov007_020cbbb0(int a0, int a1, int a2, int *a3)
{
    int d1, d2, t, c, e, a1m1, a0m1, j, i, k, i2;
    Ctx *ctx;

    ctx = (Ctx *)func_ov007_020c3df4(0, 0x38);
    a0m1 = a0 - 1;
    d1 = (int)_ZN4cstd3divEii(a2, a0m1);
    t = (int)(((long long)a2 * 0x16a1 + 0x800) >> 12);
    d2 = (int)_ZN4cstd3divEii(t / 2, a0m1);
    {
        int n = a0 - 1;
        a0m1 = n;
    }
    c = (d1 * a0m1) / 2;
    a1m1 = a1 - 1;
    e = (d2 * a1m1) / 2;
    ctx->buf0  = (Elem *)func_ov007_020c3df4(0, a0 * 12 * a1);
    ctx->buf28 = (int *)func_ov007_020c3df4(0, a0m1 * 6 * a1m1 * 2);
    ctx->buf2c = (int *)func_ov007_020c3df4(0, a0 * 6 * a1);
    ctx->buf30 = (int *)func_ov007_020c3df4(0, a0 * 4);
    ctx->buf34 = (int *)func_ov007_020c3df4(0, a0 * 4);
    ctx->s24 = (short)d1;
    ctx->s26 = (short)d2;
    ctx->buf8 = (int *)func_ov007_020c3df4(0, a0m1 * 0x18 * a1m1);
    ctx->grid = (Grid *)func_ov007_020c73f0(3, a0, a1, *a3);

    for (i = 0; i < a1; i++) {
        for (j = 0; j < a0; j++) {
            Elem *elem = &ctx->buf0[i * a0 + j];
            elem->x = j * d1 - c + ((i % 2 == 0) ? 0 : d1 / 2);
            elem->y = d2 * (a1 - 1 - i) - e;
            elem->z = a3[5];
            *(Elem **)((*ctx->grid->rows[i])[j]) = elem;
            if (j < a0m1 && i < a1m1) {
                ctx->buf8[i * a0m1 + j] = (int)func_ov007_020c80a4();
            }
            if (j == 0 || j == a0 - 1 || i == 0 || i == a1 - 1) {
                ((int *)(*ctx->grid->rows[i])[j])[8] |= 1;
            }
        }
    }

    func_ov007_020c72d0(ctx->grid);

    {
        i = 0;
        if (a1m1 > 0) {
            a0 = i;
            do {
                j = 0;
                if (a0m1 > 0) {
                    do {
                        int i2mod = i % 2;
                        int t = i2mod;
                        int i2p1 = i + 1;
                        int sum = j + a0;
                        if (i2mod == 0) {
                            Grid *g = ctx->grid;
                            int *b8p = ctx->buf8;
                            void *node = (void *)b8p[sum];
                            void *ea = (*g->rows[i])[j + 1];
                            void *eb = (*g->rows[i2p1])[j];
                            func_ov007_020c8010(node, ea, eb);
                        } else {
                            Grid *g = ctx->grid;
                            int *b8p = ctx->buf8;
                            void *node = (void *)b8p[sum];
                            void *ea = (*g->rows[i])[j];
                            void *eb = (*g->rows[i2p1])[j + 1];
                            func_ov007_020c8010(node, ea, eb);
                        }
                        j++;
                    } while (j < a0m1);
                }
                i++;
                a0 += a0m1;
            } while (i < a1m1);
        }
    }

    ctx->copy = *(Copy6 *)a3;
    func_ov007_020cb3dc(ctx);
    return ctx;
}
