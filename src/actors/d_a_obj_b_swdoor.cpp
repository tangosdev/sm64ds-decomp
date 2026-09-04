//cpp
/* Bob-omb Battlefield's switch-operated shutter -- ov014/daObjBSwdoor_c.
 *
 * A GENUINE TRANSLATION UNIT, ENROLLED AND CANONICAL. It is the whole of the
 * cartridge's contiguous linker run .text 0x021111a0..0x02111308, six functions
 * and nothing else, and it is what the ROM build compiles for that span. It
 * replaces six one-function files that used to sit under src/; the shadow
 * disclaimer that stood here while it was still under src_tu/ is gone with the
 * condition it described.
 *
 * THE CLASS IS NAMED FROM THE CARTRIDGE, not coined. ov014 0x021145b8 is a
 * __si_class_type_info whose _ZTS at 0x021145d0 reads exactly `14daObjBSwdoor_c`,
 * and whose one base at subobject offset 0 is ov002 0x021099c0, `13daObjSwdoor_c`,
 * itself over `10dBgActor_c`. That is why this TU can be its class's key-function
 * TU at all: the records it emits are word-comparable against the ROM.
 *
 * WHAT THE ROM SAYS ABOUT THE DATA THIS TU EMITS. Naming the class puts the
 * vague-linkage RTTI in this object, and tools/romdata_check.py compares each
 * record against the cartridge with relocations applied, before production
 * isolation discards it:
 *
 *   _ZTV14daObjBSwdoor_c  ov014 0x02114608  VERIFIED, 128 bytes = 32 slots
 *   _ZTI14daObjBSwdoor_c  ov014 0x021145b8  VERIFIED
 *   _ZTI13daObjSwdoor_c   ov002 0x021099c0  VERIFIED
 *   _ZTI10dBgActor_c / _ZTI8dActor_c / _ZTI7dBase_c / _ZTI7fBase_c  VERIFIED
 *   _ZTS13daObjSwdoor_c   ov002 0x021099cc  VERIFIED
 *   the five other _ZTS records                                    PARTIAL
 *
 * THE VTABLE CLAIM IS SCOPED TO THE SLOTS, deliberately. A symbols.txt _ZTV
 * address is the ADDRESS POINT, eight bytes past the table's real start; the
 * {offset-to-top, _ZTI pointer} header before it is emitted here and word-compared
 * by nothing. 32 slots are verified. The two header words are not, and no claim is
 * made about them. The five PARTIAL rows are the known dsd extent shortfall on
 * _ZTS records, not a disagreement: no record here reads DIFFERS.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder.
 * The destructor is the documented exception: it is inline in the header and
 * mwccarm places its D1/D0 pair itself, in ROM order.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN14daObjBSwdoor_cD1Ev.cpp
 *   [1] 0x021111f0  src/_ZN14daObjBSwdoor_cD0Ev.c
 *   [2] 0x02111254  src/_ZN14daObjBSwdoor_c16CleanupResourcesEv.cpp
 *   [3] 0x02111268  src/_ZN14daObjBSwdoor_c8BehaviorEv.cpp
 *   [4] 0x02111294  src/_ZN14daObjBSwdoor_c13InitResourcesEv.cpp
 *   [5] 0x021112cc  src/daObjBSwdoor_c_Spawn.c
 */

/* THE ONE PRAGMA, RESOLVED. tubuild create dropped the legacy
 * _ZN14daObjBSwdoor_c16CleanupResourcesEv.cpp's `#pragma long_calls on`, and in
 * 2004/b56 that directive is POSITIONAL rather than file-global -- unlike
 * opt_propagation or optimize_for_size, which take the file's last setting. It is
 * restored at that one member below and bracketed with `off`, so it cannot reach
 * the other five in source order. See the member's own comment for what it buys.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. */
#include "daObjBSwdoor_c.h"
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
#include "dBgW.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

