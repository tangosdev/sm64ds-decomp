#ifndef DAOBJC0WATER_C_H
#define DAOBJC0WATER_C_H

#include "types.h"

/* daObjC0Water_c -- the basement moat in the castle courtyard interior, the
 * one that drains when the level's flood flag clears.
 *
 * THE NAME COMES OUT OF THE CARTRIDGE. The vtable storage at ov012 0x02112400
 * is {0, 0x021123c4}; that _ZTI is the three-word __si_class_type_info record
 * {0x0209a764, 0x021123d0, 0x021089ec} -- first word __si_class_type_info's
 * vtable+8, second the length-prefixed string "14daObjC0Water_c" at 0x021123d0,
 * third _ZTI10dBgActor_c, which is homed in ov002 rather than arm9. So the ROM
 * states both the name and the `: dBgActor_c` base. The file was BasementWater.h
 * until that read renamed it.
 *
 * The single profile C0_WATER already carried its ROM name; its descriptor at
 * 0x021123e4 names the factory at 0x02111730 and is the third object in the
 * .data run this class owns.
 *
 * LAYOUT unchanged from the BasementWater spelling: dBgActor_c ends at 0x31e and
 * sizeof rounds it to 0x320, where the owned TextureTransformer the factory
 * constructs sits (_ZN18TextureTransformerC1Ev, a relocation the ROM build
 * checks). SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this
 * declaration; it is not independent evidence about the ROM.
 */

#ifdef __cplusplus

/* MEASURED -- dBgActor_c.h must come before Model.h reaches us, because
   Matrix4x3 has two guarded spellings sharing the same 0x30 bytes and
   whichever a TU sees first stands. See include/dBgActor_c.h. */
#include "dBgActor_c.h"
#include "TextureTransformer.h"

struct daObjC0Water_c : dBgActor_c {
    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;/* 0x320 */
    s32 mLoweredY;                     /* 0x334 */
    u32 mSoundID;                      /* 0x338 */
    u16 mSoundTimer;                   /* 0x33c */
    u8  mWasJustDrained;               /* 0x33e */

    /* --- vtable. The destructor is INLINE, and that is load-bearing rather
       than a style choice. Out of line, mwccarm emits D0 before D1 -- the
       reverse of the ROM's 0x02111450 D1 / 0x0211149c D0 order, which makes
       objisolate refuse the whole TU -- and additionally emits a D2 that has
       no home anywhere in the cartridge. Inline, it emits exactly the retail
       D1/D0 pair in ROM order and no D2. Nothing derives from this class.

       The body stays empty: every store and call the ROM's D1 contains is
       compiler-generated -- this class's vptr, dBgActor_c's inlined, then the
       TextureTransformer, dBgW_KcMbg and Model members and the dActor_c base.

       Declared FIRST, deliberately: with the destructor inline the key
       function is the first DECLARED non-inline virtual, so this ordering is
       what makes src/actors/d_a_obj_c0_water.cpp the TU that emits the
       _ZTV/_ZTI/_ZTS group -- exactly what the promotion needs it to be. --- */
    virtual ~daObjC0Water_c() {}       /* slots 16 (D1), 17 (D0) */

    /* --- overrides of inherited fBase_c slots. The ROM installs all four in
       the vtable at 0x02112408; they were declared as ordinary methods here
       before, which contradicted the cartridge. Slot 31, dBgActor_c::Kill, is
       NOT overridden -- the table's last word is dBgActor_c's own. --- */
    virtual int InitResources();       /* slot 0 -- 0x0211164c */
    virtual int CleanupResources();    /* slot 3 -- 0x021114fc */
    virtual int Behavior();            /* slot 6 -- 0x02111574 */
    virtual int Render();              /* slot 9 -- 0x02111540 */
};

/* Holds the chain to the size the factory's operator new(0x340) evidences.
   A silently-added member anywhere fails this. */
typedef char daObjC0Water_c_size_must_be_0x340[sizeof(daObjC0Water_c) == 0x340 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJC0WATER_C_H */
