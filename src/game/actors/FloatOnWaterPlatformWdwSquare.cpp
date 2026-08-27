//cpp
/* Canonical production translation unit -- reconstructed with tools/tubuild.py.
 * ov029/FloatOnWaterPlatformWdwSquare  (4 function(s))
 *
 * The default stock build compiles this file once, derives its 4
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
 *   [0] 0x021111a0  src/game/actors/FloatOnWaterPlatformWdwSquare/_ZN29FloatOnWaterPlatformWdwSquareD1Ev.cpp
 *   [1] 0x021111f0  src/_ZN29FloatOnWaterPlatformWdwSquareD0Ev.c
 *   [2] 0x02111254  src/game/actors/FloatOnWaterPlatformWdwSquare/_ZN29FloatOnWaterPlatformWdwSquare13InitResourcesEv.cpp
 *   [3] 0x02111340  src/game/actors/FloatOnWaterPlatformWdwSquare/FloatOnWaterPlatformWdwSquare_Spawn.c
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "FloatOnWaterPlatformWdwSquare.h"
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

extern "C" {
extern int _ZTV29FloatOnWaterPlatformWdwSquare[];
extern int _ZTV17daObjFloatBoard_c[];
extern int _ZTV10dBgActor_c[];
extern void *data_020a0eac;
extern int func_ov002_020b5e58(char* c, char* d); /* decl_common's view */
extern int _ZN8dActor_c17GetWaterHeightWDWEv(void* c);
extern void _ZN9dBgCh_LinC1Ev(void*);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void*, void*, void*, void*);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void*);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(void*, void*);
extern void _ZN9dBgCh_LinD1Ev(void*);
extern int data_ov029_02113be8[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- FloatOnWaterPlatformWdwSquare_Spawn, 0x02111340, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol FloatOnWaterPlatformWdwSquare_Spawn
/* recovered: vtable identified, declarations from a shared header */
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjFloatBoard_c */
extern "C" int *FloatOnWaterPlatformWdwSquare_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(840);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV17daObjFloatBoard_c;
        p[0] = (int)&_ZTV29FloatOnWaterPlatformWdwSquare[2]; /* +8: this TU defines the vtable */
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN29FloatOnWaterPlatformWdwSquare13InitResourcesEv, 0x02111254, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol _ZN29FloatOnWaterPlatformWdwSquare13InitResourcesEv
/* FloatOnWaterPlatformWdwSquare::InitResources -- vtable slot 0, ov029
 * 0x02111254.
 *
 * Real member function: mWaterY is daObjFloatBoard_c's own named field
 * (include/daObjFloatBoard_c.h). func_ov002_020b5e58 is daObjFloatBoard_c's
 * shared file-load helper (out of this task's scope, kept under its
 * existing name -- same cross-overlay call FloatOnWaterPlatformJrb::
 * InitResources makes in ov016). dActor_c::GetWaterHeightWDW and the
 * dBgCh_Lin helpers are this class's own recovery, kept exactly as
 * found: dBgCh_Lin is a raw byte buffer (0x7c bytes matches its real
 * size but this task does not migrate it to a named type), and the
 * sppad[] staging array reproduces the ROM's own stack layout for the
 * Vector3-by-value SetObjAndLine call. mWaterY/mPosX/mPosY/mPosZ are read
 * by name where the header already provides them; 0x320 stays a raw
 * offset -- daObjFloatBoard_c.h documents it as UNOBSERVED padding, not a
 * field this class's own bytes confirm the name of. */
int FloatOnWaterPlatformWdwSquare::InitResources()
{
    char *c = (char *)this;
    int sppad[6]; /* a[3] + b[3] at low stack */
    int pos[3];
    char rl[0x7c];
    int wh;
    int x, y, z;

    if (func_ov002_020b5e58((char*)c, (char*)data_ov029_02113be8) != 0) {
        wh = _ZN8dActor_c17GetWaterHeightWDWEv(c);
        if (mPosY > wh) {
            _ZN9dBgCh_LinC1Ev(rl);
            x = mPosX;
            sppad[3] = x; /* b.x */
            y = mPosY;
            sppad[4] = y; /* b.y temp */
            z = mPosZ;
            sppad[1] = y; /* a.y temp */
            sppad[5] = z; /* b.z */
            sppad[0] = x; /* a.x */
            sppad[2] = z; /* a.z */
            sppad[1] = y + 0x14000;
            sppad[4] = wh;
            _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &sppad[0], &sppad[3], c);
            if (_ZN9dBgCh_Lin10DetectClsnEv(rl) == 0) {
                mPosY = wh;
            } else {
                _ZN9dBgCh_Lin10GetClsnPosEv(pos, rl);
                mPosY = pos[1];
            }
            _ZN9dBgCh_LinD1Ev(rl);
        }
        *(int*)(c + 0x320) = mPosX;
        mWaterY = mPosY;
        *(int*)(c + 0x328) = mPosZ;
        return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN29FloatOnWaterPlatformWdwSquareD0Ev, 0x021111f0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN29FloatOnWaterPlatformWdwSquareD0Ev
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
/* recovered: vtable identified, renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
/* (no separate definition: the single ~FloatOnWaterPlatformWdwSquare() below emits the D0 and D1
 * variants together -- a hand-mangled D0 next to a real destructor is the
 * known mwccarm ICE, ELFgen.c:483.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN29FloatOnWaterPlatformWdwSquareD1Ev, 0x021111a0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN29FloatOnWaterPlatformWdwSquareD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores, and the middle one is the finding. `struct FloatOnWaterPlatformWdwSquare :
 * daObjFloatBoard_c : dBgActor_c` emits its own vptr, then daObjFloatBoard_c's --
 * inlined, because that destructor is defined in its class body -- then dBgActor_c's,
 * then dBgActor_c's dBgW_KcMbg and Model, then dActor_c. Nothing in the chain
 * adds a member with a destructor, so the body is empty.
 */
FloatOnWaterPlatformWdwSquare::~FloatOnWaterPlatformWdwSquare()
{
}
