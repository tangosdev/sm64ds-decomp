#ifndef DAOBJHATENASWITCH_C_H
#define DAOBJHATENASWITCH_C_H

#include "ModelAnim.h"
#include "dBgActor_c.h"
#include "dBgW_KcMbg.h"

struct Player;

/* daObjHatenaSwitch_c is the ROM-proven class identity: the ov002 RTTI at
 * 0x02108e14 names this type and dBgActor_c as its sole base. The adjacent
 * factory allocates 0x724 bytes and constructs the two dBgW_KcMbg members and
 * ModelAnim below in declaration order; retail D1 destroys them in reverse. */
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
    u8 pad_71e[0x2];
    Player *mTalkingPlayer;             /* 0x720 */

    /* Inline plus the out-of-line InitResources key function makes mwccarm
     * emit retail D1 then D0 without a retained D2 or forcing helper. */
    virtual ~daObjHatenaSwitch_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnGroundPounded(dActor_c &other);

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

/* InitResources owns the compiler-emitted definition. Natural new selects the
 * wrong allocator, so the measured factory must address the public vtable
 * point directly; this declaration only exposes that compiler-owned object. */
extern int _ZTV19daObjHatenaSwitch_c[];

#endif /* DAOBJHATENASWITCH_C_H */
