//cpp
// @symbol _ZN11ChiefChillyD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Three array cleanups, four member destructors and the chain into Enemy, all
 * of it reverse declaration order out of ChiefChilly.h. Nothing here is
 * written by hand.
 *
 * The arrays are what took the longest to name. The ROM destroys them with
 * __destroy_arr(this + 0x3e8, 8, 0xc, _ZN7Vector3D1Ev), and a POD array needs no
 * cleanup at all -- so the element type had to be a 0xc class with a declared
 * destructor. It is Vector3: InitResources fills each element as x/y/z from
 * the actor's position, and 0x020072c0 is four bytes of `bx lr`, an empty
 * destructor, now named _ZN7Vector3D1Ev in config/arm9/symbols.txt.
 * __destroy_arr is __cxa_vec_cleanup (PR #1353), which is what the compiler
 * emits here.
 */
#include "ChiefChilly.h"

ChiefChilly::~ChiefChilly()
{
}
