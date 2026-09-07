//cpp
/* Original class-form reconstruction of fBase_c::SceneNode. */
#include "fBase_c.h"

/* Written first because mwccarm emits functions in reverse source order. */
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
