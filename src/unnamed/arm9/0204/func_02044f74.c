#include "types.h"
void func_02048234(int* out, int a, int b, int c, int p0, int p1, int p2, int p3, int p4, int p5);
void MulMat4x3Mat4x3(int* out, int a, int b, int c);

struct Node {
    s16 f0; s16 f2; s16 f4; s16 f6;
    s16 f8; s16 fa; s16 fc; s16 fe;
    int f10, f14, f18;
    u16 f1c, f1e, f20; s16 f22;
    int f24, f28, f2c;
};

void func_02044f74(void* g, struct Node* n)
{
    int m[12];
    int a = n->f0;
    func_02048234(m,
        n->f10, n->f14, n->f18,
        ((unsigned)n->f1c << 20) >> 16,
        ((unsigned)n->f1e << 20) >> 16,
        ((unsigned)n->f20 << 20) >> 16,
        n->f24, n->f28, n->f2c);
    {
        int* base = *(int**)((char*)g + 0x34);
        int t = base[3];
        MulMat4x3Mat4x3(m, (a + n->f8) * 0x30 + t, (s16)a * 0x30 + t, t);
    }
    if (n->fa != 0) func_02044f74(g, (struct Node*)((char*)n + n->fa * 0x40));
    if (n->fc != 0) func_02044f74(g, (struct Node*)((char*)n + n->fc * 0x40));
}
