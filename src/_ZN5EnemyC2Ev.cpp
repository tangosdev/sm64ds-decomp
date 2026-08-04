//cpp
extern "C" {
extern int _ZN5ActorC2Ev(void*);
extern int data_ov002_021081e4[];
int _ZN5EnemyC2Ev(void* c){
  _ZN5ActorC2Ev(c);
  *(int*)c=(int)data_ov002_021081e4;
  return (int)c;
}
}
