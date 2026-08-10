//cpp
// @symbol _ZN5UnagiD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through Unagi and Enemy, then hand the object back through Actor's
 * inline operator delete, which is why nothing here mentions a heap. The array
 * cleanup at 0x448 is Vector3[7]; see the note in the D1 file.
 *
 * The hand-written version declared its own `extern void *data_020a0eac[]`,
 * which collides with the `void *` Actor.h supplies for that same heap once the
 * real header is in scope. Declaring the destructor is enough; the compiler
 * writes the rest.
 */
#include "Unagi.h"

Unagi::~Unagi()
{
}
