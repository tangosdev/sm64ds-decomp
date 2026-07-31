//cpp
// @symbol _ZN6Player9DropActorEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef unsigned int u32;
typedef unsigned long long u64;
struct State;
struct Player;
extern "C" int _ZN6Player7IsStateERNS_5StateE(Player* thiz, State* s);
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(Player* thiz, State* s);
extern State data_ov002_02110364;
extern State data_ov002_02110394;
extern State data_ov002_02110604;
extern State data_ov002_0211013c;
extern State data_ov002_021105d4;
extern State data_ov002_02110034;

int Player::DropActor()
{
    u32* s = *(u32**)((char*)&mHeldObj);
    int present = (s != 0);
    if (present) {
        int held = ((s[0x2c] & 0x200) != 0);
        if (!held) {
            u32* s_ptr1 = (u32*)(((int)s + 0xb0));
            *s_ptr1 &= ~0x4000u;

            u32* s_ptr2 = *(u32**)((char*)&mHeldObj);
            u32* field_ptr2 = (u32*)(((int)s_ptr2 + 0xb0));
            *field_ptr2 &= ~0x100u;

            u32* s_ptr3 = *(u32**)((char*)&mHeldObj);
            u32* field_ptr3 = (u32*)(((int)s_ptr3 + 0xb0));
            *field_ptr3 &= ~0x400u;

            u32* s_ptr4 = *(u32**)((char*)&mHeldObj);
            u32* field_ptr4 = (u32*)(((int)s_ptr4 + 0xb0));
            *field_ptr4 &= ~0x2000u;

            u32* s_ptr5 = *(u32**)((char*)&mHeldObj);
            u32* field_ptr5 = (u32*)(((int)s_ptr5 + 0xb0));
            *field_ptr5 &= ~0x800u;

            u32* s_ptr6 = *(u32**)((char*)&mHeldObj);
            u32* field_ptr6 = (u32*)(((int)s_ptr6 + 0xb0));
            *field_ptr6 &= ~0x1000u;

            *(u32**)((char*)&mHeldObj) = 0;

            if (_ZN6Player7IsStateERNS_5StateE((Player*)((char*)this), &data_ov002_02110364) ||
                _ZN6Player7IsStateERNS_5StateE((Player*)((char*)this), &data_ov002_02110394) ||
                _ZN6Player7IsStateERNS_5StateE((Player*)((char*)this), &data_ov002_02110604))
            {
                _ZN6Player11ChangeStateERNS_5StateE((Player*)((char*)this), &data_ov002_0211013c);
            }
        } else {
            _ZN6Player11ChangeStateERNS_5StateE((Player*)((char*)this), &data_ov002_021105d4);
        }
        return 1;
    }
    {
        int b = (mObjInMouth != 0);
        if (!b) {
            return 0;
        }
        _ZN6Player11ChangeStateERNS_5StateE((Player*)((char*)this), &data_ov002_02110034);
        return 1;
    }
}
