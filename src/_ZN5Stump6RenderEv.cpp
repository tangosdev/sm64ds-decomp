//cpp
// @symbol _ZN5Stump6RenderEv
#include "Stump.h"
// recovered name: daObjPile_c_Render
/* recovered: renamed to Class_Method */
/* daObjPile_c::Render - byte-matched vs the ROM at mwccarm 2004/b56 with strict relocs (tools/match.py, size from ov091 symbols.txt); the vtable-slot-guess marker it carried was stale */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
s32 Stump::Render() {
    Derived * d = (Derived *)this; Base *b = &d->base; b->m(0); return 1; }
