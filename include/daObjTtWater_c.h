#ifndef DAOBJTTWATER_C_H
#define DAOBJTTWATER_C_H

#include "types.h"

/* daObjTtWater_c -- the draining pool in Tiny-Huge Island.
 *
 * THE NAME COMES OUT OF THE CARTRIDGE. The vtable header at ov033 0x02112438
 * is {0, 0x021123fc}; that _ZTI is the three-word __si_class_type_info record
 * {0x0209a764, 0x02112408, 0x021089ec} -- first word __si_class_type_info's
 * vtable+8, second the length-prefixed string "14daObjTtWater_c" at 0x02112408,
 * third _ZTI10dBgActor_c, which is homed in ov002 rather than arm9. So the ROM
 * states both the name and the `: dBgActor_c` base. The file was TinyWater.h
 * until that read renamed it.
 *
 * The single profile TT_WATER already carried its ROM name; its descriptor at
 * 0x0211241c names the factory at 0x02111690 and is the third object in the
 * .data run this class owns.
 *
 * LAYOUT unchanged from the TinyWater spelling: dBgActor_c ends at 0x31e and
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

struct daObjTtWater_c : dBgActor_c {
    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;/* 0x320 */
    s32 mMinPosY;                      /* 0x334 */
    /* Named out of the padding by this class's own Behavior, which stores
       Sound::PlayLong's handle here and passes it back as that call's first
       argument on the next frame -- the same field the sibling ShipWater
       already calls mSoundID. Four bytes of pad became four bytes of s32, so
       nothing below it moves. */
    s32 mSoundID;                      /* 0x338 */
    u16 mSoundTimer;                   /* 0x33c */

    /* --- vtable. The destructor is INLINE, and that is load-bearing rather
       than a style choice. Out of line, mwccarm emits D0 before D1 -- the
       reverse of the ROM's 0x021113d4 D1 / 0x02111420 D0 order, which makes
       objisolate refuse the whole TU -- and additionally emits a D2 that has
       no home anywhere in the cartridge. Inline, it emits exactly the retail
       D1/D0 pair in ROM order and no D2. Nothing derives from this class.

       The body stays empty: every store and call the ROM's D1 contains is
       compiler-generated -- this class's vptr, dBgActor_c's inlined, then the
       TextureTransformer, dBgW_KcMbg and Model members and the dActor_c base.

       Declared FIRST, deliberately: with the destructor inline the key
       function is the first DECLARED non-inline virtual, so this ordering is
       what makes src/actors/d_a_obj_tt_water.cpp the TU that emits the
       _ZTV/_ZTI/_ZTS group -- exactly what the promotion needs it to be. --- */
    virtual ~daObjTtWater_c() {}       /* slots 16 (D1), 17 (D0) */

    /* --- overrides of inherited fBase_c slots. The ROM installs all four in
       the vtable at 0x02112440; they were declared as ordinary methods here
       before, which contradicted the cartridge. Slot 31, dBgActor_c::Kill, is
       NOT overridden -- the table's last word is dBgActor_c's own. --- */
    virtual int InitResources();       /* slot 0 -- 0x021115bc */
    virtual int CleanupResources();    /* slot 3 -- 0x02111480 */
    virtual int Behavior();            /* slot 6 -- 0x021114f8 */
    virtual int Render();              /* slot 9 -- 0x021114c4 */
};

/* Holds the chain to the size the factory's operator new(0x340) evidences.
   A silently-added member anywhere fails this. */
typedef char daObjTtWater_c_size_must_be_0x340[sizeof(daObjTtWater_c) == 0x340 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJTTWATER_C_H */
