//cpp
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern int _ZN16MeshColliderBase7DisableEv(void*);
extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern int* data_ov036_02113d78[];
int _ZN10DonutBlock16CleanupResourcesEv(void* c){
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)c+0x124)) _ZN16MeshColliderBase7DisableEv((char*)c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(data_ov036_02113d78[0]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov036_02113d78[1]);
  return 1;
}
}
