#ifndef FLOATONLAVAPLATFORM_H
#define FLOATONLAVAPLATFORM_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* A dBgActor_c: model at 0xd4, moving mesh collider at 0x124, clsn matrix at
 * 0x2ec, all inherited. Layout evidence: notes/platform-provenance.md. */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct FloatOnLavaPlatform : dBgActor_c {
    ROM_BASE_TAIL_PAD(31e, 0x2)
    s32 mMaxPosY;                      /* 0x320 */
    u8 mHadClsn;                       /* 0x324 */

    /* --- vtable --- */
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~FloatOnLavaPlatform();   /* no slot */
#else
    virtual ~FloatOnLavaPlatform();   /* D1 and D0 */
#endif

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char FloatOnLavaPlatform_size_must_be_0x328[sizeof(FloatOnLavaPlatform) == 0x328 ? 1 : -1];
#endif

#else

/* The same object spelled flat, for the compiler-generated destructor, which
   lives in a C translation unit and can never be migrated. */
struct FloatOnLavaPlatform {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x2a];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    Model mModel;            /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mMaxPosY;            /* 0x320 */
    u8  mHadClsn;            /* 0x324 */
};

#endif /* __cplusplus */

#endif /* FLOATONLAVAPLATFORM_H */
