/* class dScMgCurling2_c : dScMgBase_c. Same shape as dScMgCurling_c --
 * confirmed leaf, D1 writes only its own vtable then calls dScMgBase_c's
 * D2 directly. See include/dScMgCurling_c.h for the shared reasoning
 * (inherited-field misattribution below 0x4660, no size assertion). */
#ifndef DSCMGCURLING2_C_H
#define DSCMGCURLING2_C_H
#include "dScMgBase_c.h"

struct dScMgCurling2_c : dScMgBase_c {
    virtual ~dScMgCurling2_c();

    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */

    /* Slot 18 left unnamed -- same reasoning as dScMgCurling_c.h. */

    u8  pad_465d[0xf23];
    s32 unk_5580;            /* 0x5580 */
    u8  pad_5584[0x28];
    s32 unk_55ac;            /* 0x55ac */
    u8  pad_55b0[0x4];
    u16 unk_55b4;            /* 0x55b4 */
    u16 unk_55b6;            /* 0x55b6 */
    u8  pad_55b8[0x3];
    u8  unk_55bb;            /* 0x55bb */
    u8  pad_55bc[0x7];
    u8  unk_55c3;            /* 0x55c3 */
};

#endif
