//cpp
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short* p);
void func_ov064_0211987c(void* c);
}
struct C;
typedef void (C::*PMF)();
struct Obj { int x; int y; PMF pmf; };
struct C {
  char pad[0x300];
  Obj* obj;
};
extern "C" int _ZN17BowserPuzzlePiece8BehaviorEv(C* c){
  DecIfAbove0_Short((unsigned short*)((char*)c+0x100));
  if(c->obj->pmf){
    (c->*(c->obj->pmf))();
  }
  *(short*)((char*)c+0x8c) = *(short*)((char*)c+0x92);
  *(short*)((char*)c+0x8e) = *(short*)((char*)c+0x94);
  *(short*)((char*)c+0x90) = *(short*)((char*)c+0x96);
  func_ov064_0211987c(c);
  return 1;
}
