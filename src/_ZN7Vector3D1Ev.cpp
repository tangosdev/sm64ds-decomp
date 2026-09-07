//cpp
// @symbol _ZN7Vector3D1Ev
/* recovered: Vector3's destructor, and it does nothing
 *
 * This file was _ZN7Vector3D1Ev.c -- four bytes, `bx lr`, an empty function
 * nothing claimed. It is Vector3::~Vector3, and the ROM proves it: six classes
 * destroy arrays of a 0xc type through
 * `__destroy_arr(ptr, N, 0xc, _ZN7Vector3D1Ev)`, a POD array needs no cleanup at
 * all, and ChiefChilly::InitResources fills those elements as x/y/z from the
 * actor's position. types.h defines Vector3 as exactly `Fix12i x, y, z`.
 *
 * Compiling `Vector3 a[8]` inside a class with a destructor emits
 * __cxa_vec_cleanup -- which IS __destroy_arr (PR #1353) -- against this
 * symbol, which is the ROM's call verbatim.
 *
 * The body being empty is not an omission: a declared destructor that does
 * nothing is exactly what makes the compiler emit the array cleanup while
 * leaving the element work to nobody.
 */
#include "types.h"

/* ~Vector3 is defined inline in types.h -- it has to be, because every class
   holding a Vector3 array needs the body visible to emit the array cleanup. So
   this file cannot define it again, and a TU that merely includes the header
   emits nothing.
   A plain explicit destructor call is not enough either: the body is empty, so
   the compiler inlines it away and still emits no out-of-line copy. What DOES
   force one is the thing this symbol exists for -- an ARRAY cleanup, which has
   to pass the destructor's address to __cxa_vec_cleanup and therefore cannot
   inline it. Never instantiated; objisolate keeps the declared function and
   drops the rest. */
struct Vector3_ForceDestructor {
    Vector3 v[2];
    ~Vector3_ForceDestructor();
};

Vector3_ForceDestructor::~Vector3_ForceDestructor()
{
}
