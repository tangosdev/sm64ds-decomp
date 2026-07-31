//cpp
extern "C" {
struct Inner { char pad[0x10]; unsigned short id; };
struct EHA { char pad[0x24]; struct Inner inner; };
}

struct ExpandingHeapAllocator {
    int SetNodeID(unsigned int id);
};

int ExpandingHeapAllocator::SetNodeID(unsigned int id)
{
    EHA* thiz = (EHA*)this;
struct Inner* p = &thiz->inner;
  unsigned short old = p->id;
  p->id = (unsigned short)id;
  return old;

}
