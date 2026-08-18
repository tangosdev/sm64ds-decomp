//cpp
// @symbol _ZN15daObjGuragura_c6RenderEv
#include "daObjGuragura_c.h"

/* daObjGuragura_c::Render -- vtable slot 9, ov002 0x020b6144.
 * reloc verified earlier (rtti_cls daObjGuragura_c, header banner already
 * documents the override and the key-function ordering this file now
 * completes -- Render stays the FIRST virtual declared in
 * include/daObjGuragura_c.h, so this file, not
 * src/_ZN15daObjGuragura_c8BehaviorEv.cpp, is the one that becomes the key
 * function and picks up _ZTV15daObjGuragura_c; that symbol is already
 * delinked data (ov002 0x02109084), so this file drops out of
 * tools/eligible.py's name list -- expected, not a regression.
 *
 * Render dispatches through dBgActor_c's own mModel (0xd4), exactly as
 * every sibling in this series does. */
s32 daObjGuragura_c::Render()
{
    mModel.Render(0);
    return 1;
}
