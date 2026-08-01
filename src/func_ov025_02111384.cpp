//cpp
// @symbol func_ov025_02111384
// recovered name: daDgr_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daDgr_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void* self);
int _ZN16MeshColliderBase9IsEnabledEv(void* self);
void _ZN16MeshColliderBase7DisableEv(void* self);
extern int data_ov025_02113a68[];
int func_ov025_02111384(char* c) {
  _ZN13SharedFilePtr7ReleaseEv(data_ov025_02113a68);
  _ZN13SharedFilePtr7ReleaseEv(data_ov036_02113a60);
  if (_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  return 1;
}
}
