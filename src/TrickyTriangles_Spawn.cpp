//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void*);
extern void func_020733a8(void*,int,int,void*,void*);
extern int data_ov002_02108d94[];
extern int _ZTV21ArmedRotatingPlatform[];
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN18MovingMeshColliderC1Ev(void*);
void* TrickyTriangles_Spawn(void){
  char* c = (char*)_ZN9ActorBasenwEj(0xdcc);
  if(c){
    _ZN8PlatformC2Ev(c);
    *(int*)c = (int)data_ov002_02108d94;
    func_020733a8(c+0x320, 5, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    func_020733a8(c+0x4b0, 5, 0x1c8, (void*)_ZN18MovingMeshColliderC1Ev, (void*)_ZN18MovingMeshColliderD1Ev);
    *(int*)c = (int)_ZTV21ArmedRotatingPlatform;
  }
  return c;
}
}
