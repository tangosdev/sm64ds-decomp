//cpp
#include "types.h"
// @symbol _ZN6Player20St_CeilingGrate_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char* c, u32 anim, int a, int b, u32 d);
extern int _ZNK6Player14GetBodyModelIDEjb(char* c, u32 a, int b);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void* v);
extern void Player_AdvanceAnims(char* c);

extern u8 data_020a0e40;
extern u16 data_0209f49c[];
extern u16 data_0209f49e[];
extern s16 data_0209f4a0[];
extern int data_ov002_0211013c;
extern int data_ov002_0211004c;
extern int data_ov002_021105a4;
}

int Player::St_CeilingGrate_Main()
{
    int r4;
    int spd;

    r4 = func_ov002_020cf20c(((char*)this));
    if (r4 == 0x80000000) {
        mPosX = unk_548;
        mPosY = unk_54c;
        mPosZ = unk_550;
        r4 = mPosY + 0x90000;
    }

    {
        int idx = data_020a0e40 * 0x18;
        if ((*(u16*)((char*)data_0209f49c + idx) & 2) == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211013c);
            return 1;
        }
        if (*(u16*)((char*)data_0209f49e + idx) & 0x400) {
            if (func_ov002_020d674c(((char*)this)) != 0)
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211004c);
            else
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_021105a4);
            return 1;
        }
        {
            u8* p = data_0209f4ab + idx;
            if (*p != 0)
                *p = 5;
        }
    }

    switch (mStateStep) {
    case 0:
        func_ov002_020cf384(((char*)this));
        if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
            mStateStep = 1;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a60c[mStateWork & 1], 0, 0x1000, 0);
        }
        {
            char* anim = (char*)(((long long)(int)(*(char**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0) << 2) + 0xdc) + 0x50)));
            if ((u32)(*(int*)(anim + 8) << 4) >> 0x10 < 0x21)
                break;
        }
    case 1:
        func_ov002_020cf384(((char*)this));
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            *(u8*)(((int)((char*)this) + 0x6e5)) ^= 1;
            mStateStep = 2;
            spd = mHorzSpeed >> 3;
            if (spd < 0x800)
                spd = 0x800;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a60c[(mStateWork & 1) + 2], 0x40000000, spd, 0);
        }
        break;
    case 2:
        func_ov002_020cf2f8(((char*)this));
        spd = mHorzSpeed >> 3;
        if (spd < 0x800)
            spd = 0x800;
        {
            char* anim = (char*)(((long long)(int)(*(char**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0) << 2) + 0xdc) + 0x50)));
            *(int*)(anim + 0xc) = spd;
        }
        if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0xb2, ((char*)this) + 0x74);
            if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
                *(u8*)(((int)((char*)this) + 0x6e5)) ^= 1;
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a60c[(mStateWork & 1) + 2], 0x40000000, 0x1000, 0);
            } else {
                mStateStep = 1;
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a60c[mStateWork & 1], 0, 0x1000, 0);
            }
        }
        break;
    }

    mPosY = r4 - 0x90000;
    Player_AdvanceAnims(((char*)this));
    return 1;
}
