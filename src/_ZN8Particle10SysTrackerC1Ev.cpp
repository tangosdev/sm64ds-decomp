//cpp
// @symbol _ZN8Particle10SysTrackerC1Ev
/* recovered: real C++ constructor -- the compiler spells the mangled name.
 *
 * The callback bank uses its RTTI-backed member types, so mwcc emits the
 * base/member construction and vptr writes in ROM order before this body.
 * mwcc emits C1 and C2 from this definition and objisolate keeps the one this
 * file is bound to. */
/* recovered: real class form */
#include "Particle__SysTracker.h"

extern void *data_0209ee74;

namespace Particle {

SysTracker::SysTracker()
{
    data_0209ee74 = (char *)this;
    mManager = 0;
    mRunningSlidingDustSystemID = 0;
    mSystemID_75c = 0;
    mBigSplashSystemID = 0;
    mSystemID_774 = 0;
    mSystemID_780 = 0;
    mRippleSystemID = 0;
    mSystemID_798 = 0;
    mSystemID_7a4 = 0;
    mSystemID_7b0 = 0;
    mSystemID_7c0 = 0;
    mCallback_800.distance = 0x4b000;
}

}
