#ifndef DAMANTA_C_H
#define DAMANTA_C_H

#include "types.h"

/* Jolly Roger Bay manta. ROM RTTI ov090:0x021341f4 names daManta_c; the
 * debug table names MANTA (226). One direct base, dEnemyBase_c.
 *
 * The destructor and daManta_c_classInit agree on the owned objects:
 *     0x110 dCcAcPos_c   0x40  -> 0x150
 *     0x150 dBgCh_Actr   0x1bc -> 0x30c
 *     0x30c ModelAnim    0x64  -> 0x370
 * classInit allocates 0x404. The tail through 0x400 is the ring challenge
 * (see src/actors/daManta_c.cpp); nothing past 0x400 is read.
 *
 * mState points at a two-pointer-to-member record. daManta_c.cpp defines
 * data_ov090_0213454c; the compiler copies the init PMF
 * (func_ov090_02132a58) and the execute PMF (func_ov090_021327e4) into it.
 * Behavior calls execute. The record type is completed in the TU.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct MantaState;

struct daManta_c : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;    /* 0x110 */
    dBgCh_Actr mWithMeshClsn;  /* 0x150 */
    ModelAnim mModelAnim;      /* 0x30c */
    MantaState *mState;        /* 0x370 */
    u8 pad_374[4];             /* 0x374 */
    s32 mRingCount;            /* 0x378 -- rings taken in order; 5 starts the star wait; 0xa after it spawns */
    s32 mPathID;               /* 0x37c -- param1 & 0xff, PathPtr::FromID */
    s32 mNumNodes;             /* 0x380 -- PathPtr::NumNodes */
    s32 mPathNode;             /* 0x384 */
    s32 mStarID;               /* 0x388 -- (param1 >> 12) & 0xf, OR 0x40 into the STAR spawn */
    s32 mStarDelay;            /* 0x38c -- frames after the fifth ring, before the star */
    u8 pad_390[0xc];           /* 0x390 */
    Vector3 mRingPos;          /* 0x39c -- bone 3's translation, where the next WATER_RING spawns */
    dActor_c *mHitRing;        /* 0x3a8 -- ring whose cylinder just reported a hit; cleared after scoring */
    s32 mRingIDs[0x14];        /* 0x3ac -- uniqueIDs of the rings spawned this lap */
    s32 mRingWrite;            /* 0x3fc */
    s32 mRingRead;             /* 0x400 */

    /* --- vtable --- */
    virtual ~daManta_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    void func_ov090_02132730();
    int func_ov090_021327e4();
    int func_ov090_02132a58();
    int func_ov090_02132ac4(MantaState *state);
    void func_ov090_02132b14();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daManta_c_size_must_be_0x404[sizeof(daManta_c) == 0x404 ? 1 : -1];
#endif

#endif /* DAMANTA_C_H */
