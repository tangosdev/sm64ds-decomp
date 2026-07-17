struct Vec3 { int x; int y; int z; };

#define LAUNDER(p) ((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL)

extern void AddVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);

void func_ov007_020c5948(char *self, struct Vec3 *src)
{
    int i;
    int j;
    int **tab;
    int *obj;
    int *vec;
    int mode;
    int *dst;
    int *hi;
    int *lo;

    i = *(int *)(self + 0x58);
    if (i == -1)
        return;
    j = *(int *)(self + 0x5c);
    if (j == -1)
        return;

    tab = *(int ***)(self + 0x3c);
    obj = (*(int ***)tab[i])[j];
    vec = (int *)obj[0];

    obj[6] = 0;
    obj[5] = obj[6];
    obj[4] = obj[5];
    obj[3] = obj[4];
    obj[2] = obj[3];
    obj[1] = obj[2];

    vec[0] = src->x;
    vec[1] = src->y;

    mode = *(int *)(self + 4);
    if (mode == 0)
        goto do_mode0;
    if (mode == 1)
        goto do_mode1;
    if (mode == 2) {
        *(struct Vec3 *)(self + 0x98) = *src;
    }
    return;

do_mode0:
    *(struct Vec3 *)(self + 0x70) = *src;
    return;

do_mode1:
    dst = (int *)(int)LAUNDER(self + 0x70);
    hi = (int *)(int)LAUNDER(self + 0xb8);
    lo = (int *)(int)LAUNDER(self + 0xc4);
    AddVec3(src, (struct Vec3 *)(self + 0x60), (struct Vec3 *)dst);
    if (hi[0] > dst[0])
        dst[0] = hi[0];
    if (lo[0] < dst[0])
        dst[0] = lo[0];
    if (hi[1] > dst[1])
        dst[1] = hi[1];
    if (lo[1] < dst[1])
        dst[1] = lo[1];
    return;
}
