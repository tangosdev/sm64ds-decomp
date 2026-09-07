//cpp
// @symbol _ZN10dMgPsOpt_cC1Ev
#include "dMgPsOpt_c.h"

extern "C" {
int GetSoundMode();
s32 GetGameLanguage();
void func_ov004_020b8dc0(void *icon, int type, int soundMode, s16 x, s16 y);
extern s16 data_ov004_020bfe74[];
extern s16 data_ov004_020bfe88[];
}

dMgPsOpt_c::dMgPsOpt_c()
{
    func_ov004_020b8dc0(&mIcons[2], 0, GetSoundMode(), 0xac, 0x38);
    func_ov004_020b8dc0(&mIcons[0], 2, 0, 0x64, 0x38);
    func_ov004_020b8dc0(&mIcons[1], 3, 0, 0xf4, 0x38);
    func_ov004_020b8dc0(&mIcons[6], 4, 0,
        data_ov004_020bfe74[GetGameLanguage() * 2] + 0x18, 0x38);
    func_ov004_020b8dc0(&mIcons[3], 1, 0, 0x8c, 0x68);
    func_ov004_020b8dc0(&mIcons[4], 1, 0, 0xcc, 0x68);
    func_ov004_020b8dc0(&mIcons[7], 5, 0,
        data_ov004_020bfe88[GetGameLanguage() * 2] + 0x18, 0x68);
    func_ov004_020b8dc0(&mIcons[5], 6, 0, 0x80, 0xa8);

    mSelectedIcon = 8;
    mSelectedIcon = 0;
    mActive = 0;
}
