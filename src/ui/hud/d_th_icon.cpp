//cpp
/* dThIcon_c -- the polymorphic touch-screen icon base, ov001.
 *
 * RTTI names dThIcon_c. Both virtuals are defined out of line here;
 * constructor and destructor are inline in include/dThIcon_c.h, so this
 * TU anchors the vague-linkage group and emits no C1/C2/D0/D1/D2.
 * The unit spans ov001 .text 0x020ab41c..0x020ab5f8: Select, the pen
 * test TouchArea_Update, Render, Behavior, then the icon placement
 * helper func_ov001_020ab5b0. The press pulse func_ov001_020ab3f0 just
 * below it is defined in d_cap_icon.cpp.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. mwccarm emits one .text
 * section per function, and with codegen deferred -- the default, which
 * this unit leaves alone -- every body is generated at end of file in
 * the reverse of source order. Behavior before Render puts Render first
 * in the object. No optimisation pragma is needed.
 *
 * The field names are coined (see include/dThIcon_c.h); the cartridge
 * carries none. TouchArea_Update and func_ov001_020ab5b0 keep C
 * linkage: the ov004/ov006/ov075 callers
 * declare them as C functions that take the icon as a char * or void *.
 */

#include "dThIcon_c.h"
#include "PlayerInput.h"

extern "C" {
/* local extern: arm9 C helpers with no shared header. */
int func_0203da9c(void);
void func_ov001_020ab3f0(void *);
}

extern int data_0208ee44;

// @symbol func_ov001_020ab5b0
/* Place an icon: store its touch kind, centre and half extents, clear
 * mPressTimer, mOn and mPenInside, arm it at once when the kind is
 * touchable (the latch Behavior applies), and set the press reload to
 * ten. The highest address in this unit, so first in the file. Callers
 * in ov004/ov006/ov075 pass the icon as a char *, so the entry keeps it. */
extern "C" void func_ov001_020ab5b0(char *p, int kind, short x, short y, short halfW, short halfH) {
    dThIcon_c *icon = reinterpret_cast<dThIcon_c *>(p);
    icon->mTouchKind = kind;
    icon->mX = x;
    icon->mY = y;
    icon->mHalfW = halfW;
    icon->mHalfH = halfH;
    icon->mPressTimer = 0;
    icon->mOn = 0;
    icon->mPenInside = 0;
    if (icon->mTouchKind >= 1)
        icon->mReady = 1;
    icon->mPressTime = 10;
}

// @symbol _ZN9dThIcon_c8BehaviorEv
void dThIcon_c::Behavior() {
    if (mPressTimer <= 0)
        return;

    mPressTimer -= data_0208ee44;
    if (mPressTimer > 0)
        return;

    if (mOn != 0)
        mOn = 0;
    else
        mOn = 1;

    if (mTouchKind >= 1)
        mReady = 1;
}

// @symbol _ZN9dThIcon_c6RenderEv
void dThIcon_c::Render() {}

// @symbol TouchArea_Update
/* Test player `player`'s pen (the local player when negative) against the
 * icon's box. Fires per mTouchKind: 1 on a touch that begins inside, 2 when
 * the pen enters. Records whether the pen is inside for the next frame. */
extern "C" int TouchArea_Update(char *c, int player) {
    int ret;
    int flag;
    int dx, dy;
    int v;
    dThIcon_c *icon = reinterpret_cast<dThIcon_c *>(c);

    if (icon->mReady == 0)
        return 0;
    if (player < 0)
        player = func_0203da9c();
    ret = 0;
    flag = 0;
    if (gTouchHeld[player * 4] != 0) {
        int i4 = player * 4;
        dx = gTouchX[i4] - icon->mX;
        dy = gTouchY[i4] - icon->mY;
        if (dx >= -icon->mHalfW && dx <= icon->mHalfW && dy >= -icon->mHalfH && dy <= icon->mHalfH)
            flag = 1;
    }
    if (flag != 0) {
        v = icon->mTouchKind;
        if ((v == 1 && gTouchEdge[(unsigned int)player * 4] != 0) ||
            (v == 2 && icon->mPenInside == 0))
            ret = 1;
    }
    icon->mPenInside = (unsigned char)flag;
    return ret;
}

// @symbol _ZN9dThIcon_c6SelectEv
/* Light the icon a menu has just picked: an unlit icon simply turns on, a
 * lit one plays the press pulse. The lowest address in this unit. */
void dThIcon_c::Select() {
    if (mOn == 0) {
        mOn = 1;
        return;
    }
    func_ov001_020ab3f0(this);
}
