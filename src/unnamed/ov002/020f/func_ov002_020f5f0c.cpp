//cpp
extern "C" {
void func_ov002_020f2e30(void*, int);
}
struct C;
typedef void (C::*PMF)();
extern PMF data_ov002_02110f9c[];
extern "C" void func_ov002_020f5f0c(C* c, int idx){
  (c->*data_ov002_02110f9c[idx])();
  func_ov002_020f2e30((void*)c, idx);
}
