//cpp
// @symbol _ZN9ActorBasenwEj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "ActorBase.h"
struct Heap;
extern "C" void *_ZN6Memory8AllocateEjiP4Heap(unsigned int size, int align, Heap *heap);

extern Heap *data_020a0eac;

extern "C" void *_ZN9ActorBasenwEj(unsigned int size)
{
    void *p = _ZN6Memory8AllocateEjiP4Heap(size, -4, data_020a0eac);
    if (!p)
        return 0;
    func_0206e2f8(p, 0, size);
    return p;
}
