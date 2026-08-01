// @symbol func_ov007_020cc45c
// recovered name: dScDSMT_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Scene.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScDSMT_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
extern void func_0203cbc0(void* a);
extern int data_0209d4a8;
extern void* data_0209b33c;

int func_ov007_020cc45c(void) {
    data_0209d4a8 = 0;
    _ZN5Scene20SetAndStopColorFaderEv();
    data_0209b340[0] = func_ov007_020b6f4c();
    data_0209b340[1] = 2;
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    func_0203cbc0(data_0209b33c);
    data_0209b33c = 0;
    return 1;
}
