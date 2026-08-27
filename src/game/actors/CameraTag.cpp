//cpp
/* Canonical production translation unit -- reconstructed with tools/tubuild.py.
 * ov002/CameraTag  (8 function(s))
 *
 * The default stock build compiles this file once, derives its 8
 * exact text contributions, and substitutes them for retained one-function
 * comparison objects. The manifest partial-link proof and ordinary ROM build
 * both require the linked module and final ROM to remain byte-identical.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020b0748  src/game/camera/CameraTag/_ZN9CameraTagD1Ev.cpp
 *   [1] 0x020b076c  src/_ZN9CameraTagD0Ev.c
 *   [2] 0x020b07a4  src/game/camera/CameraTag/_ZN9CameraTag16CleanupResourcesEv.cpp
 *   [3] 0x020b07ac  src/game/camera/CameraTag/_ZN9CameraTag16OnPendingDestroyEv.cpp
 *   [4] 0x020b07b0  src/game/camera/CameraTag/_ZN9CameraTag6RenderEv.cpp
 *   [5] 0x020b07b8  src/game/camera/CameraTag/_ZN9CameraTag8BehaviorEv.cpp
 *   [6] 0x020b07c0  src/game/camera/CameraTag/_ZN9CameraTag13InitResourcesEv.cpp
 *   [7] 0x020b07c8  src/game/camera/CameraTag/CameraTag_Spawn.c
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "CameraTag.h"
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"

/* Reconciled against the real project headers: include/CameraTag.h now
 * declares `struct CameraTag : dActor_c` with a real virtual destructor and
 * the five fBase_c-slot overrides (see that header for the ROM evidence), so
 * the legacy shadow redeclarations of both classes are dropped rather than
 * carried forward -- keeping them alongside the #include above would
 * redefine 'struct CameraTag' in this one TU. */
extern "C" {
extern int _ZTV9CameraTag[];
extern void _ZN8dActor_cD2Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- CameraTag_Spawn, 0x020b07c8, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol CameraTag_Spawn
/* recovered: globals resolved, declarations from a shared header */
/* recovered: globals resolved */
/* resolved: VT = _ZTV9CameraTag */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *CameraTag_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(struct CameraTag));
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)(_ZTV9CameraTag + 2); }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN9CameraTag13InitResourcesEv, 0x020b07c0, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9CameraTag13InitResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. Nothing to load: the pole has no model and no animation, only
 * the dCcAc_c its spawner already constructed. Reporting success
 * without doing anything is the whole point -- see include/CameraTag.h.
 */
int CameraTag::InitResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN9CameraTag8BehaviorEv, 0x020b07b8, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9CameraTag8BehaviorEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. The pole never moves and never thinks; it is queried, not
 * ticked.
 */
int CameraTag::Behavior()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN9CameraTag6RenderEv, 0x020b07b0, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9CameraTag6RenderEv
/* recovered: shared header, real C++ method
 *
 * `return 1` -- which is what makes it INVISIBLE. There is no model to draw,
 * and the override exists so the base never tries.
 */
int CameraTag::Render()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN9CameraTag16OnPendingDestroyEv, 0x020b07ac, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9CameraTag16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * Empty in the ROM -- a single `bx lr`.
 */
void CameraTag::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN9CameraTag16CleanupResourcesEv, 0x020b07a4, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9CameraTag16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. InitResources claimed nothing, so there is nothing to release --
 * the pair stays balanced by both doing nothing.
 */
int CameraTag::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN9CameraTagD0Ev, 0x020b076c, size 0x38 */
/* -------------------------------------------------------------------------- */
/* NOT hand-written: a real, already out-of-line CameraTag::~CameraTag() (D1,
 * below) coexisting with a hand-written mangled-name D0 free function in the
 * SAME translation unit is an mwcc internal compiler error (ELFgen.c:483) --
 * confirmed on InvisiblePole (PR #1722), same real-D1 shape. Deleting the
 * hand-written body and letting the compiler emit D0 itself from the real
 * destructor + dActor_c's inline `operator delete` reproduces the ROM's bytes
 * byte-for-byte; see include/dActor_c.h's operator delete note. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN9CameraTagD1Ev, 0x020b0748, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9CameraTagD1Ev
CameraTag::~CameraTag()
{
}
