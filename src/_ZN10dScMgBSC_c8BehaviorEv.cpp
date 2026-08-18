//cpp
// @symbol _ZN10dScMgBSC_c8BehaviorEv
#include "dScMgBSC_c.h"
/* dScMgBSC_c::Behavior -- vtable slot 6, ov006 0x021254c0.
 *
 * Named from the table, not from the old file's `recovered name:` comment
 * (which agreed here): 0x021254c0 is the word slot 6 of _ZTV10dScMgBSC_c
 * holds where dScMgSingle3DBase_c's table holds something else, so it is
 * this class's own override of the virtual fBase_c declares.
 *
 * The dispatch is a pointer-to-member table indexed by unk_51b8 -- the
 * minigame's state. The pre-migration file had to invent a `struct C` with
 * a 0x51b8 pad just to spell that member pointer; with the real class it is
 * the field the header already names. */

typedef void (dScMgBSC_c::*BSCState)();

struct dScMgBSC_c_StateEntry {
    BSCState mFn;
};

extern "C" {
extern dScMgBSC_c_StateEntry data_ov006_02142f94[];
void func_ov004_020b65e4(void);
int  func_ov006_020c19d0(void *p);
}

s32 dScMgBSC_c::Behavior()
{
    (this->*(data_ov006_02142f94[unk_51b8].mFn))();
    func_ov004_020b65e4();
    func_ov006_020c19d0(mTable);
    return 1;
}
