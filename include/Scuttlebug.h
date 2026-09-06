#ifndef SCUTTLEBUG_H
#define SCUTTLEBUG_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daSpd_c_classInit  fBase_c::operator new(940 = 0x3ac), dActor_c::dActor_c(), stores _ZTV10Scuttlebug,
 *                 then the four members below in this order.
 *   ~Scuttlebug   the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x3ac is the factory's own literal, and the trailing byte fields close exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (relocs.txt, ov071). Only the
 * slots declared below differ; every other slot holds the base's own word and is inherited,
 * so it is deliberately not redeclared here.
 *
 * SM64DS RTTI names the implementation daSpd_c. The reconstructed
 * factory daSpd_c_classInit (historical alias
 * Scuttlebug_Spawn) constructs it for the SPIDER
 * registry profile.
 */
struct Scuttlebug : dActor_c {
    u8  pad_0d0[0x4];
    ModelAnim mModelAnim;                    /* 0x0d4 */
    ShadowModel mShadowModel;                /* 0x138 */
    dCcAc_c mdCcAc_c;  /* 0x160 */
    dBgCh_Actr mWithMeshClsn;              /* 0x194 */
    u8  pad_350[0x58];
    /* Counted down once per frame at the top of Behavior with
       DecIfAbove0_Short -- as a HALFWORD, though the generated header typed the
       slot u8; the result is discarded there, so what expiring means is up to
       the not-yet-enrolled helpers of this class.
       [_ZN10Scuttlebug8BehaviorEv.cpp] */
    u8  mTimer;                             /* 0x3a8 */
    u8  pad_3a9[0x3];

    /* DEFINED INLINE, and declared ahead of every other virtual. The cartridge
       orders the pair D1 (0x0211f000) then D0 (0x0211f048) with no D2 anywhere
       in ov071, which is exactly what mwccarm 2004/b56 emits for an inline
       destructor; an out-of-line one emits D2, D0, D1 plus a homeless D2.
       Declaring it inline also moves the KEY FUNCTION down to OnYoshiTryEat,
       the first non-inline virtual this class declares, which sits inside the
       licensed run at 0x0211f0a4 and so anchors _ZTV10Scuttlebug here. */
    virtual ~Scuttlebug() {}          /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
};

typedef char Scuttlebug_size_must_be_0x3ac[sizeof(Scuttlebug) == 0x3ac ? 1 : -1];

/* OnYoshiTryEat, the key function, owns the compiler-emitted definition of
 * this vtable. daSpd_c_classInit must store its public address point directly
 * because the measured factory calls fBase_c::operator new rather than a
 * natural new; this declaration only exposes that compiler-owned address to
 * the factory seam. */
extern int _ZTV10Scuttlebug[];

#endif /* SCUTTLEBUG_H */
