//cpp
extern "C" {
int _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(char* c, char* h) {
  if (h == 0) return *(int*)(c+4);
  unsigned short off = *(unsigned short*)(c+0xa);
  return *(int*)(h+off);
}
}
