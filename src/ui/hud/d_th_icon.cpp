//cpp
/* dThIcon_c -- the polymorphic touch-screen icon base, ov001.
 *
 * RTTI names dThIcon_c. Both virtuals are defined out of line here;
 * constructor and destructor are inline in include/dThIcon_c.h, so this
 * TU anchors the vague-linkage group and emits no C1/C2/D0/D1/D2.
 * The unit spans ov001 .text 0x020ab41c..0x020ab5f8: Select, the pen
 * test TouchArea_Update, Render, Behavior, then Place. The press pulse,
 * dThIcon_c::Press, just below it is defined in d_cap_icon.cpp.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. mwccarm emits one .text
 * section per function, and with codegen deferred -- the default, which
 * this unit leaves alone -- every body is generated at end of file in
 * the reverse of source order. Behavior before Render puts Render first
 * in the object. No optimisation pragma is needed.
 *
 * The field names are coined (see include/dThIcon_c.h); the cartridge
 * carries none. TouchArea_Update is dThIcon_c::TouchArea_Update.
 */

#include "dThIcon_c.h"
#include "PlayerInput.h"

/* local extern: arm9 C helpers with no shared header. */
extern "C" int func_0203da9c(void);

extern int gFrameStep;

/* Place an icon: store its touch kind, centre and half extents, clear
 * mPressTimer, mOn and mPenInside, arm it at once when the kind is
 * touchable (the latch Behavior applies), and set the press reload to
 * ten. The highest address in this unit, so first in the file. The
 * derived icons in ov004/ov006/ov075 call it from their own Place. */
void dThIcon_c::Place(int kind, s16 x, s16 y, s16 halfW, s16 halfH) {
    mTouchKind = kind;
    mX = x;
    mY = y;
    mHalfW = halfW;
    mHalfH = halfH;
    mPressTimer = 0;
    mOn = 0;
    mPenInside = 0;
    if (mTouchKind >= 1)
        mReady = 1;
    mPressTime = 10;
}

void dThIcon_c::Behavior() {
    if (mPressTimer <= 0)
        return;

    mPressTimer -= gFrameStep;
    if (mPressTimer > 0)
        return;

    if (mOn != 0)
        mOn = 0;
    else
        mOn = 1;

    if (mTouchKind >= 1)
        mReady = 1;
}

void dThIcon_c::Render() {}

/* Test player `player`'s pen (the local player when negative) against the
 * icon's box. Fires per mTouchKind: 1 on a touch that begins inside, 2 when
 * the pen enters. Records whether the pen is inside for the next frame. */
int dThIcon_c::TouchArea_Update(int player) {
    int fired;
    int inside;
    int dx, dy;
    int kind;

    if (this->mReady == 0)
        return 0;
    if (player < 0)
        player = func_0203da9c();
    fired = 0;
    inside = 0;
    if (gTouchInfo[player].held != 0) {
        dx = gTouchInfo[player].x - this->mX;
        dy = gTouchInfo[player].y - this->mY;
        if (dx >= -this->mHalfW && dx <= this->mHalfW && dy >= -this->mHalfH && dy <= this->mHalfH)
            inside = 1;
    }
    if (inside != 0) {
        kind = this->mTouchKind;
        /* The unsigned index is load-bearing; a signed one DIFFs. */
        if ((kind == 1 && gTouchInfo[player].edge != 0) || (kind == 2 && this->mPenInside == 0))
            fired = 1;
    }
    this->mPenInside = inside;
    return fired;
}

/* Light the icon a menu has just picked: an unlit icon simply turns on, a
 * lit one plays the press pulse. The lowest address in this unit. */
void dThIcon_c::Select() {
    if (mOn == 0) {
        mOn = 1;
        return;
    }
    Press();
}
