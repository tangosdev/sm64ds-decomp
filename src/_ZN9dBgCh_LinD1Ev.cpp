//cpp
// @symbol _ZN9dBgCh_LinD1Ev
/* recovered: real C++ destructor -- the compiler emits the complete MI teardown
 *
 * The old C body manually restored both dBgCh_Lin vptr blocks, destroyed the
 * dM3dGSph member, then tore down dM3dGLin, dBgPi, and dBgCh in reverse
 * declaration order. All of that follows from the shared class declaration,
 * so the period-accurate source body is empty.
 */
#include "dBgCh_Lin.h"

dBgCh_Lin::~dBgCh_Lin()
{
}
