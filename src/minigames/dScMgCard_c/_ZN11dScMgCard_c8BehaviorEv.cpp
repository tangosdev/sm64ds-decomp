//cpp
// @symbol _ZN11dScMgCard_c8BehaviorEv
#include "decl_common.h"
#include "dScMgCard_c.h"
/* dScMgCard_c::Behavior -- vtable slot 6, ov006 0x020dabec.
 *
 * Named from the table: 0x020dabec is the word slot 6 of _ZTV11dScMgCard_c
 * holds where its base's table holds something else, so it is this class's
 * own override of the virtual fBase_c declares.
 *
 * mFrameCounter is the frame counter the header's "own tail" note lists among the
 * nine fields five of this class's methods already touch; pad_4f38 is the
 * start of the 0x270-byte table six classes in this family share, which
 * func_ov006_020c19d0 steps. */

extern "C" {
void func_ov006_020c19d0(void *table);
void func_ov004_020b65e4(void *p);
}

s32 dScMgCard_c::Behavior()
{
    mFrameCounter += 1;
    func_ov006_020c19d0(pad_4f38);
    func_ov004_020b65e4(func_ov006_020dac34(this));
    return 1;
}
