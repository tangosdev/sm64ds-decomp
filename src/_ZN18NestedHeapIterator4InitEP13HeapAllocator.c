struct HeapAllocator;
void _ZN18NestedHeapIterator4InitEP13HeapAllocator(char *it, char *a)
{
  char *new_var = a + (*((unsigned short *) (it + 0xa)));
  *((int *) ((a + (*((unsigned short *) (it + 0xa)))) + 4)) = 0;
  *((int *) new_var) = 0;
  *((int *) it) = (int) a;
  *((int *) (it + 4)) = (int) a;
  *(unsigned short *)(((long long)(int)(it + 8)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
}
