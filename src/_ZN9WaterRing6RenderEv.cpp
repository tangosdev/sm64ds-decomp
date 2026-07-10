//cpp
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };
extern "C" {
int _ZN9WaterRing6RenderEv(char* c){
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x35c, c+0x314);
  ((Sub*)(c+0x30c))->g5(c+0x80);
  return 1;
}
}
