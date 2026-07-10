//cpp
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual void g5(void*); };
extern "C" int _ZN9PushBlock6RenderEv(char* c){
  int f = (int)((*(int*)(c+0xb0) & 0x40000) != 0);
  if (f != 0) return 1;
  unsigned char st = *(unsigned char*)(c+0x3ca);
  if (st < 0x2d && (st & 1)) return 1;
  switch (*(int*)(c+0x3c0)) {
  case 0: ((Sub*)(c+0xd4))->g5(c+0x80); break;
  case 1: ((Sub*)(c+0x124))->g5(c+0x80); break;
  case 2: ((Sub*)(c+0x124))->g5(c+0x80); break;
  }
  return 1;
}
