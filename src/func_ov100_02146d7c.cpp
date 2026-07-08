//cpp
extern "C" {
extern int data_ov100_0214857c[];
extern int _ZTV8PathLift[];
extern int _ZTV8Platform[];
void ModelAnim_dtor(void*);
void func_0207328c(void*,int,int,void*);
void MovingMeshCollider_dtor(void*);
void Model_dtor(void*);
void Actor_dtor(void*);
}
extern "C" int func_ov100_02146d7c(char* c){
  *(int**)c=(int*)data_ov100_0214857c;
  ModelAnim_dtor(c+0x450);
  *(int**)c=(int*)_ZTV8PathLift;
  func_0207328c(c+0x320,3,0x50,(void*)Model_dtor);
  *(int**)c=(int*)_ZTV8Platform;
  MovingMeshCollider_dtor(c+0x124);
  Model_dtor(c+0xd4);
  Actor_dtor(c);
  return (int)c;
}
