// NONMATCHING: different op / idiom (div=11). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_ov002_020db8bc(unsigned char* p, unsigned char val);
extern void func_ov002_020bd984(void* c, int x);
extern void* CAMERA;
void func_ov002_020bdd2c(void* c) {
  if (*(unsigned char*)((char*)c+0x703) != 1) return;
  func_ov002_020db8bc((unsigned char*)c, 3);
  {
    int* f = (int*)((char*)CAMERA+0x154);
    *f &= ~0x80;
  }
  func_ov002_020bd984(c, 0x31);
  *(int*)((char*)c+0x638) = 0;
  *(int*)((char*)c+0x634) = *(int*)((char*)c+0x638);
  *(int*)((char*)c+0x628) = 0;
  *(short*)((char*)c+0x6d0) = 0;
  *(unsigned char*)((char*)c+0x703) = 0;
}
