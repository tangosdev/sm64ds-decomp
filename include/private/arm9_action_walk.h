#ifndef ARM9_ACTION_WALK_H
#define ARM9_ACTION_WALK_H

#include "types.h"

/* Walk buffer shared with func_02072d90. The action iterator is the first
 * three words; the throw context func_02071910 reads begins at 0x18. */
struct ActionWalk {
    int mFuncStart;
    u8 *mActionTable;
    u8 *mAction;
    char pad_0c[0xc];
    char mThrow;
};

#endif
