//cpp
// @symbol _ZN17dScMgTrampoline_c8BehaviorEv
#include "decl_common.h"
#include "dScMgTrampoline_c.h"
/* dScMgTrampoline_c::Behavior -- vtable slot 6, ov006 0x021214f8.
 *
 * Named from the table: 0x021214f8 is the word slot 6 of
 * _ZTV17dScMgTrampoline_c holds where dScMgD3DBase_c's table holds something
 * else, so it is this class's own override of the virtual fBase_c declares.
 *
 * 0x5004 IS A POINTER-TO-MEMBER-FUNCTION, not a pad. The pre-migration file
 * had already worked that out -- it invented a `struct C` whose only purpose
 * was to put a `void (C::*)()` eight bytes in -- but it could not say WHICH
 * class, because it had no name for the receiver. With the receiver named the
 * member pointer is this class's own, and the header's `pad_5004[0x8]` is
 * exactly its two words. It stays declared as pad there rather than retyped:
 * eight bytes either way, and retyping it is a claim about every other file
 * that includes the header, not about this one. */

typedef void (dScMgTrampoline_c::*TrampolineState)();

s32 dScMgTrampoline_c::Behavior()
{
    int saved = data_ov006_02140588;
    func_ov006_02120c40();
    (this->*(*(TrampolineState *)pad_5004))();
    func_ov006_0212157c(this);
    func_ov006_021209ac((short *)pad_5d84);
    if (saved != data_ov006_02140588)
        func_ov004_020adb1c(data_ov006_02140588);
    return 1;
}
