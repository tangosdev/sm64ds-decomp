//cpp
// @symbol _ZN12HauntedChairD1Ev
/* Real compiler-spelled complete destructor. The empty body is intentional:
 * mwccarm synthesises reverse member teardown and the dActor_c base chain from
 * HauntedChair's declared layout. objisolate retains D1 and discards the
 * compatibility-name vtable/RTTI and sibling destructor passengers. */
#include "HauntedChair.h"

HauntedChair::~HauntedChair()
{
}
