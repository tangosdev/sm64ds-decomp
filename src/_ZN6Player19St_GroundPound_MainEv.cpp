//cpp
// @symbol _ZN6Player19St_GroundPound_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Ground pound, per frame. mStateStep is read SIGNED and acts as a three-way
 * selector: positive counts down the spin-up animations, 0 is the falling and
 * impact phase, and negative (0xff, set on impact) is the recovery. The impact
 * branch picks one of three particle sets by cap state and character, shakes
 * the camera, and skips the upper particles in shallow water.
 */
#include "Player.h"

extern "C" {
void _ZN6Player7SetAnimEji5Fix12IiEj(void* self, unsigned int anim, int a, int b, unsigned int c);
void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3* v);
void func_ov002_020ef2a4(void* c, void* arg);
void func_ov002_020c2f64(void* c);
void func_ov002_020c0364(void* c, u32 arg);
int func_ov002_020e2c84(void* self);
void func_ov002_020dd908(void* sb);
void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, const Vector3* v);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void func_0200d8c8(void* cam, const void* v, int strength);
void func_ov002_020dbc94(void* c);
void _ZN5dCc_c5ClearEv(void* self);
void _ZN5dCc_c6UpdateEv(void* self);
void _ZN6Player11ChangeStateERNS_5StateE(void* self, void* state);
int func_ov002_020d36d8(void* c, int arg);
void Player_AdvanceAnims(void* self);
}

extern void* data_0209f318;
extern u8 data_020a0e40;
extern char data_0209f4a0[];
extern int data_ov002_0211013c;

#pragma opt_strength_reduction off
#pragma opt_common_subs off

int Player::St_GroundPound_Main()
{
    s8 timer = *(s8*)&mStateStep;
    if (timer > 0) {
        mVertSpeed = 0;
        if (mStateStep == 2)
            mVertSpeed = 0xa000;
        if (FinishedAnim() != 0) {
            mStateStep -= 1;
            if (mStateStep != 0) {
                u32 anim = 0x3c;
                if (mIsMega != 0) anim = 0xa2;
                _ZN6Player7SetAnimEji5Fix12IiEj(this, anim, 0x40000000, 0x1000, 0);
            } else {
                u32 anim = 0x3d;
                if (mIsMega != 0) anim = 0xa1;
                _ZN6Player7SetAnimEji5Fix12IiEj(this, anim, 0x40000000, 0x1000, 0);
                mVertSpeed = -0x32000;
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x15, (const Vector3*)&mCamSpacePosX);
                mPeakY = mPosY;
            }
        }
    } else if (timer == 0) {
        u16 st;
        func_ov002_020ef2a4(&mMeshClsn, this);
        st = mStateTimer;
        if (st != 0) {
            if (st == 1) {
                func_ov002_020c2f64(this);
                _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x42, 0x40000000, 0x1000, 0);
                mStateStep = 0xff;
                *(u32*)&mdCcAcPos_c.flags &= ~0x20;
            }
            {
                u16 f = mStateFlags & 1;
                if (f != 0) {
                    func_ov002_020c0364(this, 1);
                    return 1;
                }
            }
        } else {
            if (mIsAirborne == 0) {
                if (func_ov002_020e2c84(this) == 2)
                    return 1;
                func_ov002_020dd908(this);
                if (mIsMega != 0) {
                    _ZN5Sound9PlayBank0EjRK7Vector3(0xd3, (const Vector3*)&mCamSpacePosX);
                } else {
                    _ZN5Sound9PlayBank0EjRK7Vector3((u32)mGroundSoundType + 0x90, (const Vector3*)&mCamSpacePosX);
                }
                {
                    Vector3 v;
                    int x = mPosX;
                    v.x = x;
                    v.y = mPosY;
                    int z = mPosZ;
                    v.z = z;
                    if (mIsMega != 0) {
                        int yy = mPosY + 0x14000;
                        v.y = yy;
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x57, x, yy, z);
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x58, v.x, v.y, v.z);
                        if (mIsInShallowWater == 0) {
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5a, v.x, v.y, v.z);
                            int y2 = mPosY + 0x3c000;
                            int xx = v.x;
                            int zz = v.z;
                            *(volatile int*)&v.y = y2;
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x59, xx, y2, zz);
                        }
                    } else if (param1 != 2) {
                        int yy = mPosY + 0xa000;
                        v.y = yy;
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd3, x, yy, z);
                        if (mIsInShallowWater == 0) {
                            int y2 = mPosY + 0x3c000;
                            int xx = v.x;
                            int zz = v.z;
                            v.y = y2;
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd4, xx, y2, zz);
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd5, v.x, v.y, v.z);
                        }
                    } else {
                        int yy = mPosY + 0xa000;
                        v.y = yy;
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x134, x, yy, z);
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x135, v.x, v.y, v.z);
                        if (mIsInShallowWater == 0) {
                            int y2 = mPosY + 0x32000;
                            int xx = v.x;
                            int zz = v.z;
                            v.y = y2;
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x136, xx, y2, zz);
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x137, v.x, v.y, v.z);
                        }
                    }
                }
                func_0200d8c8(data_0209f318, &mPosX, 0x578000);
                mStateTimer = 6;
            } else {
                func_ov002_020dbc94(this);
                _ZN5dCc_c5ClearEv(&mAttackClsn);
                _ZN5dCc_c6UpdateEv(&mAttackClsn);
            }
        }
    } else {
        if (*(short*)(data_0209f4a0 + data_020a0e40 * 0x18) != 0 || FinishedAnim() != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211013c);
            return 1;
        }
        func_ov002_020d36d8(this, 1);
    }
    Player_AdvanceAnims(this);
    return 1;
}
