//cpp
// @symbol _ZN14KnockDownPlank8OnKickedER8dActor_c
#include "KnockDownPlank.h"

/* KnockDownPlank::OnKicked -- vtable slot 24, ov015 0x021113fc.
 *
 * A pure tail-call veneer to func_ov015_02111414(this, &other), same shared
 * hit handler as OnAttacked2 (slot 23) -- see that file's note. */
extern "C" {
extern void func_ov015_02111414(void *self, void *other);
}

void KnockDownPlank::OnKicked(dActor_c &other)
{
    func_ov015_02111414(this, &other);
}
