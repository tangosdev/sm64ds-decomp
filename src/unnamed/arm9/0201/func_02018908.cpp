//cpp
#include "types.h"
#include "Heap.h"

/* A mounted .narc archive (src/engine/sound/SoundEngine.cpp). */
struct S82 {
    int func_0204ee10();
};

extern "C" {

/* func_02018908 at 0x02018908 */
/* Unmount the archive and give its block back to heap. */
void func_02018908(struct S82 *archive, void *heap) {
    archive->func_0204ee10();
    ((mHeap::Heap_t *)heap)->_Deallocate(archive);
}
}
