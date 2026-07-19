//cpp
extern "C" {

struct MemoryNode {
  unsigned short tag;
  unsigned short flags;
  unsigned int size;
  MemoryNode* prev;
  MemoryNode* next;
};

struct NodeList {
  MemoryNode* head;
  MemoryNode* tail;
  unsigned char pad[0xa];
  unsigned short flag;
};

void* _ZN22ExpandingHeapAllocator12AllocateNodeEP10MemoryNodeS1_Pvjj(NodeList* c, MemoryNode* node, void* target, unsigned int size, unsigned int z);

void* _ZN22ExpandingHeapAllocator17AllocateBackwardsEjj(void* thiz, unsigned int size, unsigned int align) {
  NodeList* c = (NodeList*)((char*)thiz + 0x24);
  unsigned short flag = c->flag;
  int firstFit = ((unsigned short)(flag & 1) == 0);
  MemoryNode* best = 0;
  MemoryNode* node = c->tail;
  unsigned int bestSize = 0xFFFFFFFF;
  void* bestTarget = 0;
  if (node != 0) {
    unsigned int mask = align - 1;
    do {
      char* data = (char*)node + 0x10;
      unsigned int nsize = node->size;
      unsigned int t = nsize + (unsigned int)data - size;
      char* aligned = (char*)(~mask & t);
      if ((int)(aligned - data) >= 0 && bestSize > nsize) {
        best = node;
        bestSize = nsize;
        bestTarget = aligned;
        if (firstFit) break;
        if (nsize == size) break;
      }
      node = node->prev;
    } while (node != 0);
  }
  if (best == 0) return 0;
  return _ZN22ExpandingHeapAllocator12AllocateNodeEP10MemoryNodeS1_Pvjj(c, best, bestTarget, size, 1);
}
}