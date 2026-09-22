#ifndef ROTATINGFIREBAR_H
#define ROTATINGFIREBAR_H

#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

/* The rotating bar of flames. Derives from dBgActor_c, and the ONE thing it adds
 * is the array of collision cylinders -- one per flame.
 *
 *     dCcPos_c[8]   0x360 + 8 * 0x3c = 0x540
 *
 * The ROM destroys it with
 * __cxa_vec_cleanup(this + 0x360, 8, 0x3c, _ZN8dCcPos_cD1Ev): 0x3c is
 * sizeof(dCcPos_c) and the element destructor is a name the tree
 * already had, so this class needed no new type.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dCcPos_c.h"

struct RotatingFirebar : dBgActor_c {
    ROM_BASE_TAIL_PAD(31e, 0x2)
    u8  pad_320[0x40];
    dCcPos_c mdCc_cs[8];   /* 0x360 */

    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~RotatingFirebar();   /* no slot */
#else
    virtual ~RotatingFirebar();   /* D1 and D0 */
#endif

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char RotatingFirebar_size_must_be_0x540[sizeof(RotatingFirebar) == 0x540 ? 1 : -1];
#endif
/* daObjFl_KomaU_c_classInit runs __cxa_vec_ctor over eight dCcPos_c at a raw
   this + 0x360, and this class's own five bodies reach them by name, so the two
   have to agree. Without the macro MSVC put the array at 0x364. */
ROM_OFFSET_ASSERT(RotatingFirebar, mdCc_cs, 0x360);

#else

/* The same object for a C translation unit, flat. */
struct RotatingFirebar {
    u8  pad_000[0x8e];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x20];
    s32 mFlags;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    /* Model member. The cartridge's own ~RotatingFirebar calls _ZN5ModelD1Ev at +0x0d4
       (D0/D1), a relocation the ROM build checks; recovered by tools/dtor_members.py.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~RotatingFirebar calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
};

#endif /* __cplusplus */

#endif /* ROTATINGFIREBAR_H */
