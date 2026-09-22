//cpp
// @symbol _ZN17daObjKurumajiku_c6RenderEv
#include "daObjKurumajiku_c.h"

/* daObjKurumajiku_c::Render -- vtable slot 9, ov002 0x020b6b10.
 * Behavior (slot 6) already holds the key function for this class, so
 * declaring Render after it is layout- and key-function-neutral. Render
 * dispatches through dBgActor_c's own mModel (0xd4), as every sibling in
 * this series does. */
s32 daObjKurumajiku_c::Render()
{
    mModel.Render(0);
    return 1;
}
