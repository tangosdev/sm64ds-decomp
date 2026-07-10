//cpp
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };
extern "C" {
int _ZN3MrI6RenderEv(char* c){
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x138, c+0xdc);
  ((Sub*)(c+0xd4))->g5(c+0x80);
  return 1;
}
}
