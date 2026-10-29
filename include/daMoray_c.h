#ifndef DAMORAY_C_H
#define DAMORAY_C_H

#include "types.h"

/* The eel. Its destructor is the layout, and every member closes exactly on
 * the next field:
 *
 *     dEnemyBase_c                     ends 0x110
 *     dCcAcPos_c 0x110 + 0x040 = 0x150  -> the second one
 *     dCcAcPos_c 0x150 + 0x040 = 0x190  -> dBgCh_Actr
 *     dBgCh_Actr              0x190 + 0x1bc = 0x34c  -> mState
 *     dExtBlendAnmModel_c            0x350 + 0x070 = 0x3c0
 *     Vector3[7]                0x448 + 0x054 = 0x49c  -> mStarUniqueID
 *
 * unk_360 and unk_3ac are gone: both fall INSIDE the dExtBlendAnmModel_c at 0x350
 * (+0x10 and +0x5c), so they were never daMoray_c's fields -- the generated header
 * declared them beside a `u8 mBlendAnmModel` marker whose padding stopped
 * short of the real object.
 *
 * The array at 0x448 is Vector3[7]: the ROM destroys it with
 * __cxa_vec_cleanup(ptr, 7, 0xc, _ZN7Vector3D1Ev), and 0xc is sizeof(Vector3).
 *
 * SM64DS RTTI names the implementation daMoray_c. The reconstructed factory
 * daMoray_c_classInit (historical alias Unagi_Spawn) installs this class's
 * cartridge vtable for the MORAY registry profile.
 */

#include "dEnemyBase_c.h"
#include "dExtBlendAnmModel_c.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

struct daMoray_c : dEnemyBase_c {
    /* One state is two member-function pointers: init runs once, when
       func_ov016_02111bf0 installs the state, and execute runs every frame
       from Behavior. The five tables (0x02114d7c..0x02114dbc, 0x10 each) are
       filled in by __sinit_ov016_021136ec. */
    struct State {
        int (daMoray_c::*init)();
        int (daMoray_c::*execute)();
    };

    dCcAcPos_c mdCcAcPos_c1;  /* 0x110 */
    dCcAcPos_c mdCcAcPos_c2;  /* 0x150 */
    dBgCh_Actr mWithMeshClsn;                             /* 0x190 */
    const State *mState;                                    /* 0x34c */
    dExtBlendAnmModel_c mBlendAnmModel;                     /* 0x350 */
    /* Where the carried star sits: the jaw bone's transform (bone 4) moved
       0x28000 along it. The star actor keeps a pointer to this matrix. */
    Matrix4x3 mStarMtx;                                     /* 0x3c0 */
    s32 mHomePosX;                                          /* 0x3f0 */
    s32 mHomePosY;                                          /* 0x3f4 */
    s32 mHomePosZ;                                          /* 0x3f8 */
    /* Which mSegmentPos entry the first hit cylinder sits on: +1 per call of
       func_ov016_02111284, wrapping to 0 after 6. */
    s32 mCylSegment;                                        /* 0x3fc */
    s32 unk_400;                                            /* 0x400 -- zeroed by two state inits */
    s32 mPathID;                                            /* 0x404 */
    s32 mVariant;                                           /* 0x408 */
    s32 mPathNodeCount;                                     /* 0x40c */
    s32 mPathNodeIndex;                                     /* 0x410 */
    u8  mStarParam;                                         /* 0x414 */
    u8  pad_415[0x1];
    s16 mStarSpinAngle;                                     /* 0x416 */
    /* [1..6] bend the body bones; [7] is the target [6] eases toward. */
    s16 mSegmentAngle[8];                                   /* 0x418 */
    s16 mInitAngleX;                                        /* 0x428 */
    s16 mInitAngleY;                                        /* 0x42a */
    s16 mInitAngleZ;                                        /* 0x42c */
    u8  pad_42e[0xe];
    Vector3 mStarPos;                                       /* 0x43c */
    Vector3 mSegmentPos[7];                                 /* 0x448 */
    s32 mStarUniqueID;                                      /* 0x49c */
    /* trailing extent the ROM's `new daMoray_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_4a0[0x10];

    virtual ~daMoray_c();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    void OnPendingDestroy();

    void func_ov016_02111284();
    int func_ov016_02111534();
    int BookSwitch_Spawn();
    int func_ov016_021115c0();
    int func_ov016_02111718();
    int func_ov016_02111758();
    int func_ov016_02111860();
    int func_ov016_021118b4();
    int func_ov016_02111994();
    int func_ov016_021119ec();
    int func_ov016_02111bac();
    int func_ov016_02111bf0(State *state);
    void func_ov016_02111c40();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daMoray_c_size_must_be_0x4b0[sizeof(struct daMoray_c) == 0x4b0 ? 1 : -1];
#endif

#endif /* DAMORAY_C_H */
