//cpp
extern "C" {
int _ZN4cstd3absEi(int);
struct EHA;
int _ZN22ExpandingHeapAllocator18MaxAllocatableSizeEi(EHA* thiz, int arg) {
  int align = _ZN4cstd3absEi(arg);
  unsigned int bestSize = 0;
  unsigned int bestOffset = -1;
  char* n = *(char**)((char*)thiz + 0x24);
  while (n) {
    unsigned int base = (unsigned int)(n + 0x10);
    unsigned int aligned = ((align - 1) + base) & ~(align - 1);
    unsigned int end = *(int*)(n + 4) + base;
    if (aligned < end) {
      unsigned int usable = end - aligned;
      unsigned int offset = aligned - base;
      if (bestSize < usable || (bestSize == usable && bestOffset > offset)) {
        bestSize = usable;
        bestOffset = offset;
      }
    }
    n = *(char**)(n + 0xc);
  }
  return bestSize;
}
}
