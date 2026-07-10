//cpp
extern "C" {
extern char data_ov094_02136b40[];
}
struct O {
  virtual void m0();
  virtual void m1();
  virtual void m2();
  virtual void m3();
  virtual void m4();
  virtual void m5(int);
};
extern "C" int _ZN10HootTheOwl6RenderEv(char *t){
  if(*(void**)(t+0x3c8) == (void*)data_ov094_02136b40) return 1;
  ((O*)(t+0x30c))->m5(0);
  return 1;
}
