//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void*);
extern void func_020733a8(void*,int,int,void*,void*);
extern int func_ov075_0211478c[];
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN18MovingMeshColliderC1Ev(void*);
void* WorkElevator_Spawn(void){
  char* c = (char*)_ZN9ActorBasenwEj(0xc80);
  if(c){
    _ZN8PlatformC2Ev(c);
    *(int*)c = (int)func_ov075_0211478c;
    func_020733a8(c+0x320, 4, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    func_020733a8(c+0x520, 4, 0x1c8, (void*)_ZN18MovingMeshColliderC1Ev, (void*)_ZN18MovingMeshColliderD1Ev);
  }
  return c;
}
}
