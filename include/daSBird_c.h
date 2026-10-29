#ifndef DASBIRD_C_H
#define DASBIRD_C_H

#include "dExtAnmModel_c.h"
#include "dExtShadowModel_c.h"
#include "dActor_c.h"

/* Castle grounds birds (SBIRD 343) -- ov009/daSBird_c.
 *
 * RTTI ov009:0x02113928 / 0x0211391c names daSBird_c. The debug table names
 * SBIRD. Historical aliases: Bird, Bird_Spawn. SIZE 0x184 is the literal
 * daSBird_c_classInit passes to operator new.
 *
 * Factory constructs dExtAnmModel_c at 0xd4 and dExtShadowModel_c at 0x138; D1 tears
 * those down before ~dActor_c. dActor_c ends at 0xd0; pad_0d0 is the 4-byte
 * gap before mAnmModel.
 *
 * The 31-slot ROM vtable differs from dActor_c in slots 0, 3, 6, 9, 12, 16
 * and 17. Slot 12 is an empty OnPendingDestroy. All other actor virtuals
 * are inherited and must not be redeclared.
 *
 * Behavior indexes data_ov009_02113c48, a 4-entry member-pointer table
 * filled by ov009 sinit from the PMF records at 0x02113914 / 0x0211390c /
 * 0x021138fc / 0x02113904: StateSpawnFlock, StateWaitForPlayer,
 * StateLaunchFlock, StateFly. The member names are descriptive (ROM has no
 * symbol names); the table order and the mState stores in the TU are
 * recovered.
 */
struct daSBird_c : dActor_c {
    u8 pad_0d0[0x4];
    dExtAnmModel_c mAnmModel;       /* 0x0d4 */
    dExtShadowModel_c mShadowModel; /* 0x138 */
    Vector3 mTargetPos;             /* 0x160 */
    s16 mTargetPitch;               /* 0x16c */
    s16 mTargetYaw;                 /* 0x16e */
    s16 mBank;                      /* 0x170 */
    u8 pad_172[0x2];
    s32 mFlySpeed; /* 0x174 */
    u32 mOwnerID;  /* 0x178 */
    s32 mState;    /* 0x17c */
    u8 mIsLeader;  /* 0x180 */
    u8 pad_181[0x3];

    /* Out of line: key function is ~daSBird_c(), the first declared
       non-inline virtual, so d_a_s_bird.cpp emits D1/D0 and the class
       vtable and RTTI. Empty body: mShadowModel then mAnmModel teardown,
       the vptr store and dActor_c's teardown are synthesised. */
    virtual ~daSBird_c(); /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();     /* slot  0 */
    virtual s32 CleanupResources();  /* slot  3 */
    virtual s32 Behavior();          /* slot  6 */
    virtual s32 Render();            /* slot  9 */
    virtual void OnPendingDestroy(); /* slot 12 */

    /* PMF-table states, in data_ov009_02113c48 order, then the follower
       attach the spawn loops call on each child. */
    void StateSpawnFlock();    /* 0x021116ec, state 0 -- leader spawn/setup */
    void StateWaitForPlayer(); /* 0x021115d8, state 1 -- proximity check */
    void StateLaunchFlock();   /* 0x0211145c, state 2 -- spawn and take off */
    void StateFly();           /* 0x02111234, state 3 -- fly/steer */
    void AttachToLeader(int);  /* 0x02111224, follower attach */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daSBird_c_size_must_be_0x184[sizeof(daSBird_c) == 0x184 ? 1 : -1];
#endif

#endif /* DASBIRD_C_H */
