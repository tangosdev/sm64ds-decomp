/* class dScMgSound_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgSound_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). English Spawn-function name is MgBoomBox_Spawn; kept
 * as the symbol name for the factory (already attributed), but the class
 * itself takes its real ROM identity, matching this tree's current
 * convention.
 *
 * SIZE 0x562c, from MgBoomBox_Spawn.cpp's own `_ZN9ActorBasenwEj(0x562c)`.
 *
 * ONE OWN MEMBER, evidenced by the destructor (func_ov006_02119904,
 * pre-migration): a single explicit call to func_ov006_020c3288(c +
 * 0x4f38), mirrored by the factory's own func_ov006_020c33dc(p + 0x4f38)
 * constructor call. 0x562c - 0x4f38 = 0x6f4, so this member spans the
 * entire remainder of the object. This is a DIFFERENT ctor/dtor pair from
 * the 0x270-byte one shared by five siblings (dScMgCard_c, dScMgBSC_c,
 * dScMgMemory_c, dScMgMemory2_c, dScMgRoulette_c, dScMgMCarlo2_c) --
 * func_ov006_020c33dc/020c3288's own bodies show BlendModelAnim, Model,
 * TextureSequence, TextureTransformer and ~17 SharedFilePtr entries, a
 * larger and structurally distinct helper. No source file in this tree
 * references any offset within it, so left as raw bytes for the same
 * reason as the shared table (see include/dScMgMemory_c.h's own note) --
 * nothing to preserve, nothing to invent.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN12dScMgSound_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy for its own key-function TU. No separate
 * operator delete is needed -- dScMgBase_c, two levels up, already
 * provides one. */
#ifndef DSCMGSOUND_C_H
#define DSCMGSOUND_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c3288(char *t); /* decl_common.h's own signature */

struct dScMgSound_c : dScMgSingle3DBase_c {
    virtual ~dScMgSound_c();

    u8  mTable[0x6f4]; /* 0x4f38 -- ctor func_ov006_020c33dc, dtor func_ov006_020c3288 */
};

typedef char dScMgSound_c_size_must_be_0x562c[sizeof(dScMgSound_c) == 0x562c ? 1 : -1];

#endif
