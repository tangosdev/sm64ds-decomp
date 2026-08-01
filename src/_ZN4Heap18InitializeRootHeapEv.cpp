//cpp
#include "types.h"
/* Heap::InitializeRootHeap() at 0x0203cae8 -- clears Memory::rootParamOffset
 * then tail-calls Heap::SetupRootHeap(). */
namespace Memory
{
    extern u32 rootParamOffset;
}

class Heap
{
public:
    static void SetupRootHeap();
    static void InitializeRootHeap();
};

void Heap::InitializeRootHeap()
{
    Memory::rootParamOffset = 0;
    SetupRootHeap();
}
