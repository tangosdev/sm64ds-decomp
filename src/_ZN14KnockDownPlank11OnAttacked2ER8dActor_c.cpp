//cpp
// @symbol _ZN14KnockDownPlank11OnAttacked2ER8dActor_c
#include "KnockDownPlank.h"

/* KnockDownPlank::OnAttacked2 -- vtable slot 23, ov015 0x02111408.
 *
 * A pure tail-call veneer to func_ov015_02111414(this, &other), the class's
 * own shared (non-virtual, unmigrated) hit handler -- OnKicked (slot 24,
 * same overlay) is an identical veneer to the same address. Both slots share
 * one implementation; only the vtable entries differ. func_ov015_02111414 is
 * out of this task's scope and stays under its existing name; its extern
 * declaration here claims `int` to match this override's return type -- the
 * real definition is void (falls through, return value unused by any
 * caller), which does not affect call-site codegen. `return` on the final
 * statement is what makes this compile to the ROM's plain tail call. */
extern "C" {
extern int func_ov015_02111414(void *self, void *other);
}

int KnockDownPlank::OnAttacked2(dActor_c &other)
{
    return func_ov015_02111414(this, &other);
}
