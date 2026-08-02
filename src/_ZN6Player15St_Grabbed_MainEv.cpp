//cpp
#include "types.h"
// @symbol _ZN6Player15St_Grabbed_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" void Player_AdvanceAnims(char* self);
extern "C" void _ZN6Player9DropActorEv(char* a);
extern "C" unsigned int _ZNK6Player14GetBodyModelIDEjb(char* self, unsigned int a, int b);

int Player::St_Grabbed_Main()
{
    char* grab;
    u8* p;
    unsigned int id;
    int ret;
    volatile int* pRate;
    char* model;

    mPeakY = mPosY;
    grab = *(char**)((char*)&unk_35c);
    if (grab != 0) {
        int isBob = (*(u16*)(grab + 0xc) == 0xbf);
        if (isBob != 0) {
            ret = func_ov002_020beb38(((char*)this));
            if (ret != 0) {
                p = (u8*)(int)(((long long)(int)((char*)&mStateStep)));
                *p = (u8)(*p + ret);
                mStateTimer = 6;
                if ((s16)(mStateStep - ret) < 0) {
                    _ZN6Player9DropActorEv(*(char**)((char*)&unk_35c));
                    func_ov002_020db54c(((char*)this), 0x10000, 0x10000, mAngleY);
                    return 1;
                }
            }
        }
    }

    if (mStateTimer != 0) {
        id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0);
        model = *(char**)(((char*)this) + (id << 2) + 0xdc);
        pRate = (int*)(model + 0x50);
        pRate = (int*)((char*)pRate + 0xc);
        *pRate = 0x4000;
    } else {
        id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0);
        model = *(char**)(((char*)this) + (id << 2) + 0xdc);
        pRate = (int*)(model + 0x50);
        pRate = (int*)((char*)pRate + 0xc);
        *pRate = 0x1000;
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
