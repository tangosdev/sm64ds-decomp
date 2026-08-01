//cpp
extern "C" {
extern int data_ov100_0214857c[];
extern int data_ov002_0210af70[];
extern int _ZTV17ExclamationSwitch[];
void _ZN11ShadowModelD1Ev(void*);
void __destroy_arr(void*,int,int,void*);
void _ZN18MovingMeshColliderD1Ev(void*);
void _ZN5ModelD1Ev(void*);
void _ZN5ActorD2Ev(void*);
}
extern "C" int func_ov100_02146d7c(char* c){
  *(int**)c=(int*)data_ov100_0214857c;
  _ZN11ShadowModelD1Ev(c+0x450);
  *(int**)c=(int*)data_ov002_0210af70;
  __destroy_arr(c+0x320,3,0x50,(void*)_ZN5ModelD1Ev);
  *(int**)c=(int*)_ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN5ActorD2Ev(c);
  return (int)c;
}
