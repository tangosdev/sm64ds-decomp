//cpp
#include "types.h"
// @symbol _ZN6Player16SetRealCharacterEj
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_ModelAnim2.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
#include "SharedFilePtr.h"
struct SharedFilePtr;

extern "C" void _ZN6Player18SetNewHatCharacterEjjb(void *self, u32 a, u32 b, bool c);
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN6Player4HealEi(void *self, int hp);
extern "C" u32 _ZNK6Player14GetBodyModelIDEjb(void *self, u32 a, bool b);

extern SharedFilePtr *data_ov002_020ff480[];
extern u8 data_02092128[];
extern u8 data_0209caa0[];

void Player::SetRealCharacter(unsigned int chr_)
{
    u32 chr = (u32)chr_;

    u32 cur = param1;
    u32 base = mCharFileBase;
    u32 m1, m2;

    ((SharedFilePtr *)(data_ov002_020ff480[base + (cur & 3)]))->Release();
    _ZN6Player18SetNewHatCharacterEjjb(((char *)this), chr, 0, 1);
    param1 = chr;
    mCharacter = (u8)chr;
    data_02092128[mPlayerNo] = (u8)param1;
    data_0209caa0[0x41] = (u8)param1;
    _ZN9Animation8LoadFileER13SharedFilePtr(*data_ov002_020ff480[mCharFileBase + (param1 & 3)]);
    func_ov002_020e6330(((char *)this));
    _ZN6Player4HealEi(((char *)this), 0x880);
    unk_73c = 0;

    m1 = _ZNK6Player14GetBodyModelIDEjb(((char *)this), chr, 0);
    m2 = _ZNK6Player14GetBodyModelIDEjb(((char *)this), param1 & 0xff, 0);
    _ZN10ModelAnim24CopyERKS_Pcj(
        *(void **)(((char *)this) + m1 * 4 + 0xdc),
        *(void **)(((char *)this) + m2 * 4 + 0xdc),
        *(char **)((char *)data_ov002_020ff480[mCharFileBase + chr] + 4),
        0);
    m1 = _ZNK6Player14GetBodyModelIDEjb(((char *)this), chr, 0);
    _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(
        *(void **)(((char *)this) + m1 * 4 + 0xdc),
        *(int *)((char *)data_ov002_020ff480[chr + 0xc4] + 4), 0, 0x1000, 0);
}
