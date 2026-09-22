/* The two-word pair every cMgSmartball_object_c constructor takes by pointer.
 *
 * func_ov006_0211474c (that class's C1/C2 in all but name) copies its fourth
 * argument's two words into BOTH mCurrent0/mCurrent1 and mInitial0/mInitial1 --
 * see include/cMgSmartball_object_c.h, which names those four fields and
 * deliberately claims nothing about what they mean. This type claims no more:
 * two words, in that order, nothing about their meaning.
 *
 * IT IS A STRUCT AND NOT TWO INTS BECAUSE THE COPY IS LOAD-BEARING. Every
 * caller builds the pair in one local and hands the constructor a pointer to a
 * SECOND local copied from the first -- func_ov006_02115b0c.c's own banner
 * records the same idiom ("V2 passed via pointer to a caller-local copy") and
 * spells the type locally because it is a C translation unit under a different
 * compiler pin. Written as `s32 r[2]` with the two words copied element by
 * element instead, dScMgSmartball_c::InitResources comes out 0x8cc against the
 * ROM's 0x8dc -- sixteen bytes short. The struct assignment is what the ROM's
 * compiler saw. */
#ifndef OV006_SMARTBALL_PAIR_H
#define OV006_SMARTBALL_PAIR_H
#include "types.h"

struct cMgSmartballPair {
    s32 v0;
    s32 v1;
};

#endif
