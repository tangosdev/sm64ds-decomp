//cpp
// @symbol _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j
/* recovered: named members + shared header, real C++ method */
#include "dActor_c.h"

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in dActor_c.h is the real one and callers may use it. */
extern "C" void _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
    dActor_c *self, const Vector3 *pos, const Vector3_16 *rot,
    int horzSpeed, int unk35c, u32 param1)
{
    dActor_c *a = dActor_c::Spawn(0xfe, param1, *pos, rot, self->mAreaId, (s16)-1);
    if (a != 0) {
        a->mHorzSpeed = horzSpeed;
        /* 0x35c is past dActor_c's own 0xd0 span -- a field of whatever
           subclass actor ID 0xfe really is, not modelled here. */
        *(s32 *)((char *)a + 0x35c) = unk35c;
    }
}
