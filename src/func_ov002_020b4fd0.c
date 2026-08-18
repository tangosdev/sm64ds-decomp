extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern int _ZN16MeshColliderBase7DisableEv(void*);
extern int _ZN16MeshColliderBase6EnableEP8dActor_c(void*, char*);
int func_ov002_020b4fd0(char* c){
  int b = (int)((*(int*)(c+0xb0) & 8) != 0);
  if(b){
    if(_ZN16MeshColliderBase9IsEnabledEv(*(void**)(c+0x320)))
      _ZN16MeshColliderBase7DisableEv(*(void**)(c+0x320));
    return 1;
  }
  if(!_ZN16MeshColliderBase9IsEnabledEv(*(void**)(c+0x320)))
    _ZN16MeshColliderBase6EnableEP8dActor_c(*(void**)(c+0x320), c);
  return 0;
}
