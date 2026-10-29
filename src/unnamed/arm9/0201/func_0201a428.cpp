//cpp
#include "Heap.h"

extern "C" {

/* func_0201a458 -- create the root heap over 0x0211d9c0..0x02140d80 and store it in
 * data_0209d524. Both callers (func_02034fbc in d_s_mb.cpp and dScDSMT_c's result-6
 * path) load overlays 100 and 102 right after, and 0x02140d80 is where ov100 is
 * loaded, so the heap ends at OV100_TEXT_START. 0x0211d9c0 is where ov075 ends
 * (OV075_BSS_END); ov062/063/065 run past it and so cannot be resident beside this
 * heap. Both are linker-defined in dsd's generated arm9.lcf, like OVERLAY_<n>_ID.
 * The ROM's relocations for the two words are ambiguous (config/arm9/relocs.txt lists
 * overlays(62,65) and overlays(6,100)), and the names dsd invents for them
 * (data_ov062_0211d9c0, data_ov006_02140d80) are `ambiguous` placeholders.
 * local extern: linker-defined symbols; no header declares them. */
extern char OV075_BSS_END;
extern char OV100_TEXT_START;
extern void *data_0209d524; /* global: void* (Heap*) */

void func_0201a458(void) {
    data_0209d524 = mHeap::Heap_t::CreateRootHeap(
        &OV075_BSS_END, (unsigned int)(&OV100_TEXT_START - &OV075_BSS_END));
}

/* func_0201a428 at 0x0201a428
 * Destroys a heap referenced by global pointer and nulls the pointer.
 */
void func_0201a428(void) {
    ((mHeap::Heap_t *)data_0209d524)->Destroy();
    data_0209d524 = 0;
}
}
