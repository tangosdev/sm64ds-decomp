//cpp
#include "types.h"
/* Heap::InitializeRootHeap() at 0x0203cae8 -- clears ROOT_HEAP_ARENA_ID then
 * tail-calls Heap::SetupRootHeap().
 *
 * The word at 0x020a0ea4 is the NitroSDK arena id the root heap is carved
 * from: Heap::SetupRootHeap loads it six times and passes it as the first
 * argument to the OS arena calls at 0x02058cd0/0x02058d58/0x02058ea0/
 * 0x02058eb4/0x02059040. Zeroing it here selects OS_ARENA_MAIN. An earlier
 * recovery pass called it `Memory::rootParamOffset', which the callers do
 * not support. */
extern "C" u32 ROOT_HEAP_ARENA_ID;

class Heap
{
public:
    static void SetupRootHeap();
    static void InitializeRootHeap();
};

void Heap::InitializeRootHeap()
{
    ROOT_HEAP_ARENA_ID = 0;
    SetupRootHeap();
}
