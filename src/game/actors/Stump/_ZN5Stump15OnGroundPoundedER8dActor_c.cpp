//cpp
// @symbol _ZN5Stump15OnGroundPoundedER8dActor_c
#include "Stump.h"
#include "dActor_c.h"

/* Stump::OnGroundPounded -- vtable slot 21, ov091 0x02133648.
 * include/dActor_c.h's own slot 21 supplies the signature -- `void`, the
 * correction this override forced (int gave a 4-word mismatch, all in
 * register choice; see include/dActor_c.h and include/Stump.h).
 *
 * mState/mAttackCooldown are this class's own fields (include/Stump.h).
 * `&other + 0x703` reads a byte far past dActor_c's own span -- the
 * pre-migration recovery read it the same way off the raw `arg` pointer
 * without a cast to Player, so it stays a raw pointer offset here too.
 * func_ov091_021334b8 is this class's own, still unmigrated. */
extern "C" void func_ov091_021334b8(void *c, int f);
extern "C" void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *v);

void Stump::OnGroundPounded(dActor_c &other)
{
    if (&other == 0) return;
    if (mState == 0) return;
    if (mAttackCooldown) return;
    _ZN5Sound9PlayBank3EjRK7Vector3(0x62, &mCamSpacePosX);
    int f = 0;
    if (other.param1 == 2) goto set;
    if (*(unsigned char *)((char *)&other + 0x703) == 0) goto done;
set:
    f = 1;
done:
    func_ov091_021334b8(this, f);
}
