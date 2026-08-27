//cpp
#include "types.h"
// @symbol _ZN6Player12Unk_020c6a10Ej
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void* thiz, void* st);
extern int func_ov002_020d91e0(void* thiz, u32 a, int b);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void* v);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* thiz, void* st);
}

int Player::Unk_020c6a10(unsigned int arg_)
{
    u32 arg = (u32)arg_;

    int flag;

    if (!(mClsnFlags & 1)) return 0;
    if (_ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_021102ec)) return 0;
    if (!func_ov002_020d82f0(((char*)this))) return 0;

    flag = 1;
    if (mIsMetal == 0) {
        if (func_ov002_020d91e0(((char*)this), arg << 8, 1))
            flag = 0;
    }

    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 7, ((char*)this) + 0x74);
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_021102ec);
    mScaleY = 0x100;
    mStateStep = 4;
    mStateTimer = 0x1e;
    mStateArg = flag;
    unk_6f2 = 0;
    return 1;
}
