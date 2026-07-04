//cpp
extern "C" {
extern void func_ov063_0211c684(char* c);
extern int func_ov063_0211c6f8(char* c);
}
struct C;
typedef void (C::*PMF)();
extern PMF data_ov063_0211ef38[];

extern "C" int func_ov063_0211ce74(char* c);
extern "C" int func_ov063_0211ce74(char* c){
  unsigned char before = *(unsigned char*)(c + 0x150);
  int idx = *(int*)(c + 0x140);
  C* obj = (C*)c;
  (obj->*data_ov063_0211ef38[idx])();
  unsigned short* ctr = (unsigned short*)(((int)c + 0x14c) & 0xFFFFFFFFFFFFFFFF);
  *ctr = *ctr + 1;
  if (before != *(unsigned char*)(c + 0x150)) {
    unsigned short* base = (unsigned short*)(((int)c + 0x100) & 0xFFFFFFFFFFFFFFFF);
    base[0x4c/2] = 0;
  }
  func_ov063_0211c684(c);
  func_ov063_0211c6f8(c);
  *(int*)(c + 0x124) = 0;
  return 1;
}
