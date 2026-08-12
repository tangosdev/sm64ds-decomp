//cpp
// @symbol _ZN14UnchainedChompD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and ten teardowns, every one a consequence of
 * `struct UnchainedChomp : Enemy` and the members that declaration types. Six of
 * them are arrays, and the compiler's own loops reproduce the ROM's __destroy_arr
 * calls with the same counts and strides -- which is what makes this body the
 * evidence for the header rather than a transcription of it.
 */
#include "UnchainedChomp.h"

UnchainedChomp::~UnchainedChomp()
{
}
