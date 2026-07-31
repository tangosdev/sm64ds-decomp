//cpp
// @symbol _ZN6Player17St_NoControl_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;
typedef int s32;

extern "C" {
extern short GetAngleToCamera(int i);
extern void func_ov002_020c9718(char* c);
extern void func_ov002_020c8a4c(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, unsigned int id, int flags, int speed, unsigned int extra);
extern int _ZNK6Player14GetBodyModelIDEjb(void* thiz, unsigned int a, int b);
extern int _ZN6Player12FinishedAnimEv(void* thiz);
extern void func_ov002_020bedd4(char* c);

extern int data_ov002_0211013c[];
}

int Player::St_NoControl_Main()
{
    u8 mode = mNoCtrlKind;
    if (mode == 0 || mode == 1 || (mode == 0x11 && mStateStep != 0x18)) {
        if (func_0200ee68() == 0) {
            mAngleY = GetAngleToCamera(mPlayerNo);
            mTargetAngleY = mAngleY;
        }
    }

    switch (mStateStep) {
    case 0:
        func_ov002_020c965c(((char*)this));
        break;
    case 1:
        if (func_ov002_020c94a4(((char*)this)) != 0)
            return 1;
        break;
    case 2:
    case 9:
        func_ov002_020c92fc(((char*)this));
        break;
    case 10:
        func_ov002_020c9288(((char*)this));
        break;
    case 20:
        func_ov002_020c924c(((char*)this));
        return 1;
    case 21:
        if (func_ov002_020c91bc(((char*)this)) != 0)
            return 1;
        break;
    case 3:
    case 6:
        if (mStateWaitTimer == 0)
            mHorzSpeed = 0;
        if (mNoCtrlKind == 6 && mIsUnderwater != 0) {
            mVertAccel = 0;
            mVertSpeed = 0;
        } else {
            func_ov002_020c9718(((char*)this));
        }
        if (mNoCtrlKind == 0 || mNoCtrlKind == 3)
            return 1;
        break;
    case 4:
    case 5:
        if (func_ov002_020c9128(((char*)this)) != 0)
            return 1;
        break;
    case 7:
    case 8:
        func_ov002_020c8cb0(((char*)this));
        break;
    case 11:
        func_ov002_020c904c(((char*)this));
        break;
    case 12:
        func_ov002_020c8f80(((char*)this));
        break;
    case 13:
        func_ov002_020c8f0c(((char*)this));
        break;
    case 14:
        func_ov002_020c8b54(((char*)this));
        break;
    case 15:
        func_ov002_020c8a4c(((char*)this));
        break;
    case 16:
        func_ov002_020c897c(((char*)this));
        break;
    case 17:
        func_ov002_020c8d14(((char*)this));
        break;
    case 18:
        func_ov002_020c8b78(((char*)this));
        break;
    case 19:
        if (mIsOpeningBigDoor == 0)
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        break;
    case 22:
        if (mIsAirborne == 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x98, 0x40000000, 0x1000, 0);
            mStateStep = 0x17;
        }
        break;
    case 23: {
        int id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam & 0xff, 0);
        char* anim = (char*)((int*)((char*)&mBodyModels))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x1c))
            unk_71a = 0;
        if (_ZN6Player12FinishedAnimEv(((char*)this)))
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        break;
    }
    case 24:
        func_ov002_020c8714(((char*)this));
        break;
    case 25:
        if (func_ov002_020c8540(((char*)this)) != 0)
            return 1;
        break;
    case 26:
        if (func_ov002_020c84b0(((char*)this)) != 0)
            return 1;
        break;
    }

    func_ov002_020bedd4(((char*)this));
    return 1;
}
