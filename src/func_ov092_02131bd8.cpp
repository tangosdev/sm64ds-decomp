//cpp
extern "C" {
int _ZN13SharedFilePtr7ReleaseEv(void*);
int _ZN16MeshColliderBase9IsEnabledEv(void*);
int _ZN16MeshColliderBase7DisableEv(void*);
extern int data_ov092_02132540[];
extern int data_ov092_02132548[];
int func_ov092_02131bd8(char* c){
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov092_02132540);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov092_02132548);
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  return 1;
}
}
