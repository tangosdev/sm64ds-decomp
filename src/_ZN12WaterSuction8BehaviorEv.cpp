//cpp
// @symbol _ZN12WaterSuction8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterSuction.h"
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* thiz, void* clsn);
extern void _ZN5dCc_c5ClearEv(void* a);
extern void _ZN5dCc_c6UpdateEv(void* a);
}

struct C;
typedef void (C::*PMF)();
struct Obj { char pad[8]; PMF pmf; };
struct C {
    char pad[0x30c];
    Obj* obj;
};

int WaterSuction::Behavior()
{
    C* c = (C*)((char*)this);
    DecIfAbove0_Short((unsigned short*)((char*)&(*(u8 *)&mStateTimer)));
    Obj* o = c->obj;
    if (*(int*)((char*)o + 8) != 0) {
        (c->*(o->pmf))();
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), ((char*)this) + 0x110);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov026_02112324(((char*)this));
    _ZN5dCc_c5ClearEv((char*)&(*(dCcAcPos_c *)&mdCcAcPos_c));
    _ZN5dCc_c6UpdateEv((char*)&(*(dCcAcPos_c *)&mdCcAcPos_c));
    return 1;
}
