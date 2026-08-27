//cpp
/* dGraph_c::callback_c::GraphCallback2() -- one of the four nullary virtual
 * scene-graph hook defaults, 8 bytes: `mov r0,#1; bx lr`. See
 * src/_ZN8dGraph_c10callback_c14GraphCallback0Ev.cpp for the evidence that
 * these are virtuals in dGraph_c::callback_c's vtable (0x0208ee14), not
 * dScene_c statics. */
#include "dGraph_c.h"

int dGraph_c::callback_c::GraphCallback2()
{
    return 1;
}
