//cpp
// @symbol _ZN14dScMgMemory2_c8BehaviorEv
#include "dScMgMemory2_c.h"
/* dScMgMemory2_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the three slots where this class's table
 * differs from dScMgSingle3DBase_c's. One dispatch through the pointer-to-member
 * table at data_ov006_021423e0, indexed by the state word at 0x53d4, then the two
 * per-frame calls every minigame in this family makes.
 *
 * `C` stays a bare non-polymorphic stand-in rather than dScMgMemory2_c: a PMF on a
 * polymorphic class is a different representation, so the shape here is codegen and
 * not decoration -- the same reason src/_ZN15daObjMarioCap_c8BehaviorEv.cpp keeps
 * its own stand-in. Its `idx` at 0x53d4 is the header's unk_53d4. */

struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_021423e0[];
struct C { char pad[0x53d4]; int idx; };

extern "C" void func_ov004_020b65e4(void);
extern "C" int func_ov006_020c19d0(void *p);

s32 dScMgMemory2_c::Behavior()
{
    C *c = (C *)this;
    (c->*(data_ov006_021423e0[c->idx].pmf))();
    func_ov004_020b65e4();
    func_ov006_020c19d0((char *)c + 0x4f38);
    return 1;
}
