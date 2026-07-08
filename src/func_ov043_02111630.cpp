//cpp
extern "C" {
extern int func_0207328c(void*,int,int,void*);
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN5ActorD1Ev(void*);
extern int data_ov043_0211255c[];
extern int data_ov002_02108d94[];
extern int _ZTV8Platform[];
void* func_ov043_02111630(char* c){
  *(int**)c=data_ov043_0211255c;
  *(int**)c=data_ov002_02108d94;
  func_0207328c(c+0x4b0,5,0x1c8,(void*)_ZN18MovingMeshColliderD1Ev);
  func_0207328c(c+0x320,5,0x50,(void*)_ZN5ModelD1Ev);
  *(int**)c=_ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN5ActorD1Ev(c);
  return c;
}
}
