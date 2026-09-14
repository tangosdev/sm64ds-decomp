//cpp
/* fBase_c::SceneNode -- the intrusive scene-graph node every actor carries
 * (arm9 0x0203b4ac..0x0203b4dc).
 *
 * Two members, written first-to-last because mwccarm emits .text in reverse
 * source order. Reset clears the four links and deliberately leaves the owner
 * back-pointer at +0x10 alone; fBase_c's own constructor writes that one
 * (see the Manager ctor in include/fBase_c.h).
 *
 * deslop: no leftovers. Both members are real, fully typed, and named by the
 * ROM's own symbols.
 */
#include "fBase_c.h"

#ifndef SM64DS_PLATFORM_PC
/* Five words: parent, child, prev, next, owner. fBase_c.h asserts the whole
   0x50 actor root; this pins the nested node itself. */
typedef char SceneNode_size_must_be_0x14[sizeof(fBase_c::SceneNode) == 0x14 ? 1 : -1];
#endif

fBase_c::SceneNode::SceneNode()
{
    Reset();
}

void fBase_c::SceneNode::Reset()
{
    parent = 0;
    child = 0;
    prev = 0;
    next = 0;
}
