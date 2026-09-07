//cpp
/* dGraph_c::callback_c::GraphCallback0() at 0x02018eb8, 8 bytes:
 * `mov r0,#1; bx lr`.
 *
 * One of the four default scene-graph hooks filling dGraph_c::callback_c's
 * own vtable (0x0208ee14, slots 0..3 at 0x02018eb8..0x02018ea0). They are
 * virtual members, nullary: every dispatcher calls through the object's
 * vptr with r0 = the callback object and no further arguments
 * (func_02018ec0 / func_02018efc / 0x02019144), and the table they sit in
 * carries a compiler-emitted RTTI vtable header, not a hand-built pointer
 * list. This file previously argued the opposite (static, plain function
 * table) under the name dScene_c::GraphCallback0 -- both the scope and the
 * theory were wrong; dScene_c's own vtable (0x02092680) contains none of
 * these four addresses. Object isolation retains only this method from the
 * compiler's key-function TU output.
 */
#include "dGraph_c.h"

int dGraph_c::callback_c::GraphCallback0()
{
    return 1;
}
