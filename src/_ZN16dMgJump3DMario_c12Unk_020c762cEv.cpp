//cpp
// @symbol _ZN16dMgJump3DMario_c12Unk_020c762cEv
#include "dMgJump3DMario_c.h"

struct Pair { int a, b; };
extern "C" Pair data_ov006_0213b058;
extern "C" Pair data_ov006_0213b070;

int dMgJump3DMario_c::Unk_020c762c()
{
    Pair *p;
    Pair *g;
    int r = 0;
    int m = 1;
    char *c = (char *)this;

    p = (Pair *)((int)c + 0x3c);
    g = &data_ov006_0213b058;
    if (p->a == g->a) {
        if (p->b == g->b)
            goto cl1;
        if (*(int *)(c + 0x3c) == 0) {
cl1:
            m = 0;
        }
    }
    if (m != 0) {
        m = 1;
        p = (Pair *)(c + 0x3c);
        g = &data_ov006_0213b070;
        if (p->a == g->a) {
            if (p->b == g->b)
                goto cl2;
            if (*(int *)(c + 0x3c) == 0) {
cl2:
                m = 0;
            }
        }
        if (m != 0)
            r = 1;
    }
    return r;
}
