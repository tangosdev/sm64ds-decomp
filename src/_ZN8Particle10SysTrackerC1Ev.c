// @symbol _ZN8Particle10SysTrackerC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Particle.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle.h"

extern char data_0208f3b4[];
extern void *data_0209ee74;

void *_ZN8Particle10SysTrackerC1Ev(struct Particle *self) {
    func_02021c90((char *)&self->mContents);
    _ZN8Particle14SimpleCallbackC2Ev((char *)&self->mRunningSlidingDustCallback);
    _ZN8Particle14SimpleCallbackC2Ev((char *)&self->mCallback_760);
    func_020226a4((char *)&self->mBigSplashCallback);
    *(void **)((char *)&self->mBigSplashCallback) = data_0208f3e4;
    func_020226a4((char *)&self->mCallback_778);
    *(void **)((char *)&self->mCallback_778) = data_0208f3e4;
    func_020226a4((char *)&self->mCallback_784);
    *(void **)((char *)&self->mCallback_784) = data_0208f3a4;
    func_020226a4((char *)&self->mRippleCallback);
    *(void **)((char *)&self->mRippleCallback) = data_0208f444;
    _ZN8Particle14SimpleCallbackC2Ev((char *)&self->mCallback_79c);
    _ZN8Particle14SimpleCallbackC2Ev((char *)&self->mCallback_7a8);
    func_020225fc((char *)&self->mCallback_7b4);
    func_020225fc((char *)&self->mCallback_7c4);
    func_020225fc((char *)&self->mCallback_7d4);
    func_020225fc((char *)&self->mCallback_7e4);
    *(volatile void **)((char *)&self->mCallback_7f0) = data_0208f3b4;
    *(volatile void **)((char *)&self->mCallback_7f0) = data_0208f3f4;
    *(volatile void **)((char *)&self->mCallback_7f4) = data_0208f3b4;
    *(volatile void **)((char *)&self->mCallback_7f4) = data_0208f424;
    *(volatile void **)((char *)&self->mCallback_7f8) = data_0208f3b4;
    *(volatile void **)((char *)&self->mCallback_7f8) = data_0208f454;
    self->mCallbackParam_7fc = 0x3000;
    *(volatile void **)((char *)&self->mCallback_800) = data_0208f3b4;
    *(volatile void **)((char *)&self->mCallback_800) = data_0208f454;
    self->mCallbackParam_804 = 0x3000;
    func_020226a4((char *)&self->mCallback_808);
    *(void **)((char *)&self->mCallback_808) = data_0208f404;
    *(volatile void **)((char *)&self->mWeatherCallback) = data_0208f3b4;
    *(volatile void **)((char *)&self->mWeatherCallback) = data_0208f434;
    self->mWeatherCallbackCount = 1;
    *(volatile void **)((char *)&self->mCallback_818) = data_0208f3b4;
    *(volatile void **)((char *)&self->mCallback_818) = data_0208f464;
    data_0209ee74 = ((char *)self);
    self->mManager = 0;
    self->mRunningSlidingDustSystemID = 0;
    self->mSystemID_75c = 0;
    self->mBigSplashSystemID = 0;
    self->mSystemID_774 = 0;
    self->mSystemID_780 = 0;
    self->mRippleSystemID = 0;
    self->mSystemID_798 = 0;
    self->mSystemID_7a4 = 0;
    self->mSystemID_7b0 = 0;
    self->mSystemID_7c0 = 0;
    self->mCallbackParam_804 = 0x4b000;
    return ((char *)self);
}
