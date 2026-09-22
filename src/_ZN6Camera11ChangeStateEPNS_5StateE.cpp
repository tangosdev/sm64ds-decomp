//cpp
// @symbol _ZN6Camera11ChangeStateEPNS_5StateE
/* recovered: named members + shared header, real C++ method, nested type
 *
 * Swaps the camera's state object, and reports what the tail hook returns.
 *
 * `Camera::State` is nested, and that is READ OFF THE MANGLING rather than
 * chosen: `PNS_5StateE` is pointer-to-`Camera::State`, where `NS_` is the
 * substitution for the enclosing `Camera`. A file-scope `State` would mangle
 * `P5State` and name a symbol that exists nowhere. Its LAYOUT is unknown and
 * does not need to be -- every use is by pointer, so the header forward-
 * declares it and nothing here dereferences one.
 *
 * Three things happen, and only the middle is conditional:
 *   - flag 0x10 vetoes the change outright, before anything is touched;
 *   - a genuine change (state != mState) tears down the outgoing state, but
 *     ONLY when it is the specific one at data_0209b0c8 -- so that teardown
 *     belongs to that state, it is not a general exit path -- then installs
 *     the new pointer and clears unk_1a6;
 *   - the tail call runs regardless, including when the state was unchanged.
 */
#include "decl_common.h"
#include "Camera.h"

int Camera::ChangeState(State * state)
{
    if ((mFlags & 0x10) != 0)
        return 0;
    if (state != mState) {
        if (mState == (State *)&data_0209b0c8) {
            FUN_02029a68();
            func_020089f8(this);
        }
        mState = state;
        unk_1a6 = 0;
    }
    return func_0200cae4(this);
}
