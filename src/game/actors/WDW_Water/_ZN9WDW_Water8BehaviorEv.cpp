//cpp
#include "types.h"
// @symbol _ZN9WDW_Water8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WDW_Water.h"
#include "dBgW.h"
extern "C" {
extern int IsAreaShowing(int idx);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned a, unsigned b, unsigned c, void *pos, unsigned e);
extern void _ZN9Animation7AdvanceEv(void *a);
}
extern s16 data_02082214[];
extern int data_0209f32c;

int WDW_Water::Behavior()
{
    int t;
    s16 *q;
    int i;

    mAreaId = -1;

    /* area id at 0x340: ROM does add r0,r4,#0x300; ldrsb r0,[r0,#0x40] */
    if (IsAreaShowing(*(s8 *)((u8 *)(((int)((u8 *)this) + 0x300)) + 0x40)) == 0) {
        mAreaId = *(s8 *)((u8 *)(((unsigned)((u8 *)this) + 0x300)) + 0x40);
        if (((dBgW *)((u8 *)&(*(u8 *)&mMeshCollider)))->IsEnabled() != 0) {
            ((dBgW *)((u8 *)&(*(u8 *)&mMeshCollider)))->Disable();
        }
    } else {
        if (((dBgW *)((u8 *)&(*(u8 *)&mMeshCollider)))->IsEnabled() == 0) {
            ((dBgW *)(((u8 *)this) + 0x124))->Enable((dActor_c *)(((u8 *)this)));
        }
    }

    t = mTargetPosY;
    if (mPosY != t) {
        if (mPosY < t) {
            int *p = (int *)(((int)((u8 *)this) + 0x60));
            *p = *p + 0xa000;
            if (mPosY > mTargetPosY)
                mPosY = mTargetPosY;
        } else {
            int *p = (int *)(((int)((u8 *)this) + 0x60));
            *p = *p - 0xa000;
            if (mPosY < mTargetPosY)
                mPosY = mTargetPosY;
        }
    }

    if (mPosY != mPrevPosY) {
        mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            *(unsigned *)((u8 *)&mSoundID), 3, 0x96, ((u8 *)this) + 0x74, 0);
    }

    {
        /* angle at 0x342: pool offset 0x342 */
        s16 *q = (s16 *)(((int)((u8 *)this) + 0x342));
        *q = (s16)(*q + 0x200);
        /* index from (u16)angle at 0x342 via 0x300+0x42 */
        i = *(u16 *)((u8 *)(((unsigned)((u8 *)this) + 0x300)) + 0x42) >> 4;
        mWaterHeight = (int)data_02082214[i * 2] * 0xf + mPosY;
    }
    data_0209f32c = mWaterHeight;
    func_ov029_021122b4(((u8 *)this));
    func_ov029_02112250(((u8 *)this));
    (*(s32 *)((char *)&mTextureTransformer + 0xc)) = 0x1000;
    _ZN9Animation7AdvanceEv((u8 *)&(*(u8 *)&mTextureTransformer));
    return 1;
}
