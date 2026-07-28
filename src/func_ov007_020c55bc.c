typedef struct { int x, y, z; } V3;

struct Elem {
    int _0;
    int count;
};

struct Self {
    int _0[2];
    int f8;
    int _c[8];
    V3 **f2c;
    int _30;
    int ***f34;
    int _38;
    struct Elem **f3c;
    int _40[15];
    int f7c;
    int f80;
};

void func_ov007_020c55bc(struct Self *self, int m1, int m2)
{
    int i;
    int j;
    int *row;
    int b7c, b80;
    int *e;
    int x, y;
    int dx, dy;
    int *p;
    int *q;
    long long M1, M2;
    int k;

    i = 0;
    if (self->f8 <= 0)
        return;

    M1 = m1;
    M2 = m2;

    do {
        j = 0;
        if (self->f3c[i]->count > 0) {
            k = 0;
            do {
                row = (int *)self->f2c[i];
                p = self->f34[i][j];
                q = (int *)(((int)p + 0xc) & 0xFFFFFFFFFFFFFFFFLL);
                b7c = self->f7c;
                b80 = self->f80;
                e = (int *)((char *)row + k);
                x = e[0];
                y = e[1];
                dx = b7c - x;
                dy = b80 - y;

                q[0] = (int)((M1 * dx + 0x800) >> 12);
                q[1] = (int)((M2 * dy + 0x800) >> 12);
                k += 0xc;
                j++;
            } while (j < self->f3c[i]->count);
        }
        i++;
    } while (i < self->f8);
}
