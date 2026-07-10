//cpp
// NONMATCHING: different op / idiom (div=8). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12i;
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov064_0211c904[];
struct C { unsigned char pad[0x336]; unsigned char idx; };
extern "C" void func_ov064_02118ee4(C* c);
extern "C" char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern "C" void func_ov064_02119010(char* c);
extern "C" int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char* c, Fix12i a, Fix12i b);
extern "C" void func_ov064_02118fa4(char* c);

extern "C" int _ZN19BowserPuzzleManager8BehaviorEv(C* c){
  func_ov064_02118ee4(c);
  (c->*data_ov064_0211c904[c->idx].pmf)();
  char* cc = (char*)c;
  char* p = 0;
  unsigned int id = *(unsigned int*)(cc+0x320);
  if (id != 0)
    p = _ZN5Actor10FindWithIDEj(id);
  if (p != 0 && *(unsigned char*)(p+0xd6) == 0) {
    unsigned short* ctr = (unsigned short*)(cc+0x334);
    *ctr = *ctr + 1;
    func_ov064_02119010(cc);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(cc, 0, 0) != 0)
      func_ov064_02118fa4(cc);
  }
  return 1;
}
