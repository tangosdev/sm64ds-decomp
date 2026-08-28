//cpp
#include "types.h"
// @symbol _ZN16daObjCtMecha04_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha04_c.h"
/* _ZN16daObjCtMecha04_c8BehaviorEv at 0x0211a6d0 (ov065), size 0x1a0
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
extern void func_020393c4(int* p, int v);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
extern int _Z14ApproachLinearRiii(int* r, int t, int step);
extern u16 DecIfAbove0_Short(u16* p);
extern int RandomIntInternal(int* seed);
extern void* _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int d);
}

extern u8 data_0209f2c0;
extern int data_0209e650;
extern int data_ov065_0211c0b8[];

int daObjCtMecha04_c::Behavior()
{
    if (data_0209f2c0 == 3) {
        func_020393c4((int*)((char*)&mMeshCollider), 0);
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0);
    } else {
        if (((mFlags & 8) ? 1 : 0) == 0) {
            if (func_020393bc((int*)((char*)&mMeshCollider)) == 0) {
                func_020393c4((int*)((char*)&mMeshCollider),
                              (int)&daObjCtMecha04_c::AfterClsnCallback);
            }

            if (data_0209f2c0 == 2) {
                if (_Z14ApproachLinearRiii((int*)((char*)&mBeltSpeed), mTargetBeltSpeed, 0xcc) != 0
                    && DecIfAbove0_Short((u16*)((char*)&mDirectionTimer)) == 0) {
                    unsigned int r = (u16)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
                    mDirectionTimer = (s16)(((int)r % 7) * 0x14 + 0xa);
                    if (r >= 0x7fff) {
                        mTargetBeltSpeed = 0x1000;
                    } else {
                        mTargetBeltSpeed = -0x1000;
                    }
                }
            } else {
                mBeltSpeed = data_ov065_0211c0b8[data_0209f2c0];
            }

            /* +0xc inside the TextureTransformer at 0x320: its Animation base's speed. */
            mTextureTransformer.speed = mBeltSpeed;
            mTextureTransformer.Advance();
            if (mBeltSpeed != 0) {
                mSoundHandle = (int)_ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 3, 0x88, ((char*)this) + 0x74, 0);
            }
        }

        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0);
    }

    UpdateShadow();
    return 1;
}
