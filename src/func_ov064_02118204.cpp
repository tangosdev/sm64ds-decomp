//cpp
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void* self);
void _ZN16MeshColliderBase7DisableEv(void* self);
void _ZN13SharedFilePtr7ReleaseEv(void* self);
extern int data_ov022_02113cc8[];
int func_ov064_02118204(char* c) {
  if (_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov022_02113cc8[0]);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov022_02113cc8[1]);
  return 1;
}
}
