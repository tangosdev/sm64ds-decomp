//cpp
struct Sub {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void m5(int);
};
extern "C" int func_ov098_0213a314(char* c){
  if(*(unsigned char*)(c+0x342)) return 1;
  if(*(unsigned char*)(c+0x33c) == 3) return 1;
  Sub* o = (Sub*)(c+0xd4);
  o->m5(0);
  return 1;
}
