#ifndef DAOBJCTMECHA05_C_H
#define DAOBJCTMECHA05_C_H

#include "types.h"

/* Derives from dBgActor_c directly: RTTI _ZTI16daObjCtMecha05_c /
 * _ZTS16daObjCtMecha05_c (config/arm9/overlays/ov065/symbols.txt) give the
 * class name; the un-migrated D1 (func_ov065_0211ab60) stores this class's
 * own vtable, then dBgActor_c's -- inlined, because dBgActor_c's destructor
 * is defined in its class body -- then destroys the dBgW_KcMbg at
 * 0x124 and the Model at 0xd4 before chaining to dActor_c. All three belong
 * to dBgActor_c and are inherited now.
 *
 * THE AUTO-GENERATED FLAT PLACEHOLDER'S FIELDS BELOW 0x320 WERE ALL
 * dActor_c's, not this class's own -- same defect the daObjCtMecha03_c pass
 * found at 0x08e/0x090. Every one of unk_05c/060/064/08e/094/098 is read or
 * written by this class's own functions, which is why the generator saw
 * them, but each already has a name in dActor_c.h: mPosX/mPosY/mPosZ
 * (0x05c/0x060/0x064), mAngleY (0x08e), mPrevAngleY (0x094 -- the flat
 * header's u16 was really dActor_c's s16), mHorzSpeed (0x098, reused here as
 * a plain accumulator rather than a physical speed). InitResources confirms
 * the 0x08e/0x094 pair directly: it copies mAngleY into mPrevAngleY
 * (`*(short*)(self+0x94) = *(short*)(self+0x8e)`), the ordinary
 * current-into-previous idiom the two field names already describe. dActor_c
 * ends its own data at 0x31e (sizeof rounds to 0x320, via dBgActor_c) -- see
 * the header comment there. This class's own storage starts at 0x320 and
 * nothing below it is restated.
 *
 * SIZE IS THE ROM'S OWN LITERAL: the factory (TTC_MovingBar_Spawn, despite
 * the name -- it stores THIS class's vtable, data_ov065_0211d2b4, not
 * _ZTV13TTC_MovingBar) calls fBase_c::operator new(0x394), read off the
 * retail instruction. The field span only reaches 0x364; the rest is
 * unclaimed trailing pad, same shape as TTC_MovingBar's own pad_34c[0x30]
 * tail.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ShadowModel.h"

struct daObjCtMecha05_c : dBgActor_c {
    s32 mHomePosX;                      /* 0x320 -- snapshot of mPosX at InitResources */
    s32 mHomePosY;                      /* 0x324 -- snapshot of mPosY at InitResources */
    s32 mHomePosZ;                      /* 0x328 -- snapshot of mPosZ at InitResources */
    s32 mTravel;                      /* 0x32c -- distance travelled along the path; += mHorzSpeed each tick, compared against 0xfa000 */
    s32 mPrevTravel;                      /* 0x330 -- previous tick's mTravel; their product's sign is the turn test */
    s16 mStateTimer;                      /* 0x334 -- DecIfAbove0_Short countdown, seeded from data_ov065_0211c0c8[setting] */
    u8  mState;                      /* 0x336 -- Behavior's switch key, 0..3, incremented in place */
    u8  pad_337[0x1];
    s32 mGroundY;                      /* 0x338 -- ground height from InitResources' dBgCh_Gnd raycast */
    ShadowModel mShadowModel;         /* 0x33c */

    /* --- vtable --- */
    virtual ~daObjCtMecha05_c() {}

    int CleanupResources();
    int InitResources();
    int Behavior();
    int Render();

    /* Tail padding. The field span stops short of the real size: the
       factory calls fBase_c::operator new(0x394), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_364[0x30];      /* 0x364, to the ROM's 0x394 */
};

typedef char daObjCtMecha05_c_size_must_be_0x394[sizeof(daObjCtMecha05_c) == 0x394 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCTMECHA05_C_H */
