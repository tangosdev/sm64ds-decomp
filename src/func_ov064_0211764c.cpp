//cpp
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };
extern "C" {
int func_ov064_0211764c(char* c){
  if(*(unsigned char*)(c+0x3fe) >= 3)
    ((Sub*)(c+0x110))->g5(0);
  return 1;
}
}
