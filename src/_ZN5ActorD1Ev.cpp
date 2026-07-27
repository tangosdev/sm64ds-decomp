//cpp
extern "C" {
extern int data_0208e3a4[];
extern int data_0209b468[];
extern int data_0208e4b8[];
extern void func_0203b27c(int a, int c);
extern void func_02044104(int a);
extern void _ZN9ActorBaseD2Ev(int c);
int _ZN5ActorD1Ev(int c) {
  *(int*)c = (int)data_0208e3a4;
  func_0203b27c((int)data_0209b468, c+0x50);
  func_02044104(c+0x50);
  *(int*)c = (int)data_0208e4b8;
  _ZN9ActorBaseD2Ev(c);
  return c;
}
}
