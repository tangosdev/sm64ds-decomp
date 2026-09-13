#ifndef DAOBJHATENASWITCH_C_H
#define DAOBJHATENASWITCH_C_H

#include "ModelAnim.h"
#include "dBgActor_c.h"
#include "dBgW_KcMbg.h"

struct Player;

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* daObjHatenaSwitch_c is the ROM-proven class identity: the ov002 RTTI at
 * 0x02108e14 names this type and dBgActor_c as its sole base. The factory
 * allocates 0x724 bytes. ov002 also has EXCLAMATION_SWITCH(11),
 * STAR_SWITCH(12), and BLUE_COIN_SWITCH(10); this class is QUESTION_SWITCH
 * (HATENA_SWITCH 26). */
struct daObjHatenaSwitch_c : dBgActor_c {
    dBgW_KcMbg *mActiveMeshCollider;    /* 0x320 */
    dBgW_KcMbg mStaticMeshCollider;     /* 0x324 */
    dBgW_KcMbg mMovingMeshCollider;     /* 0x4ec */
    ModelAnim mModelAnim;               /* 0x6b4 */
    u8 mPressTimer;                     /* 0x718 */
    u8 pad_719[0x1];
    u8 mPressedThisFrame;               /* 0x71a */
    u8 mTalking;                        /* 0x71b */
    u16 mSoundDelay;                    /* 0x71c */
    Player *mTalkingPlayer;             /* 0x720 */

    /* Inline plus the out-of-line InitResources key function makes mwccarm
     * emit retail D1 then D0 without a retained D2 or forcing helper. */
    virtual ~daObjHatenaSwitch_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnGroundPounded(dActor_c &other);

    /* size_t == unsigned long here; unsigned int is illegal. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

private:
    /* Class ownership and bodies are proven. These readable private spellings
     * are inferred aliases because the original English names do not survive. */
    s32 UpdateClsnState();
    void UpdateClsnTransform();
    void UpdateModelTransform();
    void HandleClsn(dActor_c &other);
    static void AfterClsnCallback(
        dBgW *collider, dActor_c *owner, dActor_c *other);
};

typedef char daObjHatenaSwitch_c_size_must_be_0x724[
    sizeof(daObjHatenaSwitch_c) == 0x724 ? 1 : -1];

#endif /* DAOBJHATENASWITCH_C_H */
