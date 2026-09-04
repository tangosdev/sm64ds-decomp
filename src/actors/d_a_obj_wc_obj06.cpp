//cpp
/* Genuine production translation unit for ov029/daObjWcObj06_c (4 function(s)),
 * enrolled as one `complete` delinks span.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py):
 * daObjWcObj06_c -> d_a_obj_wc_obj06.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources, all now deleted (ROM
 * address order):
 *   [0] 0x02111ea4  src/_ZN14daObjWcObj06_cD1Ev.cpp
 *   [1] 0x02111ef4  src/_ZN14daObjWcObj06_cD0Ev.cpp
 *   [2] 0x02111f58  src/_ZN14daObjWcObj06_c13InitResourcesEv.cpp
 *   [3] 0x02112044  src/daObjWcObj06_c_Spawn.c
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `FloatOnWaterPlatformWdwRectangle`, a coined name. MEASURED in
 * extracted/overlays/overlay_0029.bin (ov029 base 0x021111a0, the `.text
 * start:` on line 1 of config/arm9/overlays/ov029/delinks.txt):
 * _ZTS14daObjWcObj06_c at 0x02113f0c is the byte string "14daObjWcObj06_c",
 * and _ZTI14daObjWcObj06_c at 0x02113ef4 reads [0x0209a764, 0x02113f0c,
 * 0x02108fb4] -- _ZTVN3abi20__si_class_type_infoE (config/arm9/symbols.txt),
 * that same typeinfo name, and _ZTI17daObjFloatBoard_c, so the ROM states the
 * direct base too. tools/class_rename.py performed the rename, and it is what
 * makes this promotion possible at all: a coined class reaches no symbol home,
 * so its _ZTI/_ZTS/_ZTV records can never be word-compared against the
 * cartridge and its key-function TU cannot declare a verifiable
 * compiler_only_output (tools/tubuild.py apply_compiler_only_policy).
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjWcObj06_c.h"
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

extern "C" {
extern int _ZTV14daObjWcObj06_c[];
extern int _ZTV17daObjFloatBoard_c[];
extern int func_ov002_020b5e58(char* c, char* d); /* decl_common's view */
extern int _ZN8dActor_c17GetWaterHeightWDWEv(void* c);
extern void _ZN9dBgCh_LinC1Ev(void*);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void*, void*, void*, void*);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void*);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(void*, void*);
extern void _ZN9dBgCh_LinD1Ev(void*);
extern int data_ov029_02113f00[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- daObjWcObj06_c_classInit, 0x02112044, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol daObjWcObj06_c_classInit
/* recovered: vtable identified, declarations from a shared header */
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjFloatBoard_c */
/* Reconstructed source-style name: SM64DS proves daObjWcObj06_c through RTTI,
 * allocation size, vtable identity, and the WC_OBJ06 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daObjWcObj06_c_Spawn. */
extern "C" int *daObjWcObj06_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(840);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV17daObjFloatBoard_c;
        p[0] = (int)&_ZTV14daObjWcObj06_c[2]; /* +8: this TU defines the vtable */
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN14daObjWcObj06_c13InitResourcesEv, 0x02111f58, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjWcObj06_c13InitResourcesEv
/* daObjWcObj06_c::InitResources -- vtable slot 0, ov029
 * 0x02111f58. Identical shape to daObjWcObj01_c::
 * InitResources (this overlay), just a different file table
 * (data_ov029_02113f00) -- see that file's note for the field/helper
 * rationale. */
int daObjWcObj06_c::InitResources()
{
    char *c = (char *)this;
    int sppad[6];
    int pos[3];
    char rl[0x7c];
    int wh;
    int x, y, z;

    if (func_ov002_020b5e58((char*)c, (char*)data_ov029_02113f00) != 0) {
        wh = _ZN8dActor_c17GetWaterHeightWDWEv(c);
        if (mPosY > wh) {
            _ZN9dBgCh_LinC1Ev(rl);
            x = mPosX;
            sppad[3] = x;
            y = mPosY;
            sppad[4] = y;
            z = mPosZ;
            sppad[1] = y;
            sppad[5] = z;
            sppad[0] = x;
            sppad[2] = z;
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
/* ROM ordinals 0 and 1 -- _ZN14daObjWcObj06_cD1Ev 0x02111ea4 size 0x50 and   */
/* _ZN14daObjWcObj06_cD0Ev 0x02111ef4 size 0x64 -- are NOT written here.      */
/*                                                                            */
/* The destructor is defined INLINE in include/daObjWcObj06_c.h. Written      */
/* out-of-line here the real destructor makes mwccarm emit D0 BEFORE D1,      */
/* the reverse of the cartridge's order, which objisolate refuses for the     */
/* whole TU, and it emits a third D2 body with no ROM home. The inline        */
/* definition gives the retail D1/D0 pair in ROM order and no D2, while       */
/* InitResources -- an override of a base virtual, so virtual itself, and     */
/* defined out-of-line above -- keeps this TU as the class's key-function     */
/* TU, so it still owns the complete _ZTV/_ZTI/_ZTS group declared in this    */
/* entry's compiler_only_output. D0 is that destructor plus the inherited     */
/* inline `operator delete`; slot 17 is the deleting variant. An older      */
/* comment in the deleted per-function D0 source called slot 17              */
/* OnYoshiTryEat; that is slot 18, and the claim did not survive the merge.   */
/*                                                                            */
/* The three vptr stores in the ROM's D1 are unchanged by the move: this      */
/* class's, daObjFloatBoard_c's (inlined, its destructor being in its own     */
/* class body) and dBgActor_c's, followed by dBgActor_c's dBgW_KcMbg and      */
/* Model and then dActor_c. Nothing in the chain adds a member with a         */
/* destructor, which is why the body is empty either way.                     */
/* -------------------------------------------------------------------------- */
