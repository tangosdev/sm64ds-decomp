#ifndef DAOBJBK_DOSSUNBAR_C_H
#define DAOBJBK_DOSSUNBAR_C_H

#include "types.h"

/* ov015 -- the Bob-omb Battlefield sliding bar (registry profiles
 * BK_DOSSUNBAR_L and BK_DOSSUNBAR_S): a mesh-collider platform that runs a
 * seven-state cycle, sliding out from its spawn position and back again.
 *
 * MEASURED from extracted/overlays/overlay_0015.bin, base 0x021111a0:
 *   _ZTS19daObjBk_Dossunbar_c  0x0211451c  bytes spell "19daObjBk_Dossunbar_c"
 *   _ZTI19daObjBk_Dossunbar_c  0x02114510  +4 -> _ZTS, +8 -> 0x021089ec = _ZTI10dBgActor_c
 *   _ZTV19daObjBk_Dossunbar_c  0x0211458c  V-8 = 0, V-4 = &_ZTI  (the address point)
 *
 * THE DIRECT BASE IS dBgActor_c -- the __si_class_type_info base pointer says
 * so outright, and the destructor bears it out: it stores this class's vtable,
 * then dBgActor_c's (inlined), then destroys the dBgW_KcMbg at 0x124 and the
 * Model at 0xd4 before chaining to dActor_c. All three of those belong to
 * dBgActor_c and are inherited, not restated here. The 32-word table is only
 * corroboration; 31 would have meant dActor_c. Eight of the 32 words differ
 * from _ZTV10dBgActor_c's and all eight point inside this overlay's .text:
 * slots 0, 3, 6, 9, 16, 17, 27 and 31.
 *
 * ONE CLASS, TWO SPAWN PROFILES. Both reconstructed factories --
 * daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L (historical alias
 * MovingBarBig_Spawn) and daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S
 * (historical alias MovingBarSmall_Spawn) -- call fBase_c::operator new(0x338)
 * and store the same vptr, so these are two registry entries for one class and
 * not two classes. TTC_MovingBar / daObjCtMecha05_c in ov065 is a different
 * class in a different overlay and shares only a coined word.
 *
 * THE TREE NAME WAS COINED. This class was called MovingBar until the ROM's own
 * type string was read; "MovingBar" occurs nowhere in the 9.0 MiB cartridge
 * dump, case-sensitive or case-folded, while "Dossunbar" does.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjBk_Dossunbar_c : dBgActor_c {
    u8  pad_31e[0x2];
    /* InitResources copies mPosX/mPosY/mPosZ into these three as its last act
       before installing state 5, and the state bodies steer mPosX back towards
       mHomePosX. The word "Home" is a reading of that, not a ROM fact. */
    s32 mHomePosX;                    /* 0x320 */
    s32 mHomePosY;                    /* 0x324 */
    s32 mHomePosZ;                    /* 0x328 */
    /* Set in InitResources to (mActorID == 0x35), then used as a *0xc index
       into the two-record resource table at 0x02114534. Name coined. */
    s32 mVariant;                     /* 0x32c */
    /* Written only by the state installer at 0x02111fb8 and read only by
       Behavior, where it indexes the seven-entry dispatch table at
       0x021149ec. Range 0..6. Name coined. */
    u32 mState;                       /* 0x330 */
    /* MEASURED: the six state-entry bodies seed this word with a frame count
       (20, 10, 5, 0x18, 10, 10) and the state bodies decrement it and test it
       against 0 before advancing. Offset, width and countdown role are proven
       by those bodies; the word "Timer" is coined. It is the last word of the
       0x338 both factories allocate. */
    s32 mStateTimer;                  /* 0x334 */

    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE, and declared ahead of the out-of-line
       virtuals. The cartridge carries D1 at 0x02111ba0, D0 at 0x02111be4 and no
       D2 anywhere; written out of line mwcc emits D0 before D1 and adds the D2
       the ROM never had, and the isolation step rejects that object. */
    virtual ~daObjBk_Dossunbar_c() {}  /* slots 16 (D1), 17 (D0) */

    /* declared in reverse of ROM address order, as the TU emits them */
    int InitResources();                            /* slot  0 -- 0x021120fc */
    int Behavior();                                 /* slot  6 -- 0x02112090 */
    int Render();                                   /* slot  9 -- 0x02112068 */
    int CleanupResources();                         /* slot  3 -- 0x02112004 */
    virtual void OnHitByMegaChar(Player &player);   /* slot 27 -- 0x02111cb8 */
    /* dBgActor_c's own slot, overridden here: this table's word at +0x7c
       relocates to 0x02111c3c while _ZTV10dBgActor_c's relocates to
       _ZN10dBgActor_c4KillEv. An override, so it adds no slot and no field. */
    virtual void Kill();                            /* slot 31 -- 0x02111c3c */
};

typedef char daObjBk_Dossunbar_c_size_must_be_0x338[sizeof(daObjBk_Dossunbar_c) == 0x338 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJBK_DOSSUNBAR_C_H */
