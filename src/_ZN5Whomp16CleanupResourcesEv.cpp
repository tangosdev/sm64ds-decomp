//cpp
// @symbol _ZN5Whomp16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Whomp.h"
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void data_ov079_02128168(void);
extern void data_ov079_02128178(void);
extern void data_ov079_02128170(void);
extern void *data_ov079_021275ec[];

int Whomp::CleanupResources()
{
  int i;
  if(_ZN16MeshColliderBase9IsEnabledEv((unsigned char *)&mMovingMeshCollider))
    _ZN16MeshColliderBase7DisableEv((unsigned char *)&mMovingMeshCollider);
  _ZN13SharedFilePtr7ReleaseEv(data_ov079_02127bf0[((unsigned char *)this)[0x414]]);
  if(((unsigned char *)this)[0x414]){
    _ZN13SharedFilePtr7ReleaseEv((void*)&data_ov079_02128168);
    for(i=0;i<6;i++)
      _ZN13SharedFilePtr7ReleaseEv(data_ov079_02127600[i]);
    _ZN13SharedFilePtr7ReleaseEv((void*)&data_ov079_02128178);
  } else {
    _ZN13SharedFilePtr7ReleaseEv((void*)&data_ov079_02128170);
    for(i=0;i<5;i++)
      _ZN13SharedFilePtr7ReleaseEv(data_ov079_021275ec[i]);
  }
  return 1;
}
