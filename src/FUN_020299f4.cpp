//cpp
struct A {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6(); virtual int v7();
};
struct B {
  virtual void w0(); virtual void w1(); virtual void w2();
  virtual void w3(); virtual void w4(int,int); virtual void w5();
  virtual void w6(); virtual void w7(); virtual void w8(); virtual void w9();
};
extern "C" { extern A* SCENE_FADER; extern char* WIPES; extern B* KS_FADER; }
extern "C" void FUN_020299f4(void){
  if(SCENE_FADER->v7() != 0) return;
  B* o2 = (B*)(WIPES + 0x240);
  KS_FADER = o2;
  o2->w9();
  o2->w4(0x1e, 0);
}
