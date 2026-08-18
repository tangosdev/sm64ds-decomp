// @symbol _ZN9dScDSMT_c16CleanupResourcesEv
/* dScDSMT_c::CleanupResources() -- vtable slot 3. See include/dScDSMT_c.h. */
#include "decl_Scene.h"
#include "decl_common.h"
extern void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
extern void func_0203cbc0(void* a);
extern int data_0209d4a8;
extern void* data_0209b33c;

int _ZN9dScDSMT_c16CleanupResourcesEv(void) {
    data_0209d4a8 = 0;
    _ZN8dScene_c20SetAndStopColorFaderEv();
    data_0209b340[0] = func_ov007_020b6f4c();
    data_0209b340[1] = 2;
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    func_0203cbc0(data_0209b33c);
    data_0209b33c = 0;
    return 1;
}
