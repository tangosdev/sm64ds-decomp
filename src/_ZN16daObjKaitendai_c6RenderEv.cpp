//cpp
// @symbol _ZN16daObjKaitendai_c6RenderEv
#include "daObjKaitendai_c.h"

/* daObjKaitendai_c::Render -- vtable slot 9, ov002 0x020b66f0.
 * Behavior (slot 6) already holds the key function for this class, so
 * declaring Render after it is layout- and key-function-neutral. Render
 * dispatches through dBgActor_c's own mModel (0xd4), as every sibling in
 * this series does. */
s32 daObjKaitendai_c::Render()
{
    mModel.Render(0);
    return 1;
}
