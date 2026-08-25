#ifndef JETSTREAM_H
#define JETSTREAM_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct JetStream : dEnemyBase_c {
    /* What mState points at. Behavior calls the handler at +0x08 through it and
       nothing else in the tree reads the rest, so only that field is evidenced.
       Same shape as Bullet::State -- see include/Bullet.h. */
    struct State {
        u8  pad_00[0x8];
        void (JetStream::*mMain)();   /* 0x08 */
    };

    dCcAc_c mdCcAc_c;/* 0x110 */
    dBgCh_Actr mWithMeshClsn;       /* 0x144 */
    State *mState;                  /* 0x300 -- run every frame by Behavior */
    u8  pad_304[0x10];
    /* Both are decoded out of the spawn word (fBase_c::param1) by InitResources
       and never read back anywhere in the tree, so what they select is unknown:
       0x314 takes nibble 3 (param1 >> 12 & 0xf), 0x318 takes bit 0 but only when
       the low nibble is 0 or 1. Deliberately left unk_ -- see
       notes/enemy-leaf-provenance.md. */
    s32 unk_314;                      /* 0x314 */
    s32 unk_318;                      /* 0x318 */

    /* --- vtable --- */
    virtual ~JetStream();

    virtual s32 Behavior();
    virtual s32 CleanupResources();
    virtual s32 InitResources();
    virtual void OnPendingDestroy();
    virtual s32 Render();

    /* Tail padding. The field span stops short of the real size: JetStream_Spawn
       calls fBase_c::operator new(0x378), read off the retail
       instruction. A span is only a LOWER BOUND. */
    u8 pad_31c[0x5c];      /* 0x31c, to the ROM's 0x378 */
};

typedef char JetStream_size_must_be_0x378[sizeof(JetStream) == 0x378 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct JetStream {
    u8  pad_000[0x8];
    u32 param1;            /* 0x008 */
    u8  pad_00c[0x80];
    /* 0x008..0x10f is the fBase_c/dBase_c/dActor_c/dEnemyBase_c chain restated flat.
       Every name below is dActor_c.h's own (fBase_c.h's for param1); nothing here
       is an independent claim, and the C++ branch above inherits them properly. */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;                  /* 0x0b0 */
    s32 mClipOffsetY;            /* 0x0b4 */
    s32 mClipRadius;             /* 0x0b8 */
    s32 mClipDistance;           /* 0x0bc */
    s32 mFarDistance;            /* 0x0c0 */
    u8  mClipResult;             /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 mDeathTableID;           /* 0x0ce */
    u8  pad_0d0[0x30];
    u8  mStateTimer;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mdCcAc_c;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    void *mState;                /* 0x300 */
    u8  pad_304[0x10];
    s32 unk_314;            /* 0x314 */
    s32 unk_318;            /* 0x318 */
};

#endif /* __cplusplus */

#endif /* JETSTREAM_H */
