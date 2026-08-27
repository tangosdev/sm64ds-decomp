//cpp
// @symbol _ZN13dScMgMemory_c8BehaviorEv
#include "dScMgMemory_c.h"
/* dScMgMemory_c::Behavior -- vtable slot 6, ov006 0x020f5388.
 *
 * Attributed by the vtable: dScMgMemory_c's own table is ov006 0x0213d1b8 and
 * its slot 6 relocates here. The signature is include/dScMgBase_c.h's own slot
 * 6, `virtual s32 Behavior()`.
 *
 * THE STATE TABLE IS A POINTER-TO-MEMBER ARRAY indexed by unk_5314, the same
 * shape dScMg3DEsp_c::Behavior has with its own table. The pre-migration file
 * reached it through a local `struct C { char pad[0x5314]; int idx; }` -- a
 * hand-cut stand-in for this class whose only purpose was to name that one
 * field -- and through a one-member `Entry` wrapper that added nothing to the
 * layout. Both are gone: the receiver is the real class, the index is the real
 * member, and the array is the member pointers themselves. Byte-identical,
 * which is what makes the 0x5314 offset evidence rather than a guess. */

extern "C" {
void func_ov004_020b65e4(void);
int func_ov006_020c19d0(void *p);
}

typedef void (dScMgMemory_c::*dScMgMemory_cState)();
extern "C" dScMgMemory_cState data_ov006_021422dc[];

s32 dScMgMemory_c::Behavior()
{
    (this->*data_ov006_021422dc[unk_5314])();
    func_ov004_020b65e4();
    func_ov006_020c19d0((char *)this + 0x4f38);
    return 1;
}
