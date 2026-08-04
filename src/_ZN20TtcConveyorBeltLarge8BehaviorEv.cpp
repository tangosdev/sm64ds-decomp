//cpp
#include "types.h"
// @symbol _ZN20TtcConveyorBeltLarge8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcConveyorBeltLarge.h"
/* _ZN20TtcConveyorBeltLarge8BehaviorEv at 0x0211a6d0 (ov065), size 0x1a0
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
extern void func_020393c4(int* p, int v);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
extern int _Z14ApproachLinearRiii(int* r, int t, int step);
extern u16 DecIfAbove0_Short(u16* p);
extern int RandomIntInternal(int* seed);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void* _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int d);
}

extern u8 data_0209f2c0;
extern int func_ov065_0211aacc;
extern int data_0209e650;
extern int data_ov065_0211c0b8[];

int TtcConveyorBeltLarge::Behavior()
{
    if (data_0209f2c0 == 3) {
        func_020393c4((int*)((char*)&mMeshCollider), 0);
        _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0);
    } else {
        if (((unk_0b0 & 8) ? 1 : 0) == 0) {
            if (func_020393bc((int*)((char*)&mMeshCollider)) == 0) {
                func_020393c4((int*)((char*)&mMeshCollider), (int)&func_ov065_0211aacc);
            }

            if (data_0209f2c0 == 2) {
                if (_Z14ApproachLinearRiii((int*)((char*)&mBeltSpeed), mTargetBeltSpeed, 0xcc) != 0
                    && DecIfAbove0_Short((u16*)((char*)&unk_39c)) == 0) {
                    unsigned int r = (u16)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
                    *(s16*)(((char*)this) + 0x300 + 0x9c) = (s16)(((int)r % 7) * 0x14 + 0xa);
                    if (r >= 0x7fff) {
                        mTargetBeltSpeed = 0x1000;
                    } else {
                        mTargetBeltSpeed = -0x1000;
                    }
                }
            } else {
                mBeltSpeed = data_ov065_0211c0b8[data_0209f2c0];
            }

            unk_32c = mBeltSpeed;
            _ZN9Animation7AdvanceEv((char*)&mTextureTransformer);
            if (mBeltSpeed != 0) {
                unk_398 = (int)_ZN5Sound8PlayLongEjjjRK7Vector3j(unk_398, 3, 0x88, ((char*)this) + 0x74, 0);
            }
        }

        _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0);
    }

    func_ov065_0211a550(((char*)this));
    return 1;
}
