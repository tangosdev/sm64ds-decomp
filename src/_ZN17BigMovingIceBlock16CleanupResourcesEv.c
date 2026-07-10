extern int _ZN16MeshColliderBase9IsEnabledEv();
extern int _ZN16MeshColliderBase7DisableEv();
extern int _ZN13SharedFilePtr7ReleaseEv();
extern int *data_ov056_02113314[];
int _ZN17BigMovingIceBlock16CleanupResourcesEv(char *c){
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)c+0x124))
    _ZN16MeshColliderBase7DisableEv((char*)c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(data_ov056_02113314[0]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov056_02113314[1]);
  return 1;
}
