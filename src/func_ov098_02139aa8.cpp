//cpp
struct Sub {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void m5(int);
};
extern "C" int func_ov098_02139aa8(char* c){
  if(*(int*)(c+0x560) == 6) return 1;
  {
    int b = (int)((*(int*)(c+0xb0) & 0x40000) != 0);
    if(b) return 1;
  }
  Sub* o = (Sub*)(c+0xd4);
  o->m5(0);
  return 1;
}
