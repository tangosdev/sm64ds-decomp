//cpp
// @symbol _ZN12dScMgAmida_c21AfterCleanupResourcesEj
/* dScMgAmida_c::AfterCleanupResources -- the recovered source locally
   declared the base override as returning void*; the real dScMgBase_c.h
   override returns void, so this now calls dScMgBase_c::AfterCleanupResources
   as a plain statement instead of returning it, same fix dScMgLuigi_c's own
   slot 5 needed. The early `if (vfSuccess != 2) return` is preserved exactly
   as recovered -- the base-class call is skipped too when vfSuccess != 2,
   not just the two Deallocate calls. */
#include "dScMgAmida_c.h"

namespace Memory { void Deallocate(void *); }

void dScMgAmida_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess != 2)
        return;
    Memory::Deallocate(unk_470c);
    Memory::Deallocate(unk_4710);
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}
