//cpp
// @symbol _ZN9dScDSMT_c16CleanupResourcesEv
/* recovered: real C++ method */
/* dScDSMT_c::CleanupResources() -- vtable slot 3. Unregisters the graph
 * callback, hands the colour fader back, records the exit reason in
 * data_0209b340, then releases the download-play voice group and buffer. */
#include "dScDSMT_c.h"
#include "decl_common.h"

extern "C" {
void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
void func_0203cbc0(void *a);
extern int data_0209d4a8;
extern void *data_0209b33c;
}

s32 dScDSMT_c::CleanupResources()
{
    data_0209d4a8 = 0;
    dScene_c::SetAndStopColorFader();
    data_0209b340[0] = func_ov007_020b6f4c();
    data_0209b340[1] = 2;
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    func_0203cbc0(data_0209b33c);
    data_0209b33c = 0;
    return 1;
}
