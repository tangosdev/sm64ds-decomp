#include "types.h"
/* func_02017060 at 0x02017060
 * Reallocates a file/object on the game heap using its own reported size.
 * arg0 = ptr; if null returns 0.
 * Calls Heap::_Sizeof(gameHeap, ptr) [result discarded],
 * then calls func_020469e0(ptr) to get the target size,
 * then Heap::Reallocate(gameHeap, ptr, new_size). Returns result. */
struct Heap;

extern struct Heap* gGameHeapPtr; /* at 0x020a0eac: Memory::gameHeapPtr */
extern u32 _ZN4Heap7_SizeofEPv(struct Heap* heap, void* ptr);
extern u32 func_020469e0(void* ptr);
extern u32 _ZN4Heap10ReallocateEPvj(struct Heap* heap, void* ptr, u32 newSize);

u32 func_02017060(void* ptr)
{
    if (!ptr)
        return 0;
    struct Heap* heap = gGameHeapPtr;
    _ZN4Heap7_SizeofEPv(heap, ptr);
    u32 newSize = func_020469e0(ptr);
    return _ZN4Heap10ReallocateEPvj(heap, ptr, newSize);
}
