//cpp
#include "PathLift.h"

typedef void (PathLift::*PathLiftStateFn)();

struct PathLiftState {
    char pad_00[8];
    PathLiftStateFn behavior;
    char pad_0c[4];
};

extern "C" PathLiftState data_ov002_0210af2c[];

void PathLift::BaseBehavior()
{
    PathLiftState &state = data_ov002_0210af2c[mState];
    (this->*state.behavior)();
    mAfterClsnRan = 0;
}
