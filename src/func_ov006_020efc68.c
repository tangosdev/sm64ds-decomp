// @symbol func_ov006_020efc68
// @emits dScMgLuigi_c_AfterCleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgLuigi_c::AfterCleanupResources - recovered from vtable slot identity */
typedef unsigned short u16;
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned int irq, void *handler);
extern int func_ov004_020b0840(int a, int b);
int dScMgLuigi_c_AfterCleanupResources(int a, int irq){
    if(irq == 2 && _ZN3IRQ13GetIRQHandlerEj(2) == (void*)func_ov006_020efcf8){
        u16 ime;
        do {
            ime = *(volatile u16*)0x4000208;
            *(volatile u16*)0x4000208 = 0;
        } while(ime != 0);
        _ZN3IRQ11DisableIRQsEj(2);
        func_02053c10(0);
        _ZN3IRQ13SetIRQHandlerEjPFvvE(2, 0);
        ime = *(volatile u16*)0x4000208;
        *(volatile u16*)0x4000208 = 1;
    }
    return func_ov004_020b0840(a, irq);
}
