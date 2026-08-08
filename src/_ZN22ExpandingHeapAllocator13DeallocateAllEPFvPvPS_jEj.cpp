//cpp
extern "C" {
typedef void (*Fn)(void*, void*, void*);
void* _ZN22ExpandingHeapAllocator13DeallocateAllEPPFvPvPS_jEj(void* thiz, Fn fn, void* ctx) {
  void* node = *(void**)((char*)thiz + 0x2c);
  if (!node) return node;
  do {
    void* next = *(void**)((char*)node + 0xc);
    fn((char*)node + 0x10, thiz, ctx);
    node = next;
  } while (node);
  return node;
}
}
