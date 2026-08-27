//cpp
/* Canonical production translation unit -- reconstructed with tools/tubuild.py.
 * ov029/RotatingPlatformWdw  (5 function(s))
 *
 * The default stock build compiles this file once, derives its 5
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
 *   [0] 0x02112080  src/game/actors/RotatingPlatformWdw/_ZN19RotatingPlatformWdwD1Ev.cpp
 *   [1] 0x021120d0  src/_ZN19RotatingPlatformWdwD0Ev.c
 *   [2] 0x02112134  src/_ZN19RotatingPlatformWdw16CleanupResourcesEv.c
 *   [3] 0x02112148  src/_ZN19RotatingPlatformWdw13InitResourcesEv.c
 *   [4] 0x02112168  src/game/actors/RotatingPlatformWdw/RotatingPlatformWdw_Spawn.c
 */

/* TUBUILD WARNING -- #pragma directive(s) were present in the legacy
 * sources of this TU and were NOT carried into this file automatically.
 * Per notes/translation-unit-reconstruction-plan.md section 10, a pragma
 * that was FUNCTION-scoped in its own one-function file can become
 * TU-scoped once merged and silently change codegen for the OTHER
 * functions here. Decide by hand whether/where each one still applies:
 *   _ZN19RotatingPlatformWdw16CleanupResourcesEv: #pragma long_calls on
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "RotatingPlatformWdw.h"
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'Arg' */
struct Arg { void *m[3]; };

extern "C" {
extern int _ZTV19RotatingPlatformWdw[];
extern int _ZTV16daObjKaitendai_c[];
extern int _ZTV10dBgActor_c[];
extern void *data_020a0eac;
extern int func_ov002_020b66a8(void *thisp, void *data);
extern char data_ov029_02113fd4[];
extern int func_ov002_020b676c(unsigned char *self, struct Arg *a, short arg2);
extern short data_ov029_02113fc4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- RotatingPlatformWdw_Spawn, 0x02112168, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol RotatingPlatformWdw_Spawn
/* recovered: vtable identified, declarations from a shared header */
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16daObjKaitendai_c */
extern "C" int *RotatingPlatformWdw_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjKaitendai_c;
        p[0] = (int)&_ZTV19RotatingPlatformWdw[2]; /* +8: this TU defines the vtable */
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN19RotatingPlatformWdw13InitResourcesEv, 0x02112148, size 0x20 */
/* -------------------------------------------------------------------------- */
extern "C" int _ZN19RotatingPlatformWdw13InitResourcesEv(unsigned char *self)
{
    return func_ov002_020b676c(self, (struct Arg *)&data_ov029_02113fd4, data_ov029_02113fc4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN19RotatingPlatformWdw16CleanupResourcesEv, 0x02112134, size 0x14 */
/* -------------------------------------------------------------------------- */
// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
extern "C" int _ZN19RotatingPlatformWdw16CleanupResourcesEv(void *thisp)
{
    return func_ov002_020b66a8(thisp, data_ov029_02113fd4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN19RotatingPlatformWdwD0Ev, 0x021120d0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19RotatingPlatformWdwD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV19RotatingPlatformWdw, VT1 = _ZTV16daObjKaitendai_c */
/* (no separate definition: the single ~RotatingPlatformWdw() below emits the D0 and D1
 * variants together -- a hand-mangled D0 next to a real destructor is the
 * known mwccarm ICE, ELFgen.c:483.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN19RotatingPlatformWdwD1Ev, 0x02112080, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19RotatingPlatformWdwD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, and the middle one is the finding. `struct RotatingPlatformWdw :
 * daObjKaitendai_c : dBgActor_c` emits its own vptr, then daObjKaitendai_c's --
 * inlined, because that destructor is defined in its class body -- then dBgActor_c's,
 * then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. Nothing in the chain
 * adds a member with a destructor, so the body is empty.
 */
RotatingPlatformWdw::~RotatingPlatformWdw()
{
}
