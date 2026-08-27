//cpp
// @symbol _ZN8SignPost11OnAttacked1ER8dActor_c
#include "SignPost.h"
#include "dActor_c.h"

/* SignPost::OnAttacked1 -- vtable slot 22, ov002 0x020bb23c.
 * reloc: _ZTV8SignPost+0x58 -> 0x020bb23c, _ZTV10dBgActor_c+0x58 ->
 * 0x02010144 (different, real override).
 *
 * include/dActor_c.h's own slot 22 supplies the signature -- still `int`,
 * unlike slots 21/24/27 (see a64045669).
 *
 * The pre-migration recovery read `other`'s actorID (dActor_c +0xc) through
 * a shadow struct and dispatched through a bare virtual-call shape (`Base::M`
 * at the shadow's slot 31, this class's own Kill -- include/SignPost.h). An
 * unqualified `Kill()` here is that same virtual dispatch. */
int SignPost::OnAttacked1(dActor_c &other)
{
    int isCode = (other.actorID == 0xce);
    if (isCode) {
        Kill();
    }
}
