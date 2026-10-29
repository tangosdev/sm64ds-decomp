#ifndef DAKURIKING_C_H
#define DAKURIKING_C_H

#include "types.h"
#include "dExtAnmModel_c.h"
#include "dBgCh_Actr.h"

/* The daKuriKing_c. Its destructor is the layout, and eight boundaries close on
 * sizes other headers assert:
 *
 *     dEnemyBase_c                        ends 0x110
 *     dCcAcPos_c[4] 0x110 + 4*0x40 = 0x210  -> dExtAnmModel_c
 *     dExtAnmModel_c                    0x210 +   0x64 = 0x274  -> the shadows
 *     dExtShadowModel_c[3]               0x274 + 3*0x28 = 0x2ec
 *     Vector3[3]                   0x3ac + 3*0x0c = 0x3d0  -> dExtAnmMaterial_c
 *     dExtAnmMaterial_c              0x3d0 +   0x14 = 0x3e4  -> dExtAnmTexPat_c
 *     dExtAnmTexPat_c              0x3e4 +   0x14 = 0x3f8  -> dExtAnmTexSRT_c
 *     dExtAnmTexSRT_c           0x3f8 +   0x14 = 0x40c  -> dBgCh_Actr
 *     dBgCh_Actr                 0x40c +  0x1bc = 0x5c8
 *
 * SIX OF THE GENERATED FIELDS WERE THE Vector3 ARRAY'S OWN COMPONENTS: the
 * header had split a Vector3[3] at 0x3ac into nine scalars and named six of
 * them (elements 1 and 2). Both branches spell the array now. Two more former
 * unknowns sat inside embedded animations rather than in daKuriKing_c at all --
 * 0x3d8 is the dExtAnmMaterial_c's dExtFrameCtrl_c cursor (+0x08) and 0x3f0 is the
 * dExtAnmTexPat_c's playback speed (+0x0c); see include/dExtFrameCtrl_c.h.
 *
 * Field provenance: notes/enemy-leaf-provenance.md.
 *
 * The cartridge names this class daKuriKing_c: _ZTI12daKuriKing_c at
 * ov074:0x02122e30 and _ZTS12daKuriKing_c at 0x02122e3c, with the vtable
 * this class owns at 0x02122eb8. The tree used to spell it Goomboss; the
 * rename and its ABI argument are in the manifest's class-label note. The
 * reconstructed factory daKuriKing_c_classInit_KURIKING (historical alias
 * Goomboss_Spawn) constructs it for the KURIKING registry profile.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "dExtAnmModel_c.h"
#include "dExtShadowModel_c.h"
#include "dExtAnmMaterial_c.h"
#include "dExtAnmTexPat_c.h"
#include "dExtAnmTexSRT_c.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

struct daKuriKing_c : dEnemyBase_c {
    dCcAcPos_c mdCc_cs[4];  /* 0x110 */
    dExtAnmModel_c mAnmModel;                     /* 0x210 */
    dExtShadowModel_c mShadowModels[3];           /* 0x274 */
    /* One drop-shadow matrix per foot. func_ov074_02121800 sets each to the
       identity with that foot's mCylClsnPos x/z and mPosY (>> 3) as translation
       and hands it to DropShadowRadHeight; func_ov074_021223bc uses [0] alone.
       Raw words rather than Matrix4x3: see d_a_kuri_king.cpp's deslop notes. */
    s32 mShadowMatrices[3][12]; /* 0x2ec */
    /* Body transform. func_ov074_02121300 builds it from mPos >> 3, mAngleY and
       mScale; func_ov074_02121270 composes a bone's transform with it to get
       that bone's world position. */
    s32 mBodyMatrix[12];                          /* 0x37c */
    Vector3 mCylClsnPos[3];                       /* 0x3ac */
    dExtAnmMaterial_c mMaterialChanger;           /* 0x3d0 */
    dExtAnmTexPat_c mTextureSequence;             /* 0x3e4 */
    dExtAnmTexSRT_c mTextureTransformer;          /* 0x3f8 */
    dBgCh_Actr mWithMeshClsn;                   /* 0x40c */
    /* ClosestPlayer, kept for StartTalk / ShowMessage / GetTalkState. */
    Player *mTalkPlayer;                          /* 0x5c8 */
    s32 mState;            /* 0x5cc */
    /* InitResources copies mPosX/mPosY/mPosZ into these three once, right after
       the collision cylinders are sized, and nothing writes them again. */
    s32 mSpawnPosX;         /* 0x5d0 */
    s32 mSpawnPosY;         /* 0x5d4 */
    s32 mSpawnPosZ;         /* 0x5d8 */
    s32 mMegaMushroomID;            /* 0x5dc */
    u8  pad_5e0[0x4];
    s32 mCurrentScale;            /* 0x5e4 */
    s32 mGoombaTargetSpeed;            /* 0x5e8 */
    /* Scale mCurrentScale is eased toward. */
    s32 mTargetScale;                         /* 0x5ec */
    s32 mDirection;            /* 0x5f0 */
    /* Reads in this TU are signed halfwords (ldrsh), not the old u16. */
    s16 mWalkAngle;            /* 0x5f4 */
    s16 mWalkSpeed;            /* 0x5f6 */
    s16 mMaxWalkSpeed;            /* 0x5f8 */
    /* Sine phase for the shrink wobble; advances 0x1230 a frame. */
    s16 mScalePhase;                          /* 0x5fa */
    /* DecIfAbove0_Short countdown between spawns and in the death talk. */
    u16 mTimer;                               /* 0x5fc */
    u8  mLeftFootSteppedOnGround;            /* 0x5fe */
    u8  mRightFootSteppedOnGround;            /* 0x5ff */
    /* Foot-edge latches, set in func_ov074_02121380 while a foot cylinder's
       ground test reads below the body. */
    u8  mFoot2Latch;                          /* 0x600 */
    u8  mFoot1Latch;                          /* 0x601 */
    u8  mSpawnedCount;                        /* 0x602 */
    u8  mSubState;                            /* 0x603 */
    u8  mSizeIndex;            /* 0x604 */
    /* DecIfAbove0_Byte; set to 0x1e or 0x10 when the player is hit. */
    u8  mHurtTimer;                           /* 0x605 */
    u8  pad_606;
    /* Remaining shrink steps. Starts at 3, and the last step is the one <= 1. */
    u8  mScaleSteps;                          /* 0x607 */
    /* Set on death. func_ov074_02121800 skips the drop shadows while it is set. */
    u8  mNoShadow;                            /* 0x608 */
    /* Set when a mini is thrown. func_ov074_021223bc returns immediately while it is 0. */
    u8  mLaunched;                            /* 0x609 */
    u8  mShouldRender;            /* 0x60a */
    /* Latched once func_ov074_0211f38c reports the wave has all spawned. */
    u8  mWaveFinished;                        /* 0x60b */
    /* Layer-3 music one-shot during the intro talk. */
    u8  mMusicStarted;                        /* 0x60c */
    /* The field span ends at 0x60d, but a span is only a LOWER BOUND. Both factories
       that store _ZTV12daKuriKing_c (ov074:0x02122eb8) -- daKuriKing_c_classInit_KURIKING and
       daKuriKing_c_classInit_KURIKING_VANISH -- call fBase_c::operator new(0x610). Two factories
       building one actor is a spawn-info variant, not a second class. */
    u8  pad_60d[0x3];                         /* 0x60d, to the ROM's 0x610 */

    virtual ~daKuriKing_c();

    virtual s32 Behavior();
    virtual s32 CleanupResources();
    /* Declared here so InitResources (src/game/actors/d_a_kuri_king.cpp) can be a real
       method rather than an extern "C" free function under the mangled name. Safe to
       declare virtual: ~daKuriKing_c is still the first virtual DECLARED, so the key
       function -- and with it _ZTV12daKuriKing_c -- stays where it already was. */
    virtual s32 InitResources();
    virtual s32 Render();

    /* The class's ordinary methods, under the ROM's only surviving spellings:
       their addresses. The state handlers are the records __sinit copies into
       data_ov074_021230f8; func_ov074_021203e4 runs the enter side. */
    void func_ov074_0211f154();
    void func_ov074_0211f244();
    void func_ov074_0211f344(u8 value);
    int  func_ov074_0211f38c();
    void func_ov074_0211f5b8();
    void func_ov074_0211fa08();
    void func_ov074_0211fa74();
    void func_ov074_0211fb84();
    int  func_ov074_0211fbd0();
    void func_ov074_0211fc34();
    void func_ov074_0211fc38();
    void func_ov074_0211fd48();
    void func_ov074_0211fd74();
    void func_ov074_0211ffac();
    void func_ov074_0211ffcc();
    void func_ov074_02120080();
    void func_ov074_0212016c();
    void func_ov074_0212018c();
    void func_ov074_021201f0();
    int  func_ov074_021203e4(int i);
    void func_ov074_0212042c();
    int  func_ov074_02120474();
    int  func_ov074_021204c0();
    int  func_ov074_021206c8();
    int  func_ov074_021207b8();
    void func_ov074_02120808();
    int  func_ov074_02120b24();
    void func_ov074_02120b90();
    void func_ov074_02120bb8(dActor_c *other, dCcAcPos_c *clsn, int mode);
    void func_ov074_02120d74();
    void func_ov074_02121300();
    void func_ov074_02121380();
    int  func_ov074_021216f4();
    void func_ov074_02121800();
    void func_ov074_0212195c();
    void func_ov074_0212199c();
    int  func_ov074_02121a20(int idx);
    int  func_ov074_0212229c();
    int  func_ov074_021222e0();
    int  func_ov074_021223bc();
    int  func_ov074_02122634();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKuriKing_c_size_must_be_0x610[sizeof(daKuriKing_c) == 0x610 ? 1 : -1];
