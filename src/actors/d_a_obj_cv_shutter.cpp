//cpp
/* Production translation unit for ov021/daObjCvShutter_c, hand-curated.
 * 6 function(s), .text 0x02112db4..0x02112f10.
 *
 * Hazy Maze Cave's switch-operated shutter (`cv` is the ROM's tag for that
 * stage). It adds no state of its own beyond four unobserved bytes at 0x320
 * (see include/daObjCvShutter_c.h) and fills the three vtable slots its base
 * daObjSwdoor_c leaves null: InitResources (0), CleanupResources (3) and
 * Behavior (6). The first two hand the actor and this level's descriptor to
 * daObjSwdoor_c's shared ov002 helpers; Behavior runs the shared shutter step
 * and then re-syncs the collision mesh when it is in range.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02112db4  src/_ZN16daObjCvShutter_cD1Ev.cpp
 *   [1] 0x02112e04  src/_ZN16daObjCvShutter_cD0Ev.cpp
 *   [2] 0x02112e68  src/_ZN16daObjCvShutter_c16CleanupResourcesEv.cpp
 *   [3] 0x02112e7c  src/_ZN16daObjCvShutter_c8BehaviorEv.cpp
 *   [4] 0x02112ec0  src/_ZN16daObjCvShutter_c13InitResourcesEv.cpp
 *   [5] 0x02112ed4  src/daObjCvShutter_c_classInit.c
 *
 * THE SIXTH IS THE FACTORY. daObjCvShutter_c_classInit (historical alias
 * ShutterHmc_Spawn) is the CV_SHUTTER registry profile's spawn function and
 * sits immediately after InitResources in the ROM's own .text order, so it is
 * part of this TU. It keeps C linkage and is written first here, being the
 * highest-address member.
 */

/* Includes and externs: the union of the legacy files', kept at their legacy
 * spelling. `data_ov021_021148d0` and `func_ov002_020baba8` come in through
 * decl_common.h, exactly as the legacy CleanupResources/InitResources did. */
#include "daObjCvShutter_c.h"
#include "decl_common.h"

extern "C" {
int func_ov002_020bac18(void);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern int func_ov002_020bad10(void *self, void *data);

/* The factory's own dependencies, restated here rather than pulled in through
   decl_ActorBase.h / decl_Platform.h as the legacy file did -- this TU declares
   in place, and pulling new decl_*.h headers in changes what the TU sees and
   can perturb members that already match. */
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern int _ZTV13daObjSwdoor_c[];
extern int _ZTV16daObjCvShutter_c[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- daObjCvShutter_c_classInit, 0x02112ed4, size 0x3c         */
/* -------------------------------------------------------------------------- */
// @symbol daObjCvShutter_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjCvShutter_c through RTTI,
   allocation size, most-derived vtable identity, and the CV_SHUTTER registry
   profile; later EAD lineage supplies classInit. Exact original spelling is not
   preserved. Historical alias: ShutterHmc_Spawn.

   Two vptr stores, not one: the inlined daObjSwdoor_c constructor writes its
   own vptr and this class's write follows it. 804 = 0x324 = the whole object,
   the four unobserved bytes at 0x320 included. */
extern "C" int *daObjCvShutter_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(804);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13daObjSwdoor_c;
        p[0] = (int)_ZTV16daObjCvShutter_c;
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN16daObjCvShutter_c13InitResourcesEv, 0x02112ec0, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCvShutter_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjCvShutter_c::InitResources()
{
    return func_ov002_020bad10(((void *)this), data_ov021_021148d0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjCvShutter_c8BehaviorEv, 0x02112e7c, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCvShutter_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
int daObjCvShutter_c::Behavior()
{
  int r = func_ov002_020bac18();
  if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((void*)this), 0, 0))
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((void*)this));
  return r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN16daObjCvShutter_c16CleanupResourcesEv, 0x02112e68, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCvShutter_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjCvShutter_c::CleanupResources()
{
    return func_ov002_020baba8(((void *)this), data_ov021_021148d0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN16daObjCvShutter_cD0Ev, 0x02112e04, size 0x64          */
/* ROM ordinal 0 -- _ZN16daObjCvShutter_cD1Ev, 0x02112db4, size 0x50          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCvShutter_cD1Ev
// @symbol _ZN16daObjCvShutter_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjCvShutter_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are THREE vptr stores, and the middle one is the finding:
   `daObjCvShutter_c : daObjSwdoor_c : dBgActor_c` emits this class's vptr,
   then daObjSwdoor_c's -- inlined, because that destructor is defined in its
   class body -- then dBgActor_c's, then dBgActor_c's dBgW_KcMbg and Model,
   then dActor_c. A one-level chain would emit two. This class adds no member
   with a destructor of its own, and D0's trailing deallocation is the inline
   `operator delete` it inherits, which is why nothing here names a heap. */
