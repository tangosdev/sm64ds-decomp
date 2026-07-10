//cpp
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };
extern "C" {
int _ZN9LakituBro6RenderEv(char* c){
  if (*(unsigned char*)(c+0x2dc) == 1) return 1;
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x1d8, c+0x118);
  ((Sub*)(c+0x110))->g5(0);
  return 1;
}
}
