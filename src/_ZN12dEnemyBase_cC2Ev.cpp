//cpp
extern "C" {
extern int _ZN8dActor_cC2Ev(void*);
extern int data_ov002_021081e4[];
int _ZN12dEnemyBase_cC2Ev(void* c){
  _ZN8dActor_cC2Ev(c);
  *(int*)c=(int)data_ov002_021081e4;
  return (int)c;
}
}
