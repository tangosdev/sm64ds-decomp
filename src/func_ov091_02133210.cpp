//cpp
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(int); };
extern "C" {
int func_ov091_02133210(char* c){
  int check = *(int*)(*(int*)(c+0x320)+0xc);
  if (check != 0)
    _ZN15TextureSequence6UpdateER15ModelComponents(c+0x324, c+0xdc);
  ((Sub*)(c+0xd4))->g5(0);
  return 1;
}
}
