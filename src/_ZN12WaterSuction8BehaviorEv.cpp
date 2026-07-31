//cpp
// @symbol _ZN12WaterSuction8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterSuction.h"
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* clsn);
extern void _ZN12CylinderClsn5ClearEv(void* a);
extern void _ZN12CylinderClsn6UpdateEv(void* a);
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
    DecIfAbove0_Short((unsigned short*)((char*)&unk_100));
    Obj* o = c->obj;
    if (*(int*)((char*)o + 8) != 0) {
        (c->*(o->pmf))();
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), ((char*)this) + 0x110);
    unk_08c = unk_092;
    unk_08e = unk_094;
    unk_090 = unk_096;
    func_ov026_02112324(((char*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
    _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos);
    return 1;
}
