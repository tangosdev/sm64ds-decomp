//cpp
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* self, char* cc);
extern void func_ov064_02119f1c(char* c);
extern void _ZN12CylinderClsn5ClearEv(char* c);
extern void _ZN12CylinderClsn6UpdateEv(char* c);
extern void _ZN9Animation7AdvanceEv(char* c);
}
struct C;
typedef void (C::*PMF)();

extern "C" int func_ov064_02119ffc(char* r4);
extern "C" int func_ov064_02119ffc(char* r4){
  DecIfAbove0_Short((unsigned short*)(r4 + 0x100));
  char* obj = *(char**)(r4 + 0x370);
  if (*(int*)(obj + 8) != 0) {
    PMF* p = (PMF*)(obj + 8);
    C* c = (C*)r4;
    (c->**p)();
  }
  _ZN5Actor9UpdatePosEP12CylinderClsn(r4, r4 + 0x110);
  *(short*)(r4 + 0x8c) = *(short*)(r4 + 0x92);
  *(short*)(r4 + 0x8e) = *(short*)(r4 + 0x94);
  *(short*)(r4 + 0x90) = *(short*)(r4 + 0x96);
  func_ov064_02119f1c(r4);
  _ZN12CylinderClsn5ClearEv(r4 + 0x110);
  _ZN12CylinderClsn6UpdateEv(r4 + 0x110);
  _ZN9Animation7AdvanceEv(r4 + 0x35c);
  return 1;
}
