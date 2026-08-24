//cpp
#include "types.h"
// @symbol _ZN6Player17SetNoControlStateEhih
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct State;
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void* thiz, struct State* s);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* thiz, struct State* s);
}

extern struct State data_ov002_0211010c;
extern struct State data_ov002_02110124;
extern struct State data_ov002_0211022c;

int Player::SetNoControlState(unsigned char a_, int b, unsigned char c_)
{
    int a = (int)a_;
    u8 c = (u8)c_;

    u8 flag;

    if (_ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_0211010c) ||
        _ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_02110124)) return 0;

    flag = 0;
    if (a <= 3) {
        if (mIsNoControl != 0) return 0;
        flag = c;
    } else {
        if (mIsNoControl != 0) {
            if (c == 0) return 0;
        }
    }

    mNoCtrlKind = a;
    mAttachOffsetY = b;
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211022c);
    mStatePhase = flag;
    return 1;
}
