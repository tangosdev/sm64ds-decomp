//cpp
// NONMATCHING: base materialization / addressing (div=26). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void func_ov098_0213b15c(void*);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
extern char data_ov098_0213c8e8;
extern char data_ov098_0213c91c;
extern char BUBBLE_MODEL_PTR;
int func_ov098_0213b2d4(char* c){
  void* f;
  char* p60;
  unsigned char st;
  f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov098_0213c8e8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
  {
    int v194 = *(int*)(*(char**)(c + 0xe4) + 0x58);
    p60 = c + 0x60;
    *(int*)(c + 0x194) = v194;
  }
  st = *(int*)(c + 8) & 3;
  *(unsigned char*)(c + 0x184) = st;
  *(int*)(c + 0x174) = 0;
  *(int*)p60 = *(int*)p60 - 0x50000;
  *(int*)(c + 0x15c) = *(int*)(c + 0x5c);
  *(int*)(c + 0x160) = *(int*)(c + 0x60);
  *(int*)(c + 0x164) = *(int*)(c + 0x64);
  *(short*)(c + 0x17c) = *(short*)(c + 0x94);
  *(unsigned char*)(c + 0x185) = 0;
  if (*(unsigned char*)(c + 0x184) != 2) {
    if (*(unsigned char*)(c + 0x184) == 1) *(short*)(c + 0x8c) = 0x2000;
    *(short*)(c + 0x178) = 0x2000;
    *(short*)(c + 0x17a) = *(short*)(c + 0x8e);
    *(int*)(c + 0x180) = 0;
    func_ov098_0213b15c(c);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x124, c, 0xa0000, 0x12c000, 0x800004, 0);
  } else {
    char* pb0;
    *(int*)p60 = *(int*)p60 - 0x190000;
    *(short*)(c + 0x17a) = *(short*)(c + 0x90);
    *(short*)(c + 0x178) = 0x2000;
    *(int*)(c + 0x180) = 2;
    pb0 = c + 0xb0;
    *(int*)pb0 = *(int*)pb0 & ~1;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x124, c, 0x50000, 0x12c000, 0x800004, 0);
  }
  _ZN5Model8LoadFileER13SharedFilePtr(&data_ov098_0213c91c);
  _ZN5Model8LoadFileER13SharedFilePtr(&BUBBLE_MODEL_PTR);
  return 1;
}
}
