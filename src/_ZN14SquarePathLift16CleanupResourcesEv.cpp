//cpp
extern "C" {
struct SFP{int x;};
extern SFP data_ov052_021125a0[2];
int _ZN16MeshColliderBase9IsEnabledEv(void*);
void _ZN16MeshColliderBase7DisableEv(void*);
void _ZN13SharedFilePtr7ReleaseEv(void*);
int _ZN14SquarePathLift16CleanupResourcesEv(char* c){
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov052_021125a0[0].x);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov052_021125a0[1].x);
  return 1;
}
}
