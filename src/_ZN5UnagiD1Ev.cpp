//cpp
// @symbol _ZN5UnagiD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One array cleanup, four member destructors and the chain into dEnemyBase_c, all of
 * it reverse declaration order out of Unagi.h.
 *
 * The array at 0x448 is Vector3[7] -- the ROM destroys it with
 * __destroy_arr(ptr, 7, 0xc, _ZN7Vector3D1Ev), and 0xc is sizeof(Vector3).
 */
#include "Unagi.h"

Unagi::~Unagi()
{
}
