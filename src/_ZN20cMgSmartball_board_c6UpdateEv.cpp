//cpp
/* Slot 1. Draws the board. Three passes:
 *
 * 1. The eight lines (mLineTimer timers / mLineScored flags). A line is drawn while
 *    its timer is running, or once the manager's 0x4660 flag is up and the
 *    line's own flag is set. The banner sprite is picked by GetOwnerLanguage,
 *    which is why the same five-way chain repeats four times. Each case also
 *    lights that line's three cells in mCellHighlight, and those triples are what
 *    prove the indexing: 0-2 are the columns {i, i+3, i+6}, 3-5 the rows
 *    {3(i-3)..3(i-3)+2}, 6 the diagonal {0,4,8} and 7 the antidiagonal
 *    {2,4,6}.
 *
 * 2. The nine cells. A cell's angle in mCellFlipAngle picks its face sprite (past
 *    0x4000 it has flipped), a lit cell blinks on bit 0x20 of mFanfareTimer, and a
 *    cell mid-flip is sheared with a matrix built from the shared sine table
 *    data_02082214 -- edge-on at +-0x4000 it degenerates to the flat sprite.
 *
 * 3. The nine slot markers along the bottom, in the order the shuffle table
 *    data_ov006_02142c1c currently holds.
 *
 * No raw offsets are needed anywhere here: this slot never touches the base's
 * 0x31-0x33 span, so unlike SaveSnapshot and RestoreInitial it reaches
 * everything through real member names. */
#include "cMgSmartball_board_c.h"

extern "C" int GetOwnerLanguage(void);
extern "C" void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern "C" void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern "C" int _ZN4cstd4fdivEii(int a, int b);

extern "C" void *data_ov006_021381f0[];
extern "C" void *data_ov006_02138260[];
extern "C" void *data_ov006_02138180[];
extern "C" void *data_ov006_02138110[];
extern "C" void *data_ov006_021380a0[];
extern "C" void *data_ov006_02137fb8[];
extern "C" s16 data_02082214[];
extern "C" s32 data_ov006_02142c1c[];
extern "C" void *data_ov006_0213807c[];

void cMgSmartball_board_c::Update()
{
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < 8; i++) {
        if (mLineTimer[i] > 0 || (*(s32 *)((u8 *)unk_004 + 0x4660) != 0 && mLineScored[i] == 1)) {
            switch (i) {
            case 0:
            case 1:
            case 2:
                if (GetOwnerLanguage() == 5)
                    func_ov004_020afdd0(data_ov006_02138260[0], (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 4)
                    func_ov004_020afdd0(data_ov006_021381f0[0], (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 3)
                    func_ov004_020afdd0(data_ov006_02138180[0], (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 2)
                    func_ov004_020afdd0(data_ov006_02138110[0], (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                else
                    func_ov004_020afdd0(data_ov006_021380a0[0], (i - 1) * 0x18 + (mCurrent0 >> 12), mCurrent1 >> 12, -1, 1);
                mCellHighlight[i] = 1;
                mCellHighlight[i + 3] = 1;
                mCellHighlight[i + 6] = 1;
                break;
            case 3:
            case 4:
            case 5:
                if (GetOwnerLanguage() == 5)
                    func_ov004_020afdd0(data_ov006_02138260[1], mCurrent0 >> 12, (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                else if (GetOwnerLanguage() == 4)
                    func_ov004_020afdd0(data_ov006_021381f0[1], mCurrent0 >> 12, (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                else if (GetOwnerLanguage() == 3)
                    func_ov004_020afdd0(data_ov006_02138180[1], mCurrent0 >> 12, (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                else if (GetOwnerLanguage() == 2)
                    func_ov004_020afdd0(data_ov006_02138110[1], mCurrent0 >> 12, (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                else
                    func_ov004_020afdd0(data_ov006_021380a0[1], mCurrent0 >> 12, (i - 4) * 0x18 + (mCurrent1 >> 12), -1, 1);
                mCellHighlight[(i - 3) * 3] = 1;
                mCellHighlight[(i - 3) * 3 + 1] = 1;
                mCellHighlight[(i - 3) * 3 + 2] = 1;
                break;
            case 6:
                if (GetOwnerLanguage() == 5)
                    func_ov004_020afdd0(data_ov006_02138260[2], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 4)
                    func_ov004_020afdd0(data_ov006_021381f0[2], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 3)
                    func_ov004_020afdd0(data_ov006_02138180[2], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 2)
                    func_ov004_020afdd0(data_ov006_02138110[2], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                else
                    func_ov004_020afdd0(data_ov006_021380a0[2], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                mCellHighlight[0] = 1;
                mCellHighlight[4] = 1;
                mCellHighlight[8] = 1;
                break;
            case 7:
                if (GetOwnerLanguage() == 5)
                    func_ov004_020afdd0(data_ov006_02138260[3], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 4)
                    func_ov004_020afdd0(data_ov006_021381f0[3], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 3)
                    func_ov004_020afdd0(data_ov006_02138180[3], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                else if (GetOwnerLanguage() == 2)
                    func_ov004_020afdd0(data_ov006_02138110[3], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                else
                    func_ov004_020afdd0(data_ov006_021380a0[3], mCurrent0 >> 12, mCurrent1 >> 12, -1, 1);
                mCellHighlight[2] = 1;
                mCellHighlight[4] = 1;
                mCellHighlight[6] = 1;
                break;
            }
        }
    }

    for (j = 0; j < 9; j++) {
        s32 ang = mCellFlipAngle[j];
        s32 idx;

        if (ang < 0x4000)
            idx = j + 0xb;
        else if (mCellHighlight[j] == 1 && (mFanfareTimer & 0x3c) >= 0x1e)
            idx = j + 0x16;
        else
            idx = j + 2;

        if (ang == 0 || ang == 0x8000) {
            func_ov004_020afdd0(data_ov006_02137fb8[idx], (j % 3 - 1) * 0x18 + (mCurrent0 >> 12), (j / 3 - 1) * 0x18 + (mCurrent1 >> 12), -1, 1);
        } else if ((ang >= 0x3c00 && ang <= 0x4400) || (ang >= -0x3c00 && ang <= -0x4400)) {
            func_ov004_020afdd0(data_ov006_02137fb8[1], (j % 3 - 1) * 0x18 + (mCurrent0 >> 12), (j / 3 - 1) * 0x18 + (mCurrent1 >> 12), -1, 1);
        } else {
            s32 vec[4];

            if (ang < 0x4000)
                vec[0] = -_ZN4cstd4fdivEii(0x1000, data_02082214[((u16)(ang + 0x8000) >> 4) * 2 + 1]);
            else
                vec[0] = -_ZN4cstd4fdivEii(0x1000, data_02082214[((u16)ang >> 4) * 2 + 1]);
            vec[1] = 0;
            vec[2] = 0;
            vec[3] = 0x1000;
            RenderOamBothScreens(data_ov006_02137fb8[idx], (j % 3 - 1) * 0x18 + (mCurrent0 >> 12), (j / 3 - 1) * 0x18 + (mCurrent1 >> 12), -1, 1, vec);
            RenderOamBothScreens(data_ov006_02137fb8[0], (j % 3 - 1) * 0x18 + (mCurrent0 >> 12), (j / 3 - 1) * 0x18 + (mCurrent1 >> 12), -1, 1, vec);
        }
    }

    for (k = 0; k < 9; k++) {
        func_ov004_020afdd0(data_ov006_0213807c[data_ov006_02142c1c[k]], k * 0x18 + 0x10, 0x78, -1, 1);
    }
}
