//cpp
// _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE at 0x0204e40c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
extern "C" {

struct MemoryNode {
  unsigned short tag;
  unsigned short pad;
  int size;
  int prev;
  int next;
};

struct Target {
  int start;
  int end;
};

extern void* _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(void* c, MemoryNode* node);
extern MemoryNode* _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(Target* t, unsigned short tt);
extern void* _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(void* c, MemoryNode* node, MemoryNode* r2);

int _ZN22ExpandingHeapAllocator8FreeNodeEP10MemoryNodePNS0_6TargetE(int* c, Target* node, void* target) {
  Target tgt = *node;

  MemoryNode* below = 0;
  MemoryNode* p = (MemoryNode*)c[0];
  while (p) {
    if ((unsigned)p < (unsigned)node->start) {
      below = p;
    } else {
      if ((unsigned)p == (unsigned)node->end) {
        tgt.end = p->size + ((int)p + 0x10);
        _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(c, p);
      }
      break;
    }
    p = (MemoryNode*)p->next;
  }

  if (below) {
    int e = below->size + ((int)below + 0x10);
    if (e == node->start) {
      tgt.start = (int)below;
      below = (MemoryNode*)_ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(c, below);
    }
  }

  if ((unsigned)(tgt.end - tgt.start) < 0x10u) {
    return 0;
  }

  MemoryNode* nn = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&tgt, 0x4652);
  _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(c, nn, below);
  return 1;
}

}
