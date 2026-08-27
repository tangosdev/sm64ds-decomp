//cpp
// @symbol _ZN11MirrorLuigiD1Ev
/* Real compiler-spelled complete destructor. The empty body is intentional:
 * mwccarm synthesises the reverse member teardown and dActor_c base chain from
 * MirrorLuigi's declared object layout. objisolate retains this D1 contribution
 * and discards D0/D2 plus the compatibility-name vtable and RTTI passengers. */
#include "MirrorLuigi.h"

MirrorLuigi::~MirrorLuigi()
{
}
