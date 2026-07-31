//cpp
// @symbol _ZN6Player14InitMetalWarioEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_ModelAnim2.h"
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned short u16;

extern "C" void func_ov002_020bda48(char *self);
extern "C" u32 _ZNK6Player14GetBodyModelIDEjb(char *self, u32 a, bool b);
extern "C" void func_ov002_020bd9ec(char *self, u32 a);
extern "C" void func_ov002_020c43c4(char *self, u32 a);

extern char *data_ov002_020ff480[];

void Player::InitMetalWario()
{
    u32 v;
    u32 id;

    func_ov002_020bda48(((char *)this));
    if (func_ov002_020bea7c(((char *)this)) != 0) {
        u32 b = mHatCharacter;
        if (b != 2)
            return;
        _ZN6Player18TurnOffToonShadingEj(((char *)this), b);
        _ZN6Player18TurnOffToonShadingEj(((char *)this), unk_6dc);
        unk_73c = 0;
        mParam = mHatCharacter;
        unk_71a = 0;
    }

    mIsMetal = 1;

    v = mParam;
    id = _ZNK6Player14GetBodyModelIDEjb(((char *)this), v & 0xff, 0);
    _ZN10ModelAnim24CopyERKS_Pcj(
        *(void **)(((char *)this) + id * 4 + 0xdc),
        *(void **)(((char *)this) + v * 4 + 0xdc),
        *(char **)((char *)data_ov002_020ff480[mCharFileBase + v] + 4),
        0);

    unk_6ae = 0x258;
    func_ov002_020bd9ec(((char *)this), 0x34);
    func_ov002_020c43c4(((char *)this), 4);
}
