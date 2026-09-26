//cpp
// @symbol _ZN12dScMg3DEsp_c8BehaviorEv
#include "dMg3DEspModel_c.h"
#include "dScMg3DEsp_c.h"
#include "TextureTransformer.h"
/* dScMg3DEsp_c::Behavior, vtable slot 6 (the class's vtable is ov006
 * 0x0213c8c4). Runs the current state from the member-function table,
 * indexed by unk_553c, then steps a helper, the texture animation and the
 * dMg3DEspModel_c block. The table's receiver is the complete class; an
 * incomplete receiver compiles to the same bytes here. */

extern "C" void func_ov006_020e8a44(dScMg3DEsp_c *self);

typedef void (dScMg3DEsp_c::*dScMg3DEsp_cState)();
extern "C" dScMg3DEsp_cState data_ov006_02141f2c[];

s32 dScMg3DEsp_c::Behavior()
{
    (this->*data_ov006_02141f2c[unk_553c])();
    func_ov006_020e8a44(this);
    ((TextureTransformer *)mTextureTransformer)->Advance();
    ((dMg3DEspModel_c*)pad_4fd8)->Behavior();
    return 1;
}
