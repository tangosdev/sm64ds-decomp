//cpp
// @symbol _ZN6Camera16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Frees the object the camera owns at 0x148 and reports success.
 *
 * The doubled null check is not redundant source -- it is a delete-expression
 * seen from the far side: the outer guard is the ROM's own, and the inner one
 * is the inlined deleting-destructor's, giving two consecutive `cmp r0,#0;
 * beq`. Spelling it as one test loses a branch, so both stay.
 */
#include "Camera.h"

extern "C" void _ZN6Memory16operator_delete2EPv(void *p); /* 0x0203cbcc */

int Camera::CleanupResources()
{
    void *p = mFixedViewPos;
    if (p) {
        if (p)
            _ZN6Memory16operator_delete2EPv(p);
    }
    return 1;
}
