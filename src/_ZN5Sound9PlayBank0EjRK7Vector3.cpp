//cpp
// @symbol _ZN5Sound9PlayBank0EjRK7Vector3
/* Sound::PlayBank0(u32, const Vector3&) at 0x0201264c -- play `id' from bank 0 at
 * a world position. One of a pair of thin wrappers that exist only to bind the
 * bank number, so a caller naming a sound does not have to know which bank it
 * lives in.
 *
 * THE PARAMETER IS A REFERENCE, NOT A POINTER, and the mangled name is what
 * says so: `RK7Vector3' is `const Vector3&'. Every file in this family declared
 * `const Vector3* v' instead. It is byte-identical -- a reference is an address
 * in a register either way -- which is exactly why it survived: nothing that
 * compiles or links can tell the two apart, and only the symbol disagreed.
 *
 * Vector3 comes from types.h rather than the three-int shadow this file used to
 * declare for itself. */
#include "types.h"

namespace Sound {

extern "C" void _ZN5Sound4PlayEjjRK7Vector3(u32 bank, u32 id, const Vector3& pos);

void PlayBank0(u32 id, const Vector3& pos)
{
    _ZN5Sound4PlayEjjRK7Vector3(0, id, pos);
}

}
