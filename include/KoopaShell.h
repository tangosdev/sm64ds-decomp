#ifndef KOOPASHELL_H
#define KOOPASHELL_H

#include "types.h"

/* Six boundaries close on sizes other headers assert:
 *
 *     dEnemyBase_c              ends 0x110
 *     dCcAc_c 0x110 + 0x034 = 0x144  -> dBgCh_Actr
 *     dBgCh_Actr       0x144 + 0x1bc = 0x300  -> Model
 *     Model              0x300 + 0x050 = 0x350  -> ShadowModel
 *     ShadowModel        0x350 + 0x028 = 0x378  -> the second cylinder
 *     dCcAc_c 0x378 + 0x034 = 0x3ac  -> mState
 *
 * Two cylinder collisions, one at each end of the shell.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"
#include "dCcAc_c.h"

struct KoopaShell : dEnemyBase_c {
    dCcAc_c mdCc_c;        /* 0x110 */
    dBgCh_Actr mMeshClsn;                  /* 0x144 */
    Model mModel;                            /* 0x300 */
    ShadowModel mShadowModel;                /* 0x350 */
    dCcAc_c mdCc_c2;       /* 0x378 */
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
    /* Trailing remainder, 8 bytes. Every sub-object is typed and every field
       the eight recovered functions touch ends at 0x3d8; KoopaShell_Spawn
       allocates 0x3e0. */
    u8  pad_3d8[0x8];

    virtual ~KoopaShell();

    virtual s32   OnYoshiTryEat();         /* slot 18 */

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
    /* dActor_c's own 0x09c/0x0a0 -- gravity and terminal velocity. This twin
       used to call 0x09c "mSpeed", which contradicts include/dActor_c.h and the
       C++ branch above; InitResources writes -0x2000 and -0x32000 here. */
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;     /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;            /* 0x0b0 */
    u8  pad_0b4[0x4c];
    s16 mStateTimer;            /* 0x100 */
    u8  pad_102[0x5];
    u8  mEatenByYoshi;            /* 0x107 */
    u8  pad_108[0x8];
    /* Sub-objects, kept as byte markers: their sizes are fixed by the NEXT
       marker's offset, which is what the ROM evidences, and none of the five
       functions here needs a view into one. 0x110 is initialised as a
       dCcAc_c and later driven through dCc_c::Clear/Update,
       so the moving flavour derives from the plain one. */
    u8  mdCc_c;            /* 0x110 */
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
    /* Trailing remainder, 8 bytes. Every sub-object is typed and every field
       the eight recovered functions touch ends at 0x3d8; KoopaShell_Spawn
       allocates 0x3e0. */
    u8  pad_3d8[0x8];
};

#endif /* __cplusplus */

typedef char KoopaShell_size_must_be_0x3e0[sizeof(struct KoopaShell) == 0x3e0 ? 1 : -1];

#endif /* KOOPASHELL_H */
