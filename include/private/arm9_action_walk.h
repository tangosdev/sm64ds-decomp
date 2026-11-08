#ifndef ARM9_ACTION_WALK_H
#define ARM9_ACTION_WALK_H

#include "types.h"
#include "MWCPlusLib.h"

/* Walk buffer shared with func_02072d90. The exception-table cursor fills
 * 0x00..0x13; the word at 0x14 remains unknown. The throw context
 * func_02071910 reads begins at 0x18. */
struct ActionWalk {
    ActionIterator iterator; /* 0x00..0x13 */
    char unknown_14[4];
    char mThrow;
};

#ifndef SM64DS_PLATFORM_PC
typedef char
    ActionWalk_throw_state_must_start_at_0x18[(int)&((struct ActionWalk *)0)->mThrow == 0x18 ? 1
                                                                                             : -1];
#endif

#endif
