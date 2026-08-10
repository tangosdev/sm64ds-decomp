//cpp
#include "types.h"
// @symbol func_ov075_0211a2b8
// recovered name: dScEntry_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScEntry_c::Behavior - recovered from vtable slot identity */
extern "C" {
    void func_020551f0(void *s, int v);
    int func_ov075_02119dc4(void *c, void *arg);
    extern int data_0209fc68;
}

struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
};
extern "C" VObj *data_0209f5bc;

struct Poly0 { virtual void v0(); };

struct Self;
typedef void (Self::*PMF)();
struct Self {
    u8 pad0[0x5c];
    PMF pmf;
    u8 pad64[0x280 - 0x64];
    u8 cnt280;
    u8 pad281[2];
    u8 f283;
    u8 f284;
    u8 f285;
};

extern "C" int func_ov075_0211a2b8(Self *c)
{
    char *cc = (char *)c;

    if (c->f285 != 0) {
        (*(u8 *)(cc + 0x284))++;
        if ((c->f284 & 1) == 0) {
            (*(u8 *)(cc + 0x283))--;
            func_020551f0((void *)0x4001050, c->f283);
            if (c->f283 == 0) {
                *(u16 *)0x4001050 = 0;
                c->f283 = 0;
                c->f285 = 0;
            }
        }
    }

    if (data_0209f5bc->v5() == 0 || c->f285 != 0)
        return 1;

    if (data_0209fc68 != 0) {
        int t = (data_0209fc68 == 6);
        if (t != 0)
            func_ov075_02119dc4(c, &data_ov075_0211d930);
    }

    if (*(volatile int *)&c->pmf != 0) {
        PMF *p = &c->pmf;
        (c->**p)();
    }

    {
        int i = 0;
        int n = c->cnt280;
        if (n > 0) {
            Poly0 *o = (Poly0 *)(cc + 0x70);
            do {
                o->v0();
                i++;
                o = (Poly0 *)((char *)o + 0x24);
            } while (i < c->cnt280);
        }
    }
    return 1;
}
