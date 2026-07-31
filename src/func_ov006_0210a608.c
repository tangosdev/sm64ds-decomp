// @symbol func_ov006_0210a608
// @emits dScMgFlower_c_AfterCleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgFlower_c::AfterCleanupResources - recovered from vtable slot identity */
extern void func_ov004_020b0840(int a, int b);
void dScMgFlower_c_AfterCleanupResources(int a, int b) {
    if (b == 2) {
        *(volatile int*)0x40004c8 = 0x296a5800;
        *(volatile int*)0x40004cc = 0x7fff;
        CleanCommonModelDataArr();
    }
    func_ov004_020b0840(a, b);
}
