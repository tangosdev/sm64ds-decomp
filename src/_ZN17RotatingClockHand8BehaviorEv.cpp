//cpp
// @symbol _ZN17RotatingClockHand8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingClockHand.h"
extern int DecIfAbove0_Short(char *p);
extern int RandomIntInternal(char *p);
extern void func_020393a4(int *p, int v);
extern void func_02039394(int *p, int v);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *c, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char *c);
extern unsigned char data_0209f2c0[];
extern int data_0209e650[];

int RotatingClockHand::Behavior()
{
    unsigned char idx = data_0209f2c0[0];
    mPrevAngleX = data_ov035_02112b80[idx];
    if (idx == 2) {
        if (DecIfAbove0_Short((char *)&unk_320) == 0) {
            int r = (unsigned short)((unsigned)RandomIntInternal((char*)data_0209e650) >> 16);
            if ((unsigned)r >= 0x7fff) unk_31e = 1;
            else unk_31e = -1;
            unk_320 = (short)((r % 4 + 1) * 0x1e);
            unk_322 = unk_320;
        } else {
            if ((int)unk_320 < (int)unk_322 - 5) {
                short *q = (short*)(((int)((char *)this) + 0x92));
                *q = (short)(*q * unk_31e);
            } else {
                mPrevAngleX = 0;
            }
        }
    }
    func_020393a4((int*)((char *)&mMeshCollider), 0x180000);
    func_02039394((int*)((char *)&mMeshCollider), 0x1000);
    {
        short *s = (short*)(((int)((char *)this) + 0x8c));
        *s = (short)(*s + mPrevAngleX);
    }
    func_ov035_021118a8(((char *)this));
    func_ov035_02111798(((char *)this));
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0x180000, 0x1000))
        _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
    return 1;
}
