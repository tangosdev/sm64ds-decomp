//cpp
// @symbol _ZN17daObjFloatBoard_c6RenderEv
#include "daObjFloatBoard_c.h"

/* daObjFloatBoard_c::Render -- vtable slot 9, ov002 0x020b5c24.
 * CleanupResources (slot 3) already holds the key function for this class,
 * so declaring Render after Behavior is layout- and key-function-neutral.
 * Render dispatches through dBgActor_c's own mModel (0xd4), as every
 * sibling in this series does. */
s32 daObjFloatBoard_c::Render()
{
    mModel.Render(0);
    return 1;
}
