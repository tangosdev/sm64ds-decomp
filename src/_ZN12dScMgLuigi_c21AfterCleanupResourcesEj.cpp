//cpp
// @symbol _ZN12dScMgLuigi_c21AfterCleanupResourcesEj
// recovered name: dScMgLuigi_c_AfterCleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgLuigi_c.h"
/* dScMgLuigi_c::AfterCleanupResources - recovered from vtable slot identity.
   The recovered source returned the base call's result as if it were int;
   the real override (dScMgBase_c.h) returns void, so this now calls the
   base method as a plain statement instead. */
extern "C" {
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned int irq, void *handler);
}

void dScMgLuigi_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2 && _ZN3IRQ13GetIRQHandlerEj(2) == (void *)func_ov006_020efcf8) {
        u16 ime;
        do {
            ime = *(volatile u16 *)0x4000208;
            *(volatile u16 *)0x4000208 = 0;
        } while (ime != 0);
        _ZN3IRQ11DisableIRQsEj(2);
        func_02053c10(0);
        _ZN3IRQ13SetIRQHandlerEjPFvvE(2, 0);
        ime = *(volatile u16 *)0x4000208;
        *(volatile u16 *)0x4000208 = 1;
    }
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}
