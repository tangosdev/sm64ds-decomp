#ifndef ARM9_ACTION_WALK_H
#define ARM9_ACTION_WALK_H

#include "types.h"
#include "MWCPlusLib.h"
#include "private/throw_context.h"

/* The 0x18-byte cursor func_02071be8 keeps on its stack: the action iterator
 * func_02072dac fills, and one word nothing in this image reads.
 * func_02071d3c and func_02072014 copy it member by member into the front of
 * an ActionWalk. */
struct ActionCursor {
    ActionIterator iterator; /* 0x00..0x13 */
    int unknown_14;
};

/* Walk buffer shared with func_02072d90. The exception-table cursor fills
 * 0x00..0x13; the word at 0x14 remains unknown. The throw context
 * func_02071910 reads begins at 0x18. */
struct ActionWalk {
    ActionIterator iterator; /* 0x00..0x13 */
    int unknown_14;
    ThrowContext mThrow; /* 0x18 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char
    ActionWalk_throw_state_must_start_at_0x18[(int)&((struct ActionWalk *)0)->mThrow == 0x18 ? 1
                                                                                             : -1];
#endif

#endif
