typedef long long s64;

extern int func_020531a4(int a);
extern int _ZN4cstd4fdivEii(int a, int b);

typedef struct Vec3 {
    int x;
    int y;
    int z;
} Vec3;

typedef struct S {
    Vec3 *p;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
} S;

int func_ov007_020c704c(S *self, Vec3 *b, int r2arg, int r3arg, int radius)
{
    int dx, dy, sum, spd, ratio;
    s64 sdx, sdy;
    int *q;

    dx = b->x - self->p->x;
    dy = b->y - self->p->y;
    sum = (int)(((s64)dx * dx + 0x800) >> 12) + (int)(((s64)dy * dy + 0x800) >> 12);
    sdx = dx;
    sdy = dy;

    if (sum <= (int)(((s64)radius * radius + 0x800) >> 12)) {
        self->f18 = 0;
        self->f14 = self->f18;
        self->f10 = self->f14;
        self->fc = self->f10;
        self->f8 = self->fc;
        self->f4 = self->f8;
        *self->p = *b;
        return 2;
    }

    if (sum <= (int)(((s64)r3arg * r3arg + 0x800) >> 12)) {
        spd = func_020531a4(sum);
        ratio = _ZN4cstd4fdivEii(r2arg, spd);

        q = (int *)(int)(((s64)(int)((char *)self + 0x10)));
        *q = *q + (int)((ratio * sdx + 0x800) >> 12);
        self = (S *)(int)(((s64)(int)((char *)self + 0x14)));
        *(int *)self = *(int *)self + (int)((ratio * sdy + 0x800) >> 12);
        return 1;
    }
    return 0;
}
