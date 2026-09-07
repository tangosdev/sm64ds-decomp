#ifndef DAOBJWC_OBJ03_C_H
#define DAOBJWC_OBJ03_C_H

#include "dActor_c.h"
#include "Model.h"
#include "dCcAc_c.h"

/* daObjWc_Obj03_c is the name the cartridge's own RTTI gives this class, and it
 * is now the only name the tree uses for it. The typeinfo string at 0x02113d7c
 * reads daObjWc_Obj03_c and every method mangles against that spelling; the
 * project's earlier coined name, WaterDiamond, is gone from the sources and
 * survives only in the rename ledger and in generated snapshots.
 *
 * The factory allocates 0x160 bytes, constructs dActor_c, then constructs the
 * Model at 0x0d4 and dCcAc_c at 0x124. Both destructor variants destroy those
 * members in reverse order before chaining to dActor_c, independently proving
 * their ownership and the class extent.
 *
 * The 31-slot ROM vtable differs from dActor_c only in slots 0, 3, 6, 9, 16
 * and 17, exactly the virtual declarations below. The three non-virtual
 * helpers form the rest of the same high-confidence ten-function TU and are
 * called directly by Behavior.
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

    /* DECLARED LAST ON PURPOSE, after the other members. Nothing DEFINES this
       destructor as a C++ member -- D1 and D0 are carried in
       src/actors/d_a_obj_wc_obj03.cpp as `// @symbol` marked mangled bodies, for
       the emission-order reason that file's header gives -- so the class's
       vtable and RTTI have no key function to home them. With the declaration
       LAST, mwccarm still emits them as vague linkage into the TU that defines
       the class's members, and tools/romdata_check.py word-compares that
       emitted copy against the cartridge; with the declaration FIRST it emits
       no data at all and those ROM records go unverified by any source.
       Measured both ways on the sibling daObjWc_Mizu_c: first -> 0 data
       symbols, last -> 11. include/daObjWc_Mizu_c.h and
       include/daObjWc_Obj04_c.h have the same shape for the same reason. */
    virtual ~daObjWc_Obj03_c();               /* slots 16 (D1), 17 (D0) */
};

typedef char daObjWc_Obj03_c_size_must_be_0x160[
    sizeof(daObjWc_Obj03_c) == 0x160 ? 1 : -1];

#endif
