//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling. Then a three-way
 * state machine gated by two level-indexed byte tables (data_020a0de8/de9)
 * and driven by two fixed-point tables (data_020a0dea/deb): trigger sets
 * unk_031 and latches unk_034/mDriveBase/unk_03c/mDriveNow; active recomputes
 * and clamps mDriveNow each call and plays a sound on change; release clears
 * unk_031, or -- if neither holds -- eases mCurrent1 back toward baseline.
 * See cMgSmartball_spring_c.h for the field-by-field evidence.
 *
 * Three statements in the original read and wrote the SAME struct member
 * through a raw pointer cast twice, solely to keep a compound assignment
 * off it (`int *p = (int *)(c + off); *p = *p - k;`). All three are plain
 * compound assignments below and reproduce byte-exact. */
#include "cMgSmartball_spring_c.h"

extern "C" u8 data_020a0e40;
extern "C" u8 data_020a0de8[];
extern "C" u8 data_020a0de9[];
extern "C" u8 data_020a0dea[];
extern "C" u8 data_020a0deb[];

extern "C" int Sound_PlayIfNotActive(int, int, int, int);

#pragma opt_propagation off
void cMgSmartball_spring_c::SaveSnapshot()
{
    u8 lvl;
    int de8v;
    int flag;
    int d;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (*(u8 *)((char *)unk_004 + 0x595d) != 0)
        return;

    de8v = data_020a0de8[data_020a0e40 * 4];
    lvl = data_020a0e40;
    flag = 0;
    if (de8v != 0)
        flag = data_020a0de9[lvl * 4] != 0;

    if (flag != 0) {
        unk_031 = 1;
        unk_020 = 0;
        unk_024 = 0;
        unk_03c = data_020a0dea[data_020a0e40 * 4] << 12;
        unk_034 = unk_03c;
        mDriveNow = data_020a0deb[data_020a0e40 * 4] << 12;
        mDriveBase = mDriveNow;
        if (mCurrent1 <= 0xa0000)
            return;
        mDriveBase -= (mCurrent1 - 0xa0000);
        return;
    }

    if (unk_031 == 1 && de8v != 0) {
        mDriveNow = data_020a0deb[lvl * 4] << 12;
        d = mDriveNow - mDriveBase;
        if (d >= 0x38000)
            mDriveNow = mDriveBase + 0x38000;
        else if (mDriveNow < mDriveBase)
            mDriveNow = mDriveBase;
        mCurrent1 = mDriveNow - mDriveBase + 0xa0000;
        if (mSnapshot1 == mCurrent1)
            return;
        mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 2, 0x16c, 0);
        return;
    }

    {
        int m;
        if (de8v == 0 && data_020a0de9[lvl * 4] != 0)
            m = 1;
        else
            m = 0;
        if (m != 0) {
            unk_031 = 0;
            unk_020 = 0;
            unk_024 = 0;
            return;
        }
    }

    unk_024 -= (mCurrent1 - 0xa0000) / 16;
    mCurrent1 += unk_024;
    if (mCurrent1 < 0xa0000) {
        mCurrent1 = 0xa0000;
        unk_024 = 0;
    }
}
