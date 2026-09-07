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
 * __destroy_arr(this + 0x360, 8, 0x3c, _ZN8dCcPos_cD1Ev): 0x3c is
 * sizeof(dCcPos_c) and the element destructor is a name the tree
 * already had, so this class needed no new type.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dCcPos_c.h"

struct RotatingFirebar : dBgActor_c {
    u8  pad_31e[0x42];
    dCcPos_c mdCc_cs[8];   /* 0x360 */

    virtual ~RotatingFirebar();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char RotatingFirebar_size_must_be_0x540[sizeof(RotatingFirebar) == 0x540 ? 1 : -1];

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
