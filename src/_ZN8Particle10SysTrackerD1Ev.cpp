//cpp
// @symbol _ZN8Particle10SysTrackerD1Ev
/* recovered: real C++ destructor -- the compiler spells the mangled name.
 *
 * SysTracker has no bases and no virtuals, so mwcc emits D1 and D2 from this
 * one definition and objisolate keeps the one this file is bound to. */
/* recovered: real class form */
#include "Particle__SysTracker.h"

#include "decl_common.h"

extern "C" void func_0203cbc0(void *p);
extern void *data_0209ee74;

namespace Particle {

SysTracker::~SysTracker()
{
    if (data_0209ee80 != 0) {
        mContents.Clear();
        func_0203cbc0(data_0209ee80);
        data_0209ee80 = 0;
    }
    if (mResourceFile != (void *)data_02075f14) {
        _ZN6Memory10DeallocateEPv(mResourceFile);
    }
    data_0209ee74 = 0;
}

}
