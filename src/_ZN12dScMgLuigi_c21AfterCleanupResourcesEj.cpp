//cpp
// @symbol _ZN12dScMgLuigi_c21AfterCleanupResourcesEj
// recovered name: dScMgLuigi_c_AfterCleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgLuigi_c.h"
#include "IRQ.h"
/* dScMgLuigi_c::AfterCleanupResources - recovered from vtable slot identity.
   The recovered source returned the base call's result as if it were int;
   the real override (dScMgBase_c.h) returns void, so this now calls the
   base method as a plain statement instead. */
void dScMgLuigi_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2 && IRQ::GetIRQHandler(2) == func_ov006_020efcf8) {
        u16 ime;
        do {
            ime = *(volatile u16 *)0x4000208;
            *(volatile u16 *)0x4000208 = 0;
        } while (ime != 0);
        IRQ::DisableIRQs(2);
        func_02053c10(0);
        IRQ::SetIRQHandler(2, 0);
        ime = *(volatile u16 *)0x4000208;
        *(volatile u16 *)0x4000208 = 1;
    }
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}
