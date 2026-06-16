//cpp
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };
extern "C" {
int func_ov079_02125f78(char* c){
  if(*(unsigned char*)(c+0x404)==0) return 1;
  if(*(unsigned char*)(c+0x414)!=0)
    _ZN15TextureSequence6UpdateER15ModelComponents(c+0x330, c+0x2d4);
  ((Sub*)(c+0x2cc))->g5(0);
  return 1;
}
}
