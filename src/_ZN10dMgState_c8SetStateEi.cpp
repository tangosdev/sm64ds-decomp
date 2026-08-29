//cpp
// @symbol _ZN10dMgState_c8SetStateEi
#include "dMgState_c.h"

extern dMgState_c::Callback data_ov004_020bc974;
extern dMgState_c::Callback data_ov004_020bc96c;
extern dMgState_c::Callback data_ov004_020bc964;
extern dMgState_c::Callback data_ov004_020bc95c;
extern dMgState_c::Callback data_ov004_020bc954;
extern dMgState_c::Callback data_ov004_020bc94c;
extern dMgState_c::Callback data_ov004_020bc944;
extern dMgState_c::Callback data_ov004_020bc92c;
extern dMgState_c::Callback data_ov004_020bc934;
extern dMgState_c::Callback data_ov004_020bc97c;
extern dMgState_c::Callback data_ov004_020bc984;
extern dMgState_c::Callback data_ov004_020bc99c;
extern dMgState_c::Callback data_ov004_020bc9a4;
extern dMgState_c::Callback data_ov004_020bca3c;
extern dMgState_c::Callback data_ov004_020bc9bc;
extern dMgState_c::Callback data_ov004_020bc9d4;
extern dMgState_c::Callback data_ov004_020bc9dc;
extern dMgState_c::Callback data_ov004_020bc9ec;
extern dMgState_c::Callback data_ov004_020bc9f4;
extern dMgState_c::Callback data_ov004_020bca0c;
extern dMgState_c::Callback data_02086b58;

void dMgState_c::SetState(s32 state)
{
    static Callback sEnterTable[20] = {
        data_ov004_020bc974,
        data_ov004_020bc96c,
        data_ov004_020bc964,
        data_ov004_020bc95c,
        data_ov004_020bc954,
        data_ov004_020bc94c,
        data_ov004_020bc944,
        data_ov004_020bc92c,
        data_ov004_020bc934,
        data_ov004_020bc97c,
        data_ov004_020bc984,
        data_ov004_020bc99c,
        data_ov004_020bc9a4,
        data_ov004_020bca3c,
        data_ov004_020bc9bc,
        data_ov004_020bc9d4,
        data_ov004_020bc9dc,
        data_ov004_020bc9ec,
        data_ov004_020bc9f4,
        data_ov004_020bca0c,
    };
    mState = state;
    mEnter = sEnterTable[mState];
    mRender = data_02086b58;
    if (mEnter)
        (this->*mEnter)();
}
