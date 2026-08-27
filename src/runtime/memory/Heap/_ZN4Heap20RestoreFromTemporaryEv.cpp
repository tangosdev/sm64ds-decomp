//cpp
// @symbol _ZN4Heap20RestoreFromTemporaryEv
/* Heap::RestoreFromTemporary() at 0x0203c2a8 -- undoes
 * Heap::SetupSolidHeapAsDefault: reinstate the heap that was default before the
 * scratch heap was pushed, and drop the saved pointer.
 *
 * The file used to open with `typedef void Heap;' so that it could spell its
 * own symbol by hand and still say "Heap" in the text. */
#include "Heap.h"

/* 0x020a0ea8 -- where SetupSolidHeapAsDefault parks the outgoing default heap.
   Unnamed in config/arm9/symbols.txt; left as the address symbol rather than
   minted here, since naming it is a claim of its own. */
extern "C" Heap* data_020a0ea8;

void Heap::RestoreFromTemporary()
{
    data_020a0ea8->SetDefault();
    data_020a0ea8 = 0;
}
