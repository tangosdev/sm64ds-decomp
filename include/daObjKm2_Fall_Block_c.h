#ifndef DAOBJKM2_FALL_BLOCK_C_H
#define DAOBJKM2_FALL_BLOCK_C_H

#include "types.h"

/* Bowser in the Fire Sea's falling block.
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `FallBlockBfs`, a coined name. MEASURED in
 * extracted/overlays/overlay_0045.bin (ov045 base 0x021111a0, the `.text
 * start:` on line 1 of config/arm9/overlays/ov045/delinks.txt): the vtable
 * object's preamble at 0x021130ec is [offset-to-top 0, 0x021130a0], and
 * _ZTI21daObjKm2_Fall_Block_c at 0x021130a0 reads [0x0209a764, 0x021130b8,
 * 0x02114874] -- _ZTVN3abi20__si_class_type_infoE (config/arm9/symbols.txt), a
 * typeinfo name at 0x021130b8 whose bytes are the string
 * "21daObjKm2_Fall_Block_c", and _ZTI16daObjFallBlock_c.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjFallBlock_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjFallBlock_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI21daObjKm2_Fall_Block_c  ov045 0x021130a0
 *   _ZTS21daObjKm2_Fall_Block_c  ov045 0x021130b8
 *   _ZTV21daObjKm2_Fall_Block_c  ov045 0x021130f4  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjFallBlock_c, ov015 0x02114874  -- CROSS-OVERLAY: the base's own
 *         _ZTI lives in ov015 and its out-of-line destructor in ov098, so
 *         nothing of the base is this module's to license.
 *
 * NO FIELDS OF ITS OWN: daObjKm2_Fall_Block_c_Spawn passes 844 = 0x34c, which daObjFallBlock_c
 * fills exactly. It overrides slots 0 and 3, which the base leaves null.
 */

#ifdef __cplusplus

#include "daObjFallBlock_c.h"

struct daObjKm2_Fall_Block_c : daObjFallBlock_c {
    /* --- vtable --- */
    /* INLINE IS LOAD-BEARING. Written out-of-line in the TU, mwccarm emits D0
       BEFORE D1 -- the reverse of the cartridge's order, which objisolate
       refuses for the whole translation unit -- plus a third D2 with no ROM
       home. Defined here it emits the retail D1/D0 pair in ROM order and no
       D2. Unlike daObjFallBlock_c's and dBgActor_c's, this body is inlined by
       nobody: the class has no descendants. */
    virtual ~daObjKm2_Fall_Block_c() {}     /* slots 16 (D1), 17 (D0) */

    /* THE KEY FUNCTION IS CleanupResources -- the first DECLARED non-inline
       virtual, not the first slot. Both of these override daObjFallBlock_c's
       pure `virtual s32 InitResources() = 0` / `= 0` pair, so they are already
       virtual and add no slot; the return type is spelt s32 to match that
       declaration exactly. Whichever TU defines the key function emits this
       class's _ZTV/_ZTI/_ZTS group -- src/actors/d_a_obj_km2_fall_block.cpp,
       which licenses all three as deadstrip-data so romdata_check word-compares
       them against the cartridge.

       notes/plan-cpp-conversion-queue.md sec 5 used to hold this pair up as the
       exemplar of declaring vtable-slot methods NON-virtual on purpose, to keep
       a per-function file from becoming the key-function TU and emitting _ZTV.
       That trap is now the goal: with one genuine TU there is exactly one place
       for the group to land, and the class stays unverifiable until it does. */
    s32 CleanupResources();            /* slot  3 */
    s32 InitResources();               /* slot  0 */
};

typedef char daObjKm2_Fall_Block_c_size_must_be_0x34c[sizeof(daObjKm2_Fall_Block_c) == 0x34c ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM2_FALL_BLOCK_C_H */
