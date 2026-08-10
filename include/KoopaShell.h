#ifndef KOOPASHELL_H
#define KOOPASHELL_H

#include "types.h"

/* Six boundaries close on sizes other headers assert:
 *
 *     Enemy              ends 0x110
 *     MovingCylinderClsn 0x110 + 0x034 = 0x144  -> WithMeshClsn
 *     WithMeshClsn       0x144 + 0x1bc = 0x300  -> Model
 *     Model              0x300 + 0x050 = 0x350  -> ShadowModel
 *     ShadowModel        0x350 + 0x028 = 0x378  -> the second cylinder
 *     MovingCylinderClsn 0x378 + 0x034 = 0x3ac  -> mState
 *
 * Two cylinder collisions, one at each end of the shell.
 */

#ifdef __cplusplus

#include "Enemy.h"
#include "Model.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"
#include "MovingCylinderClsn.h"

struct KoopaShell : Enemy {
    MovingCylinderClsn mCylinderClsn;        /* 0x110 */
    WithMeshClsn mMeshClsn;                  /* 0x144 */
    Model mModel;                            /* 0x300 */
    ShadowModel mShadowModel;                /* 0x350 */
    MovingCylinderClsn mCylinderClsn2;       /* 0x378 */
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

    virtual ~KoopaShell();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

#else

/* The same object for a C translation unit, flat. */
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
};

#endif /* __cplusplus */

#endif /* KOOPASHELL_H */
