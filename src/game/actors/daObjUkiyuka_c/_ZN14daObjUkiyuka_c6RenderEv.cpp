//cpp
// @symbol _ZN14daObjUkiyuka_c6RenderEv
#include "daObjUkiyuka_c.h"

/* daObjUkiyuka_c::Render -- vtable slot 9, ov002 0x020b646c.
 * Behavior (slot 6) already holds the key function for this class, so
 * declaring Render after it is layout- and key-function-neutral. Render
 * dispatches through dBgActor_c's own mModel (0xd4), as every sibling in
 * this series does. */
s32 daObjUkiyuka_c::Render()
{
    mModel.Render(0);
    return 1;
}
