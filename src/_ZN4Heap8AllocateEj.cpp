//cpp
// @symbol _ZN4Heap8AllocateEj
/* Heap::Allocate(u32) at 0x0203c28c -- convenience overload that forwards to
 * Heap::Allocate(u32, int) with the default alignment of 4.
 *
 * This file used to declare its own `class Heap { void* Allocate(u32); void*
 * Allocate(u32, int); }' rather than include the real header. That local copy
 * was the only place in the tree that got this overload's return type right --
 * _ZN4Heap8AllocateEji.cpp declared the very same function `int'. Both now come
 * from Heap.h and cannot drift apart again. */
#include "Heap.h"

void* Heap::Allocate(u32 size)
{
    return Allocate(size, 4);
}
