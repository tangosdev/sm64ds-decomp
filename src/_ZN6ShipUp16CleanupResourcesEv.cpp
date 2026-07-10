//cpp
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern void _ZN16MeshColliderBase7DisableEv(void*);
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern void* data_ov016_021136e4[];
extern void* data_ov016_021136dc[];
int _ZN6ShipUp16CleanupResourcesEv(char* c){
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(data_ov016_021136e4[*(unsigned char*)(c+0x31e)]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov016_021136dc[*(unsigned char*)(c+0x31e)]);
  return 1;
}
}
