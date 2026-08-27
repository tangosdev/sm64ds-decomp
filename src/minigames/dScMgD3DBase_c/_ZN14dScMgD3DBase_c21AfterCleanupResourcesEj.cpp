//cpp
// @symbol _ZN14dScMgD3DBase_c21AfterCleanupResourcesEj
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "dScMgD3DBase_c.h"
extern "C" {
extern void Ov004_Deallocate(void* x);
extern void _ZN11dScMgBase_c21AfterCleanupResourcesEj(void* a, int b);
}
extern void* data_ov006_02141a48;
extern unsigned char data_0209f5f8;

void dScMgD3DBase_c::AfterCleanupResources(unsigned int b_)
{
    int b = (int)b_;

    if (b == 2) {
        CleanCommonModelDataArr();
        *(int*)0x40004c8 = 0x296a5800;
        *(int*)0x40004cc = 0x7fff;
        *(int*)0x40004c8 = 0x696a5800;
        *(int*)0x40004cc = 0x40007fff;
        func_ov006_020e7110(((void*)this));
        Ov004_Deallocate(data_ov006_02141a4c);
        Ov004_Deallocate(data_ov006_02141a48);
    }
    data_0209f5f8 = 0;
    _ZN11dScMgBase_c21AfterCleanupResourcesEj(((void*)this), b);
}
