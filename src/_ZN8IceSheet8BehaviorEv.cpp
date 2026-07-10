//cpp
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void*);
void _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
int _ZN8IceSheet8BehaviorEv(char* c){
  if (!_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
  return 1;
}
}
