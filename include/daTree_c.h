/* Reconciled from matched-function, vtable, RTTI, and constructor evidence.
 * class daTree_c: 7 matched methods plus its matched spawn function.
 * Offsets/widths are observed, not guessed. Field NAMES are placeholders -
 * renaming cannot change codegen.
 *
 * Real inheritance (was a flat, non-inheriting struct re-declaring
 * fBase_c's mParam and dActor_c's mPosX/mPosY/mPosZ inline). parent:
 * dActor_c, parent_slots: 31, own overrides at slots 0/3/6/9/12
 * (InitResources/CleanupResources/Behavior/Render/OnPendingDestroy) plus
 * the destructor pair at 16/17.
 *
 * InitResources, CleanupResources, Render and OnPendingDestroy are real
 * compiler-spelled daTree_c methods in src/actors/d_a_tree.cpp; the destructor
 * pair is compiler-generated from the inline definition below. Behavior stays a
 * hand-written free function under its own mangled ROM name, and daTree_c_classInit
 * under its unmangled one, because neither body references a named daTree_c
 * member. InitResources also keeps one low-level extern-C declaration for
 * dCcPos_c::Init: its ROM name carries by-value Fix12 parameters whose honest
 * C++ declaration changes the call ABI. Both exceptions are about a callee
 * signature, not about daTree_c method ownership.
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp called it `Tree`, a coined
 * name. MEASURED in extracted/overlays/overlay_0002.bin (ov002 base 0x020ad660,
 * the `.text start:` on line 1 of config/arm9/overlays/ov002/delinks.txt): the
 * vtable object's preamble at 0x0210abf8 is [offset-to-top 0, 0x0210abd0], and
 * _ZTI8daTree_c at 0x0210abd0 reads [0x0209a764, 0x0210abc4, 0x0208e390] --
 * _ZTVN3abi20__si_class_type_infoE (config/arm9/symbols.txt), a typeinfo name
 * at 0x0210abc4 whose bytes are the string "8daTree_c", and _ZTI8dActor_c, so
 * the cartridge states the direct base too. config/arm9/overlays/ov002/
 * symbols.txt already carried _ZTS8daTree_c and _ZTI8daTree_c at those
 * addresses while the class was still spelled `Tree`; under the coined name the
 * compiler emitted _ZTS4Tree/_ZTI4Tree, which reached no symbol home and could
 * not be word-compared. With the names agreed, src/actors/d_a_tree.cpp declares
 * all three records as `deadstrip-data` compiler_only_output and romdata_check
 * compares them against the ROM.
 */
#ifndef DATREE_C_H
#define DATREE_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daTree_c -- daTree_c_classInit (was Tree_Spawn),
 *       g_profile_TREE (was Tree_SpawnInfo)
 */
#include "types.h"
#include "dActor_c.h"
#include "Model.h"

struct daTree_c : dActor_c {
    u8  pad_0d0[0x4];
    /* Model[5] array, named by _ZN5ModelD1Ev/_ZN5ModelC1Ev at +0xd4 via
       __destroy_arr/func_020733a8 (a relocation the ROM build checks) --
       five Models, one per compass-ish direction slot indexed by
       InitResources/Render. Fills the class to its exact trailing size,
       0xd0 (dActor_c) + 0x4 (pad) + 5*0x50 (Model) = 0x264. */
    Model mModel[5];                  /* 0x0d4 */

    /* Slots 16 (D1) and 17 (D0). DEFINED INLINE ON PURPOSE. Written
     * out-of-line in the TU, mwccarm emits D0 before D1 -- the reverse of the
     * cartridge's order -- which objisolate refuses for the whole translation
     * unit, and it emits a third D2 body with no ROM home. Defined here the
     * compiler produces the retail D1/D0 pair in ROM order and no D2, and
     * src/actors/d_a_tree.cpp still emits the complete _ZTV/_ZTI/_ZTS group,
     * which that entry licenses record by record against the ROM. The body is
     * genuinely empty: mModel[5] is destroyed by the compiler's own
     * __destroy_arr call, which is exactly what the ROM's D1 does. */
    virtual ~daTree_c() {}
    virtual s32 InitResources();       /* slot  0 */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 Behavior();            /* slot  6 */
    virtual s32 Render();              /* slot  9 */
    virtual void OnPendingDestroy();   /* slot 12 -- empty body in the ROM */
};

typedef char daTree_c_size_must_be_0x264[sizeof(daTree_c) == 0x264 ? 1 : -1];

#endif
