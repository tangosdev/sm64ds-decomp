//cpp
extern "C" {
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* thiz, void* mc);
extern unsigned char data_ov066_0211ae04;
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };
extern "C" int func_ov066_021197a4(char* c);
extern "C" int func_ov066_021197a4(char* c){
  if (*(int*)(c + 0x49c) == 0) {
    if (data_ov066_0211ae04 == 1) {
      ((Sub*)(c + 0x3d0))->g5(0);
    }
    return 1;
  }
  if (data_ov066_0211ae04 == 1) return 1;
  _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x448, c + 0x368);
  ((Sub*)(c + 0x360))->g5(0);
  return 1;
}
