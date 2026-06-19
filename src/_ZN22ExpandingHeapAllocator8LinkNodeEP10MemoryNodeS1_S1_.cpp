//cpp
extern "C" {
void* _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(int* c, int node, int r2) {
  char* n = (char*)node;
  *(int*)(n+8) = r2;
  int r3;
  if (r2) { r3 = *(int*)((char*)r2+0xc); *(int*)((char*)r2+0xc) = node; }
  else { r3 = c[0]; c[0] = node; }
  *(int*)(n+0xc) = r3;
  if (r3) { *(int*)((char*)r3+8) = node; }
  else { c[1] = node; }
  return n;
}
}
