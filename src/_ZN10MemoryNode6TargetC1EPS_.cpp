//cpp
// @symbol _ZN10MemoryNode6TargetC1EPS_
#include "MemoryNode.h"

MemoryNode::Target::Target(MemoryNode* node)
    : start((char*)node - (u16)((node->flags >> 8) & 0x7f)),
      end((char*)(node->size + (u32)((char*)node + 0x10)))
{
}
