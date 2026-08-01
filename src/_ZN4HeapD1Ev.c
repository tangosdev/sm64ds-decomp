// @symbol _ZN4HeapD1Ev
/* recovered: named members + shared header */
#include "Heap.h"
/* Heap::~Heap() at 0x0203ca44 -- complete object destructor (D1).
 * A trivial destructor whose only effect is to reset the object's vtable pointer
 * (at offset 0x00) back to Heap's own vtable. Returns void.
 *
 * data_02099d90 is the Itanium vtable symbol for Heap. The stored address is a
 * pooled wildcard reloc, so the extern name is not byte-verified -- it is named
 * by convention. */

extern int data_02099d90[]; /* vtable for Heap */

void _ZN4HeapD1Ev(int *self)
{
    self[0] = (int)data_02099d90; /* reset vptr at +0x00 */
}
