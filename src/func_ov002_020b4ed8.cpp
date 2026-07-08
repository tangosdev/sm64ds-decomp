//cpp
extern "C" {
extern int _ZN9ModelAnimD1Ev(char*);
extern int _ZN18MovingMeshColliderD1Ev(char*);
extern int _ZN5ModelD1Ev(char*);
extern int _ZN5ActorD1Ev(char*);
extern int data_ov002_02108e5c[];
extern int _ZTV8Platform[];
char* func_ov002_020b4ed8(char* c){
  *(int**)c = data_ov002_02108e5c;
  _ZN9ModelAnimD1Ev(c+0x6b4);
  _ZN18MovingMeshColliderD1Ev(c+0x4ec);
  _ZN18MovingMeshColliderD1Ev(c+0x324);
  *(int**)c = _ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN5ActorD1Ev(c);
  return c;
}
}
