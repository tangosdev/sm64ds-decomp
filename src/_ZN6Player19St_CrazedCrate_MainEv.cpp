//cpp
#include "Player.h"
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;


extern short data_02082214[];
extern "C" {
extern void func_ov002_020e28d4(void*, int, int);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int, int);
extern int AngleDiff(int, int);
extern void func_ov002_020e0f38(void*, int);
extern void _ZN7fBase_c18MarkForDestructionEv(void*);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void*, const Vector3&, unsigned int, int, short);
extern void func_ov002_020d718c(void*);
extern void Player_ReleaseHeldActor(void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
}
extern char data_ov002_0211031c;


int Player::St_CrazedCrate_Main() {
    char* self = (char*)this;
    func_ov002_020e28d4(self, 0x1000, 0x1800);

    if (mClsnFlags & 2) {
        int atan = _ZN4cstd5atan2E5Fix12IiES1_(unk_560, *(int*)(self + 0x568));
        int diff = AngleDiff(atan, *(s16*)(self + 0x8e));
        if (diff >= 0x6000) {
            int t = (s16)(2 * atan - *(s16*)(self + 0x8e));
            t += 0x8000;
            mAngleY = t;
            int half = 0x800;
            int* xpos = (int*)(int)(self + 0x5c);
            int* zpos;
            mPrevAngleY = mAngleY;
            *xpos += (int)(((s64)mHorzSpeed
                     * data_02082214[(((int)*(u16*)(self + 0x94) >> 4) << 1) + 1] + half) >> 12);
            zpos = (int*)(int)(self + 0x64);
            *zpos += (int)(((s64)mHorzSpeed
                     * data_02082214[((int)*(u16*)(self + 0x94) >> 4) << 1] + half) >> 12);
        }
    }

    mAngleY = mPrevAngleY;

    if (mIsAirborne == 0) {
        if (mJumpComboStage < 2) {
            (*(u8*)(int)(self + 0x6e1))++;
            func_ov002_020e0f38(self, *(u8*)(self + 0x6e1));
        } else {
            if (*(int*)(self + 8) == 3) {
                if (*(void**)(self + 0x360) != 0) {
                    _ZN7fBase_c18MarkForDestructionEv(*(void**)(self + 0x360));
                }
                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, pos, 5, 0xf000, 0);
                func_ov002_020d718c(self);
            } else {
                Player_ReleaseHeldActor(self);
            }
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211031c);
            _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x43, 0x40000000, 0x1000, 0);
        }
    }

    int v98 = mHorzSpeed;
    int va8 = mVertSpeed;
    int atan2 = _ZN4cstd5atan2E5Fix12IiES1_(v98 >> 8, va8 >> 8);
    mAngleX = (s16)(atan2 - 0x4000);

    return 1;
}
