//cpp
// @symbol _ZN13daObjEmmLog_c8BehaviorEv
/* daObjEmmLog_c::Behavior -- vtable slot 6. The rolling log of Tiny-Huge
 * Island: it spins at a fixed rate and rides up and down on a sine of its own
 * spin angle, mBobAmplitude being the amplitude and mBasePosY the centre height.
 *
 * THE HALFWORD AT 0x31e IS NOT NAMED BY ANY HEADER. It is the spin angle: read
 * here as the index into the sine table and written back 0x200 larger every
 * frame. It sits in dBgActor_c's tail padding (that class ends at 0x31e and
 * rounds to 0x320), so it is dBgActor_c's own field rather than this class's --
 * BowserFireSeaArena reads the same bytes. Naming it means editing
 * include/dBgActor_c.h, which 70-plus subclasses include; left as a raw offset
 * here deliberately, and kept as the original two-step base-plus-index
 * expression because that is the addressing the ROM emits. */
#include "daObjEmmLog_c.h"

extern "C" {
void func_020393a4(int *clsn, int v);
/* The name claims Fix12<int> by value; the bytes take a plain word, the same
   disagreement include/dBgActor_c.h records for both IsClsnInRange symbols. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int range, int b);
extern short data_02082214[];
}

int daObjEmmLog_c::Behavior()
{
    char *c = (char *)this;

    func_020393a4((int *)&mMeshCollider, 0x600000);

    short *p = (short *)(c + 0x300);
    int idx = (unsigned short)p[0xf] >> 4;
    int s = *(short *)((char *)data_02082214 + (idx << 2));
    int m = (int)(((long long)mBobAmplitude * s + 0x800) >> 12);
    mPosY = mBasePosY + m;

    short *q = (short *)(((int)c + 0x31e));
    *q = (short)(*q + 0x200);

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x600000, 0);
    return 1;
}
