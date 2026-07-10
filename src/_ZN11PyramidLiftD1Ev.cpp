//cpp
extern "C" {
extern void func_0207328c(void*,int,int,void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN5ActorD1Ev(void*);
extern int data_ov027_021139d4[];
extern int func_020072c0();
extern int _ZTV17ExclamationSwitch[];
void* _ZN11PyramidLiftD1Ev(char* c){
  *(int**)c = data_ov027_021139d4;
  func_0207328c(c+0x37c, 0xa, 0xc, (void*)func_020072c0);
  _ZN5ModelD1Ev(c+0x320);
  *(int**)c = _ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN5ActorD1Ev(c);
  return c;
}
}