#endif

#else

/* The same object for a C translation unit, flat. */
struct daKuriKing_c {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    /* 0x088..0x09c is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c -- dActor_c's; InitResources sets -0xa000 */
    s32 mTerminalVelocity;       /* 0x0a0 -- dActor_c's; InitResources sets -0x3c000 */
    u8 pad_0a4[0x16c];
    dExtAnmModel_c mAnmModel; /* 0x210 */
    u8 pad_274[0x78];
    s32 mShadowMatrices[3][12]; /* 0x2ec */
    s32 mBodyMatrix[12];        /* 0x37c */
    /* The C++ branch's Vector3 mCylClsnPos[3], flat -- the six scalars that used
       to sit here as unk_3b8..unk_3cc were elements 1 and 2 of it. */
    s32 mCylClsnPos[9];              /* 0x3ac */
    u8  mMaterialChanger;            /* 0x3d0 */
    u8  pad_3d1[0x7];
    /* mMaterialChanger's dExtFrameCtrl_c base +0x08, the 20.12 playback cursor.
       InitResources sets it to data_ov074_02122e04[mSizeIndex] << 12. */
    s32 mMaterialChangerFrame;       /* 0x3d8 */
    u8  pad_3dc[0x8];
    u8  mTextureSequence;            /* 0x3e4 */
    u8  pad_3e5[0xb];
    /* mTextureSequence's dExtFrameCtrl_c base +0x0c, the playback speed. InitResources
       calls SetFile with 0x1000 and then zeroes this, freezing the sequence. */
    s32 mTextureSequenceSpeed;       /* 0x3f0 */
    u8  pad_3f4[0x4];
    u8  mTextureTransformer;            /* 0x3f8 */
    u8  pad_3f9[0x13];
    struct dBgCh_Actr mWithMeshClsn; /* 0x40c */
    void *mTalkPlayer;       /* 0x5c8 */
    s32 mState;            /* 0x5cc */
    /* InitResources copies mPosX/mPosY/mPosZ into these three once, right after
       the collision cylinders are sized, and nothing writes them again. */
    s32 mSpawnPosX;         /* 0x5d0 */
    s32 mSpawnPosY;         /* 0x5d4 */
    s32 mSpawnPosZ;         /* 0x5d8 */
    s32 mMegaMushroomID;            /* 0x5dc */
    u8  pad_5e0[0x4];
    s32 mCurrentScale;            /* 0x5e4 */
    s32 mGoombaTargetSpeed;            /* 0x5e8 */
    s32 mTargetScale;        /* 0x5ec */
    s32 mDirection;            /* 0x5f0 */
    s16 mWalkAngle;            /* 0x5f4 */
    s16 mWalkSpeed;            /* 0x5f6 */
    s16 mMaxWalkSpeed;            /* 0x5f8 */
    s16 mScalePhase;         /* 0x5fa */
    u16 mTimer;              /* 0x5fc */
    u8  mLeftFootSteppedOnGround;            /* 0x5fe */
    u8  mRightFootSteppedOnGround;            /* 0x5ff */
    u8  pad_600[0x2];
    u8  mSpawnedCount;       /* 0x602 */
    u8  mSubState;           /* 0x603 */
    u8  mSizeIndex;            /* 0x604 */
    u8  mHurtTimer;          /* 0x605 */
    u8  pad_606;
    u8  mScaleSteps;         /* 0x607 */
    u8  mNoShadow;           /* 0x608 */
    u8  mLaunched;           /* 0x609 */
    u8  mShouldRender;            /* 0x60a */
    u8  mWaveFinished;       /* 0x60b */
    u8  mMusicStarted;       /* 0x60c */
    u8  pad_60d[0x3];
};

#endif /* __cplusplus */

#endif /* DAKURIKING_C_H */
