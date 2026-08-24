#ifndef WINGFEATHER_H
#define WINGFEATHER_H
#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* daFeather_c in the ROM's RTTI (same 11-char length as this class's own
 * decomp name, but a different spelling -- so unlike every prior pilot's
 * coined-name mismatch, this one cannot be caught by comparing lengths;
 * only reading config/arm9/overlays/ov002/symbols.txt next to _ZTV11WingFeather
 * shows it). WingFeather_Spawn allocates 0x388 bytes (fBase_c::operator new(904)),
 * constructs dActor_c, stores _ZTV11WingFeather, then four more members in
 * declaration order; ~WingFeather (tools/dtor_members.py) tears them down in
 * reverse. That closes the object with no unaccounted bytes:
 *
 *   dActor_c    0x000..0x0d0  (inherited, not redeclared)
 *   (padding)   0x0d0..0x0d4
 *   Model        0x0d4  D1 at +0x0d4
 *   dCcAc_c      0x124  D1 at +0x124
 *   dBgCh_Actr   0x158  D1 at +0x158
 *   ShadowModel  0x314  D1 at +0x314, ends 0x33c
 *   (own fields) 0x33c..0x388
 *
 * THE VTABLE, at ov002 0x021088a8, diffs four slots against _ZTV8dActor_c --
 * InitResources(0), CleanupResources(3), Behavior(6), Render(9) -- plus the
 * destructor at 16/17; every other slot is inherited and not redeclared here.
 * No OnPendingDestroy override, unlike InvisiblePole/CameraTag/VirtualDoor.
 */
struct WingFeather : dActor_c {
    u8 pad_0d0[0x4];
    Model mModel;                 /* 0x0d4 */
    dCcAc_c mdCcAc_c;              /* 0x124 */
    dBgCh_Actr mWithMeshClsn;      /* 0x158 */
    ShadowModel mShadowModel;      /* 0x314 */
    u8  pad_33c[0x3c];
    s32 unk_378;            /* 0x378 */
    u16 unk_37c;            /* 0x37c */
    u8  pad_37e[0x2];
    u32 unk_380;            /* 0x380 */
    u8  unk_384;            /* 0x384 */

    virtual ~WingFeather();

    virtual s32 InitResources();        /* slot  0 */
    virtual s32 CleanupResources();     /* slot  3 */
    virtual s32 Behavior();             /* slot  6 */
    virtual s32 Render();               /* slot  9 */
};

typedef char WingFeather_size_must_be_0x388[sizeof(WingFeather) == 0x388 ? 1 : -1];

#endif
