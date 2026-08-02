//cpp
#include "types.h"
// @symbol _ZN6Player17St_Headstand_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual Obj* v2();
};

extern "C" {
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int d, unsigned int e);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* state);
extern void Player_AdvanceAnims(void* c);

extern u8 data_020a0e40;
extern u16 data_0209f49c[];
extern u16 data_0209f49e[];
}

int Player::St_Headstand_Main()
{
    Obj* obj = *(Obj**)((char*)&unk_37c);
    Obj* r = obj->v2();
    mPosX = *(int*)r;
    mPosZ = *(int*)((char*)r+8);
    mPosY = *(int*)((char*)r+4) + *(int*)((char*)(*(Obj**)((char*)&unk_37c))+8);

    u8 st = mStateStep;
    switch (st) {
    case 2:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x1d, 0, 0x1000, 0);
            mStateStep = 3;
        }
        break;
    case 3: {
        int idx = data_020a0e40 * 0x18;
        s16 val = *(s16*)((char*)data_0209f4a4 + idx);
        u16 mag = *(u16*)((char*)data_0209f49c + idx);
        if (val > 0x200) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x1f, 0x40000000, 0x1000, 0);
            mStateStep = 4;
        }
        func_ov002_020cc05c(((char*)this), mag);
        {
            s16* p = (s16*)((((long long)(int)((char*)&mAngleY))));
            *p = *p + mAngleYSpeed;
        }
        if (*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2) {
            mPrevAngleY = mAngleY;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110724);
            return 1;
        }
        break;
    }
    case 4:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021106dc);
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
