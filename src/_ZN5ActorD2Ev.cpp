//cpp
extern "C" {
extern int _ZTV5Actor[];
extern int FIRST_ACTOR_LIST_NODE[];
extern int _ZTV12ActorDerived[];
extern void func_0203b27c(int a, int c);
extern void func_02044104(int a);
extern void _ZN9ActorBaseD1Ev(int c);
int _ZN5ActorD2Ev(int c) {
  *(int*)c = (int)_ZTV5Actor;
  func_0203b27c((int)FIRST_ACTOR_LIST_NODE, c+0x50);
  func_02044104(c+0x50);
  *(int*)c = (int)_ZTV12ActorDerived;
  _ZN9ActorBaseD1Ev(c);
  return c;
}
}
