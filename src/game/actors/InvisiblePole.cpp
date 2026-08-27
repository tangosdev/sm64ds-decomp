//cpp
/* Canonical production translation unit -- reconstructed with tools/tubuild.py.
 * ov002/InvisiblePole  (8 function(s))
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
 *   [0] 0x020b05d0  src/game/actors/InvisiblePole/_ZN13InvisiblePoleD1Ev.cpp
 *   [1] 0x020b0600  src/_ZN13InvisiblePoleD0Ev.c
 *   [2] 0x020b0644  src/game/actors/InvisiblePole/_ZN13InvisiblePole16CleanupResourcesEv.cpp
 *   [3] 0x020b064c  src/game/actors/InvisiblePole/_ZN13InvisiblePole16OnPendingDestroyEv.cpp
 *   [4] 0x020b0650  src/game/actors/InvisiblePole/_ZN13InvisiblePole6RenderEv.cpp
 *   [5] 0x020b0658  src/game/actors/InvisiblePole/_ZN13InvisiblePole8BehaviorEv.cpp
 *   [6] 0x020b067c  src/game/actors/InvisiblePole/_ZN13InvisiblePole13InitResourcesEv.cpp
 *   [7] 0x020b0710  src/game/actors/InvisiblePole/InvisiblePole_Spawn.c
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "InvisiblePole.h"
#include "decl_Actor.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
#include "decl_ActorBase.h"

extern "C" {
extern int _ZTV13InvisiblePole[];
extern void *data_020a0eac;
/* Both carry Fix12<int> parameters, so their definitions stay mangled free
functions -- see include/ShadowModel.h. A call is unaffected. */
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(dActor_c *self, int a, int b, int c, int d);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
dCcAc_c *self, dActor_c *actor, int radius, int height, u32 e, u32 f);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- InvisiblePole_Spawn, 0x020b0710, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol InvisiblePole_Spawn
/* recovered: vtable identified, declarations from a shared header */
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13InvisiblePole */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *InvisiblePole_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(264);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)(_ZTV13InvisiblePole + 2);
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN13InvisiblePole13InitResourcesEv, 0x020b067c, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13InvisiblePole13InitResourcesEv
/* Vtable slot 0. param1's low byte carries the pole's height in units of ten, and
 * bit 0x100 selects a second collision behaviour. */
s32 InvisiblePole::InitResources()
{
    int height = (((param1 & 0xff) - 0xa) * 0xa) << 0xc;
    int half;
    if (height <= 0) height = 0x1000;
    half = height >> 1;
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, half, half, half + 0x640000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mClsn, this, 0x35555, height,
        (param1 & 0x100) ? 0x280000c : 0x80000c, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN13InvisiblePole8BehaviorEv, 0x020b0658, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13InvisiblePole8BehaviorEv
/* Vtable slot 6. Rebuild the climbing cylinder every frame. */
s32 InvisiblePole::Behavior()
{
    mClsn.Clear();
    mClsn.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN13InvisiblePole6RenderEv, 0x020b0650, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13InvisiblePole6RenderEv
/* Vtable slot 9. The pole is invisible, so rendering it is a no-op that still
 * reports success. */
s32 InvisiblePole::Render()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13InvisiblePole16OnPendingDestroyEv, 0x020b064c, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13InvisiblePole16OnPendingDestroyEv
/* Vtable slot 12. Four bytes, a bare `bx lr`: the override exists only to stop
 * dActor_c's implementation running. */
void InvisiblePole::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN13InvisiblePole16CleanupResourcesEv, 0x020b0644, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13InvisiblePole16CleanupResourcesEv
/* Vtable slot 3. Nothing to give back -- the pole owns no file. */
s32 InvisiblePole::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN13InvisiblePoleD0Ev, 0x020b0600, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13InvisiblePoleD0Ev
/* No hand-written body: keeping this alongside the real out-of-line D1 below
   in the same TU is an mwcc internal compiler error (ELFgen.c:483) -- see
   memory. The real `InvisiblePole::~InvisiblePole(){}` (D1, below) makes the
   compiler emit BOTH D1 and D0 itself; D0 still MATCHes the ROM. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN13InvisiblePoleD1Ev, 0x020b05d0, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13InvisiblePoleD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Vtable slot 16. One vtable store and two teardowns, none of them written here: the
 * single member in reverse declaration order, then dActor_c. This body IS the evidence
 * for the header above it -- dCcAc_c is 0x34 and starts at 0xd4, which
 * closes exactly on the 0x108 the factory allocates.
 */
InvisiblePole::~InvisiblePole()
{
}
