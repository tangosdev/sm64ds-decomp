#ifndef DAOBJWLPOLELIFT_C_H
#define DAOBJWLPOLELIFT_C_H
#include "dActor_c.h"
#include "Model.h"
#include "dCcAcPos_c.h"
#include "PathPtr.h"
#include "ShadowModel.h"

/* Pole-mounted lift platform in ov026 (Wet-Dry World's "Wl" prefix): rides a
 * path of nodes, oscillating back and forth (or looping) at a fixed speed,
 * dropping a shadow onto whatever ground a raycast finds beneath it.
 *
 * BASE. dActor_c, confirmed by the RTTI edge (daObjWlPolelift_c
 * ov026:0x02113a90 -> dActor_c arm9:0x0208e390) and independently by the
 * vtable: data_ov026_02113ae0 (31 slots, matching dActor_c's own 31)
 * overrides exactly six -- 0, 3, 6, 9, 16, 17 -- and every other slot is
 * still whatever dActor_c's own table holds there.
 *
 * CONSTRUCTION. The reconstructed class initializer
 * daObjWlPolelift_c_classInit (historical placeholder func_ov026_02111678): it opens
 * with `_ZN7fBase_cnwEj(484)`, i.e. fBase_c::operator new(0x1e4). SIZE
 * 0x1e4 is read directly off the allocator call. The same constructor
 * placement-constructs four sub-objects after installing the vtable chain:
 *   Model                       at 0x0d4 (sizeof 0x50, ends 0x124)
 *   dCcAcPos_c   at 0x124 (sizeof 0x40, ends 0x164)
 *   PathPtr                     at 0x164 (sizeof 0x08, ends 0x16c)
 *   ShadowModel                 at 0x188 (sizeof 0x28, ends 0x1b0)
 * dActor_c's own fields run to 0xd0 (dActor_c.h); the Model sub-object
 * starts 4 bytes later at 0xd4.
 *
 * The destructor (func_ov026_021111a0 / _021111e0) tears down the same
 * three sub-objects that have non-trivial destructors -- ShadowModel,
 * dCcAcPos_c, Model -- in reverse-ish order (PathPtr has no
 * destructor call at all, consistent with it being POD).
 *
 * MEMBERS BETWEEN THE SUB-OBJECTS, read from InitResources/Behavior/Render
 * (func_ov026_02111598/_02111330/_02111308) -- everything else in 0xd0..0x1e4
 * not covered by a sub-object above is left as opaque padding, pending a
 * members-focused pass:
 *   0x16c  s32  numNodes    -- PathPtr::NumNodes(), cached once by InitResources
 *   0x170  s32  curNodeIdx  -- current path-node index; Behavior's state
 *   0x180  s32  moveSpeed   -- +/-1 step applied to curNodeIdx each advance
 *   0x184  s16  bounceTimer -- DecIfAbove0_Short pauses movement while nonzero;
 *                              set to 0x3c when a non-looping path end is hit
 *   0x1b0  Matrix4x3 dropShadowMtx -- copy of the model's own matrix (0xf0),
 *                              fed to dActor_c::DropShadowScaleXYZ (0x30 bytes,
 *                              ends 0x1e0)
 *   0x1e0  s32  groundY     -- dBgCh_Gnd hit height beneath the lift;
 *                              last field, 0x1e0 + sizeof(s32) == 0x1e4 == SIZE
 * Field-span and allocator witnesses agree exactly on 0x1e4.
 *
 * VTABLE ORDER follows dActor_c's (unchanged slot indices; daObjWlPolelift_c
 * adds no new virtual). The destructor pair is at slots 16/17, the
 * fBase_c/actor-family convention, matching every other actor and scene
 * class recovered so far -- not the Fader family's 0/1.
 */
struct daObjWlPolelift_c : dActor_c {
    u8   pad_0d0[0x4];              /* 0x0d0 -- unobserved */
    Model model;                     /* 0x0d4 */
    dCcAcPos_c clsn;  /* 0x124 */
    PathPtr path;                    /* 0x164 */
    s32  numNodes;                   /* 0x16c */
    s32  curNodeIdx;                 /* 0x170 */
    u8   pad_174[0xc];               /* 0x174 -- unobserved */
    s32  moveSpeed;                  /* 0x180 */
    s16  bounceTimer;                /* 0x184 */
    u8   pad_186[0x2];               /* 0x186 -- unobserved */
    ShadowModel shadow;              /* 0x188 */
    struct Matrix4x3 dropShadowMtx;  /* 0x1b0 */
    s32  groundY;                    /* 0x1e0 */

    /* Inline, and declared first. Out of line, mwccarm emits the D2/D1/D0 triple
       in D0-before-D1 order, and this class's retail layout puts D1 (0x021111a0)
       BELOW D0 (0x021111e0) -- objisolate then refuses the whole TU because its
       licensed functions are not emitted in ROM address order. The inline body
       emits only the retail D1/D0 pair, in retail order. */
    virtual ~daObjWlPolelift_c() {}                      /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dActor_c/_ZTV7fBase_c order. --- */
    virtual s32 InitResources();                         /* slot  0 */
    virtual s32 CleanupResources();                        /* slot  3 */
    virtual s32 Behavior();                                 /* slot  6 */
    virtual s32 Render();                                   /* slot  9 */

};

typedef char daObjWlPolelift_c_size_must_be_0x1e4[
    sizeof(daObjWlPolelift_c) == 0x1e4 ? 1 : -1];

#endif
