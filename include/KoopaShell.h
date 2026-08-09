/* Hand-written from matched-function evidence, not gen_header.py output:
 * class KoopaShell, 5 matched functions in one clean TU (ov102
 * 0x0214c748-0x0214d70c, 24 functions, no other class in it -- tu_map.py).
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 *
 * KoopaShell derives from Enemy -- Behavior calls Enemy::UpdateYoshiEat and
 * Enemy::UpdateWMClsn on itself, and Enemy spans exactly 0x110, which is where
 * this class's own sub-objects start. It is written FLAT anyway, with the
 * inherited slots restated as padding and named fields, because that is what
 * every other generated header in this tree does and because Enemy.h asserts
 * no size for a derived struct to build on. Restating a base is a known cost;
 * re-basing the whole family is its own slice.
 *
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef KOOPASHELL_H
#define KOOPASHELL_H
#include "types.h"

struct KoopaShell {
    u8  pad_000[0x8];
    /* Spawn parameter word. InitResources takes bit 0 as the model index into
       data_ov102_0214d70c and bit 4 as unk_3c5 -- so one spawn word selects
       both the shell's artwork and a behaviour variant. */
    u32 mSpawnParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x2c];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    s32 mSpeed;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;            /* 0x0b0 */
    u8  pad_0b4[0x4c];
    s16 unk_100;            /* 0x100 */
    u8  pad_102[0x5];
    u8  unk_107;            /* 0x107 */
    u8  pad_108[0x8];
    /* Sub-objects, kept as byte markers: their sizes are fixed by the NEXT
       marker's offset, which is what the ROM evidences, and none of the five
       functions here needs a view into one. 0x110 is initialised as a
       MovingCylinderClsn and later driven through CylinderClsn::Clear/Update,
       so the moving flavour derives from the plain one. */
    u8  mCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModel;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  mShadowModel;            /* 0x350 */
    u8  pad_351[0x5b];
    /* Current state. Behavior compares it against four file-scope state
       objects by ADDRESS and reads a pointer-to-member at +0x8 out of it, so
       it points at a record whose third word is the per-state tick. */
    void* mState;            /* 0x3ac */
    /* Where it was spawned: InitResources copies mPos here verbatim, and
       Behavior stashes mPrevAngleY into mSpawnAngleY when the shell is spat
       back out of Yoshi's mouth. */
    s32 mSpawnPosX;            /* 0x3b0 */
    s32 mSpawnPosY;            /* 0x3b4 */
    s32 mSpawnPosZ;            /* 0x3b8 */
    s16 mSpawnAngleY;            /* 0x3bc */
    u8  pad_3be[0x2];
    s32 unk_3c0;            /* 0x3c0 */
    /* Model index, 0 or 1, off bit 0 of mSpawnParam. Behavior gives index 0 a
       per-frame call the other does not get. */
    u8  mModelIndex;            /* 0x3c4 */
    u8  unk_3c5;            /* 0x3c5 */
    /* Despawn countdown, and it only runs in one state. Render blinks the
       shell while it is below 0x2d by skipping odd values, so the shell
       flashes out rather than vanishing. */
    u8  mDespawnTimer;            /* 0x3c6 */
    u8  pad_3c7[0x1];
    s32 unk_3c8;            /* 0x3c8 */
    s32 unk_3cc;            /* 0x3cc */
    s32 unk_3d0;            /* 0x3d0 */
    s32 unk_3d4;            /* 0x3d4 */
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
