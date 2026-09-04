#ifndef DAOBJBK_ROTEBAR_C_H
#define DAOBJBK_ROTEBAR_C_H

#include "types.h"

/* ov015 -- the Bob-omb Battlefield rotating bridge (profile BK_ROTEBAR): a
 * mesh-collider platform that pauses, then turns a quarter turn, then pauses
 * again.
 *
 * MEASURED from extracted/dsd/arm9_overlays/ov015.bin, base 0x021111a0:
 *   _ZTS17daObjBk_Rotebar_c  0x021146dc  bytes spell "17daObjBk_Rotebar_c"
 *   _ZTI17daObjBk_Rotebar_c  0x021146d0  +4 -> _ZTS, +8 -> 0x021089ec = _ZTI10dBgActor_c
 *   _ZTV17daObjBk_Rotebar_c  0x02114714  V-8 = 0, V-4 = &_ZTI  (the address point)
 *
 * THE DIRECT BASE IS dBgActor_c -- the RTTI base pointer says so, and the
 * 0x324 allocation in daObjBk_Rotebar_c_classInit agrees with dBgActor_c's
 * 0x320 plus the two fields below.
 *
 * Vtable diff against dBgActor_c: only slots 0, 3, 6, 9, 16 and 17 hold words
 * inside ov015. Every other slot of the cartridge's 31 still points into arm9,
 * so nothing else is overridden.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjBk_Rotebar_c : dBgActor_c {
    s8  mPauseTimer;   /* 0x31e -- set to 0x3c by InitResources and again on each
                          0x8000 boundary; DecIfAbove0_Byte gates the turn */
    u8  pad_31f[0x1];
    s32 mTurnSound;    /* 0x320 -- recycled Sound::PlayLong handle for the turning noise */

    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE, and declared first, so this TU is the
       vtable's home. Both ROM bodies are empty; an inline body also drops the
       D2 variant the cartridge never carried. */
    virtual ~daObjBk_Rotebar_c() {}  /* slots 16 (D1), 17 (D0) */

    /* declared in reverse of ROM address order, as the TU emits them */
    int InitResources();      /* slot  0 -- 0x02112b04 */
    int Behavior();           /* slot  6 -- 0x02112a4c */
    int Render();             /* slot  9 -- 0x02112a24 */
    int CleanupResources();   /* slot  3 -- 0x021129e0 */
};

typedef char daObjBk_Rotebar_c_size_must_be_0x324[sizeof(daObjBk_Rotebar_c) == 0x324 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJBK_ROTEBAR_C_H */
