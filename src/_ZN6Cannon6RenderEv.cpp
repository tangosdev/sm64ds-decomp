//cpp
struct Sub {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void m3();
  virtual void v4();
  virtual void m5(int);
};
extern "C" int _ZN6Cannon6RenderEv(char* c){
  if(*(int*)(c+0x180) == 3){
    if(*(unsigned char*)(c+0x185) >= 3) return 1;
  }
  Sub* o = (Sub*)(c+0xd4);
  o->m3();
  Sub* o2 = (Sub*)(c+0xd4);
  o2->m5(0);
  return 1;
}
