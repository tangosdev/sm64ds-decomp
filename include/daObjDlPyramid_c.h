#ifndef DAOBJDLPYRAMID_C_H
#define DAOBJDLPYRAMID_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the dExtSimpleModel_c at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dBgActor_c.h"
#include "dExtSimpleModel_c.h"

struct daObjDlPyramid_c : dBgActor_c {
    u8 pad_31e[0x2];
    /* The class's own model. NOT "mModel": dBgActor_c's inherited
       dExtSimpleModel_c at 0xd4 already owns that name. */
    dExtSimpleModel_c mTopModel; /* 0x320 */
    /* The collider's own matrix: InitResources passes it as the
       `const Matrix4x3 &' argument of dBgW_KcMbg::SetFile, and the per-frame
       collider update rebuilds it from the yaw and the actor position.
       0x370 + 0x30 lands exactly on mHomePosX. */
    Matrix4x3 mClsnMat2;  /* 0x370 */
    s32 mHomePosX;        /* 0x3a0 -- InitResources copies mPosX/Y/Z here */
    s32 mHomePosY;        /* 0x3a4 */
    s32 mHomePosZ;        /* 0x3a8 */
    s32 mSpinParticleID;  /* 0x3ac */
    s16 mAngVelY;         /* 0x3b0 */
    u16 mStateTimer;      /* 0x3b2 */
    s16 mSoundTimer;      /* 0x3b4 */
    u8 mNumTagsTriggered; /* 0x3b6 */
    u8 mState;            /* 0x3b7 */

    /* --- vtable --- */
    virtual ~daObjDlPyramid_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    void FinishDescent();     /* dust, event 0xe, then remove the top */
    void Descend();           /* one frame of the wobble, bob, spin and sink */
    void UpdateClsnMatrix();  /* yaw + position into mClsnMat2 and the collider */
    void UpdateModelMatrix(); /* yaw + position >> 3 into mTopModel */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjDlPyramid_c_size_must_be_0x3b8[sizeof(daObjDlPyramid_c) == 0x3b8 ? 1 : -1];
#endif

#endif /* DAOBJDLPYRAMID_C_H */
