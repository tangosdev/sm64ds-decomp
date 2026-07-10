//cpp
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };
extern "C" {
int _ZN7Wiggler6RenderEv(char* c){
  int i = 0;
  char* p6 = c+0x110;
  char* p5 = c+0x368;
  char* p4 = c+0x408;
  for(;i<5;i++){
    _ZN15TextureSequence6UpdateER15ModelComponents(p5, p6+8);
    ((Sub*)p6)->g5(p4);
    p6 += 0x64;
    p5 += 0x14;
    p4 += 0xc;
  }
  return 1;
}
}
