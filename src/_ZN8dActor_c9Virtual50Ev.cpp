//cpp
/* Actor::Virtual50() at 0x0201014c, 8 bytes -- vtable slot 20 (vtable+0x50).
 *
 * `mov r0,#1; bx lr`. The base answer for whatever this slot asks; derived
 * actors override it to say otherwise.
 *
 * The file previously opened with `typedef int bool;`, which is how this tree
 * spells C's missing bool -- types.h does the same, but only `#ifndef
 * __cplusplus`. Compiling as C++ that is a redeclaration of a keyword, so the
 * typedef had to go rather than be carried over. Return type is `int` because
 * include/Actor.h declares slot 20 that way.
 */
#include "Actor.h"

int Actor::Virtual50()
{
    return 1;
}
