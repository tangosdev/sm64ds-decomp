//cpp
struct Vector3 { int x,y,z; };
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern void _ZN16MeshColliderBase7DisableEv(void*);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
extern void* _ZN5Actor13ClosestPlayerEv(void*);
extern int Vec3_Dist(void*, void*);
int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(char* c, int a, int b){
  int on = (*(int*)(c+0xb0) & 8) != 0;
  if (on) {
    if (_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
      _ZN16MeshColliderBase7DisableEv(c+0x124);
    return 0;
  }
  if (a == 0) {
    if (!_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
      _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
    goto done;
  }
  {
  struct Vector3 v;
  v.x = *(int*)(c+0x5c);
  v.y = *(int*)(c+0x60);
  v.z = *(int*)(c+0x64);
  if (b == 0) v.y = v.y + *(int*)(c+0xb4);
  else v.y = v.y + b;
  void* p = _ZN5Actor13ClosestPlayerEv(c);
  int d = Vec3_Dist(&v, (char*)p+0x5c);
  if (d > a) {
    if (_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
      _ZN16MeshColliderBase7DisableEv(c+0x124);
    return 0;
  }
  if (!_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
  }
done:
  return 1;
}
}
