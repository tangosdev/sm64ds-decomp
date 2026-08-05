//cpp
// @symbol _ZN6Player16InitBalloonMarioEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct V3 { int x, y, z; };
struct State;
extern "C" {
extern void func_ov002_020bda48(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* thiz, struct State* s);
extern void func_ov002_020bd9ec(char* c, unsigned int a);
extern void func_ov002_020c43c4(char* c, int a);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void* pos);
}

extern struct State data_ov002_0211028c;

void Player::InitBalloonMario()
{
    struct V3 v;

    if (mIsBalloon != 0) return;

    func_ov002_020bda48(((char*)this));
    mIsBalloon = 1;
    unk_6c0 = 0x258;
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211028c);
    func_ov002_020bd9ec(((char*)this), 0x30);
    func_ov002_020c43c4(((char*)this), 2);

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = mPosY + 0x50000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc5, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc6, v.x, v.y, *(volatile int *)&v.z);

    _ZN5Sound9PlayBank0EjRK7Vector3(0xd7, ((char*)this) + 0x74);
}
