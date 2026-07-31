//cpp
#include "types.h"
// @symbol _ZN6Player13St_Crawl_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void* v);
extern int func_ov002_020d36d8(void* c, int arg);
extern int Player_ScaleByCharFactor(void* c, int a);
extern int func_ov002_020bf224(void* c, int a, int b);
extern void ApproachAngle(s16* cur, s16 target, int divisor, int band, int maxStep);
extern void func_ov002_020c18b0(void* c, int a);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020d4d88(void* c, int a, int b);
extern void Player_AdvanceAnims(void* c);

extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern u16 data_0209f49c[];
extern int data_ov002_021104e4;
}

int Player::St_Crawl_Main()
{
    int a = 0;
    int b = 0;

    switch (mStateStep) {
    case 3:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            mStateStep = 4;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x63, 0, 0x1000, 0);
        }
        break;
    case 4: {
        u8 idx = data_020a0e40;
        int off = idx * 0x18;
        s16 val1 = *(s16*)((char*)data_0209f4a0 + off);
        if (val1 >= 0x200 && ((*(u16*)((char*)data_0209f49c + off) & 0x400) != 0 || func_ov002_020d1164(((char*)this)) != 0)) {
            int id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam & 0xff, 0);
            char* anim = (char*)((int*)((char*)&mBodyModels))[id] + 0x50;
            if (_ZNK9Animation12WillHitFrameEi(anim, 0xe) ||
                _ZNK9Animation12WillHitFrameEi((char*)((int*)((char*)&mBodyModels))[_ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam & 0xff, 0)] + 0x50, 0x1c)) {
                _ZN5Sound9PlayBank0EjRK7Vector3(mGroundSoundType + 0xc0, ((char*)this) + 0x74);
            }
            int shifted = mHorzSpeed >> 2;
            int id3 = _ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam & 0xff, 0);
            char* anim3 = (char*)(((long long)(int)((char*)((int*)((char*)&mBodyModels))[id3] + 0x50)));
            *(int*)(anim3 + 0xc) = shifted;
            if (func_ov002_020d36d8(((char*)this), 1))
                return 1;
            int r4v = Player_ScaleByCharFactor(((char*)this), 0x8000);
            int r2v = Player_ScaleByCharFactor(((char*)this), 0x1000);
            a = func_ov002_020bf224(((char*)this), r4v, r2v);
            b = 0x1000;
            ApproachAngle((s16*)((char*)&mTargetAngleY), mDesiredAngleY, 0x10, 0x2000, 0x100);
            mAngleY = mTargetAngleY;
            func_ov002_020c18b0(((char*)this), 0);
        } else {
            mHorzSpeed = 0;
            mStateStep = 5;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x62, 0x40000000, 0x1000, 0);
        }
        break;
    }
    case 5:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            mStateStep = 1;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_021104e4);
        }
        break;
    }

    func_ov002_020d4d88(((char*)this), a, b);
    Player_AdvanceAnims(((char*)this));
    return 1;
}
