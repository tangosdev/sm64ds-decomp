// @symbol _ZN13dScGameOver_cD0Ev
/* dScGameOver_c::~dScGameOver_c() (deleting destructor / D0) -- vtable slot
 * 17. Same teardown as D1 (see src/_ZN13dScGameOver_cD1Ev.c) plus
 * Memory::Deallocate. Plain C carries the literal mangled name with no
 * mangling needed -- see include/dScGameOver_c.h. */
#include "decl_common.h"
extern void _ZN7fBase_cD2Ev(void *);
extern void *GAME_HEAP_PTR;
int *_ZN13dScGameOver_cD0Ev(int *t)
{
    t[0] = (int)_ZTV13dScGameOver_c;
    t[0] = (int)_ZTV8dScene_c;
    t[0] = (int)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, GAME_HEAP_PTR);
    return t;
}
