#ifndef LAVABUBBLE_H
#define LAVABUBBLE_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN10LavaBubbleD1Ev` destroys each member, and
 * `daBbl_c_classInit` constructs the same types at the same offsets before
 * storing `_ZTV10LavaBubble`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dCcAc_c         0x34    -> 0x144
 *     0x144 dBgCh_Actr               0x1bc   -> 0x300
 *
 * Typing them absorbed markers that were their insides:
 *   - unk_128 = dCcAc_c.flags
 *   - unk_130 = dCcAc_c.hitFlags
 *   - unk_134 = dCcAc_c.otherOwner
 *
 * SIZE IS THE ROM'S OWN: `daBbl_c_classInit` calls
 * `fBase_c::operator new(796)` -- 0x31c -- and stores this class's
 * vtable, so that literal IS this class's sizeof.
 *
 * SM64DS RTTI names the implementation daBbl_c. The reconstructed
 * factory daBbl_c_classInit (historical alias
 * LavaBubble_Spawn) constructs it for the BUBBLE
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct LavaBubble : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x144 */
    s32                          unk_300;               /* 0x300 */
    s32                          unk_304;               /* 0x304 */
    s32                          unk_308;               /* 0x308 */
    s32                          unk_30c;               /* 0x30c */
    u8                           unk_310;               /* 0x310 */
    u8  pad_311[0xb];

    /* --- vtable --- */
    virtual ~LavaBubble();

    virtual s32   OnYoshiTryEat();         /* slot 18 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char LavaBubble_size_must_be_0x31c[sizeof(LavaBubble) == 0x31c ? 1 : -1];

#endif /* LAVABUBBLE_H */
