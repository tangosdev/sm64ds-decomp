//cpp
#include "types.h"
// @symbol _ZN10dScEntry_c8BehaviorEv
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* dScEntry_c::Behavior() -- vtable slot 6. extern "C" carries the literal
 * mangled name unmangled. Runs the sub-screen fade-out if one is in flight,
 * then the per-frame callback, then every live icon. The local `Self` models
 * only the fields this function touches -- pmf at 0x5c and the icon_c[9] array
 * at 0x70 -- rather than including the shared header, matching the family
 * convention; see include/dScEntry_c.h. */
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
    u8 iconCount;
    u8 pad281[2];
    u8 fadeBrightness;
    u8 fadeTick;
    u8 isFading;
};

extern "C" int _ZN10dScEntry_c8BehaviorEv(Self *c)
{
    char *cc = (char *)c;

    if (c->isFading != 0) {
        c->fadeTick++;
        if ((c->fadeTick & 1) == 0) {
            c->fadeBrightness--;
            func_020551f0((void *)0x4001050, c->fadeBrightness);
            if (c->fadeBrightness == 0) {
                *(u16 *)0x4001050 = 0;
                c->fadeBrightness = 0;
                c->isFading = 0;
            }
        }
    }

    if (data_0209f5bc->v5() == 0 || c->isFading != 0)
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
        int n = c->iconCount;
        if (n > 0) {
            Poly0 *o = (Poly0 *)(cc + 0x70);
            do {
                o->v0();
                i++;
                o = (Poly0 *)((char *)o + 0x24);
            } while (i < c->iconCount);
        }
    }
    return 1;
}
