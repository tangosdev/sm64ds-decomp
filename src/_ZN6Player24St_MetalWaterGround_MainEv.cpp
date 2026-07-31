//cpp
// @symbol _ZN6Player24St_MetalWaterGround_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020cec2c(void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern void ApproachAngle(void*,short,int,int,int);
extern int func_ov002_020bf224(void*,int,int);
extern void _Z14ApproachLinearRiii(int*,int,int);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN6Player6IsAnimEj(void*,unsigned int);
extern int _ZN6Player12FinishedAnimEv(void*);
extern int _ZNK6Player14GetBodyModelIDEjb(void*,unsigned int,int);
extern void func_0201251c(int,int,void*,int);
extern void Player_AdvanceAnims(void*);
extern int data_0209f32c;
extern unsigned char data_0209f49e;
extern unsigned char data_020a0e40;
extern short data_0209f4a0;
extern int data_ov002_0211013c[];
extern int data_ov002_0211067c[];
}

int Player::St_MetalWaterGround_Main()
{
    if(func_ov002_020cec2c(((char*)this))) return 1;
    int u703 = mIsMega;
    if((int)(data_0209f32c - 0x50000) < (int)(mPosY - 0xa000)){
        mIsUnderwater = 0;
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        return 1;
    }
    if(*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18) & 2){
        mStateStep = 0;
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021106c4);
        return 1;
    }
    if(mIsAirborne != 0 && (mPosY - mGroundY) >= 0x32000){
        mStateStep = 1;
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021106c4);
        return 1;
    }
    if(mIsMetal == 0 && u703 == 0){
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211067c);
        return 1;
    }
    func_ov002_020cd190(((char*)this));
    int approach = 0;
    if(*(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18) != 0){
        ApproachAngle((short*)((char*)&mTargetAngleY), mDesiredAngleY, 8, 0x1000, 0x80);
        approach = func_ov002_020bf224(((char*)this), 0x14000, 0x1000);
    }
    _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), approach, 0x1000);
    mAngleY = mTargetAngleY;
    if(mIsMega != 0){
        if(mHorzSpeed == 0){
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0xa0, 0, 0x1000, 0);
        }else if(_ZN6Player6IsAnimEj(((char*)this), 0xa0) != 0){
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x9f, 0x40000000, 0x1000, 0);
        }else if(_ZN6Player6IsAnimEj(((char*)this), 0x9f) != 0){
            if(_ZN6Player12FinishedAnimEv(((char*)this)) != 0){
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x9e, 0, *(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18) * 2, 0);
            }
        }else if(_ZN6Player6IsAnimEj(((char*)this), 0x9f) == 0){
            int v = *(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18);
            if(v != 0){
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x9e, 0, v * 2, 0);
                func_ov002_020bf5e0(((char*)this));
            }else{
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x9d, 0x40000000, 0x1000, 0);
            }
        }
    }else if(mHorzSpeed == 0){
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x47, 0, 0x1000, 0);
    }else{
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x48, 0, 0x1000, 0);
        int r5 = (int)((((long long)mHorzSpeed << 9) + 0x800) >> 12);
        if(r5 < 0x400) r5 = 0x400;
        char* anim = *(char**)(((char*)this) + _ZNK6Player14GetBodyModelIDEjb(((char*)this), (unsigned char)mParam, 0) * 4 + 0xdc) + 0x50;
        *(int*)(anim + 0xc) = r5;
        if(_ZNK9Animation12WillHitFrameEi((void*)(*(char**)(((char*)this) + _ZNK6Player14GetBodyModelIDEjb(((char*)this), (unsigned char)mParam, 0) * 4 + 0xdc) + 0x50), 4) != 0
           || _ZNK9Animation12WillHitFrameEi((void*)(*(char**)(((char*)this) + _ZNK6Player14GetBodyModelIDEjb(((char*)this), (unsigned char)mParam, 0) * 4 + 0xdc) + 0x50), 0x13) != 0){
            func_0201251c(0, 0xaa, (void*)((char*)&mCamSpacePos), mHorzSpeed);
        }
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
