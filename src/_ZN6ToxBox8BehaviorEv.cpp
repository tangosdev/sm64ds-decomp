//cpp
// @symbol _ZN6ToxBox8BehaviorEv
#include "ToxBox.h"

typedef void (ToxBox::*ToxBoxState)();
struct ToxBoxStateEntry { ToxBoxState state; };
extern ToxBoxStateEntry data_ov092_02132568[];

extern "C" {
void func_ov092_02131aec(ToxBox *self);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
    dBgActor_c *self, int range, int offset);
void func_ov092_02131a88(ToxBox *self);
}

int ToxBox::Behavior()
{
    unk_576 = 1;
    unk_577 = 0;

    s32 oldState = mMoveDir;
    (this->*data_ov092_02132568[oldState].state)();
    mStateTimer++;
    if (oldState != mMoveDir)
        mStateTimer = 0;

    func_ov092_02131aec(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        func_ov092_02131a88(this);

    if (unk_577 == 1) {
        if (mAngleX != 0 || mAngleY != 0 || mAngleZ != 0) {
            mBaseMtx = mModel.mat4x3;
            mAngleX = 0;
            mAngleY = 0;
            mAngleZ = 0;
        }
    }

    mdCcAcPos_c.Clear();
    Vector3 offset;
    offset.x = 0;
    offset.y = -0xfa000;
    offset.z = 0;
    mdCcAcPos_c.SetPosRelativeToActor(offset);
    mdCcAcPos_c.Update();
    return 1;
}
