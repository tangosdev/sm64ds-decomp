//cpp
// NONMATCHING: different op / idiom (div=42). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct V3 { int x, y, z; };
extern short SINE_TABLE[];
extern "C" void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, struct V3 *pos, void *v16, int d, int e);
extern "C" void func_0201267c(int a, void *p);
extern "C" void func_ov070_02121880(void *c, int i);
extern "C" void _ZN9Animation7AdvanceEv(void *thiz);
extern "C" int func_ov070_02121a64(void *p);
extern "C" void func_ov070_02121298(char *c);
extern "C" void func_ov070_021211c4(char *c);
extern "C" void _ZN12CylinderClsn5ClearEv(void *thiz);
extern "C" void _ZN12CylinderClsn6UpdateEv(void *thiz);

extern "C" int func_ov070_0212156c(char *c){
  if(*(int*)(c+0x398) == 0x1e){
    struct V3 pos;
    int idx = (unsigned short)*(short*)(c+0x8e) >> 4;
    pos.x = *(int*)(c+0x5c) + (int)(((long long)SINE_TABLE[idx*2] * 0x50000 + 0x800) >> 12);
    pos.y = *(int*)(c+0x60) - 0x29000;
    pos.z = *(int*)(c+0x64) + (int)(((long long)SINE_TABLE[idx*2+1] * 0x50000 + 0x800) >> 12);
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x10f, 0, &pos, c+0x8c, *(signed char*)(c+0xcc), -1);
    func_0201267c(0x105, c+0x74);
  }
  if(*(int*)(c+0x398) == *(int*)(c+0x394)){
    func_ov070_02121880(c, 0);
  }
  _ZN9Animation7AdvanceEv(c+0x124);
  {
    int r = func_ov070_02121a64(c+0x38c);
    *(int*)(c+0x80) = r;
    *(int*)(c+0x84) = r;
    *(int*)(c+0x88) = r;
  }
  func_ov070_02121298(c);
  func_ov070_021211c4(c);
  _ZN12CylinderClsn5ClearEv(c+0x160);
  _ZN12CylinderClsn6UpdateEv(c+0x160);
  return 1;
}
