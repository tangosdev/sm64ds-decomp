//cpp
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
struct Sub {
  virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
  virtual void v10(); virtual void m14(int arg);
};
struct Obj {
  char pad[0xd4];
  Sub sub;
};
extern "C" int _ZN11CannonHatch6RenderEv(Obj* c){
  if (data_0209f2f8 == 6 && data_0209f220 == 1 && (*(int*)((char*)c + 8) & 0xff) == 1)
    return 1;
  if (*(unsigned char*)((char*)c + 0x32e) != 0)
    return 1;
  Sub* b = &c->sub;
  b->m14(0);
  return 1;
}
