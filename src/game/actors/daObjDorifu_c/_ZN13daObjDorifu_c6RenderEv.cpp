//cpp
// @symbol _ZN13daObjDorifu_c6RenderEv
#include "daObjDorifu_c.h"

/* daObjDorifu_c::Render -- vtable slot 9, ov002 0x020b4bc4.
 * reloc: _ZTV13daObjDorifu_c+0x24 -> 0x020b4bc4,
 * _ZTV10dBgActor_c+0x24 -> 0x02043af0 (different, real override).
 *
 * The pre-migration recovery indexed a Model[5]-shaped array at 0x320 with
 * the byte at 0xdc8 and called the array element's vtable slot 5 with a
 * literal 0 argument -- Model::Render(const Vector3 *), NULL. Named here as
 * mPlankModels[mActivePlank].Render(NULL). */
s32 daObjDorifu_c::Render()
{
    mPlankModels[mActivePlank].Render(0);
    return 1;
}
