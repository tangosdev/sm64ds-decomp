//cpp
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };
extern "C" {
int _ZN11RollingRock6RenderEv(char* c){
  if(*(unsigned char*)(c+0x3be) >= 2)
    ((Sub*)(c+0x110))->g5(0);
  return 1;
}
}
