#ifndef DAOBJWC_OBJ04_C_H
#define DAOBJWC_OBJ04_C_H

#include "types.h"

#ifdef __cplusplus
extern "C" void *_ZN7fBase_cnwEj(unsigned size);

#include "dBgActor_c.h"

/* Wet-Dry World's switch-triggered wooden plank (SWITCH_ACTIVATED_PLANK 98).
 *
 * RTTI ov029:0x0211415c names `15daObjWc_Obj04_c`; the type string at
 * 0x02114168 is that spelling, and the sole base is dBgActor_c
 * (ov002:0x021089ec). The vtable address point is ov029:0x021141a0.
 * overlay_actors maps ov029 profile 98 to SWITCH_ACTIVATED_PLANK; the
 * registry descriptor is g_profile_WC_OBJ04. Ugly RTTI names are final --
 * this is not a compatibility alias.
 *
 * SIZE 0x3a8, the factory's own literal (936). dBgActor_c fills 0x000..0x31f
 * (D1 restores _ZTV10dBgActor_c, destroys dBgW_KcMbg at 0x124 and Model at
 * 0x0d4, then chains to dActor_c). This class owns the extra Model at 0x320
 * -- D1 destroys it first, and the factory constructs it after the vptr
 * store -- plus the collider matrix and the four-byte state tail:
 *
 *   0x320  mModel2     -- BMD; Render calls Model::Render
 *   0x370  mClsnMat2   -- SetFile / Transform matrix; helpers write Y rot + pos
 *   0x3a0  mStateTimer -- blink counter while the plank is out
 *   0x3a2  mState      -- 0 hidden, 1 shown
 *   0x3a3  mVisible    -- Render gate; blinks with mStateTimer after 0x168
 *   0x3a4  mEventID    -- param1 & 0x1f, handed to Event::GetBit
 *
 * The destructor is declared LAST and INLINE on purpose. Class instantiation
 * via the factory's `new` emits the retail D1/D0 pair in cartridge order
 * (D1 at 0x02112630, then D0 at 0x0211267c) without a separate leaf D2 body;
 * out of line mwccarm emits D0 ahead of D1 and adds the D2 the ROM never
 * carried.
 */

struct daObjWc_Obj04_c : dBgActor_c {
    Model mModel2;            /* 0x320 */
    Matrix4x3 mClsnMat2;      /* 0x370 */
    u16 mStateTimer;          /* 0x3a0 */
    u8  mState;               /* 0x3a2 */
    u8  mVisible;             /* 0x3a3 */
    u8  mEventID;             /* 0x3a4 */

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. Virtualness is inherited. */
    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();

    /* Leaf size_t operator new. A plain `new daObjWc_Obj04_c` without this
       relocates to the global `_Znwm`; this routes the factory through
       fBase_c::operator new, the call this TU's classInit actually makes. */
    static void *operator new(unsigned long size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    virtual ~daObjWc_Obj04_c() {}     /* slots 16 (D1), 17 (D0) */
};

typedef char daObjWc_Obj04_c_size_must_be_0x3a8[
    sizeof(daObjWc_Obj04_c) == 0x3a8 ? 1 : -1];

#endif /* __cplusplus */

#endif
