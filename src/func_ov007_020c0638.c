typedef unsigned short u16;

extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void MultiCopyHalf(char *src, char *dst, int nbytes);

struct T {
    int f0;
    int f4;
    int f8;
    int fc;
    char *f10;
    char *f14;
    int f18;
    u16 f1c;
    u16 f1e;
    int f20;
    int f24;
};

void func_ov007_020c0638(struct T *self, int x, int y, u16 val)
{
    int ty;
    char *dst;
    char *src;
    int sy;
    int sh;
    int tx;
    int rx;
    int ry;
    int sw;
    int sx;
    volatile u16 v;

    tx = x >> 3;
    ty = y >> 3;
    rx = x - (tx << 3);
    ry = y - (ty << 3);
    sw = self->f1c;
    sh = self->f1e;
    sx = 0;
    sy = 0;
    dst = self->f10;
    src = self->f14;
    if (tx < 0)
    {
        sw += tx;
        sx = -tx;
        tx = 0;
    }
    else if (tx > 0)
    {
        sw -= tx;
    }
    if (ty < 0)
    {
        sh += ty;
        sy = -ty;
        ty = 0;
    }
    else if (ty > 0)
    {
        sh -= ty;
    }
    v = val;
    MultiStore16(v, dst, self->f1c * self->f1e * 2);
    self->f20 = -rx << 12;
    self->f24 = -ry << 12;
    if (sw > 0 && sh > 0)
    {
        for (rx = ty + sh; ty < rx; ty++, sy++)
        {
            MultiCopyHalf(src + sy * self->f1c * 2 + sx * 2,
                          dst + ty * self->f1c * 2 + tx * 2,
                          sw * 2);
        }
    }
    self->fc = 1;
}
