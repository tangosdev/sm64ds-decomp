//cpp
// @symbol _ZN21ClockPaintingPendulum8BehaviorEv
/* ClockPaintingPendulum::Behavior -- vtable slot 6, ov013 0x021112a8. The
 * pendulum swings while the global at data_02092110 is not positive: the swing
 * speed reverses at each end of the arc, the angle integrates it, and the tick
 * sound plays as the speed passes +-0x10.
 *
 * THE HALFWORD AT 0x90 IS NOT NAMED BY ANY HEADER. It is the swing angle, and
 * it sits in dActor_c's padding between mAngleY (0x8e) and mPrevAngleY (0x94) --
 * consistent with that gap being an mAngleZ, but nothing here proves it, and
 * naming it means editing include/dActor_c.h, whose includer set is the whole
 * tree. Left as a raw offset deliberately; see the note on the addressing
 * below, which is load-bearing. */
#include "ClockPaintingPendulum.h"

extern "C" {
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, void *);
void func_ov013_02111238(char *t);
extern signed char data_02092110[];
}

s32 ClockPaintingPendulum::Behavior()
{
    char *c = (char *)this;

    if (data_02092110[0] <= 0) {
        short *p90 = (short *)(c + 0x90);
        /* The test re-derives +0x90 instead of reading through p90, and that is
           load-bearing under 2004/b56: the ROM tests the field directly
           (`ldrsh r0,[r4,#0x90]`) and only then materialises the pointer
           (`add r3,r4,#0x90`) for the read-modify-write below. Spelled as `*p90`,
           b56 folds the test into the pointer and emits the two in the other order. */
        if (*(short *)(c + 0x90) > 0) {
            mAngSpeed -= 8;
        } else {
            mAngSpeed += 8;
        }
        *p90 = (short)(*p90 + mAngSpeed);
        short w = mAngSpeed;
        if (w == 0x10 || w == -0x10) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x16, &mCamSpacePosX);
        }
    }
    func_ov013_02111238(c);

    return 1;
}
