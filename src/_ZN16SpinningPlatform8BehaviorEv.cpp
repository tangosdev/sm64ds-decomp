//cpp
// @symbol _ZN16SpinningPlatform8BehaviorEv
#include "SpinningPlatform.h"
extern "C" {
extern int DecIfAbove0_Short(char *p);
extern int RandomIntInternal(char *p);
extern void func_020393a4(int *p, int v);
extern void func_02039394(int *p, int v);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(char *c, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(char *c);
}
extern unsigned char data_0209f2c0[];
extern int data_0209e650[];
extern short data_ov035_02112b80[];

int SpinningPlatform::Behavior()
{
    unsigned char idx = data_0209f2c0[0];
    mPrevAngleX = data_ov035_02112b80[idx];
    if (idx == 2) {
        if (DecIfAbove0_Short((char *)&mRandTimer) == 0) {
            int r = (unsigned short)((unsigned)RandomIntInternal((char *)data_0209e650) >> 16);
            if ((unsigned)r >= 0x7fff) mRandDirection = 1;
            else mRandDirection = -1;
            mRandTimer = (short)((r % 4 + 1) * 0x1e);
            mRandFrames = mRandTimer;
        } else {
            if ((int)mRandTimer < (int)mRandFrames - 5) {
                /* Keeping the member-address temporary is load-bearing: CW
                 * retains the 0x92 pointer in r2 and the earlier this+0x300
                 * base in r0, exactly as the cartridge does. */
                short *angleStep = &mPrevAngleX;
                *angleStep = (short)(*angleStep * mRandDirection);
            } else {
                mPrevAngleX = 0;
            }
        }
    }
    func_020393a4((int *)&mMeshCollider, 0x180000);
    func_02039394((int *)&mMeshCollider, 0x1000);
    /* As above, materialising the inherited field address preserves the
     * cartridge's add-r3/load/store sequence. */
    short *angle = &mAngleX;
    *angle = (short)(*angle + mPrevAngleX);
    UpdateModel();
    UpdateShadow();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
            (char *)this, 0x180000, 0x1000))
        UpdateClsnPosAndRot();
    return 1;
}
