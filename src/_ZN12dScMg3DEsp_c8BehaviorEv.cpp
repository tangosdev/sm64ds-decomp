//cpp
// @symbol _ZN12dScMg3DEsp_c8BehaviorEv
#include "decl_common.h"
#include "dScMg3DEsp_c.h"
/* dScMg3DEsp_c::Behavior -- vtable slot 6, ov006 0x020e9e00.
 *
 * Attributed by the vtable: dScMg3DEsp_c's own table is ov006 0x0213c8c4 and
 * its slot 6 relocates here, so this is this class's own override rather than
 * anything it inherits from dScMgSingle3DBase_c. The signature is
 * include/dScMgBase_c.h's own slot 6, `virtual s32 Behavior()`.
 *
 * THE STATE TABLE IS A POINTER-TO-MEMBER ARRAY, and the receiver type matters
 * to the bytes. The pre-migration file declared a local incomplete `class C`
 * for it, which forces mwccarm's most general member-pointer representation;
 * naming the real class makes it the single-inheritance one, and the element
 * stride the index is scaled by follows from that. Only the byte gate settles
 * which one the cartridge used -- it is this one.
 *
 * The three members it steps are the ones include/dScMg3DEsp_c.h's banner
 * already names from the destructor: unk_553c is the state index,
 * mTextureTransformer at 0x51f4 is advanced as an Animation, and the untyped
 * 0x21c-byte block at 0x4fd8 is stepped by its own helper. */

extern "C" {
void func_ov006_020e8a44(dScMg3DEsp_c *self);
void _ZN9Animation7AdvanceEv(void *anim);
}

typedef void (dScMg3DEsp_c::*dScMg3DEsp_cState)();
extern "C" dScMg3DEsp_cState data_ov006_02141f2c[];

s32 dScMg3DEsp_c::Behavior()
{
    int idx = unk_553c;
    (this->*data_ov006_02141f2c[idx])();
    func_ov006_020e8a44(this);
    _ZN9Animation7AdvanceEv(mTextureTransformer);
    func_ov006_020e7be8(pad_4fd8);
    return 1;
}
