//cpp
// @symbol _ZN13WaterfallMistD1Ev
/* recovered: real C++ destructor -- the compiler emits all but one line of it
 *
 * One vtable store and five teardowns. Four of them are the compiler's: the members
 * this class declares, destroyed in reverse declaration order, then Enemy.
 *
 * The CapIcon call is the exception, and it is written out for the same reason as in
 * CapEnemy -- that type's destructor is still spelt func_ov001_020ab3a0 rather than
 * CapIcon::~CapIcon, so the compiler has nothing to emit. Putting it in the body is
 * not a workaround for the ordering: a destructor body runs BEFORE every member
 * destructor, and mCapIcon is the last member, so "first statement of the body" and
 * "first member destroyed" are the same position. That is exactly where the ROM
 * calls it.
 */
#include "WaterfallMist.h"

extern "C" void func_ov001_020ab3a0(CapIcon *icon);

WaterfallMist::~WaterfallMist()
{
    func_ov001_020ab3a0(&mCapIcon);
}
