#ifndef DAOBJCLOCK_C_H
#define DAOBJCLOCK_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"

/* daObjClock_c -- the two rotating hands of the clock painting.
 *
 * THE NAME COMES OUT OF THE CARTRIDGE. The vtable header at ov013 0x021121f8
 * is {0, 0x021121a4}; that _ZTI is the three-word __si_class_type_info record
 * {0x0209a764, 0x021121b0, 0x0208e390} -- first word __si_class_type_info's
 * vtable+8, second the length-prefixed string "12daObjClock_c" at 0x021121b0,
 * third _ZTI8dActor_c in config/arm9/symbols.txt. So the ROM states the name
 * and the `: dActor_c` base. The file was ClockPaintingHandShort.h until that
 * read renamed it; _ZTV22ClockPaintingHandShort was a coined alias sitting on
 * the same 0x02112200 address point and is gone from symbols.txt.
 *
 * ONE CLASS, TWO PROFILES. CLOCK_SHORT (actor 292) and CLOCK_LONG (actor 293)
 * each own a descriptor and a byte-identical factory, and both install this
 * one vtable and the same 0x128 allocation. InitResources tells them apart at
 * run time by actorID and stores the answer in mHandIndex.
 *
 * LAYOUT: dActor_c occupies 0x000..0x0cf; four bytes of derived padding precede
 * the owned Model the factories construct at +0xd4 (_ZN5ModelC1Ev, a relocation
 * the ROM build checks); the one-byte hand index follows it at 0x124. The
 * helper at 0x02111430 writes the model matrix at +0xf0 == mModel.mat4x3 and
 * its translation row at +0x114, which is the same span by another spelling.
 */

#ifdef __cplusplus

struct daObjClock_c : dActor_c {
    u8    pad_0d0[0x4];    /* 0x0d0 */
    Model mModel;          /* 0x0d4 */
    u8    mHandIndex;      /* 0x124 -- 0 = long hand, 1 = short hand */
    u8    pad_125[0x3];    /* 0x125 */

    /* --- vtable. The destructor is INLINE, and that is load-bearing rather
       than a style choice. Out of line, mwccarm emits D0 before D1 -- the
       reverse of the ROM's 0x021113bc D1 / 0x021113ec D0 order, which makes
       objisolate refuse the whole TU -- and additionally emits a D2 that has
       no home anywhere in the cartridge. Inline, it emits exactly the retail
       D1/D0 pair in ROM order and no D2. Nothing derives from this class, so
       no descendant needs to `bl` a D2.

       Declared FIRST, deliberately: with the destructor inline the key
       function is the first DECLARED non-inline virtual, so this ordering is
       what makes src/actors/d_a_obj_clock.cpp the TU that emits the
       _ZTV/_ZTI/_ZTS group -- exactly what the promotion needs it to be. --- */
    virtual ~daObjClock_c() {}         /* slots 16 (D1), 17 (D0) */

    /* --- overrides of inherited fBase_c slots dActor_c left untouched. The
       ROM installs all four in the vtable at 0x02112200; they were declared
       as ordinary methods here before, which contradicted the cartridge. --- */
    virtual int InitResources();       /* slot 0 -- 0x021115cc */
    virtual int CleanupResources();    /* slot 3 -- 0x02111478 */
    virtual int Behavior();            /* slot 6 -- 0x021114cc */
    virtual int Render();              /* slot 9 -- 0x021114a4 */
};

/* Holds the chain to the size both factories' operator new(0x128) evidences.
   A silently-added member anywhere fails this. */
typedef char daObjClock_c_size_must_be_0x128[sizeof(daObjClock_c) == 0x128 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCLOCK_C_H */
