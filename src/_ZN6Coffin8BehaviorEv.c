// NONMATCHING: different op / idiom (div=3). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.

extern void func_ov071_02122080(char *t);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char *self);
extern void func_ov071_02122414(char *c);
int _ZN6Coffin8BehaviorEv(char *r4)
{
  char *new_var;
  if (((*((int *) (new_var = r4 + 8))) & 0xff) == 1)
  {
    func_ov071_02122080(r4);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(r4, 0, 0))
    {
      _ZN8Platform19UpdateClsnPosAndRotEv(r4);
    }
    return 1;
  }
  {
    unsigned short v328 = *((unsigned short *) (r4 + 0x328));
    *((unsigned short *) (r4 + ((unsigned char) 0x328))) = v328 + 1;
  }
  func_ov071_02122414(r4);
  func_ov071_02122080(r4);
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(r4, 0, 0))
  {
    _ZN8Platform19UpdateClsnPosAndRotEv(r4);
  }
  return 1;
}
