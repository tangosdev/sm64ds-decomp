//cpp
// @symbol _ZN7fBase_cnwEj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "fBase_c.h"
/* `data_020a0eac` used to be declared here as `extern Heap *`, with a local
   `struct Heap;` to name the type. include/fBase_c.h now declares it -- it
   needs it for the inline operator delete that lets a real ~fBase_c reproduce
   the ROM's deleting destructor -- using the `extern "C" void *` spelling
   include/dActor_c.h, include/dScene_c.h and include/dEnemyBase_c.h all already use. Two
   spellings of one name in one TU is exactly what mwcc rejects ("identifier
   'data_020a0eac' redeclared"), so this file drops its own and Allocate's heap
   parameter follows to `void *`.

   The `void *` is the tree's deliberate compromise, not an oversight: the
   mangled name says `Heap *` and include/decl_common.h says `void *`, and
   declaring the honest type in a header would put two incompatible extern "C"
   declarations of one name in every TU that sees both -- include/dActor_c.h records
   that correcting it belongs in decl_common.h, once, for every caller at the
   same time. Bytes unchanged, 0x50/0x50, re-verified under the pin. */
extern "C" void *_ZN6Memory8AllocateEjiP4Heap(unsigned int size, int align, void *heap);

extern "C" void *_ZN7fBase_cnwEj(unsigned int size)
{
    void *p = _ZN6Memory8AllocateEjiP4Heap(size, -4, data_020a0eac);
    if (!p)
        return 0;
    func_0206e2f8(p, 0, size);
    return p;
}
