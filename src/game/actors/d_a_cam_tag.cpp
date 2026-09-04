//cpp
/* Genuine production translation unit for ov002/daCamTag_c (8 function(s)),
 * enrolled as one `complete` delinks span.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py):
 * daCamTag_c -> d_a_cam_tag.
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `CameraTag`, a coined name. MEASURED in extracted/overlays/overlay_0002.bin
 * (ov002 base 0x020ad660): _ZTS10daCamTag_c at 0x02108508 is the byte string
 * "10daCamTag_c", and _ZTI10daCamTag_c at 0x021084fc reads [0x0209a764,
 * 0x02108508, 0x0208e390] -- _ZTVN3abi20__si_class_type_infoE
 * (config/arm9/symbols.txt), that same typeinfo name, and _ZTI8dActor_c, so
 * the ROM states the direct base too.
 * tools/class_rename.py performed the rename, and it is what makes this
 * promotion possible at all: a coined class reaches no symbol home, so its
 * _ZTI/_ZTS/_ZTV records can never be word-compared against the cartridge and
 * its key-function TU cannot declare a verifiable compiler_only_output
 * (tools/tubuild.py apply_compiler_only_policy).
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources, all now deleted (ROM
 * address order):
 *   [0] 0x020b0748  src/_ZN10daCamTag_cD1Ev.cpp
 *   [1] 0x020b076c  src/_ZN10daCamTag_cD0Ev.cpp
 *   [2] 0x020b07a4  src/_ZN10daCamTag_c16CleanupResourcesEv.cpp
 *   [3] 0x020b07ac  src/_ZN10daCamTag_c16OnPendingDestroyEv.cpp
 *   [4] 0x020b07b0  src/_ZN10daCamTag_c6RenderEv.cpp
 *   [5] 0x020b07b8  src/_ZN10daCamTag_c8BehaviorEv.cpp
 *   [6] 0x020b07c0  src/_ZN10daCamTag_c13InitResourcesEv.cpp
 *   [7] 0x020b07c8  src/daCamTag_c_Spawn.c
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. This order compiles and byte-matches as written. */
#include "daCamTag_c.h"
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"

/* Reconciled against the real project headers: include/daCamTag_c.h now
 * declares `struct daCamTag_c : dActor_c` with a real virtual destructor and
 * the five fBase_c-slot overrides (see that header for the ROM evidence), so
 * the legacy shadow redeclarations of both classes are dropped rather than
 * carried forward -- keeping them alongside the #include above would
 * redefine 'struct daCamTag_c' in this one TU. */
extern "C" {
extern int _ZTV10daCamTag_c[];
extern void _ZN8dActor_cD2Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daCamTag_c_Spawn, 0x020b07c8, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daCamTag_c_Spawn
/* recovered: globals resolved, declarations from a shared header */
/* recovered: globals resolved */
/* resolved: VT = _ZTV10daCamTag_c */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daCamTag_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(struct daCamTag_c));
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)(_ZTV10daCamTag_c + 2); }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN10daCamTag_c13InitResourcesEv, 0x020b07c0, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daCamTag_c13InitResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. Nothing to load: the pole has no model and no animation, only
 * the dCcAc_c its spawner already constructed. Reporting success
 * without doing anything is the whole point -- see include/daCamTag_c.h.
 */
int daCamTag_c::InitResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN10daCamTag_c8BehaviorEv, 0x020b07b8, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daCamTag_c8BehaviorEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. The pole never moves and never thinks; it is queried, not
 * ticked.
 */
int daCamTag_c::Behavior()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN10daCamTag_c6RenderEv, 0x020b07b0, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daCamTag_c6RenderEv
/* recovered: shared header, real C++ method
 *
 * `return 1` -- which is what makes it INVISIBLE. There is no model to draw,
 * and the override exists so the base never tries.
 */
int daCamTag_c::Render()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN10daCamTag_c16OnPendingDestroyEv, 0x020b07ac, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daCamTag_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * Empty in the ROM -- a single `bx lr`.
 */
void daCamTag_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN10daCamTag_c16CleanupResourcesEv, 0x020b07a4, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daCamTag_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. InitResources claimed nothing, so there is nothing to release --
 * the pair stays balanced by both doing nothing.
 */
int daCamTag_c::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN10daCamTag_cD1Ev 0x020b0748 size 0x24 and         */
/* _ZN10daCamTag_cD0Ev 0x020b076c size 0x38 -- are NOT written here.            */
/*                                                                             */
/* The destructor is defined INLINE in include/daCamTag_c.h. A hand-written     */
/* mangled-name D0 free function coexisting with a real out-of-line D1 in the   */
/* same TU is an mwcc internal compiler error (ELFgen.c:483), confirmed on      */
/* InvisiblePole in #1722; and written out-of-line the real destructor makes    */
/* mwcc emit D0 BEFORE D1, the reverse of the cartridge's order, which          */
/* objisolate refuses for the whole TU. The inline definition gives the retail  */
/* D1/D0 pair in ROM order and no D2 body, while InitResources -- the first     */
/* out-of-line virtual -- keeps this TU as the class's key-function TU, so it   */
/* still owns the complete _ZTV/_ZTI/_ZTS group declared in this entry's        */
/* compiler_only_output. D0 itself comes from that destructor plus dActor_c's   */
/* inline `operator delete`; see include/dActor_c.h's operator delete note.     */
/* -------------------------------------------------------------------------- */
