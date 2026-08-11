// @symbol func_ov006_0210a608
// recovered name: dScMgFlower_c_AfterCleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgFlower_c::AfterCleanupResources - recovered from vtable slot identity */
extern void _ZN11dScMgBase_c21AfterCleanupResourcesEj(int a, int b);
void func_ov006_0210a608(int a, int b) {
    if (b == 2) {
        *(volatile int*)0x40004c8 = 0x296a5800;
        *(volatile int*)0x40004cc = 0x7fff;
        CleanCommonModelDataArr();
    }
    _ZN11dScMgBase_c21AfterCleanupResourcesEj(a, b);
}
