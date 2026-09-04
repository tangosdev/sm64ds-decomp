/* class dScMgFlower_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgFlower_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x02140140; the RTTI
 * pointer one word below it is _ZTI13dScMgFlower_c, which is what ties
 * that table to this class rather than to a neighbour.
 *
 * SIZE 0x5ff8, from the factory's own `_ZN7fBase_cnwEj(0x5ff8)`
 * (src/d_s_mg_flower.cpp).
 *
 * FACTORY AND DESTRUCTOR AGREE MEMBER FOR MEMBER. The factory builds
 * mArray at 0x4f38 (0x16 elements of 0x20, element ctor
 * func_ov006_0212b88c) and then an object at 0x51f8
 * (func_ov006_020c3f54); the destructor destroys the 0x51f8 object first
 * (func_ov006_020c3e70), then mArray (element dtor func_ov006_0212a650).
 * Nothing is left over either way.
 *
 * THE 0x51f8 OBJECT is opaque: nothing reads inside it, so its size is
 * bounded only by the first own-tail field at 0x5fb8, giving 0xdc0. Left
 * unsplit rather than guessed at, same as dScMgCard_c.h leaves its own
 * shared table.
 *
 * OWN TAIL, 0x5fb8..0x5ff6: seventeen fields carried over verbatim from
 * this header's previous auto-generated form, all real matched access out
 * of dScMgFlower_c's own methods, at unchanged offsets and widths. The
 * trailing two bytes are alignment.
 *
 * THREE FIELDS THE OLD HEADER DECLARED BELOW 0x4f38 ARE NOT DROPPED BUT
 * MOVED: unk_0c0, unk_0c3 and unk_0c4 are dScMgBase_c's own, two levels
 * up. unk_0c3 was already declared there; unk_0c0 and unk_0c4 were sitting
 * inside that header's pad_0c0/pad_0c4 and are split out by this slice.
 * This class is NOT the first to touch them -- roughly 25 ov006 files
 * already spell the same idiom by raw offset, including two of this
 * slice's own siblings -- so the width and signedness in dScMgBase_c.h are
 * taken from that class's OWN render path instead. See its note.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN13dScMgFlower_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one.
 *
 * SM64DS RTTI names the implementation dScMgFlower_c. The reconstructed factory
 * dScMgFlower_c_classInit (historical alias func_ov006_0212b7f8) installs this class's
 * cartridge vtable for the MG_FLOWER registry profile.
 */
#ifndef DSCMGFLOWER_C_H
#define DSCMGFLOWER_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" void func_ov006_020c3e70(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_0212a650(void);

struct dScMgFlower_c : dScMgSingle3DBase_c {
    virtual ~dScMgFlower_c();

    /* THIS CLASS'S OWN OVERRIDES ARE SLOTS 0, 6 AND 9 -- AND ONLY THOSE. Four
       further addresses used to carry `recovered name: dScMgFlower_c_*` comments
       (AfterInitResources, AfterCleanupResources, BeforeBehavior, BeforeRender);
       all four are dScMgSingle3DBase_c's own, one level up, and commit 4f7406b9c
       moved them there. Flower merely inherits them. The three below are the
       slots where the ROM's own table really does differ from its parent's.

       Spelled WITHOUT the `virtual` keyword, the way include/daObjMarioCap_c.h
       and include/daObjRc_Dorifu_c.h spell theirs -- an override of a virtual an
       ancestor already declares is implicitly virtual either way, so each reuses
       an existing slot and adds no field, and the 0x5ff8 assert below still
       holds. The destructor above is declared first and out of line, so it stays
       this class's KEY FUNCTION and none of these translation units emits
       _ZTV13dScMgFlower_c. */
    s32 InitResources();   /* slot  0 -- src/_ZN13dScMgFlower_c13InitResourcesEv.cpp */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    s32 Behavior();        /* slot  6 -- src/_ZN13dScMgFlower_c8BehaviorEv.cpp */
    s32 Render();          /* slot  9 -- src/_ZN13dScMgFlower_c6RenderEv.cpp */

    u8  mArray[0x2c0];     /* 0x4f38 -- 0x16 * 0x20, elem dtor func_ov006_0212a650 */
    u8  pad_51f8[0xdc0];   /* 0x51f8 -- opaque object, see file banner */
    s32 mCursorX;          /* 0x5fb8 -- Fix12 stylus position, from the touch
                              sample data_020a0dea/deb << 12 */
    s32 mCursorY;          /* 0x5fbc */
    s32 mPrevCursorX;      /* 0x5fc0 -- last tick's mCursor; the drag delta */
    s32 mPrevCursorY;      /* 0x5fc4 */
    s32 mHeldPetal;        /* 0x5fc8 -- mArray index being dragged, -1 for none */
    u8  mPetalToggle;      /* 0x5fcc -- flips on every petal pulled; picks the
                              "loves me" or "loves me not" banner and sound */
    u8  unk_5fcd;          /* 0x5fcd */
    u8  pad_5fce[0x2];     /* 0x5fce */
    s32 mHintTimer;        /* 0x5fd0 -- held at 0x3c while a petal is dragged,
                              counts down otherwise */
    s32 mResultTimer;      /* 0x5fd4 -- 0x3c frames the banner stays up */
    s32 mPetalsLeft;       /* 0x5fd8 -- petals still on the flower; one goes on
                              each pull, 0 ends the round */
    s32 mWinStreak;        /* 0x5fdc -- consecutive "loves me" petals; at 3 the
                              payout is 3 points instead of 1 */
    s32 mLoseStreak;       /* 0x5fe0 -- the mirror count, pays nothing */
    s32 mHoldTimer;        /* 0x5fe4 -- src/func_ov006_0212aa74.c counts it up to
                              0x15 and resets; above 0x14 means "held" */
    s32 mState;            /* 0x5fe8 -- 0 playing, 1 over */
    s32 mFaceSprite;       /* 0x5fec -- index into data_ov006_0213ab94, the
                              flower's face */
    s32 mScore;            /* 0x5ff0 -- clamped to 9999 */
    u16 mBgScrollPhase;    /* 0x5ff4 -- += 0xc0 a frame; indexes the sine table
                              for both background layers */
    u8  pad_5ff6[0x2];     /* 0x5ff6 -- rounds up to the 0x5ff8 boundary */
};

typedef char dScMgFlower_c_size_must_be_0x5ff8[sizeof(dScMgFlower_c) == 0x5ff8 ? 1 : -1];

#endif
