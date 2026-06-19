//cpp
extern "C" {
struct N { int p0,p1,prev,next; };
void* _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(int* c, N* node) {
  N* r2 = (N*)node->prev;
  N* r1 = (N*)node->next;
  if (r2) r2->next = (int)r1;
  else c[0] = (int)r1;
  if (r1) r1->prev = (int)r2;
  else c[1] = (int)r2;
  return r2;
}
}
