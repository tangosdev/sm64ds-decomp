/* Hand-written from matched-function evidence, not gen_header.py output:
 * class Chuckya, 5 matched functions in one clean TU (ov062
 * 0x02115ee0-0x021174cc, 36 functions, no other class in it -- tu_map.py).
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 *
 * Chuckya derives from Enemy -- Behavior calls Enemy::UpdateKillByInvincibleChar,
 * Enemy::UpdateWMClsn and Enemy::IsGoingOffCliff on itself. Written FLAT with
 * the inherited slots restated, as every other generated header here is; see
 * include/KoopaShell.h for the same note.
 *
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef CHUCKYA_H
#define CHUCKYA_H
#include "types.h"

struct Chuckya {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    /* A SECOND angle triple, copied wholesale into the one above once a frame,
       right after the state tick and before UpdatePos -- so the state writes
       here and this copy publishes it.
       NAMED unk_ ON PURPOSE. Actor.h calls 0x094 mPrevAngleY, which cannot
       also be the middle of a triple that feeds mAngleY. One of the two
       readings is wrong and this file does not have the evidence to say which,
       so it records the copy and leaves the naming alone. */
    s16 unk_092;            /* 0x092 */
    s16 unk_094;            /* 0x094 */
    s16 unk_096;            /* 0x096 */
    s32 unk_098;            /* 0x098 */
    s32 mSpeed;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    u32 mFlags;            /* 0x0b0 */
    u8  pad_0b4[0x4c];
    u16 unk_100;            /* 0x100 */
    u8  pad_102[0x6];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x1];
    u8  unk_10a;            /* 0x10a */
    u8  pad_10b[0x5];
    /* Sub-objects, kept as byte markers: each one's extent is fixed by the
       next marker's offset, which is what the ROM evidences, and none of the
       five functions needs a view into one. */
    u8  mCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModel;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  mAnimation;            /* 0x350 */
    u8  pad_351[0xb];
    s32 unk_35c;            /* 0x35c */
    u8  pad_360[0x4];
    /* Current state: a record whose third word is a pointer-to-member tick,
       null-checked before the call. Behavior also compares it by ADDRESS
       against seven file-scope state objects to gate three separate things --
       whether to run terrain at all, whether to run the cliff check, and
       whether to call func_ov062_02116010. */
    void* mState;            /* 0x364 */
    u8  mShadowModel;            /* 0x368 */
    u8  pad_369[0x57];
    /* Where it was spawned; InitResources seeds it from mPos. */
    s32 mHomePosX;            /* 0x3c0 */
    s32 mHomePosY;            /* 0x3c4 */
    s32 mHomePosZ;            /* 0x3c8 */
    u8  pad_3cc[0xc];
    /* LAST frame's position, republished every frame that runs terrain, and
       restored into mPos when the cliff check trips -- which is how Chuckya
       refuses to walk off an edge: it rewinds one frame. */
    s32 mPrevPosX;            /* 0x3d8 */
    s32 mPrevPosY;            /* 0x3dc */
    s32 mPrevPosZ;            /* 0x3e0 */
    u8  unk_3e4;            /* 0x3e4 */
    u8  pad_3e5[0x1];
    u16 unk_3e6;            /* 0x3e6 */
    u16 unk_3e8;            /* 0x3e8 */
    u8  pad_3ea[0xe];
    /* The player Chuckya has grabbed, or null. Both Behavior and Render gate
       on it together with flag 0x4000 and the target's own +0xc8. */
    void* mHeld;            /* 0x3f8 */
    u8  pad_3fc[0x30];
    s32 unk_42c;            /* 0x42c */
    s32 unk_430;            /* 0x430 */
    s32 unk_434;            /* 0x434 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
#endif
};

#endif
