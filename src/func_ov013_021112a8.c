//cpp
// NONMATCHING: base materialization / addressing (div=10). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, void*);
void func_ov013_02111238(char* t);
extern signed char NEXT_LEVEL_ID[];
int func_ov013_021112a8(char* c){
  if(NEXT_LEVEL_ID[0] <= 0){
    short* p90 = (short*)(c+0x90);
    if(*p90 > 0) *(short*)(c+0x124) -= 8;
    else         *(short*)(c+0x124) += 8;
    *p90 = (short)(*p90 + *(short*)(c+0x124));
    short w = *(short*)(c+0x124);
    if(w==0x10 || w==-0x10){
      _ZN5Sound9PlayBank3EjRK7Vector3(0x16, c+0x74);
      func_ov013_02111238(c);
    }
  }
  return 1;
}
}
