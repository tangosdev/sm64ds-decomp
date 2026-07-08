//cpp
// NONMATCHING: register allocation (div=17). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
struct State;
extern State _ZN6Player14ST_YOSHI_POWERE;
extern int func_ov002_020d600c(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, State* st);
int func_ov002_020d5c6c(char* c){
  unsigned short f = *(unsigned short*)(c + 0x6ce) & 2;
  if (f == 0) return 0;
  if (*(int*)(c + 0x360) != 0){
    if (func_ov002_020d600c(c)) goto change;
  }
  *(unsigned short*)(c + 0x6ce) &= ~2;
  return 0;
change:
  _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player14ST_YOSHI_POWERE);
  return 1;
}
}
