//cpp
#include "types.h"
// @symbol _ZN6Player12St_Talk_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 vol, int t);
extern int _ZN6Player6IsAnimEj(void* c, u32 a);
extern void func_0201f32c(s16 a);
extern void func_0200d3f8(void* cam, u8 playerID, void* ptr);
extern int _Z15ApproachLinear2Rsss(s16* v, s16 target, s16 step);
extern void func_0200d81c(void* cam, u8 playerID);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void Player_AdvanceAnims(char* c);

extern int data_0209b454;
extern char* data_0209f318;
extern u8 data_0209d660;
extern int data_ov002_0211022c[];
extern int data_ov002_02110514[];
extern int data_ov002_0211013c[];
}

int Player::St_Talk_Main()
{
    if (unk_722 != 0) {
        if (_ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x7222) != 0)
            unk_722 = 0;
    }

    if (mNoCtrlKind == 0) {
        char* p = *(char**)((char*)&mTalkActor);
        if (p != 0)
            *(u32*)(((int)p + 0xb0)) |= 0x800000;
        mFlags |= 0x800000;
        data_0209b454 |= 0x800000;
        mNoCtrlKind = 1;
    }

    switch (mStateStep) {
    case 0:
        if (mStateWork != 0)
            func_ov002_020c47f4(((char*)this));
        break;
    case 1:
        if (!_ZN6Player6IsAnimEj(((char*)this), 0x64)) {
            if (!func_ov002_020c47f4(((char*)this)))
                break;
        }
        mStateStep = 2;
        if (mStateWork == 0)
            func_0201fc88(mAttachOffsetY);
        else
            func_0201f32c(mAttachOffsetY);
        {
            char* cam = data_0209f318;
            switch ((mStateArg >> 2) & 3) {
            case 0:
                func_0200d3f8(cam, mPlayerNo, ((char*)this) + 0x744);
                break;
            case 1:
                func_0200d7a4(cam, mPlayerNo);
                break;
            }
        }
        break;
    case 2:
        if (data_0209d660 != 0)
            break;
        switch (mStateArg & 3) {
        case 0:
            mStateStep = 4;
            break;
        case 1:
            mStateStep = 3;
            break;
        case 2:
            mStateStep = 8;
            break;
        }
        break;
    case 8:
        if (func_ov100_02144fcc() != 0)
            mStateStep = 4;
        break;
    case 4:
    case 5:
        if (_Z15ApproachLinear2Rsss((s16*)((char*)&unk_762), 0, 0x200) != 0) {
            if (mStateStep != 5)
                mStateStep = 6;
            else
                mStateStep = 7;
            mStateWork = 0;
            mPrevAngleY = mAngleY;
            char* cam = data_0209f318;
            func_0200d81c(cam, mPlayerNo);
            *(u32*)(((int)cam + 0x154)) &= ~8;
            unk_742 = 4;
        }
        break;
    case 7:
        if (mStateWaitTimer != 0)
            break;
    case 6:
        if ((*(u32*)(data_0209f318 + 0x154) & 0x8000) != 0)
            break;
        if (mIsOpeningBigDoor != 0) {
            mNoCtrlKind = 0x13;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211022c);
            mStateStep = 0x13;
        } else if (_ZN6Player6IsAnimEj(((char*)this), 0x64)) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110514);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
