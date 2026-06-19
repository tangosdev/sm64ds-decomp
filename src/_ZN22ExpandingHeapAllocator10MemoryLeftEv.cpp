//cpp
extern "C" {
struct EHA;
unsigned int _ZN22ExpandingHeapAllocator10MemoryLeftEv(EHA* thiz) {
  void* n = *(void**)((char*)thiz + 0x24);
  unsigned int total = 0;
  while (n) {
    total += *(unsigned int*)((char*)n + 4);
    n = *(void**)((char*)n + 0xc);
  }
  return total;
}
}
