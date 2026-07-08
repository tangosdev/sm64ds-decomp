// NONMATCHING: missing logic (ROM does more) (div=29). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef long long s64;
extern s16 SINE_TABLE[];
extern u16 DecIfAbove0_Short(u16* p);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char* c, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(char* c);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char* c);

int func_ov002_020b6494(char* c){
  if (DecIfAbove0_Short((u16*)(c+0x32a)) != 0) {
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
    return 1;
  }
  *(u16*)(c+0x328) += 0x100;
  {
    s16 a = SINE_TABLE[((u16)*(s16*)(c+0x328) >> 4) * 2];
    *(int*)(c+0x60) = *(int*)(c+0x60) - (int)((((s64)*(int*)(c+0x324) * a) + 0x800) >> 0xc);
  }
  {
    int d = *(int*)(c+0x60) - *(int*)(c+0x320);
    if (d < 0) d = -d;
    if (d == 0) *(u16*)(c+0x32a) = 0x3c;
  }
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
  return 1;
}
