/* The base of the level-object family: platforms, lifts, blocks and the rest of
 * the things a level is built out of. A model, a moving mesh collider, and the
 * matrix that keeps the collider where the model is.
 *
 * Layout evidence and the sizeof-0x320 argument: notes/platform-provenance.md.
 * Field names still spelled unk_ are placeholders. */
#ifndef DBGACTOR_C_H
#define DBGACTOR_C_H
#include "types.h"

/* fwd */
struct Player;
struct player_;
#ifdef __cplusplus

/* MEASURED -- MUST STAY FIRST, ahead of Model.h. Matrix4x3 has two guarded
   spellings sharing the same 0x30 bytes; whichever a TU sees first stands, and
   Model.h pulls in the wrong one. UpdateClsnPosAndRot then compiles to 0x74
   against the ROM's 0x64. notes/platform-provenance.md. */
#include "common.h"

#include "dActor_c.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct dBgActor_c : dActor_c {
    u8  pad_0d0[0x4];
    Model mModel;                           /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;       /* 0x124 */
    /* Where the collider sits: mModel's matrix with the actor's position
       written over the translation row. See UpdateClsnPosAndRot. */
    Matrix4x3 mClsnMat;     /* 0x2ec */
    u8  unk_31c;            /* 0x31c */
    u8  unk_31d;            /* 0x31d */
    /* The class ends at 0x31e; sizeof rounds to 0x320, and derived classes do
       use that tail padding (daObjRc_Guruguru_c::mAngVelY). */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* MEASURED -- INLINE ON PURPOSE, do not move out of line. Every subclass
       destructor inlines this body; an out-of-line declaration makes each one
       emit a `bl` the ROM does not have, and gives this class a key function.
       notes/platform-provenance.md. */
    virtual ~dBgActor_c() {}

    /* Slot 31, the only new virtual this class adds -- and the key function, so
       the TU defining it also emits _ZTV10dBgActor_c and _ZTI10dBgActor_c.
       MEASURED against the cartridge's 33-word table; without it every subclass
       vtable comes out a word short. notes/platform-provenance.md. */
    virtual void Kill();

    /* --- non-virtual --- */
    /* DECLARED, defined out of line in src/_ZN10dBgActor_cC2Ev.cpp as real
     * C++. Base-only class: the ROM function is the dActor_c base step, the
     * vptr store, and the two member constructions in declaration order. */
    dBgActor_c();

    void KillByMegaChar(Player &player_);
    void UpdateClsnPosAndRot();
    void UpdateModelPosAndRotY();

    /* MEASURED: the by-value Fix12<int> is confirmed by the body at ov002
       0x020ee674, not inferred from the mangled name. The two IsClsnInRange*
       symbols make the same claim and their bytes refuse it -- see
       include/dActor_c.h and notes/platform-provenance.md. */
    int UpdateKillByMegaChar(s16 a, s16 b, s16 c, Fix12<int> d);
};

typedef char dBgActor_c_size_must_be_0x320[sizeof(dBgActor_c) == 0x320 ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base class to inherit
   dActor_c's fields from and so spells the whole layout flat. */
struct dBgActor_c {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 mHorzSpeed;            /* 0x098 */
    s32 mVertAccel;            /* 0x09c */
    s32 mTerminalVelocity;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;             /* 0x0b0 */
    s32 mClipOffsetY;       /* 0x0b4 */
    s32 mClipRadius;        /* 0x0b8 */
    u8  pad_0bc[0x18];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;            /* 0x0f0 */
    u8  pad_0f1[0x23];
    s32 unk_114;            /* 0x114 */
    s32 unk_118;            /* 0x118 */
    s32 unk_11c;            /* 0x11c */
    u8  pad_120[0x4];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    struct Matrix4x3 mClsnMat;    /* 0x2ec */
    u8  unk_31c;            /* 0x31c */
    u8  unk_31d;            /* 0x31d */
    /* Ends at 0x31e; sizeof rounds to 0x320. See the C++ half. */
};

#endif /* __cplusplus */


#endif
