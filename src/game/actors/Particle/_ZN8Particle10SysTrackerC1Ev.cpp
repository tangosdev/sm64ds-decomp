//cpp
// @symbol _ZN8Particle10SysTrackerC1Ev
/* recovered: real C++ constructor -- the compiler spells the mangled name.
 *
 * Every member is POD, so the compiler contributes no implicit member
 * construction; the whole body below is the ROM's. mwcc emits C1 and C2 from
 * this one definition and objisolate keeps the one this file is bound to. */
/* declarations from a shared header */
#include "decl_Particle.h"
#include "decl_common.h"
/* recovered: real class form */
#include "Particle__SysTracker.h"

extern char data_0208f3b4[];
extern void *data_0209ee74;

namespace Particle {

SysTracker::SysTracker()
{
    func_02021c90((char *)&mContents);
    _ZN8Particle14SimpleCallbackC2Ev((char *)&mRunningSlidingDustCallback);
    _ZN8Particle14SimpleCallbackC2Ev((char *)&mCallback_760);
    func_020226a4((char *)&mBigSplashCallback);
    *(void **)((char *)&mBigSplashCallback) = data_0208f3e4;
    func_020226a4((char *)&mCallback_778);
    *(void **)((char *)&mCallback_778) = data_0208f3e4;
    func_020226a4((char *)&mCallback_784);
    *(void **)((char *)&mCallback_784) = data_0208f3a4;
    func_020226a4((char *)&mRippleCallback);
    *(void **)((char *)&mRippleCallback) = data_0208f444;
    _ZN8Particle14SimpleCallbackC2Ev((char *)&mCallback_79c);
    _ZN8Particle14SimpleCallbackC2Ev((char *)&mCallback_7a8);
    func_020225fc((char *)&mCallback_7b4);
    func_020225fc((char *)&mCallback_7c4);
    func_020225fc((char *)&mCallback_7d4);
    func_020225fc((char *)&mCallback_7e4);
    *(volatile void **)((char *)&mCallback_7f0) = data_0208f3b4;
    *(volatile void **)((char *)&mCallback_7f0) = data_0208f3f4;
    *(volatile void **)((char *)&mCallback_7f4) = data_0208f3b4;
    *(volatile void **)((char *)&mCallback_7f4) = data_0208f424;
    *(volatile void **)((char *)&mCallback_7f8) = data_0208f3b4;
    *(volatile void **)((char *)&mCallback_7f8) = data_0208f454;
    mCallbackParam_7fc = 0x3000;
    *(volatile void **)((char *)&mCallback_800) = data_0208f3b4;
    *(volatile void **)((char *)&mCallback_800) = data_0208f454;
    mCallbackParam_804 = 0x3000;
    func_020226a4((char *)&mCallback_808);
    *(void **)((char *)&mCallback_808) = data_0208f404;
    *(volatile void **)((char *)&mWeatherCallback) = data_0208f3b4;
    *(volatile void **)((char *)&mWeatherCallback) = data_0208f434;
    mWeatherCallbackCount = 1;
    *(volatile void **)((char *)&mCallback_818) = data_0208f3b4;
    *(volatile void **)((char *)&mCallback_818) = data_0208f464;
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
    mCallbackParam_804 = 0x4b000;
}

}
