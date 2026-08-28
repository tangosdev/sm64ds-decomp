//cpp
// @symbol _ZN21ClockPaintingPendulum8BehaviorEv
/* ClockPaintingPendulum::Behavior -- vtable slot 6, ov013 0x021112a8. The
 * pendulum swings while the global at data_02092110 is not positive: the swing
 * speed reverses at each end of the arc, the angle integrates it, and the tick
 * sound plays as the speed passes +-0x10.
 *
 * The swing angle is dActor_c::mAngleZ; its separately materialized pointer
 * below preserves the ROM's load/read-modify-write order under 2004/b56. */
#include "ClockPaintingPendulum.h"
#include "Sound.h"

extern s8 data_02092110[];
extern "C" void func_ov013_02111238(ClockPaintingPendulum *pendulum);

s32 ClockPaintingPendulum::Behavior()
{
    if (data_02092110[0] <= 0) {
        s16 *angleZ = &mAngleZ;
        /* The test reads the field instead of going through angleZ, and that is
           load-bearing under 2004/b56: the ROM tests the field directly
           (`ldrsh r0,[r4,#0x90]`) and only then materialises the pointer
           (`add r3,r4,#0x90`) for the read-modify-write below. Spelled as `*angleZ`,
           b56 folds the test into the pointer and emits the two in the other order. */
        if (mAngleZ > 0) {
            mAngSpeed -= 8;
        } else {
            mAngSpeed += 8;
        }
        *angleZ = (s16)(*angleZ + mAngSpeed);
        s16 speed = mAngSpeed;
        if (speed == 0x10 || speed == -0x10) {
            Sound::PlayBank3(0x16, *(Vector3 *)&mCamSpacePosX);
        }
    }
    func_ov013_02111238(this);

    return 1;
}
