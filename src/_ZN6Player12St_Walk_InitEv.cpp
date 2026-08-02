//cpp
// @symbol _ZN6Player12St_Walk_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player9GetHealthEv(void *c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *c, void *state);
extern int Player_ScaleByCharFactor(void *c, int a);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *c, unsigned int a, int b, int f, unsigned int d);
extern void func_ov002_020d4540(void *p);
extern void func_ov002_020caf68(void *self);
extern int func_ov002_020e3078(void *self, void *s);
extern int RandomIntInternal(int *seed);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void *v);
extern void _ZN6Player17SetNoControlStateEhih(void *c, unsigned char a, int b, unsigned char d);

extern unsigned char data_020a0e40;
extern short data_0209f4a0;
extern unsigned char data_0209f2d8;
extern int data_0209caa0[3];
extern unsigned char data_0209f4ac;
extern int data_ov002_0211010c[];
extern int data_ov002_02110154[];
extern int data_ov002_0210e160[];
}

int Player::St_Walk_Init()
{
    if (_ZN6Player9GetHealthEv(((char *)this)) == 0) {
        mStateStep = 1;
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), data_ov002_0211010c);
        return 1;
    }

    if (mHorzSpeed > Player_ScaleByCharFactor(((char *)this), 0x28000)) {
        mHorzSpeed = Player_ScaleByCharFactor(((char *)this), 0x28000);
    }
    if (mHorzSpeed < -0xe000) {
        mHorzSpeed = -0xe000;
    }

    if (*(short *)((char *)&data_0209f4a0 + data_020a0e40 * 0x18) == 0) {
        int byte1 = data_0209f2d8;
        int cond = (byte1 == 1);
        if (cond) goto do_anim;
        if ((data_0209caa0[2] & 0x80) == 0) goto merge;
        cond = (byte1 == 2);
        if (cond) goto merge;
do_anim:
        if (mIsMega != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char *)this), 0xa0, 0, 0x1000, 0);
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char *)this), 0x47, 0, 0x1000, 0);
        }
    } else {
        func_ov002_020d4748(((char *)this));
    }
merge:
    mStateTimer = 0;
    if (*(unsigned char *)((char *)&data_0209f4ac + data_020a0e40 * 0x18) == 0) {
        if (mHorzSpeed == 0) {
            mStateTimer = 0x3c;
        }
    }

    mPeakY = mPosY;
    mStateWork = 0;
    mStateArg = 0;
    if (mHorzSpeed > Player_ScaleByCharFactor(((char *)this), 0x24000)) {
        mStateArg = 1;
    }
    *(unsigned short *)(((long long)(int)((char *)&mStateFlags))) &= ~0x100;
    mIsUnderwater = 0;
    func_ov002_020d4540(((char *)this));

    func_ov002_020caf68(((char *)this));

    mRidingShell = 0;
    mHeldObj = 0;
    unk_35c = 0;
    mStateStep = 0;
    mEatingPlayer = 0;
    if (func_ov002_020e3078(((char *)this), data_ov002_02110154) != 0) {
        unk_6b8 = 0x10;
        if (*(short *)((char *)&data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            mPrevAngleY = mDesiredAngleY;
            mAngleY = mPrevAngleY;
        }
        if (unk_6ed != 0) {
            unsigned int rnd = (unsigned int)RandomIntInternal(data_ov002_0210e160);
            int idx = (rnd >> 6) & 3;
            int voiceId = data_ov002_020ff1b0[idx];
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, voiceId, ((char *)this) + 0x74);
        }
    }

    if (param1 == 1) {
        if (mIsInShallowWater == 0) {
            unk_6ec = 1;
        }
    }

    if (mIsMega != 0 && mScaleX != 0x3000) {
        _ZN6Player17SetNoControlStateEhih(((char *)this), 0xd, -1, 0);
    }
    return 1;
}
