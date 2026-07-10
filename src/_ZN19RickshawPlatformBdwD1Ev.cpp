//cpp
extern "C" {
extern int func_0207328c(void*,int,int,void*);
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN5ActorD1Ev(void*);
extern int _ZTV19RickshawPlatformBdw[];
extern int data_ov002_02108d94[];
extern int _ZTV17ExclamationSwitch[];
void* _ZN19RickshawPlatformBdwD1Ev(char* c){
  *(int**)c=_ZTV19RickshawPlatformBdw;
  *(int**)c=data_ov002_02108d94;
  func_0207328c(c+0x4b0,5,0x1c8,(void*)_ZN18MovingMeshColliderD1Ev);
  func_0207328c(c+0x320,5,0x50,(void*)_ZN5ModelD1Ev);
  *(int**)c=_ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN5ActorD1Ev(c);
  return c;
}
}
