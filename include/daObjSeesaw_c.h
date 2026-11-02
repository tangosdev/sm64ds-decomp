#ifndef DAOBJSEESAW_C_H
#define DAOBJSEESAW_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the dExtSimpleModel_c at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS proves this class as daObjSeesaw_c through RTTI, allocation size
 * and vtable identity. The factory and profile spellings below are
 * reconstructed source-style names -- evidence-bounded proposals, not
 * recovered SM64DS symbols.
 *
 * daObjSeesaw_c_classInit_BOMB_SEESAW at 0x02135f94 (historical alias
 * SeesawBob_Spawn) allocates 0x328 and installs this class's cartridge
 * vtable. It backs the BOMB_SEESAW registry profile, whose descriptor at
 * 0x02137484 is reconstructed as g_profile_BOMB_SEESAW.
 */

#include "dBgActor_c.h"

struct daObjSeesaw_c : dBgActor_c {
    u8 mVariant; /* 0x31e -- 0..6 from actorID; indexes all three ov095 file tables */
    u8 pad_31f[0x1];
    s32 mTiltSound; /* 0x320 -- recycled Sound::PlayLong handle, replayed while |mAngleXSpeed| > 0xa */
    s16 mAngleXSpeed; /* 0x324 -- the tilt speed SpringAngle and OnStoodOn integrate into mAngleX */
    u8 mPoundedThisFrame; /* 0x326 -- set by OnGroundPounded, cleared by the last statement of Behavior */

    /* --- vtable --- */
    virtual int InitResources();                   /* slot  0 */
    virtual int CleanupResources();                /* slot  3 */
    virtual int Behavior();                        /* slot  6 */
    virtual int Render();                          /* slot  9 */
    virtual ~daObjSeesaw_c();                      /* slots 16, 17 */
    virtual void OnGroundPounded(dActor_c &other); /* slot 21 */

    int GetActorWeight(dActor_c *);                                   /* ov095 0x0213579c */
    int SpringAngle(short *, short *, int, short, int, short);        /* ov095 0x021358cc */
    void UpdateModelMatrix();                                         /* ov095 0x0213597c */
    void OnStoodOn(dActor_c *);                                       /* ov095 0x02135cdc */
    static void StoodOnCallback(void *, daObjSeesaw_c *, dActor_c *); /* ov095 0x02135e90 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjSeesaw_c_size_must_be_0x328[sizeof(daObjSeesaw_c) == 0x328 ? 1 : -1];
#endif

#endif /* DAOBJSEESAW_C_H */
