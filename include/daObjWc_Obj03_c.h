#ifndef DAOBJWC_OBJ03_C_H
#define DAOBJWC_OBJ03_C_H

#include "dActor_c.h"
#include "Model.h"
#include "dCcAc_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* daObjWc_Obj03_c -- Wet-Dry World's water diamond (profile WATER_DIAMOND,
 * actor 97, ov029). The name is the cartridge's own RTTI: the typeinfo string
 * at 0x02113d7c reads daObjWc_Obj03_c and every method mangles against that
 * spelling. The project's earlier coined name, WaterDiamond, survives only in
 * the rename ledger.
 *
 * The factory allocates 0x160 bytes. dActor_c ends at 0xd0; Model sits at
 * 0x0d4 and dCcAc_c at 0x124, the same gap dBgActor_c uses, independently
 * proven by both destructor variants destroying those members in reverse
 * order before chaining to dActor_c.
 *
 * The 31-slot ROM vtable differs from dActor_c only in slots 0, 3, 6, 9, 16
 * and 17, exactly the virtual declarations below. The three non-virtual
 * helpers form the rest of the same high-confidence ten-function TU and are
 * called directly by Behavior.
 *
 * The destructor is declared LAST and INLINE on purpose. Class instantiation
 * via the factory's `new` emits the retail D1/D0 pair in cartridge order
 * (D1 at 0x02111728, then D0 at 0x02111760) without a separate leaf D2 body;
 * out of line mwccarm emits D0 ahead of D1 and adds the D2 the ROM never
 * carried.
 */
struct daObjWc_Obj03_c : dActor_c {
    u8      pad_0d0[0x4];
    Model   mModel;             /* 0x0d4 */
    dCcAc_c mCylinder;          /* 0x0124 */
    u32     mWaterID;           /* 0x0158 -- unique ID of actor 0x65 */
    u8      mWaterParam;        /* 0x015c -- selects the matching WDW water */
    u8      mActive;            /* 0x015d -- water movement in progress */
    u8      pad_15e[0x2];

    virtual s32 InitResources();              /* slot  0 */
    virtual s32 CleanupResources();           /* slot  3 */
    virtual s32 Behavior();                   /* slot  6 */
    virtual s32 Render();                     /* slot  9 */

    void CheckClsnWithPlayer();
    void SetWaterID();
    void UpdateModelTransform();

    /* Leaf size_t operator new. A plain `new daObjWc_Obj03_c` without this
       relocates to the global `_Znwm`; this routes the factory through
       fBase_c::operator new, the call this TU's classInit actually makes. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    virtual ~daObjWc_Obj03_c() {}             /* slots 16 (D1), 17 (D0) */
};

typedef char daObjWc_Obj03_c_size_must_be_0x160[
    sizeof(daObjWc_Obj03_c) == 0x160 ? 1 : -1];

#endif