extern "C" {
extern int _ZTV14daObjBSwdoor_c[];
extern int _ZTV13daObjSwdoor_c[];
extern int _ZTV10dBgActor_c[];
extern void *data_020a0eac;
extern int func_ov002_020bad10(void *c, void **f);
extern int data_ov014_021145c4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- daObjBSwdoor_c_classInit, 0x021112cc, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol daObjBSwdoor_c_classInit
/* recovered: vtable identified, declarations from a shared header */
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjSwdoor_c */
/* Reconstructed source-style name: SM64DS proves daObjBSwdoor_c through RTTI,
 * allocation size, vtable identity, and the SWITCHDOOR registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daObjBSwdoor_c_Spawn. */
extern "C" int *daObjBSwdoor_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(804);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13daObjSwdoor_c;
        p[0] = (int)&_ZTV14daObjBSwdoor_c[2]; /* +8: this TU defines the vtable */
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN14daObjBSwdoor_c13InitResourcesEv, 0x02111294, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBSwdoor_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * The collider used to be reached through a `u8 mMovingMeshCollider` marker this
 * class declared itself. It is dBgActor_c's, twice removed, and dBgActor_c calls it
 * mMeshCollider; the byte at that offset has not moved.
 *
 * THE TWO LOCAL SHADOW CLASSES ARE GONE, and they had to go rather than be
 * renamed. This file used to carry `class dActor_c {};` and
 * `class dBgW { void Enable(dActor_c *); };`. daObjBSwdoor_c.h now reaches
 * the real dActor_c and the real dBgW through dBgActor_c, and a second
 * definition of either name in the same translation unit makes mwccarm 2004/b56
 * die with an internal compiler error rather than a diagnostic. Renaming them
 * compiles and byte-matches and is still wrong: the call mangles to
 * _ZN10ClsnEnable6EnableEP9ClsnOwner, which nothing defines, so the relocation
 * lands nowhere. The byte gate cannot see that -- it wildcards relocation slots --
 * and tools/check_references.py reported it as newly unresolvable. Using the real
 * declarations mangles to _ZN4dBgW6EnableEP8dActor_c, which resolves.
 */
int daObjBSwdoor_c::InitResources()
{
    int r4 = func_ov002_020bad10(((char *)this), (void **)&data_ov014_021145c4);
    ((dBgW *)((char *)&mMeshCollider))->Enable((dActor_c *)((char *)this));
    return r4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN14daObjBSwdoor_c8BehaviorEv, 0x02111268, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBSwdoor_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* The local `class dBgActor_c { void UpdateClsnPosAndRot(); };` this file used to
   carry is gone: daObjBSwdoor_c.h now reaches the real one through daObjSwdoor_c, and
   two definitions of the name in one translation unit do not compile. The call is
   the same call -- dBgActor_c::UpdateClsnPosAndRot is non-virtual, so it is a direct
   `bl` either way. */
int daObjBSwdoor_c::Behavior()
{
int r4 = func_ov002_020bac18();
UpdateClsnPosAndRot();
return r4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN14daObjBSwdoor_c16CleanupResourcesEv, 0x02111254, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBSwdoor_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
// Cross-overlay tail-call veneer. #pragma long_calls forces mwccarm to emit the pooled
// `ldr ip,[pc]; bx ip` indirect tail-call (a plain near `b` otherwise) that the ROM uses
// to reach another overlay. Loads the data pointer into r1; this stays in r0.
// (tubuild create dropped the legacy file's directive; restored here, bracketed
// with `off` so it cannot leak into the members below in source order.)
#pragma long_calls on
int daObjBSwdoor_c::CleanupResources()
{
    return func_ov002_020baba8(((void *)this), (void *)&data_ov014_021145c4);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14daObjBSwdoor_cD0Ev, 0x021111f0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBSwdoor_cD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV14daObjBSwdoor_c; VT1 = _ZTV13daObjSwdoor_c */
/* (no separate definition: the single ~daObjBSwdoor_c() below emits the D0 and D1
 * variants together -- keeping the hand-mangled body alongside a real destructor
 * is the known mwccarm ICE, ELFgen.c:483.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN14daObjBSwdoor_cD1Ev, 0x021111a0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBSwdoor_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * (no definition here either: `virtual ~daObjBSwdoor_c() {}` is in
 * include/daObjBSwdoor_c.h, which is where the reasoning lives. Defined out of
 * line in this file, the pair came out D0-before-D1 -- the reverse of the
 * cartridge -- with a third, homeless D2, and objisolate refused the TU. The
 * inline definition emits both variants here, in ROM order, and no D2.)
 */
