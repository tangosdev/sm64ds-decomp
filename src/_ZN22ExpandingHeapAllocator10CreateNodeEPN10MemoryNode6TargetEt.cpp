//cpp
extern "C" {
void* _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(int* c, unsigned short t) {
  char* n = (char*)c[0];
  *(unsigned short*)(n+0) = t;
  *(unsigned short*)(n+2) = 0;
  *(int*)(n+4) = ((int)c[1]) - (int)(n + 0x10);
  *(int*)(n+8) = 0;
  *(int*)(n+0xc) = 0;
  return n;
}
}
