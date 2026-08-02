//cpp
#include "types.h"
// @symbol _ZN6Player22St_GrabBowserTail_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char* c, u32 anim, int a, Fix12i b, u32 d);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void* v);
extern void func_ov002_020daa74(char* c);
extern void _Z15ApproachLinear2Rsss(s16* ref, s16 target, s16 step);
extern void func_02012694(u32 id, void* v);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* s);
extern void Player_AdvanceAnims(char* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern s16 data_0209f4a0[];
extern char data_0209ee90[];
extern int data_ov002_02110154[];
}

int Player::St_GrabBowserTail_Main()
{
    switch (mStateStep) {
    case 0:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            mStateStep = 1;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x80, 0, 0x1000, 0);
        }
        break;
    case 1:
        if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 1) != 0) {
            s16 x = mAngleYSpeed;
            if (x < 0) x = -x;
            if (x > 0xe00)
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x32, ((char*)this) + 0x74);
            else
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x33, ((char*)this) + 0x74);
            mStateStep = 2;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x7f, 0x40000000, 0x1000, 0);
            func_ov002_020daa74(((char*)this));
            return 1;
        }
        if (*(int*)(data_0209ee90 + 0x24c) != 0)
            mStateWork = 0;
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) == 0) {
            u8 v = mStateWork;
            (*(u8*)(((int)((char*)this) + 0x6e5)))++;
            if (v > 0x78) {
                mStateStep = 2;
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x7f, 0x40000000, 0x1000, 0);
                func_ov002_020daa74(((char*)this));
                return 1;
            }
        } else {
            mStateWork = 0;
        }
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) > 0x200) {
            if (mStateArg == 0) {
                mStateArg = 1;
                unk_6d4 = mDesiredAngleY;
            } else {
                s16 xv = mAngleYSpeed;
                if (xv < 0) xv = -xv;
                s32 r3;
                if (xv < 0x800)
                    r3 = ((mDesiredAngleY - unk_6d4) << 10) >> 16;
                else
                    r3 = ((mDesiredAngleY - unk_6d4) << 9) >> 16;
                if (r3 < -0x80) r3 = -0x80;
                if (r3 > 0x80) r3 = 0x80;
                *(s16*)(((int)((char*)this) + 0x69c)) += r3;
                if (mAngleYSpeed > 0x1000) mAngleYSpeed = 0x1000;
                if (mAngleYSpeed < -0x1000) mAngleYSpeed = -0x1000;
            }
        } else {
            mStateArg = 0;
            _Z15ApproachLinear2Rsss((s16*)(((int)((char*)this) + 0x69c)), 0, 0x40);
        }
        {
            s16 before = mAngleY;
            *(s16*)(((int)((char*)this) + 0x8e)) += mAngleYSpeed;
            if ((mAngleYSpeed <= -0x100 && before < mAngleY) ||
                (mAngleYSpeed >= 0x100 && before > mAngleY))
                func_02012694(0xb4, ((char*)this) + 0x74);
            s16 sp = mAngleYSpeed;
            if (sp >= 0)
                mAngleX = -sp;
            else
                mAngleX = sp;
        }
        break;
    case 2:
        if (_ZN6Player12FinishedAnimEv(((char*)this)))
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110154);
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
