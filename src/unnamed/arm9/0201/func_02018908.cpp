//cpp
#include "types.h"
#include "Heap.h"



/* A mounted .narc archive (src/engine/sound/SoundEngine.cpp): its
   FSArchive, then the NARC it was mounted from. */
struct S82;

/* func_02018908 at 0x02018908 */
extern int func_0204ee10(struct S82 *archive);

/* Unmount the archive and give its block back to heap. */
void func_02018908(struct S82 *archive, void *heap) {
    func_0204ee10(archive);
    ((mHeap::Heap_t *)heap)->_Deallocate(archive);
}
