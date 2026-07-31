//cpp
// @symbol _ZN6Player21St_OpeningWakeUp_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef unsigned char u8;
typedef unsigned int u32;

extern int _ZN6Player12FinishedAnimEv(void* thiz);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, int a, int b, int c, u32 d);
extern void Player_AdvanceAnims(void* thiz);

int Player::St_OpeningWakeUp_Main()
{
    switch (mStateStep) {
    case 0:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0xb4, 0, 0x1000, 0);
            *(u8*)(int)(((long long)(int)((char*)&mStateStep))) += 1;
        }
        break;
    case 1:
        break;
    case 2:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0xb5, 0, 0x1000, 0);
            *(u8*)(int)(((long long)(int)((char*)&mStateStep))) += 1;
        }
        break;
    case 3:
        break;
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
