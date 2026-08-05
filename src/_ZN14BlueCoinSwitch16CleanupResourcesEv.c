// @symbol _ZN14BlueCoinSwitch16CleanupResourcesEv
// recovered name: daObjBC_Switch_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int BlueCoinSwitch_ClsnFile[];
/* recovered: renamed to Class_Method */
/* daObjBC_Switch_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int BlueCoinSwitch_ModelFile[];
int _ZN14BlueCoinSwitch16CleanupResourcesEv(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(BlueCoinSwitch_ModelFile);
    _ZN13SharedFilePtr7ReleaseEv(BlueCoinSwitch_ClsnFile);
    return 1;
}
