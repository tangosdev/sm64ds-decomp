#ifndef GOOMBOSS_H
#define GOOMBOSS_H

#include "types.h"
#include "ModelAnim.h"
#include "dBgCh_Actr.h"

/* The Goomboss. Its destructor is the layout, and eight boundaries close on
 * sizes other headers assert:
 *
 *     dEnemyBase_c                        ends 0x110
 *     dCcAcPos_c[4] 0x110 + 4*0x40 = 0x210  -> ModelAnim
 *     ModelAnim                    0x210 +   0x64 = 0x274  -> the shadows
 *     ShadowModel[3]               0x274 + 3*0x28 = 0x2ec
 *     Vector3[3]                   0x3ac + 3*0x0c = 0x3d0  -> MaterialChanger
 *     MaterialChanger              0x3d0 +   0x14 = 0x3e4  -> TextureSequence
 *     TextureSequence              0x3e4 +   0x14 = 0x3f8  -> TextureTransformer
 *     TextureTransformer           0x3f8 +   0x14 = 0x40c  -> dBgCh_Actr
 *     dBgCh_Actr                 0x40c +  0x1bc = 0x5c8
 *
 * SIX OF THE GENERATED FIELDS WERE THE Vector3 ARRAY'S OWN COMPONENTS: the
 * header had split a Vector3[3] at 0x3ac into nine scalars and named six of
 * them (elements 1 and 2). Both branches spell the array now. Two more former
 * unknowns sat inside embedded animations rather than in Goomboss at all --
 * 0x3d8 is the MaterialChanger's Animation cursor (+0x08) and 0x3f0 is the
 * TextureSequence's playback speed (+0x0c); see include/Animation.h.
 *
 * Field provenance: notes/enemy-provenance.md.
 *
 * SM64DS RTTI names the implementation daKuriKing_c. The reconstructed
 * factory daKuriKing_c_classInit_KURIKING (historical alias
 * Goomboss_Spawn) constructs it for the KURIKING
 * registry profile.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "MaterialChanger.h"
#include "TextureSequence.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

struct Goomboss : dEnemyBase_c {
    dCcAcPos_c mdCc_cs[4];  /* 0x110 */
    ModelAnim mModelAnim;                         /* 0x210 */
    ShadowModel mShadowModels[3];                 /* 0x274 */
    u8  pad_2ec[0xc0];
    Vector3 mCylClsnPos[3];                           /* 0x3ac */
    MaterialChanger mMaterialChanger;             /* 0x3d0 */
    TextureSequence mTextureSequence;             /* 0x3e4 */
    TextureTransformer mTextureTransformer;       /* 0x3f8 */
    dBgCh_Actr mWithMeshClsn;                   /* 0x40c */
    u8  pad_5c8[0x4];
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
    u8  pad_5ec[0x4];
    s32 mDirection;            /* 0x5f0 */
    u16 mWalkAngle;            /* 0x5f4 */
    s16 mWalkSpeed;            /* 0x5f6 */
    s16 mMaxWalkSpeed;            /* 0x5f8 */
    u8  pad_5fa[0x4];
    u8  mLeftFootSteppedOnGround;            /* 0x5fe */
    u8  mRightFootSteppedOnGround;            /* 0x5ff */
    u8  pad_600[0x4];
    u8  mSizeIndex;            /* 0x604 */
    u8  pad_605[0x5];
    u8  mShouldRender;            /* 0x60a */
    /* The field span ends at 0x60b, but a span is only a LOWER BOUND. Both factories
       that store _ZTV8Goomboss (ov074:0x02122eb8) -- daKuriKing_c_classInit_KURIKING and
       daKuriKing_c_classInit_KURIKING_VANISH -- call fBase_c::operator new(0x610). Two factories
       building one actor is a spawn-info variant, not a second class. */
    u8  pad_60b[0x5];       /* 0x60b, to the ROM's 0x610 */

    virtual ~Goomboss();

    virtual s32 Behavior();
    virtual s32 CleanupResources();
    /* Declared here so src/_ZN8Goomboss13InitResourcesEv.cpp can be a real method
       rather than an extern "C" free function under the mangled name. Safe to
       declare virtual: ~Goomboss is still the first virtual DECLARED, so the key
       function -- and with it _ZTV8Goomboss -- stays where it already was. */
    virtual s32 InitResources();
    virtual s32 Render();
};

typedef char Goomboss_size_must_be_0x610[sizeof(Goomboss) == 0x610 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct Goomboss {
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
    u8  pad_0a4[0x16c];
    ModelAnim mModelAnim;     /* 0x210 */
    u8  pad_274[0x138];
    /* The C++ branch's Vector3 mCylClsnPos[3], flat -- the six scalars that used
       to sit here as unk_3b8..unk_3cc were elements 1 and 2 of it. */
    s32 mCylClsnPos[9];              /* 0x3ac */
    u8  mMaterialChanger;            /* 0x3d0 */
    u8  pad_3d1[0x7];
    /* mMaterialChanger's Animation base +0x08, the 20.12 playback cursor.
       InitResources sets it to data_ov074_02122e04[mSizeIndex] << 12. */
    s32 mMaterialChangerFrame;       /* 0x3d8 */
    u8  pad_3dc[0x8];
    u8  mTextureSequence;            /* 0x3e4 */
    u8  pad_3e5[0xb];
    /* mTextureSequence's Animation base +0x0c, the playback speed. InitResources
       calls SetFile with 0x1000 and then zeroes this, freezing the sequence. */
    s32 mTextureSequenceSpeed;       /* 0x3f0 */
    u8  pad_3f4[0x4];
    u8  mTextureTransformer;            /* 0x3f8 */
    u8  pad_3f9[0x13];
    struct dBgCh_Actr mWithMeshClsn; /* 0x40c */
    u8  pad_5c8[0x4];
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
    u8  pad_5ec[0x4];
    s32 mDirection;            /* 0x5f0 */
    u16 mWalkAngle;            /* 0x5f4 */
    s16 mWalkSpeed;            /* 0x5f6 */
    s16 mMaxWalkSpeed;            /* 0x5f8 */
    u8  pad_5fa[0x4];
    u8  mLeftFootSteppedOnGround;            /* 0x5fe */
    u8  mRightFootSteppedOnGround;            /* 0x5ff */
    u8  pad_600[0x4];
    u8  mSizeIndex;            /* 0x604 */
    u8  pad_605[0x5];
    u8  mShouldRender;            /* 0x60a */
};

#endif /* __cplusplus */

#endif /* GOOMBOSS_H */
