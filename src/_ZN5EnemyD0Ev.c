// @symbol _ZN5EnemyD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* This file does NOT include Enemy.h, and that is deliberate.
 *
 * Enemy.h now declares a virtual destructor, so in C++ Enemy carries a vptr at
 * 0x00 that a C translation unit does not model -- the two languages would
 * disagree about every field offset by four bytes. This file was Enemy.h's
 * only C reader, and it never touched a field: it uses t[0] and passes t
 * along, nothing more. Dropping the include removes the disagreement without
 * pretending the file has been migrated.
 *
 * Renaming it .cpp would be the other way to remove the C reader, and it is
 * the WRONG way: D0 is the deleting destructor, which is compiler-generated
 * and cannot be written by hand, so the file would stay hand-spelt and merely
 * change extension -- `cpp_still_handspelled` 155 -> 156. That is the "renamed,
 * never migrated" debt of issue #821, and the ratchet correctly rejects it.
 */
extern void* data_020a0eac;
extern int data_ov002_021081e4[];
int *_ZN5EnemyD0Ev(int *t)
{
    t[0] = (int)data_ov002_021081e4;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
