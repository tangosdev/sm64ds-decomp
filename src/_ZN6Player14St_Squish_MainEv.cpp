//cpp
#include "types.h"
// @symbol _ZN6Player14St_Squish_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern u8 data_ov002_0211117c;
extern char data_ov002_0211013c;
extern char data_ov002_021101b4;

extern "C" {
extern int func_ov002_020d91e0(char *c, int a, int b);
extern void func_ov002_020db8bc(char *c, int a);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, char *v);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *c, char *s);
extern int _ZN6Player9GetHealthEv(char *c);
}

int Player::St_Squish_Main()
{
    switch (mStateStep) {
    case 0:
        if (func_ov002_020c6908(((char *)this)) != 0)
            return 1;
        {
            int r = func_ov002_020c647c(((char *)this), mAttachOffsetY);
            if (r >= 0) {
                mScaleY = (r << 12) / 160;
                if (r >= 0x20)
                    break;
                if (unk_6e6 != 2)
                    break;
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 7, ((char *)this) + 0x74);
                mScaleY = 0x100;
                mStateStep = 2;
                mStateArg = 1;
                if (mIsMetal == 0) {
                    if (func_ov002_020d91e0(((char *)this), 0x300, 1) != 0)
                        mStateArg = 2;
                }
                {
                    char *a = *(char **)((char *)&mAttachedActor);
                    if (a != 0) {
                        u16 id = *(u16 *)(a + 0xc);
                        int b1 = (id == 0xa4);
                        int b2;
                        if (b1 || (b2 = (id == 0xa5)) != 0)
                            *(u8 *)(a + 0x407) = 1;
                    }
                }
            } else {
                mStateTimer = 0xa;
                (*(u8 *)(int)((char *)&mStateStep))++;
            }
        }
        break;
    case 1:
        if (mStateTimer != 0) {
            (*(int *)(int)((char *)&mScaleY)) += 0x100;
            if (mScaleY > 0x1000)
                mScaleY = 0x1000;
        } else {
            mScaleY = 0x1000;
            _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
        }
        break;
    case 2:
        if (func_ov002_020c6908(((char *)this)) != 0)
            return 1;
        if (func_ov002_020c647c(((char *)this), mAttachOffsetY) < 0) {
            if (mStateArg == 1) {
                func_ov002_020db8bc(((char *)this), 1);
                _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
            } else {
                (*(u8 *)(int)((char *)&mStateStep))++;
                mStateArg = 0;
            }
        }
        break;
    case 3:
        if (mStateWork == 2 && func_ov002_020c6538(((char *)this)) == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_021101b4);
            return 1;
        }
        if (mStateArg != 0)
            break;
        if (_ZN6Player9GetHealthEv(((char *)this)) != 0) {
            func_ov002_020db8bc(((char *)this), 1);
            _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
        }
        if (data_ov002_0211117c != 0)
            break;
        KillPlayer();
        (*(u8 *)(int)((char *)&mStateArg))++;
        break;
    case 4:
        if (mStateTimer != 0)
            break;
        if (mStateArg != 1) {
            mStateStep = 3;
            break;
        }
        func_ov002_020db8bc(((char *)this), 1);
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
        break;
    }
    return 1;
}
