//cpp
// NONMATCHING: different op / idiom (div=47). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern short SINE_TABLE[];
extern char data_ov085_02130820[];
extern "C" void *_ZN5Actor13ClosestPlayerEv(void *thiz);
extern "C" void _Z14ApproachLinearRiii(int *p, int target, int step);
extern "C" int func_ov002_020c3ea0(char *c);
extern "C" unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int d, void *v, unsigned int e);
extern "C" void func_ov085_0212e728(void *c, void *p);

extern "C" int func_ov085_0212d9b8(char *c){
  void *pl = _ZN5Actor13ClosestPlayerEv(c);
  int step;
  if(pl == 0) return 1;
  *(int*)(c+0x2c8) += 1;
  *(int*)(c+0x2cc) += 0x500;
  _Z14ApproachLinearRiii((int*)(c+0x60),
    (int)(((long long)SINE_TABLE[((unsigned short)(short)*(int*)(c+0x2cc) >> 4)*2] * 0x1a000 + 0x800) >> 12) + *(int*)(c+0x2a8),
    0x10000000);
  if(*(int*)(c+0x2c8) == 0x32){
    func_ov002_020c3ea0((char*)pl);
    *(short*)(c+0x8e) = 0x6000;
    *(short*)(c+0x94) = *(short*)(c+0x8e);
    step = (*(int*)(c+0x2c8) >= 0x4b) ? 0x8000 : 0x4000;
    _Z14ApproachLinearRiii((int*)(c+0x98), step, 0x1000);
  }
  *(int*)(c+0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c+0x2e4), 3, 0x182, c+0x74, 0);
  if(*(int*)(c+0x2c8) > 0x78){
    *(short*)(c+0x8e) = 0x4000;
    *(short*)(c+0x94) = *(short*)(c+0x8e);
    func_ov085_0212e728(c, data_ov085_02130820);
  }
  return 1;
}
