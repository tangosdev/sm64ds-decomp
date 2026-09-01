//cpp
/* Genuine production translation unit for ov045/daObjKm2_Fall_Block_c
 * (5 function(s)), enrolled as one `complete` delinks span.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py):
 * daObjKm2_Fall_Block_c -> d_a_obj_km2_fall_block.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources, all now deleted (ROM
 * address order):
 *   [0] 0x02111d48  src/_ZN21daObjKm2_Fall_Block_cD1Ev.cpp
 *   [1] 0x02111d98  src/_ZN21daObjKm2_Fall_Block_cD0Ev.cpp
 *   [2] 0x02111dfc  src/_ZN21daObjKm2_Fall_Block_c16CleanupResourcesEv.cpp
 *   [3] 0x02111e10  src/_ZN21daObjKm2_Fall_Block_c13InitResourcesEv.cpp
 *   [4] 0x02111e24  src/daObjKm2_Fall_Block_c_Spawn.c
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `FallBlockBfs`, a coined name; the measurement that settles it is at the top
 * of include/daObjKm2_Fall_Block_c.h. That rename is what makes this promotion
 * possible at all: a coined class reaches no symbol home, so its _ZTI/_ZTS/_ZTV
 * records can never be word-compared against the cartridge and its key-function
 * TU cannot declare a verifiable compiler_only_output (tools/tubuild.py
 * apply_compiler_only_policy).
 *
 * The shadow this replaces (a `.c` file under src_tu/actors/) spelled all five
 * bodies as extern-C free functions over `void *self`, and its factory stored
 * the leaf vtable TWICE where the ROM stores the base's and then the leaf's.
 * The bodies below are the recovered C++ ones from the deleted per-function
 * sources, not that rewrite.
 */

#include "daObjKm2_Fall_Block_c.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"

extern "C" {
extern int _ZTV21daObjKm2_Fall_Block_c[];
extern int _ZTV16daObjFallBlock_c[];
int func_ov098_0213a794(void *self, void *data);
int func_ov098_0213a2cc(void *self, void *data);
extern int data_ov045_021130ac[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjKm2_Fall_Block_c_Spawn, 0x02111e24, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol daObjKm2_Fall_Block_c_Spawn
/* resolved: VT0 = _ZTV16daObjFallBlock_c, VT1 = _ZTV21daObjKm2_Fall_Block_c.
 *
 * The natural C++ spelling `return new daObjKm2_Fall_Block_c` reproduces the
 * text, but relocates the allocation call to the unavailable global `_Znwm`
 * rather than the cartridge's fBase_c::operator new, so the measured C
 * allocator veneer is kept. 844 = 0x34c is the size the class header asserts.
 *
 * TWO vptr stores, base then leaf, because the base sub-object is constructed
 * before the complete object -- not one store repeated. What DID change with
 * the promotion is the second one: this TU now DEFINES the leaf vtable, and
 * mwccarm's `_ZTV21daObjKm2_Fall_Block_c` addresses the object's start -- the
 * two-word [offset-to-top, typeinfo] preamble at 0x021130ec -- while
 * symbols.txt's symbol at 0x021130f4 is already the slot array. Hence `&[2]`.
 * _ZTV16daObjFallBlock_c is not defined here (ov098 owns it), so that one
 * resolves to symbols.txt's address and takes no bias. */
extern "C" int *daObjKm2_Fall_Block_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(844);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjFallBlock_c;
        p[0] = (int)&_ZTV21daObjKm2_Fall_Block_c[2]; /* +8: this TU defines it */
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN21daObjKm2_Fall_Block_c13InitResourcesEv, 0x02111e10,  */
/* size 0x14                                                                  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm2_Fall_Block_c13InitResourcesEv
/* daObjKm2_Fall_Block_c::InitResources -- vtable slot 0, ov045 0x02111e10.
 *
 * Real member function. Forwards `this` and the class's shared parameter block
 * at ov045 0x021130ac to the generic implementation the whole fall-block family
 * shares, which lives in ov098 and is still under its func_ov098_ name. */
s32 daObjKm2_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(this, data_ov045_021130ac);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN21daObjKm2_Fall_Block_c16CleanupResourcesEv,           */
/* 0x02111dfc, size 0x14                                                      */
/* -------------------------------------------------------------------------- */
// @symbol _ZN21daObjKm2_Fall_Block_c16CleanupResourcesEv
/* daObjKm2_Fall_Block_c::CleanupResources -- vtable slot 3, ov045 0x02111dfc,
 * and THE KEY FUNCTION of this class: the destructor is inline in the header,
 * so the first declared non-inline virtual is what makes this TU emit the
 * _ZTV/_ZTI/_ZTS group. Same shape as InitResources against the teardown half
 * of the shared implementation. */
s32 daObjKm2_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, data_ov045_021130ac);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN21daObjKm2_Fall_Block_cD1Ev 0x02111d48 size     */
/* 0x50 and _ZN21daObjKm2_Fall_Block_cD0Ev 0x02111d98 size 0x64 -- are NOT    */
/* written here.                                                              */
/*                                                                            */
/* The destructor is defined INLINE in include/daObjKm2_Fall_Block_c.h.       */
/* Written out-of-line here the real destructor makes mwccarm emit D0 BEFORE   */
/* D1, the reverse of the cartridge's order, which objisolate refuses for the  */
/* whole translation unit, and it emits a third D2 body with no ROM home. The  */
/* inline definition gives the retail D1/D0 pair in ROM order and no D2, while */
/* CleanupResources -- declared out-of-line above and first in the class body  */
/* -- keeps this TU as the class's key-function TU, so it still owns the       */
/* complete _ZTV/_ZTI/_ZTS group declared in this entry's                      */
/* compiler_only_output.                                                       */
/*                                                                            */
/* THREE vptr stores come out of that one empty body, and the middle one is    */
/* the finding: the class emits its own vptr, then daObjFallBlock_c's --       */
/* inlined, because that destructor is defined in its class body -- then       */
/* dBgActor_c's, then dBgActor_c's dBgW_KcMbg and Model sub-objects and        */
/* dActor_c. A one-level chain emits two. Nothing in the chain adds a member   */
/* with a destructor, so the body itself is genuinely empty. D0 is that plus   */
/* the inherited inline `operator delete`; slot 17 is the deleting variant.    */
/* -------------------------------------------------------------------------- */
