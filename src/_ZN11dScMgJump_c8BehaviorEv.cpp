//cpp
// @symbol _ZN11dScMgJump_c8BehaviorEv
#include "dScMgJump_c.h"
/* dScMgJump_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the three slots where this class's table
 * differs from dScMgD3DBase_c's. The whole body is one dispatch through the
 * pointer-to-member the object carries at 0x5004, which is the state machine --
 * include/dScMgJump_c.h records that pair as unk_5004/unk_5008, the two words a
 * PMF occupies.
 *
 * `C` stays a bare non-polymorphic stand-in rather than dScMgJump_c: a PMF on a
 * polymorphic class is a different representation, so the shape here is codegen
 * and not decoration -- the same reason src/_ZN15daObjMarioCap_c8BehaviorEv.cpp
 * keeps its own stand-in. */

struct C;
typedef void (C::*PMF)();
struct C { char pad[0x5004]; PMF m; };

s32 dScMgJump_c::Behavior()
{
    C *c = (C *)this;
    (c->*c->m)();
    return 1;
}
