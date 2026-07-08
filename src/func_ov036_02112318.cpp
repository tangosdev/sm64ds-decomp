//cpp
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void* m);
extern void _ZN16MeshColliderBase7DisableEv(void* m);
extern void _ZN13SharedFilePtr7ReleaseEv(void* f);
extern int SNUFIT_BULLET_MODEL_PTR[];
extern void* data_ov036_02113f58[];
extern int data_ov036_0211419c[];
int func_ov036_02112318(char* c){
  if (_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(SNUFIT_BULLET_MODEL_PTR);
  _ZN13SharedFilePtr7ReleaseEv(data_ov036_02113f58[0]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov036_02113f58[1]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov036_0211419c);
  return 1;
}
}
