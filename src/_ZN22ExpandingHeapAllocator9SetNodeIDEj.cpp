//cpp
extern "C" {
struct Inner { char pad[0x10]; unsigned short id; };
struct EHA { char pad[0x24]; struct Inner inner; };
int _ZN22ExpandingHeapAllocator9SetNodeIDEj(struct EHA* thiz, unsigned int id) {
  struct Inner* p = &thiz->inner;
  unsigned short old = p->id;
  p->id = (unsigned short)id;
  return old;
}
}
