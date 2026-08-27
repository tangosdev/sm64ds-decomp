#include "types.h"
void func_02045178(int* out, int a, int b, int c, int p0, int p1, int p2);
void func_02048234(int* out, int a, int b, int c, int p0, int p1, int p2, int p3, int p4, int p5);
void MulMat4x3Mat4x3(int* out, int a, int b, int c);

struct Elem {
    int f0, f4, f8;
    int fc, f10, f14;
    u16 f18, f1a, f1c, f1e;
    int f20, f24, f28, f2c, f30;
};

struct Node {
    s16 f0; s16 f2; s16 f4; s16 f6;
    s16 f8; s16 fa; s16 fc;
};

struct G {
    int f0, f4;
    struct Elem* f8;
    int fc;
};

void func_02045074(struct G* g, struct Node* n)
{
    int m[12];
    int a = n->f0;
    struct Elem* e = (struct Elem*)((char*)g->f8 + a * 0x34);
    if (e->f18 == 0) {
        func_02045178(m,
            e->f1a, e->f1c, e->f1e,
            e->f20, e->f24, e->f28);
    } else {
        func_02048234(m,
            e->fc, e->f10, e->f14,
            e->f1a, e->f1c, e->f1e,
            e->f20, e->f24, e->f28);
    }
    {
        int t = g->fc;
        MulMat4x3Mat4x3(m, (a + n->f8) * 0x30 + t, a * 0x30 + t, t);
    }
    if (n->fa != 0) func_02045074(g, (struct Node*)((char*)n + n->fa * 0x40));
    if (n->fc != 0) func_02045074(g, (struct Node*)((char*)n + n->fc * 0x40));
}
