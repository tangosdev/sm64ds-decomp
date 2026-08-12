/* class dScMg3DEsp_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMg3DEsp_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). English Spawn-function name is MgPsycheOut_Spawn;
 * kept as the symbol name for the factory (already attributed), but the
 * class itself takes its real ROM identity, matching this tree's current
 * convention.
 *
 * SIZE 0x5558, from MgPsycheOut_Spawn.cpp's own
 * `_ZN9ActorBasenwEj(0x5558)`.
 *
 * This class does NOT use the 0x270-byte shared table five siblings do
 * (see include/dScMgMemory_c.h's own note) -- its own construction/
 * destruction is entirely different, evidenced directly by its own
 * destructor (func_ov006_020e7660, pre-migration):
 *
 *   - TWO MODELS at 0x4f38 and 0x4f88, 0x50 bytes each (include/Model.h).
 *   - AN UNTYPED BLOCK at 0x4fd8, size 0x21c: destroyed via
 *     func_ov006_020e80d8 (ModelAnim @ +0xc, TextureSequence @ +0x70, a
 *     helper @ +0x84, plus six Release() calls on unrelated GLOBAL
 *     SharedFilePtr singletons, not per-instance data). One field within
 *     it is real matched access -- unk_51e4 (src/func_ov006_020e9c20.c)
 *     -- named, the rest stays pad. Not typed further: this exact block
 *     shape isn't shared by any other class in this slice, so inventing
 *     a name for it is a separate slice's work, same reasoning as the
 *     shared table.
 *   - A TextureTransformer at 0x51f4, 0x14 bytes (include/TextureTransformer.h).
 *   - OWN TAIL, 0x5208..0x5558: one more field is real matched access
 *     (unk_553c, same file), the rest stays pad.
 *
 * Models are left as raw bytes with explicit destructor calls, not typed
 * auto-destructing members -- the ROM's own order (TextureTransformer,
 * then the untyped block, then Model@0x4f88, then Model@0x4f38) can't be
 * reproduced by Itanium auto-destruction, which always runs strictly
 * after the user body in reverse declaration order. Same reasoning as
 * include/dScMgRoulette_c.h's own note.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN12dScMg3DEsp_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMG3DESP_C_H
#define DSCMG3DESP_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" void _ZN5ModelD1Ev(void *);
extern "C" void _ZN18TextureTransformerD1Ev(void *);
extern "C" void func_ov006_020e80d8(void *c); /* decl_common.h's own signature */

struct dScMg3DEsp_c : dScMgSingle3DBase_c {
    virtual ~dScMg3DEsp_c();

    u8  mModel1[0x50];        /* 0x4f38 -- Model, raw bytes, see file banner */
    u8  mModel2[0x50];        /* 0x4f88 -- Model, raw bytes, see file banner */
    u8  pad_4fd8[0x20c];       /* 0x4fd8 -- untyped block, see file banner */
    s32 unk_51e4;               /* 0x51e4 */
    u8  pad_51e8[0xc];          /* 0x51e8 */
    u8  mTextureTransformer[0x14]; /* 0x51f4 -- TextureTransformer, raw bytes */
    u8  pad_5208[0x334];        /* 0x5208 */
    s32 unk_553c;                /* 0x553c */
    u8  pad_5540[0x18];          /* 0x5540 */
};

typedef char dScMg3DEsp_c_size_must_be_0x5558[sizeof(dScMg3DEsp_c) == 0x5558 ? 1 : -1];

#endif
