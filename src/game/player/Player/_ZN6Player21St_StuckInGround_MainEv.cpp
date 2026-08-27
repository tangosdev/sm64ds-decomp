//cpp
#include "types.h"
// @symbol _ZN6Player21St_StuckInGround_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern void func_ov002_020c5444(char* c);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void* v);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void Player_AdvanceAnims(char* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern int data_ov002_0211013c[];
}

int Player::St_StuckInGround_Main()
{
    switch (mStateWork) {
    case 0:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a578[mStateStep], 0x40000000, 0x1000, 0);
            *(u8*)(((int)((char*)this) + 0x6e5)) =
                *(u8*)(((int)((char*)this) + 0x6e5)) + 1;
            return 1;
        }
        break;
    case 1:
        if (_ZN6Player12FinishedAnimEv(((char*)this)) == 0) {
            if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2) == 0)
                break;
        }
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a584[mStateStep], 0x40000000, 0x1000, 0);
        *(u8*)(((int)((char*)this) + 0x6e5)) =
            *(u8*)(((int)((char*)this) + 0x6e5)) + 1;
        break;
    case 2:
        {
            void* anim = *(void**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0) << 2) + 0xdc);
            if (_ZNK9Animation12WillHitFrameEi((char*)anim + 0x50, data_ov002_020ff0ec[mStateStep])) {
                func_ov002_020c5444(((char*)this));
                _ZN5Sound9PlayBank0EjRK7Vector3(0xb4, ((char*)this) + 0x74);
            }
        }
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            mAngleY = mPrevAngleY;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
