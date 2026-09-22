#ifndef DAOBJBK_ROTEBAR_C_H
#define DAOBJBK_ROTEBAR_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Whomp's Fortress rotating bar (profile BK_ROTEBAR). Factory
 * allocates 0x324 = sizeof(dBgActor_c) plus mPauseTimer / mTurnSound.
 * Pauses 0x3c frames, then steps yaw by 0x100 until
 * (mPrevAngleY & 0x7fff) == 0 -- a half turn -- and pauses again.
 *
 * `daObjBk_Rotebar_c` is the RTTI name. Direct base is dBgActor_c.
 */
struct daObjBk_Rotebar_c : dBgActor_c {
    s8  mPauseTimer;   /* 0x31e -- 0x3c; DecIfAbove0_Byte gates the turn */
    s32 mTurnSound;    /* 0x320 -- recycled Sound::PlayLong handle */

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~daObjBk_Rotebar_c() {}   /* no slot */
#else
    virtual ~daObjBk_Rotebar_c() {}   /* D1 and D0 */
#endif

    s32 InitResources();      /* slot  0 */
    s32 Behavior();           /* slot  6 */
    s32 Render();             /* slot  9 */
    s32 CleanupResources();   /* slot  3 */

    /* size_t == unsigned long here; unsigned int is illegal. */
    static void *operator new(size_t size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBk_Rotebar_c_size_must_be_0x324[
    sizeof(daObjBk_Rotebar_c) == 0x324 ? 1 : -1];
#endif

#endif /* DAOBJBK_ROTEBAR_C_H */
