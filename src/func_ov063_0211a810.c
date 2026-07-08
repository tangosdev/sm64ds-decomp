// NONMATCHING: base materialization / addressing (div=14). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern short SINE_TABLE[];

void func_ov063_0211a810(char* r0, int cond){
  int* f = (int*)(r0 + 0x19c);
  short* h = (short*)(r0 + 0x5d4);
  *f |= 1;
  *h = *h & ~4;
  *(short*)(r0 + 0x5b4) = *(short*)(r0 + 0x94);
  if (cond != 0) {
    char* o = *(char**)(r0 + 0x484);
    *(short*)(r0 + 0x5b6) = *(short*)(o + 0x8e);
    return;
  }
  short base = *(short*)(r0 + 0x94);
  short d = (short)(base - *(short*)(r0 + 0x5b0));
  int idx = ((unsigned short)d) >> 4;
  short t = SINE_TABLE[idx*2 + 1];
  if (t < 0) *(short*)(r0 + 0x5b6) = base;
  else *(short*)(r0 + 0x5b6) = base + 0x8000;
}
