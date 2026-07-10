//cpp
struct Obj {
  virtual void m0(); virtual void m1(); virtual void m2();
  virtual void m3(); virtual void m4(); virtual void doit(int);
};
extern "C" int _ZN5Spiny6RenderEv(char *c){
  if((*(unsigned*)(c+0xb0) & 0x40000) ? 1 : 0) return 1;
  int s=*(int*)(c+0x3d8);
  if(s==0 || s==4) ((Obj*)(c+0xd4))->doit(0);
  else ((Obj*)(c+0x124))->doit(0);
  return 1;
}
