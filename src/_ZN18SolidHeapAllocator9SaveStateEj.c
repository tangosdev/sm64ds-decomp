// NONMATCHING: base materialization / addressing (div=1). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.

typedef unsigned int u32;
struct FreeList
{
  void *begin;
  void *end;
  void *tail;
};
extern void *_ZN18SolidHeapAllocator16AllocateForwardsEPvjj(void *pair, u32 size, u32 align);
inline struct FreeList *inline_fn(void *arg0)
{
  return (struct FreeList *) (((char *) arg0) + 0x24);
}

int _ZN18SolidHeapAllocator9SaveStateEj(void *c, u32 arg)
{
  struct FreeList *fb = inline_fn(c);
  void *saved = fb->begin;
  int *p = (int *) _ZN18SolidHeapAllocator16AllocateForwardsEPvjj(fb, 0x10, 4);
  if (!p)
  {
    return 0;
  }
  p[0] = arg;
  p[1] = (int) saved;
  p[2] = (int) fb->end;
  p[3] = (int) fb->tail;
  fb->tail = p;
  return 1;
}
