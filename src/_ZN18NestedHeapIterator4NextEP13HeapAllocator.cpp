//cpp
extern "C" {
int _ZN18NestedHeapIterator4NextEP13HeapAllocator(char* c, char* h) {
  if (h == 0) return *(int*)c;
  unsigned short off = *(unsigned short*)(c+0xa);
  return *(int*)(h+off+4);
}
}
