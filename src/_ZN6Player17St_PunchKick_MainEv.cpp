//cpp
// @symbol _ZN6Player17St_PunchKick_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern void func_ov002_020d8a50(void* c, u32 a);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern void func_ov002_020c2f64(void* c);
extern int func_ov002_020c0434(char* c);
extern void func_ov002_020c0364(char* c, u32 arg);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int func_ov002_020bf9d4(char* c);
extern void func_ov002_020bedd4(char* self);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern s16 data_0209f4a0[];
extern int data_ov002_0211013c[];
}

int Player::St_PunchKick_Main()
{
    u16 st = mStateTimer;

    if (st != 0) {
        if (st == 1) {
            func_ov002_020dd5ec(((char*)this));
        }
    } else {
        if (unk_6aa == 0) {
            u8 idx = data_020a0e40;
            u16 flags = *(u16*)((char*)data_0209f49e + idx * 0x18);
            if ((flags & 1) != 0 && mPunchKickStep < 2) {
                u8* p = (u8*)(((long long)(int)((char*)&mPunchKickStep)));
                (*p)++;
                _ZN6Player17St_PunchKick_InitEv(((char*)this));
                return 1;
            }
        }

        func_ov002_020d8a50(((char*)this), mPunchKickStep);

        if (mIsAirborne != 0)
            goto end;

        if (mPunchKickStep == 2 && unk_6e6 == 0) {
            unk_6e6 = 1;
            if (func_ov002_020e2c84(((char*)this)) != 0)
                return 1;
        }

        _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), 0, 0x800);
        func_ov002_020c2f64(((char*)this));

        if (func_ov002_020c0434(((char*)this)) != 0) {
            func_ov002_020c0364(((char*)this), 3);
            return 1;
        }

        if (_ZN6Player12FinishedAnimEv(((char*)this)) || mPunchKickStep == 2) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            unk_6aa = 0x10;

            if (mPunchKickStep != 2)
                mHorzSpeed = 0;

            if (mTargetAngleY != mAngleY) {
                mHorzSpeed = 0;
                mTargetAngleY = mAngleY;
            }

            {
                u8 idx2 = data_020a0e40;
                s16 val = *(s16*)((char*)data_0209f4a0 + idx2 * 0x18);
                if (val == 0 && mPunchKickStep == 2) {
                    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x52, 0x40000000, 0x1000, 0);
                    mTargetAngleY = mAngleY;
                    mHorzSpeed = 0;
                }
            }

            if (mPunchKickStep != 2)
                goto end;
            if (mIsInShallowWater != 0)
                goto end;

            func_ov002_020bf9d4(((char*)this));
        }
    }

end:
    func_ov002_020bedd4(((char*)this));
    return 1;
}
