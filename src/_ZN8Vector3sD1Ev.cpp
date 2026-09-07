//cpp
// @symbol _ZN8Vector3sD1Ev
/* recovered: Vector3s's destructor, and it does nothing
 *
 * This file was _ZN8Vector3sD1Ev.c -- four bytes, `bx lr`, an empty function
 * nothing claimed. It is Vector3s::~Vector3s, on the same evidence that named
 * Vector3's: Wiggler destroys an array of a 6-byte type with
 * __destroy_arr(this + 0x444, 5, 6, ...), a POD array needs no cleanup at all,
 * and types.h defines Vector3s as exactly `s16 x, y, z`.
 *
 * The forcing shape below is the one Vector3's file needs too -- an explicit
 * `p->~Vector3s()` on an empty inline destructor is inlined away and emits no
 * out-of-line copy, while an ARRAY cleanup has to pass the destructor's address
 * to __cxa_vec_cleanup and therefore cannot. Never instantiated; objisolate
 * keeps the declared function and drops the rest.
 */
#include "types.h"

struct Vector3s_ForceDestructor {
    Vector3s v[2];
    ~Vector3s_ForceDestructor();
};

Vector3s_ForceDestructor::~Vector3s_ForceDestructor()
{
}
