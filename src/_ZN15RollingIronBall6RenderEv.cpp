//cpp
struct EmbeddedClass {
  virtual void method(void* a);
  virtual void dummy1();
  virtual void dummy2();
  virtual void dummy3();
  virtual void dummy4();
  virtual void virtualMethod(char* a);
};

extern "C" int _ZN15RollingIronBall6RenderEv(char* c){
  unsigned char b = *(unsigned char*)(c+0x3d0);
  if(b){
    EmbeddedClass* e = (EmbeddedClass*)(c+0x2cc);
    e->virtualMethod(c+0x3ac);
  }
  return 1;
}
