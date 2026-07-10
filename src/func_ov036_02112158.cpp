//cpp
extern "C" {
extern int data_ov036_02113f9c[];
extern int data_ov002_0210af70[];
extern int _ZTV17ExclamationSwitch[];
void ModelAnim_dtor(void*);
void func_0207328c(void*,int,int,void*);
void MovingMeshCollider_dtor(void*);
void Model_dtor(void*);
void Actor_dtor(void*);
}
extern "C" int func_ov036_02112158(char* c){
  *(int**)c=(int*)data_ov036_02113f9c;
  ModelAnim_dtor(c+0x450);
  *(int**)c=(int*)data_ov002_0210af70;
  func_0207328c(c+0x320,3,0x50,(void*)Model_dtor);
  *(int**)c=(int*)_ZTV17ExclamationSwitch;
  MovingMeshCollider_dtor(c+0x124);
  Model_dtor(c+0xd4);
  Actor_dtor(c);
  return (int)c;
}
