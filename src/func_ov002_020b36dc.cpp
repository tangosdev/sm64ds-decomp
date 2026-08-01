//cpp
#include "types.h"
// @symbol func_ov002_020b36dc
// recovered name: BigBrickBlock_OnKicked
/* recovered: renamed to Class_Method */
/* daObjBlockL_c::OnKicked - recovered from vtable slot identity */
struct State { int pad[2]; int field8; };
struct Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30();
    virtual void m();
};

extern "C" void func_ov002_020b36dc(Base *self, struct State *st)
{
    int b1 = (int)(*(u16 *)((char *)self + 0xc) == 0x2e);
    int r1;
    if (b1 != 0) self->m();
    r1 = st->field8;
    if (r1 == 3) return;
    {
        int b2 = (int)(*(u16 *)((char *)self + 0xc) == 0x11);
        if (b2 != 0) {
            if (r1 != 2) return;
            self->m();
        } else {
            self->m();
        }
    }
}
