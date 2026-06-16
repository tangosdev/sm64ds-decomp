extern int data_ov047_0211254c[];
extern int data_ov002_02108d94[];
extern int data_ov002_0210ae38[];
extern int func_0207328c(void*,int,int,void*);
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN5ActorD1Ev(void*);
int func_ov047_02111510(char* c){
  *(int*)c=(int)data_ov047_0211254c;
  *(int*)c=(int)data_ov002_02108d94;
  func_0207328c(c+0x4b0,5,0x1c8,(void*)_ZN18MovingMeshColliderD1Ev);
  func_0207328c(c+0x320,5,0x50,(void*)_ZN5ModelD1Ev);
  *(int*)c=(int)data_ov002_0210ae38;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN5ActorD1Ev(c);
  return (int)c;
}
