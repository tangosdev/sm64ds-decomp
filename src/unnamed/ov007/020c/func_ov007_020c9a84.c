typedef struct Vec3 { int x, y, z; } Vec3;
extern void SubVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern int func_020538b8(int x, int y);
extern int _ZN4cstd3divEii(int a, int b);

void func_ov007_020c9a84(char *o, int *w, short *ox, short *oy)
{
    Vec3 d;
    int ra, rb;
    short a1, a2;
    int aa, ab, t1, t2, q1, q2;

    w[0] = *(int *)(*(int *)(o + 0x18) + (*(int *)(*(int *)(o + 0x1c) + 4) - 1) * 12);
    {
        char *p = *(char **)(o + 0x1c);
        char *base = *(char **)(o + 0x18);
        int n = *(int *)(p + 4) - 1;
        w[1] = *(int *)(base + n * 12 + 4);
    }
    SubVec3((Vec3 *)(o + 0x20), (Vec3 *)(o + 0x2c), &d);
    ra = func_020538b8(d.x, d.z);
    SubVec3((Vec3 *)(o + 0x38), (Vec3 *)(o + 0x44), &d);
    rb = func_020538b8(d.z, d.y);
    a1 = (unsigned short)(ra + 0x3fff);
    a2 = (unsigned short)(rb + 0x7fff);
    aa = (a1 >= 0) ? a1 : -a1;
    {
        unsigned short lim = *(unsigned short *)(*(int *)(o + 4) + 0x28);
        if (aa <= 0) t1 = 0;
        else if (aa >= lim) t1 = 0x1000;
        else t1 = _ZN4cstd3divEii(aa << 12, lim);
    }
    ab = (a2 >= 0) ? a2 : -a2;
    {
        unsigned short lim = *(unsigned short *)(*(int *)(o + 4) + 0x2a);
        if (ab <= 0) t2 = 0;
        else if (ab >= lim) t2 = 0x1000;
        else t2 = _ZN4cstd3divEii(ab << 12, lim);
    }
    q1 = (int)(((long long)t1 * (0x2000 - t1)) >> 12);
    q2 = (int)(((long long)t2 * (0x2000 - t2)) >> 12);
    *ox = (short)((*(unsigned short *)(*(int *)(o + 4) + 0x28) * (q1 * ((a1 >= 0) ? 1 : -1))) >> 12);
    *oy = (short)((*(unsigned short *)(*(int *)(o + 4) + 0x2a) * (q2 * ((a2 >= 0) ? 1 : -1))) >> 12);
}
