//cpp
// @symbol _ZN14BlueCoinSwitch6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * Draws the switch only while it is still above its stop. Behavior sinks mPosY
 * toward unk_320 once pressed, so this comparison IS the visibility rule --
 * the switch vanishes exactly when it bottoms out, with no separate flag.
 */
#include "BlueCoinSwitch.h"

struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};

int BlueCoinSwitch::Render()
{
    if (mPosY > unk_320) {
        Base *bp = (Base *)&mModel;
        bp->m(0);
    }
    return 1;
}
