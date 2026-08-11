//cpp
// @symbol _ZN11dScMgBase_c21AfterCleanupResourcesEj
#include "decl_common.h"
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_AfterCleanupResources
/* dScMgBase_c::AfterCleanupResources - recovered from vtable slot identity. */
extern "C" {
extern void func_ov004_020ad90c(void);
extern void FreeGfxSlotsById(int arg);
extern void func_0203cbc0(void);
extern void func_02012e1c(void);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int x);
extern int data_0209b308[];
extern int data_0209d4a8[];
extern int data_ov004_020beb60[];
}

void dScMgBase_c::AfterCleanupResources(u32 arg)
{
    if (arg == 2) {
        if (data_0209b308[4] == 0)
            func_ov004_020ad90c();
        data_0209d4a8[0] = 0;
        data_ov004_020beb74[1] = 0;
        FreeGfxSlotsById(0x1d);
        if (data_ov004_020beb60[0] != 0) {
            func_0203cbc0();
            data_ov004_020beb60[0] = 0;
        }
        if (unk_4628 != 0) {
            func_02012e1c();
            _ZN5Sound22StopLoadedMusic_Layer1Ej(1);
        }
        func_ov004_020b2c84();
    }
    Scene::AfterCleanupResources(arg);
}
