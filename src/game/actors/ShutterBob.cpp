//cpp
/* Canonical production translation unit -- reconstructed with tools/tubuild.py.
 * ov014/ShutterBob  (6 function(s))
 *
 * The default stock build compiles this file once, derives its 6
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
 *   [0] 0x021111a0  src/game/actors/ShutterBob/_ZN10ShutterBobD1Ev.cpp
 *   [1] 0x021111f0  src/_ZN10ShutterBobD0Ev.c
 *   [2] 0x02111254  src/game/actors/ShutterBob/_ZN10ShutterBob16CleanupResourcesEv.cpp
 *   [3] 0x02111268  src/game/actors/ShutterBob/_ZN10ShutterBob8BehaviorEv.cpp
 *   [4] 0x02111294  src/game/actors/ShutterBob/_ZN10ShutterBob13InitResourcesEv.cpp
 *   [5] 0x021112cc  src/game/actors/ShutterBob/ShutterBob_Spawn.c
 */

/* TUBUILD WARNING -- #pragma directive(s) were present in the legacy
 * sources of this TU and were NOT carried into this file automatically.
 * Per notes/translation-unit-reconstruction-plan.md section 10, a pragma
 * that was FUNCTION-scoped in its own one-function file can become
 * TU-scoped once merged and silently change codegen for the OTHER
 * functions here. Decide by hand whether/where each one still applies:
 *   _ZN10ShutterBob16CleanupResourcesEv: #pragma long_calls on
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "ShutterBob.h"
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
#include "dBgW.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

extern "C" {
extern int _ZTV10ShutterBob[];
extern int _ZTV13daObjSwdoor_c[];
extern int _ZTV10dBgActor_c[];
extern void *data_020a0eac;
extern int func_ov002_020bad10(void *c, void **f);
extern int data_ov014_021145c4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- ShutterBob_Spawn, 0x021112cc, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol ShutterBob_Spawn
/* recovered: vtable identified, declarations from a shared header */
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjSwdoor_c */
extern "C" int *ShutterBob_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(804);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13daObjSwdoor_c;
        p[0] = (int)&_ZTV10ShutterBob[2]; /* +8: this TU defines the vtable */
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN10ShutterBob13InitResourcesEv, 0x02111294, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ShutterBob13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * The collider used to be reached through a `u8 mMovingMeshCollider` marker this
 * class declared itself. It is dBgActor_c's, twice removed, and dBgActor_c calls it
 * mMeshCollider; the byte at that offset has not moved.
 *
 * THE TWO LOCAL SHADOW CLASSES ARE GONE, and they had to go rather than be
 * renamed. This file used to carry `class dActor_c {};` and
 * `class dBgW { void Enable(dActor_c *); };`. ShutterBob.h now reaches
 * the real dActor_c and the real dBgW through dBgActor_c, and a second
 * definition of either name in the same translation unit makes mwccarm 2004/b56
 * die with an internal compiler error rather than a diagnostic. Renaming them
 * compiles and byte-matches and is still wrong: the call mangles to
 * _ZN10ClsnEnable6EnableEP9ClsnOwner, which nothing defines, so the relocation
 * lands nowhere. The byte gate cannot see that -- it wildcards relocation slots --
 * and tools/check_references.py reported it as newly unresolvable. Using the real
 * declarations mangles to _ZN4dBgW6EnableEP8dActor_c, which resolves.
 */
int ShutterBob::InitResources()
{
    int r4 = func_ov002_020bad10(((char *)this), (void **)&data_ov014_021145c4);
    ((dBgW *)((char *)&mMeshCollider))->Enable((dActor_c *)((char *)this));
    return r4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN10ShutterBob8BehaviorEv, 0x02111268, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ShutterBob8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* The local `class dBgActor_c { void UpdateClsnPosAndRot(); };` this file used to
   carry is gone: ShutterBob.h now reaches the real one through daObjSwdoor_c, and
   two definitions of the name in one translation unit do not compile. The call is
   the same call -- dBgActor_c::UpdateClsnPosAndRot is non-virtual, so it is a direct
   `bl` either way. */
int ShutterBob::Behavior()
{
int r4 = func_ov002_020bac18();
UpdateClsnPosAndRot();
return r4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN10ShutterBob16CleanupResourcesEv, 0x02111254, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ShutterBob16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
// (tubuild create dropped the legacy file's directive; restored here, bracketed
// with `off` so it cannot leak into the members below in source order.)
#pragma long_calls on
int ShutterBob::CleanupResources()
{
    return func_ov002_020baba8(((void *)this), (void *)&data_ov014_021145c4);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN10ShutterBobD0Ev, 0x021111f0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ShutterBobD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV10ShutterBob; VT1 = _ZTV13daObjSwdoor_c */
/* (no separate definition: the single ~ShutterBob() below emits the D0 and D1
 * variants together -- keeping the hand-mangled body alongside a real destructor
 * is the known mwccarm ICE, ELFgen.c:483.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN10ShutterBobD1Ev, 0x021111a0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ShutterBobD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, and the middle one is the finding. `struct ShutterBob :
 * daObjSwdoor_c : dBgActor_c` emits its own vptr, then daObjSwdoor_c's --
 * inlined, because that destructor is defined in its class body -- then dBgActor_c's,
 * then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. Nothing in the chain
 * adds a member with a destructor, so the body is empty.
 */
ShutterBob::~ShutterBob()
{
}
