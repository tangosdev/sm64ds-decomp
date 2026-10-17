//cpp
/* dGraph_c::callback_c -- the scene-graph callback base.
 *
 * ROM evidence: the four methods tile 0x02018ea0..0x02018ec0; the class's
 * vtable at 0x0208ee14 (_ZTVN8dGraph_c10callback_cE) holds them in slots
 * 0..3 (GraphCallback0 .. GraphCallback3 = 0x02018eb8 .. 0x02018ea0) under
 * the canonical {0, _ZTI} header, and the typeinfo pair _ZTI/_ZTS at
 * 0x0208ee04/0x0208ee24 names it a __class_type_info root -- the five
 * per-scene graphCallback_c classes all derive from it. No virtual
 * destructor exists and nothing constructs callback_c directly, so the TU
 * emits no ctor/dtor members.
 *
 * leftovers: none.
 */
#include "dGraph_c.h"

// @symbol _ZN8dGraph_c10callback_c14GraphCallback0Ev
int dGraph_c::callback_c::GraphCallback0()
{
    return 1;
}

// @symbol _ZN8dGraph_c10callback_c14GraphCallback1Ev
int dGraph_c::callback_c::GraphCallback1()
{
    return 1;
}

// @symbol _ZN8dGraph_c10callback_c14GraphCallback2Ev
int dGraph_c::callback_c::GraphCallback2()
{
    return 1;
}

// @symbol _ZN8dGraph_c10callback_c14GraphCallback3Ev
int dGraph_c::callback_c::GraphCallback3()
{
    return 1;
}
