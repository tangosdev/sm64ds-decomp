// @symbol func_ov027_02111d8c
// @emits daPgDfdr_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daPgDfdr_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern void* data_ov027_02113c7c;
extern void* data_ov027_02113c94;
extern void* data_ov027_02113c6c;
int daPgDfdr_c_CleanupResources(char* c){
  int i;
  _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c7c);
  for(i=0;i<3;i++) _ZN13SharedFilePtr7ReleaseEv(data_ov035_02112ca4[i]);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c94);
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c6c);
  return 1;
}
