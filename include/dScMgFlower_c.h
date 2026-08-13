/* class dScMgFlower_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgFlower_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x02140140; the RTTI
 * pointer one word below it is _ZTI13dScMgFlower_c, which is what ties
 * that table to this class rather than to a neighbour.
 *
 * SIZE 0x5ff8, from the factory's own `_ZN9ActorBasenwEj(0x5ff8)`
 * (src/func_ov006_0212b7f8.cpp).
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
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGFLOWER_C_H
#define DSCMGFLOWER_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" void func_ov006_020c3e70(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_0212a650(void);

struct dScMgFlower_c : dScMgSingle3DBase_c {
    virtual ~dScMgFlower_c();

    u8  mArray[0x2c0];     /* 0x4f38 -- 0x16 * 0x20, elem dtor func_ov006_0212a650 */
    u8  pad_51f8[0xdc0];   /* 0x51f8 -- opaque object, see file banner */
    s32 unk_5fb8;          /* 0x5fb8 */
    s32 unk_5fbc;          /* 0x5fbc */
    s32 unk_5fc0;          /* 0x5fc0 */
    s32 unk_5fc4;          /* 0x5fc4 */
    s32 unk_5fc8;          /* 0x5fc8 */
    u8  unk_5fcc;          /* 0x5fcc */
    u8  unk_5fcd;          /* 0x5fcd */
    u8  pad_5fce[0x2];     /* 0x5fce */
    s32 unk_5fd0;          /* 0x5fd0 */
    s32 unk_5fd4;          /* 0x5fd4 */
    s32 unk_5fd8;          /* 0x5fd8 */
    s32 unk_5fdc;          /* 0x5fdc */
    s32 unk_5fe0;          /* 0x5fe0 */
    s32 unk_5fe4;          /* 0x5fe4 */
    s32 unk_5fe8;          /* 0x5fe8 */
    s32 unk_5fec;          /* 0x5fec */
    s32 unk_5ff0;          /* 0x5ff0 */
    u16 unk_5ff4;          /* 0x5ff4 */
    u8  pad_5ff6[0x2];     /* 0x5ff6 -- rounds up to the 0x5ff8 boundary */
};

typedef char dScMgFlower_c_size_must_be_0x5ff8[sizeof(dScMgFlower_c) == 0x5ff8 ? 1 : -1];

#endif
