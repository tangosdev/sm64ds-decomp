//cpp
// @symbol _ZN7Chuckya6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * Draws Chuckya, and first draws whatever it is holding -- but only when all
 * three of the same conditions Behavior uses agree: something IS held, flag
 * 0x4000 is set, and the held object's own +0xc8 is non-zero. Behavior runs
 * the identical triple to decide it is mid-throw, so the two stay in step by
 * asking the same question rather than by sharing a flag.
 *
 * The model is drawn unconditionally afterwards.
 */
#include "Chuckya.h"

extern "C" {
extern void func_ov062_02116edc(void *c);
extern int _ZN5Model6RenderEPK7Vector3(void *m, void *v);
}

int Chuckya::Render()
{
    void *r1 = mHeld;
    if (r1 != 0) {
        int r0 = mFlags;
        int flag = (r0 & 0x4000) ? 1 : 0;
        if (flag != 0) {
            if (*(int*)((char*)r1 + 0xc8) != 0) {
                func_ov062_02116edc(this);
            }
        }
    }
    _ZN5Model6RenderEPK7Vector3(&mModel, (void*)0);
    return 1;
}
