//cpp
typedef unsigned short u16;
typedef short s16;

struct Matrix2x2 { int m[4]; };

extern "C" {

void _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(volatile u16 *p, Matrix2x2 *m, int a, int b, int c, int d)
{
    u16 pa = (u16)(s16)(m->m[0] >> 4);
    u16 pb = (u16)(s16)(m->m[1] >> 4);
    *(volatile unsigned int *)p = pa | (pb << 16);

    u16 pc = (u16)(s16)(m->m[2] >> 4);
    u16 pd = (u16)(s16)(m->m[3] >> 4);
    *(volatile unsigned int *)(p + 2) = pc | (pd << 16);

    int dx = c - a;
    int dy = d - b;
    int x = m->m[0] * dx + m->m[1] * dy + (a << 12);
    int y = m->m[2] * dx + m->m[3] * dy + (b << 12);

    *(volatile int *)(p + 4) = x >> 4;
    *(volatile int *)(p + 6) = y >> 4;
}

}
