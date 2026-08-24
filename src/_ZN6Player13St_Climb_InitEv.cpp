//cpp
#include "Player.h"

extern "C" {
extern int Player_ReleaseHeldActor(char*);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, const Vector3&);
extern int func_ov002_020e3078(void*, void*);
}
extern char data_ov002_021106f4[];

int Player::St_Climb_Init() {
    char* self = (char*)this;
    Player_ReleaseHeldActor(self);

    int* bitfield = (int*)(((int)self + 0x2ec));
    *bitfield |= 4;
    *bitfield &= ~8;

    if (mHorzSpeed <= 0xa000) {
        mAngleYSpeed = 0;
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x26, 0, (Fix12i)0x1000, 0);
        mStateStep = 1;
    } else {
        int val = mHorzSpeed / 16;
        mAngleYSpeed = (short)val;
        if (mAngleYSpeed >= 0x3000)
            mAngleYSpeed = 0x3000;
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x27, 0x40000000, (Fix12i)0x1000, 0);
        mStateStep = 0;
    }

    void* base = *(void**)(self + 0x37c);
    void* vtable = *(void**)base;
    void* (*func)(void*) = *(void* (**)(void*))((char*)vtable + 8);
    void* res = func(base);

    mPosX = *(int*)res;
    mPosZ = *(int*)((char*)res + 8);
    mAttachOffsetY = mPosY - *(int*)((char*)res + 4);
    mVertAccel = 0;
    mHorzSpeed = 0;
    mVertSpeed = 0;

    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xd,
                                           *(Vector3*)(self + 0x74));

    mStateWork = 0;
    *(short*)(self + 0x6b8) = 3;
    *(int*)(self + 0x2dc) = 0x5a000;
    unk_717 = 1;

    {
        char* b = *(char**)(self + 0x37c);
        if (*(int*)(b + 0x18) & 0x1000000) {
            if ((*(int*)(b + 8) >> 2) <= *(int*)(self + 0x688)) {
                if (func_ov002_020e3078(self, data_ov002_021106f4) == 0) {
                    *(short*)(self + 0x6a8) = 0xf;
                }
            }
        }
    }
    return 1;
}
