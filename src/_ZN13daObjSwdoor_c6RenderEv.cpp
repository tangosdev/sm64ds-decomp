//cpp
// @symbol _ZN13daObjSwdoor_c6RenderEv
#include "daObjSwdoor_c.h"

/* daObjSwdoor_c::Render -- vtable slot 9, ov002 0x020babf0.
 * This class's only real slot; declaring it out of line makes this file the
 * key function (see include/daObjSwdoor_c.h). Render dispatches through
 * dBgActor_c's own mModel (0xd4), as every sibling in this series does. */
s32 daObjSwdoor_c::Render()
{
    mModel.Render(0);
    return 1;
}
