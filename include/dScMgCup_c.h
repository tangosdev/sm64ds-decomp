/* class dScMgCup_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgCup_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0213c154; the RTTI
 * pointer one word below it is _ZTI10dScMgCup_c, which is what ties that
 * table to this class rather than to a neighbour.
 *
 * THIS CLASS HAD NO HEADER AT ALL before this file -- unlike its twelve
 * siblings it never got an auto-generated skeleton, so there are no
 * inherited field names to carry over and no `.c` includers to convert.
 * Every offset below comes from the two structors, nothing from a prior
 * header.
 *
 * SIZE 0x5470, from the factory's own `_ZN9ActorBasenwEj(0x5470)`
 * (src/func_ov006_020e0574.cpp).
 *
 * THE FACTORY AND THE DESTRUCTOR AGREE MEMBER FOR MEMBER, which is the
 * bar this tree sets (see include/dScMgCard_c.h's own note). The factory
 * constructs, in order: the 0x4f38 table (func_ov006_020c33dc), then
 * mArray1 at 0x50e8 (0x20 elements of 0x18, element ctor
 * func_ov006_020e0634), then mArray2 at 0x53e8 (3 elements of 8, element
 * ctor func_0203d738). The destructor destroys exactly those three in
 * reverse order, with the matching element destructors
 * (NullDestructor_0203d47c, func_ov006_020deac4, func_ov006_020c3288).
 * Nothing is left over either way.
 *
 * TABLE at 0x4f38 is opaque: nothing reads inside it, so its size is
 * bounded only by mArray1 at 0x50e8, giving 0x1b0. Left unsplit rather
 * than guessed at, same as dScMgCard_c.h leaves its own.
 *
 * OWN TAIL, 0x5400..0x5470: 0x70 bytes past the last constructed member,
 * with NO matched access anywhere -- no source in the tree reads it. That
 * is trailing space the factory allocates and nothing this tree has
 * recovered touches, exactly the shape [[size-asserts-from-field-span]]
 * warns about deriving a size from: the `operator new` literal is the
 * authority here, not the field span, which would have stopped at 0x5400.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN10dScMgCup_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGCUP_C_H
#define DSCMGCUP_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c3288(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020deac4(void);
extern "C" void NullDestructor_0203d47c(void);

struct dScMgCup_c : dScMgSingle3DBase_c {
    virtual ~dScMgCup_c();

    u8 pad_4f38[0x1b0];   /* 0x4f38 -- opaque table, see file banner */
    u8 mArray1[0x300];    /* 0x50e8 -- 0x20 * 0x18, elem dtor func_ov006_020deac4 */
    u8 mArray2[0x18];     /* 0x53e8 -- 3 * 8, elem dtor NullDestructor_0203d47c */
    u8 pad_5400[0x70];    /* 0x5400 -- no matched access, see file banner */
};

typedef char dScMgCup_c_size_must_be_0x5470[sizeof(dScMgCup_c) == 0x5470 ? 1 : -1];

#endif
