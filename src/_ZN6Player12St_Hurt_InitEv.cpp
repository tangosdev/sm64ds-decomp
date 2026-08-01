//cpp
#include "types.h"
// @symbol _ZN6Player12St_Hurt_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char* thiz, u32 anim, int a, int fix, u32 b);
extern int _ZNK6Player14GetBodyModelIDEjb(char* thiz, u32 a, int b);
extern void func_ov002_020d93ac(char* thiz);
extern char* data_0209f318;

int Player::St_Hurt_Init()
{
    int mid;
    char* m;
    u8 old;
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a6d4[mStateStep & 7], 0x40000000, 0x1000, 0);
    mid = _ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam & 0xff, 0);
    m = *(char**)(((char*)this) + mid*4 + 0xdc);
    m = (char*)(((unsigned int)m + 0x50) & 0xFFFFFFFFU);
    *(int*)(m + 8) = 0;
    unk_6e6 = 0;
    old = mStateWork;
    mStateWork = 0;
    if ((mStateStep & 0xf0) == 0x10) mStateWork = old << 4;
    mStateArg = 0;
    if (mHurtDamage != 0) {
        mStateArg = 2;
        func_ov002_020d93ac(((char*)this));
    }
    mIsTakingDamage = 1;
    mPrevVertSpeed = mVertSpeed;
    func_0200d89c(data_0209f318);
    *(s16*)(((char*)this)+0x600+0xa8) = 5;
    return 1;
}
