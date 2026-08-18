// @symbol _ZN10daPgDfdr_c16CleanupResourcesEv
/* daPgDfdr_c::CleanupResources -- vtable slot 3. A plain C file defining the
   exact mangled identifier directly (C has no name mangling of its own);
   see src/_ZN7daDgr_c13InitResourcesEv.cpp for why it stays a free function. */
#include "decl_common.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern void* data_ov027_02113c7c;
extern void* data_ov027_02113c94;
extern void* data_ov027_02113c6c;
int _ZN10daPgDfdr_c16CleanupResourcesEv(char* c){
  int i;
  _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c7c);
  for(i=0;i<3;i++) _ZN13SharedFilePtr7ReleaseEv(data_ov027_02112ca4[i]);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c94);
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c6c);
  return 1;
}
