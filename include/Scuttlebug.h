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
 * THE FACTORY daSpd_c_classInit (historical alias Scuttlebug_Spawn) constructs the
 * object for the SPIDER registry profile at 0x02122c08.
 */

/* The ROM's RTTI name for this class is daSpd_c (_ZTI7daSpd_c), while the
 * established function-symbol view spells its methods as Scuttlebug.  Scuttlebug
 * is the compiler-facing compatibility name; config/arm9/overlays/ov071/symbols.txt
 * deliberately aliases its vtable view (_ZTV10Scuttlebug) to the cartridge's
 * _ZTV7daSpd_c address.  The alias is vtable-only: _ZTS/_ZTI are named under the
 * cartridge spelling alone, so a TU that owns the key function emits
 * _ZTS10Scuttlebug and _ZTI10Scuttlebug with no home in the ROM.  That is
 * survivable on the text-only route and fatal to an intact-object one. */
struct Scuttlebug : dActor_c {
    /* Actor this bug copies its pose from, then clears when it lets go. */
    dActor_c *mParent;                       /* 0x0d0 */
    ModelAnim mModelAnim;                    /* 0x0d4 */
    ShadowModel mShadowModel;                /* 0x138 */
    dCcAc_c mdCcAc_c;  /* 0x160 */
    dBgCh_Actr mWithMeshClsn;              /* 0x194 */
    /* Drop-shadow matrix, flat words. A Matrix4x3 member would run ~Vector3
       from ~Scuttlebug, and the structured spelling breaks the store. */
    s32 mShadowMtx[12];                      /* 0x350 */
    /* Current row of data_ov071_02122fa8. Two pointer-to-member calls. */
    void *mStateRow;                         /* 0x380 */
    s32 mHomeX;                              /* 0x384 */
    s32 mHomeY;                              /* 0x388 */
    s32 mHomeZ;                              /* 0x38c */
    /* Where the bug walks back to. Refreshed from mPos when it lands. */
    s32 mAnchorX;                            /* 0x390 */
    s32 mAnchorY;                            /* 0x394 */
    s32 mAnchorZ;                            /* 0x398 */
    s32 mState;                              /* 0x39c */
    /* Leap range: set from the distance to the player, then counted down. */
    s32 mLeapDist;                           /* 0x3a0 */
    s16 mLeapAngle;                          /* 0x3a4 */
    s16 mHomeAngleY;                         /* 0x3a6 */
    /* Counted down once per frame. Every read in this file is an unsigned halfword. */
    u16 mTimer;                              /* 0x3a8 */
    /* Coins paid by OnTurnIntoEgg and by the death poof. Init writes 3. */
    u8  mCoinCount;                          /* 0x3aa */
    u8  pad_3ab[0x1];

    /* Declared, not defined: ~Scuttlebug is the key function. ROM has D1
       0x0211f000, D0 0x0211f048, no D2. src/_ZN10ScuttlebugD1Ev.cpp defines
       it; objisolate keeps its .text and drops the homeless _ZTI/_ZTS10Scuttlebug.
       An inline body would give those vague linkage in every TU. This TU's
       licensed run is 0x0211f0a4..0x02120668. */
    virtual ~Scuttlebug();            /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual void  OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */

    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Scuttlebug_size_must_be_0x3ac[sizeof(Scuttlebug) == 0x3ac ? 1 : -1];
#endif

/* ~Scuttlebug, the key function, owns the compiler-emitted definition of this
 * vtable, so to every other translation unit it is an ordinary external.
 * daSpd_c_classInit must store its public address point directly because the
 * measured factory calls fBase_c::operator new rather than a natural new; this
 * declaration only exposes that address to the factory seam. */
extern int _ZTV10Scuttlebug[];

#endif /* SCUTTLEBUG_H */
