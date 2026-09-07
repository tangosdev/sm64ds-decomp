//cpp
// @symbol _ZN13daObjKuruma_c6RenderEv
#include "daObjKuruma_c.h"

/* daObjKuruma_c::Render -- vtable slot 9, ov002 0x020b68f8.
 * Render stays the FIRST virtual declared in include/daObjKuruma_c.h (see
 * that header's own comment), so this file becomes the key function and
 * picks up _ZTV13daObjKuruma_c -- expected, drops out of
 * tools/eligible.py's name list. Render dispatches through dBgActor_c's own
 * mModel (0xd4), as every sibling in this series does. */
s32 daObjKuruma_c::Render()
{
    mModel.Render(0);
    return 1;
}
