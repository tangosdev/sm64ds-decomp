// NONMATCHING: base materialization / addressing (div=5). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.

struct HeapAllocator;
void _ZN18NestedHeapIterator4InitEP13HeapAllocator(char *it, char *a)
{
  char *new_var;
  unsigned short *cnt = (unsigned short *) (it + 8);
  new_var = a + (*((unsigned short *) (it + 0xa)));
  *((int *) ((a + (*((unsigned short *) (it + 0xa)))) + 4)) = 0;
  *((int *) new_var) = 0;
  *((int *) it) = (int) a;
  *((int *) (it + 4)) = (int) a;
  *cnt = 1;
  *cnt = (*cnt) + (*cnt);
}
