//cpp
// @symbol _ZN13ExpandingHeap7VIntactEv
/* ExpandingHeap::VIntact() at 0x0203c65c -- Heap vtable slot 6. Eight bytes:
 * `mov r0,#1 / bx lr'.
 *
 * It answers "yes" unconditionally, exactly as SolidHeap's does, so despite the
 * node list there is no consistency check here either. Heap::Intact latches a
 * global the first time any heap reports damage; nothing in this family can
 * ever trip it. */
#include "ExpandingHeap.h"

bool ExpandingHeap::VIntact()
{
    return 1;
}
