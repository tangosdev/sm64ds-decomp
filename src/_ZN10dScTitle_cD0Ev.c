// @symbol _ZN10dScTitle_cD0Ev
/* dScTitle_c::~dScTitle_c() (deleting destructor / D0) -- vtable slot 17.
 * Same teardown as D1 (see src/_ZN10dScTitle_cD1Ev.c) plus
 * Memory::Deallocate. Plain C carries the literal mangled name with no
 * mangling needed -- see include/dScTitle_c.h. */
#include "decl_common.h"
extern void _ZN7fBase_cD2Ev(void *self);
extern int data_0208e4b8[];
extern void *data_020a0eac;

int *_ZN10dScTitle_cD0Ev(int *t)
{
    t[0] = (int)_ZTV10dScTitle_c;
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)data_0208e4b8;
    _ZN7fBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
