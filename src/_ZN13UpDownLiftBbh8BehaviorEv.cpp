//cpp
#include "types.h"
// @symbol _ZN13UpDownLiftBbh8BehaviorEv
/* recovered: real C++ method over the reconstructed actor hierarchy */
#include "UpDownLiftBbh.h"
#include "Player.h"

typedef void (UpDownLiftBbh::*State)();
extern State data_ov095_02137910[];

/* A real Fix12<int> by-value call homes the two zero arguments to the stack
   under this compiler, while the ROM passes their scalar representation in
   registers. Keep this one proven codegen boundary until wall 6az is solved. */
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* c, int a, int b);

int UpDownLiftBbh::Behavior()
{
    int old;
    mClosestPlayer = ClosestPlayer();
    old = mState;
    (this->*data_ov095_02137910[old])();
    mStateTimer += 1;
    if (old != mState)
        mStateTimer = 0;

    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();

    /* Waiting at either end, or on the way back: once the rider has landed
       somewhere else, forget them and arm the trigger again. */
    if (mState == 0 || (unsigned)(mState - 3) <= 1) {
        Player *player = mRider;
        if (player != 0) {
            if (player->IsInAir() == 0) {
                if (mIsRidden == 0) {
                    mRider = 0;
                    mIsArmed = 1;
                }
            }
        }
    }
    mIsRidden = 0;
    if (mClosestPlayer != 0)
        mPlayerPosY = mClosestPlayer->mPosY;
    return 1;
}
