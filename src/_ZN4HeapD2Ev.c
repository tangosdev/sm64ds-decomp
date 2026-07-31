// @symbol _ZN4HeapD2Ev
/* recovered: named members + shared header */
#include "Heap.h"
/* Heap::~Heap() at 0x0203ca10 -- base object destructor (D2).
 * A trivial destructor whose only effect is to reset the object's vtable pointer
 * (at offset 0x00) back to Heap's own vtable before the base subobject is torn
 * down. Returns void.
 *
 * _ZTV4Heap is the Itanium vtable symbol for Heap. The stored address is a
 * pooled wildcard reloc, so the extern name is not byte-verified -- it is named
 * by convention. */

extern int _ZTV4Heap[]; /* vtable for Heap */

void _ZN4HeapD2Ev(int *self)
{
    self[0] = (int)_ZTV4Heap; /* reset vptr at +0x00 */
}
