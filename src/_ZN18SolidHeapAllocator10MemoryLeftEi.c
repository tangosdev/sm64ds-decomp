// NONMATCHING: base materialization / addressing (div=3). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.

typedef unsigned int u32;
extern int _ZN4cstd3absEi(int x);
int _ZN18SolidHeapAllocator10MemoryLeftEi(void *c, int align)
{
  u32 a = (u32) _ZN4cstd3absEi(align);
  char *new_var2;
  u32 mask = a - 1;
  u32 new_var;
  int *fb = (int *) (((char *) c) + 0x24);
  u32 aligned = (mask + ((u32) ((int *) (((char *) c) + 0x24))[0])) & (~(a - 1));
  u32 end = (u32) fb[1];
  if (!(a - 1))
  {
  }
  new_var = end;
  if (aligned > new_var)
  {
    return 0;
  }
  new_var2 = (char *) c;
  return end - (((a - 1) + ((u32) ((int *) (new_var2 + 0x24))[0])) & (~(a - 1)));
}
