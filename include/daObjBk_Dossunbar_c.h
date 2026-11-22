#ifndef DAOBJBK_DOSSUNBAR_C_H
#define DAOBJBK_DOSSUNBAR_C_H

#include "types.h"
#include "dBgActor_c.h"

struct daPly_c;

/**
 * Whomp's Fortress sliding bars (`dossunbar`). One class, two
 * registry profiles: BK_DOSSUNBAR_L (MOVING_BAR_BIG, actor 0x35)
 * and BK_DOSSUNBAR_S (MOVING_BAR_SMALL, actor 0x36). Factory size
 * 0x338. Direct base is dBgActor_c.
 *
 * `daObjBk_Dossunbar_c` is the RTTI name.
 */
struct daObjBk_Dossunbar_c : dBgActor_c {
    u8 pad_31e[0x2];
    Vector3 mHomePos; /* 0x320 -- Init copies mPos here */
    s32 mVariant;    /* 0x32c -- 1 if actor 0x35 (big), else 0 */
    u32 mState;      /* 0x330 -- 0..6, indexes the .bss dispatch table */
    s32 mStateTimer; /* 0x334 */

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnHitByMegaChar(daPly_c &player);
    virtual void Kill();

    /* The seven-state cycle; Behavior runs the execute table. */
    void ChangeState(int idx);
    void EnterWaitOut();
    void ExecuteWaitOut();
    void EnterEaseToMid();
    void ExecuteEaseToMid();
    void EnterWaitMid();
    void ExecuteWaitMid();
    void EnterEaseHome();
    void ExecuteEaseHome();
    void EnterCoastHome();
    void ExecuteCoastHome();
    void EnterWaitHome();
    void ExecuteWaitHome();
    void EnterTravelOut();
    void ExecuteTravelOut();

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjBk_Dossunbar_c() {}
};

typedef char daObjBk_Dossunbar_c_size_must_be_0x338[sizeof(daObjBk_Dossunbar_c) == 0x338 ? 1 : -1];

#endif /* DAOBJBK_DOSSUNBAR_C_H */
