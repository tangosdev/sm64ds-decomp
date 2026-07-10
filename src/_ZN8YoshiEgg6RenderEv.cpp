//cpp
struct Obj {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void m(int a);
};
extern "C" {
extern int _ZN6Player16IsInsideOfCannonEv(void*);
int _ZN8YoshiEgg6RenderEv(char* c){
  int b = (int)((*(unsigned int*)(c+0xb0) & 0x40000) != 0);
  if(b) return 1;
  if(_ZN6Player16IsInsideOfCannonEv(*(void**)(c+0x38c))) return 1;
  if(*(unsigned char*)(*(char**)(c+0x38c)+0x6f5) < 1) return 1;
  ((Obj*)(c+0x300))->m(0);
  return 1;
}
}
