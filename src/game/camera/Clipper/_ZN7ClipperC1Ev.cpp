//cpp
// @symbol _ZN7ClipperC1Ev
/* recovered: real C++ constructor. The ROM stores _ZTV7Clipper and then calls
 * the shared init helper -- vptr-then-body per the measured emission order
 * (notes/ctor-migration.md section 6); there are no bases or members, so only
 * the body statement is written here.
 *
 * The helper keeps its extern "C" spelling: its ROM symbol ends in Ev because
 * it is enrolled under that literal name, so a real member declaration with
 * four parameters would mangle differently (see include/Clipper.h).
 *
 * __sinit_02074e84 constructs the lone instance explicitly and declares this
 * symbol itself; it does not include Clipper.h.
 */
#include "Clipper.h"

extern "C" void _ZN7Clipper13Func_020156DCEv(Clipper *self, u32 a, u16 b, Fix12i c, Fix12i d);

Clipper::Clipper()
{
    _ZN7Clipper13Func_020156DCEv(this, 0x1555, 0xe38, 0x1000, 0x1388000);
}
