/* class dScMg3DEsp_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMg3DEsp_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). The coined English Spawn-function name was
 * MgPsycheOut_Spawn; the factory now carries the reconstructed
 * dScMg3DEsp_c_classInit spelling, and the class itself takes its
 * real ROM identity, matching this tree's current convention.
 *
 * SIZE 0x5558, from d_s_mg3_d_esp.cpp's own
 * `_ZN7fBase_cnwEj(0x5558)`.
 *
 * This class does NOT use the 0x270-byte shared table five siblings do
 * (see include/dScMgMemory_c.h's own note) -- its own construction/
 * destruction is entirely different, evidenced directly by its own
 * destructor (func_ov006_020e7660, pre-migration):
 *
 *   - TWO MODELS at 0x4f38 and 0x4f88, 0x50 bytes each (include/Model.h).
 *   - A dMg3DEspModel_c-SHAPED BLOCK at 0x4fd8, size 0x21c: destroyed via
 *     _ZN15dMg3DEspModel_cD1Ev (ModelAnim @ +0xc, TextureSequence @ +0x70,
 *     dMg3DEspAnimSet_c @ +0x84, plus six Release() calls on unrelated GLOBAL
 *     SharedFilePtr singletons, not per-instance data). One field within
 *     it is real matched access -- unk_51e4 (src/func_ov006_020e9c20.c)
 *     -- named. The inferred helper types now have exact constructor,
 *     destructor, and ordinary-method definitions, but this scene keeps raw
 *     storage until its construction and teardown are promoted together.
 *   - A TextureTransformer at 0x51f4, 0x14 bytes (include/TextureTransformer.h).
 *   - OWN TAIL, 0x5208..0x5558: one more field is real matched access
 *     (unk_553c, same file), the rest stays pad.
 *
 * The ROM's teardown order -- TextureTransformer, dMg3DEspModel_c, model 2,
 * model 1 -- is exactly reverse declaration order and therefore supports
 * eventual compiler-owned member destruction. The members remain raw here
 * only until the scene-wide construction/destruction boundary is verified as
 * one change.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN12dScMg3DEsp_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one.
 *
 * SM64DS RTTI names the implementation dScMg3DEsp_c. The reconstructed factory
 * dScMg3DEsp_c_classInit (historical alias MgPsycheOut_Spawn) installs this class's
 * cartridge vtable for the MG_3DESP registry profile.
 */
#ifndef DSCMG3DESP_C_H
#define DSCMG3DESP_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" void _ZN5ModelD1Ev(void *);
extern "C" void _ZN18TextureTransformerD1Ev(void *);
extern "C" void _ZN15dMg3DEspModel_cD1Ev(void *c);

struct dScMg3DEsp_c : dScMgSingle3DBase_c {
    virtual ~dScMg3DEsp_c();
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  Virtual50();                          /* slot 20 */

    u8  mModel1[0x50];        /* 0x4f38 -- Model, raw bytes, see file banner */
    u8  mModel2[0x50];        /* 0x4f88 -- Model, raw bytes, see file banner */
    u8  pad_4fd8[0x20c];       /* 0x4fd8 -- raw dMg3DEspModel_c storage */
    s32 unk_51e4;               /* 0x51e4 */
    u8  pad_51e8[0xc];          /* 0x51e8 */
    u8  mTextureTransformer[0x14]; /* 0x51f4 -- TextureTransformer, raw bytes */
    u8  pad_5208[0x334];        /* 0x5208 */
    s32 unk_553c;                /* 0x553c */
    u8  pad_5540[0x18];          /* 0x5540 */

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and neither adds a field, so the size assert below
       is untouched. The destructor above stays the key function, so no
       translation unit starts emitting _ZTV12dScMg3DEsp_c because of these.
       Signatures are include/fBase_c.h's and include/dScMgBase_c.h's own,
       copied unchanged. --- */
    s32 InitResources();      /* slot 0 -- src/_ZN12dScMg3DEsp_c13InitResourcesEv.cpp */
    s32 CleanupResources();   /* slot 3 -- ov006 0x020e9cec */
    s32 Behavior();           /* slot 6 -- ov006 0x020e9e00 */
    s32 Render();             /* slot 9 -- src/_ZN12dScMg3DEsp_c6RenderEv.cpp */
};

typedef char dScMg3DEsp_c_size_must_be_0x5558[sizeof(dScMg3DEsp_c) == 0x5558 ? 1 : -1];

#endif
