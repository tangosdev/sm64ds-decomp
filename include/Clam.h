#ifndef CLAM_H
#define CLAM_H

#include "types.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

#ifdef __cplusplus

#include "dActor_c.h"

/* daObjShell_c_classInit constructs dActor_c, then ModelAnim and dCcAc_c at +0xd4 and
 * +0x138. D1/D0 destroy the same subobjects in reverse, independently pinning
 * both the inheritance and owned-member layout. */
struct Clam : dActor_c {
    u8 pad_0d0[0x4];         /* 0x0d0 */
    ModelAnim mModelAnim;     /* 0x0d4 */
    dCcAc_c mdCcAc_c;         /* 0x138 */
    u8 mState;                /* 0x16c: 0 shut, 1 lunging */
    u8 pad_16d[0x1];
    u16 mStateTimer;          /* 0x16e */
    u16 mShutTimer;           /* 0x170 */

    virtual ~Clam();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#else

/* Flat layout for the remaining C translation units. */
struct Clam {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    /* dActor_c::mCamSpacePos -- dActor_c.h declares s32 here. The sound call in
       Behavior takes its address, which is what pins it. */
    s32 mCamSpacePosX;            /* 0x074 */
    u8  pad_078[0x5c];
    ModelAnim mModelAnim;            /* 0x0d4 */
    dCcAc_c mdCcAc_c;         /* 0x138 */
    u8  mState;            /* 0x16c */
    u8  pad_16d[0x1];
    u16 mStateTimer;            /* 0x16e */
    u16 mShutTimer;            /* 0x170 */
};

#endif

typedef char Clam_size_must_be_0x174[sizeof(struct Clam) == 0x174 ? 1 : -1];

#endif /* CLAM_H */
