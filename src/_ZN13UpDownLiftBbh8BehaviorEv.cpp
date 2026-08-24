//cpp
#include "types.h"
// @symbol _ZN13UpDownLiftBbh8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "UpDownLiftBbh.h"
struct Plat;
typedef void (Plat::*PMF)();
extern "C" PMF data_ov095_02137910[];

extern "C" void* _ZN8dActor_c13ClosestPlayerEv(void* c);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void* c);
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* c, int a, int b);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* c);
extern "C" int _ZN6Player7IsInAirEv(void* c);
int UpDownLiftBbh::Behavior()
{
    int old;
    *(void**)((char*)&mClosestPlayer) = _ZN8dActor_c13ClosestPlayerEv(((char*)this));
    old = mState;
    (((Plat*)((char*)this))->*data_ov095_02137910[old])();
    mStateTimer += 1;
    if (old != mState) mStateTimer = 0;
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0) != 0)
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    /* Waiting at either end, or on the way back: once the rider has landed
       somewhere else, forget them and arm the trigger again. */
    if (mState == 0 || (unsigned)(mState - 3) <= 1) {
        void* pl = *(void**)((char*)&mRider);
        if (pl != 0) {
            if (_ZN6Player7IsInAirEv(pl) == 0) {
                if (mIsRidden == 0) {
                    mRider = 0;
                    mIsArmed = 1;
                }
            }
        }
    }
    mIsRidden = 0;
    if (*(void**)((char*)&mClosestPlayer) != 0)
        mPlayerPosY = *(int*)(*(char**)((char*)&mClosestPlayer) + 0x60);
    return 1;
}
