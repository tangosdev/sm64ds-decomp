//cpp
// @symbol _ZN10KoopaShell6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * Draws the shell, unless it is being hidden or blinked.
 *
 * The blink is the interesting half: while mDespawnTimer is below 0x2d the
 * shell is skipped on ODD values, so it flashes at half frame rate for the
 * last 45 frames of its life instead of vanishing. Above 0x2d it draws every
 * frame, so the flashing starts partway through the countdown rather than at
 * the beginning.
 *
 * Flag 0x40000 suppresses it outright, checked first and unconditionally.
 */
#include "KoopaShell.h"

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(int p);
};

int KoopaShell::Render()
{
    int b = (mFlags & 0x40000) != 0;
    if (b != 0)
        return 1;
    {
        unsigned char v = mDespawnTimer;
        if (v < 0x2d) {
            if (v & 1)
                return 1;
        }
    }
    ((Obj*)&mModel)->m5(0);
    return 1;
}
