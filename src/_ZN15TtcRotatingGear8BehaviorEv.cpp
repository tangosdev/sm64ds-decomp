//cpp
// @symbol _ZN15TtcRotatingGear8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingGear.h"
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;

extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
extern u16 DecIfAbove0_Short(void *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *c, void *cc);
extern int RandomIntInternal(int *seed);

extern u8 data_0209f2c0;
extern int data_0209e650;
extern u8 data_ov065_0211c0d4[];
extern u8 data_ov065_0211c0d0[];

int TtcRotatingGear::Behavior()
{
    if (data_0209f2c0 == 3) {
        mPosY = unk_324 + 0x14a000;
        _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0) != 0)
            _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
        goto ret1;
    }

    if (DecIfAbove0_Short((char *)&unk_32c) == 0) {

        s32 lower, upper, y, in;
        _ZN5Actor9UpdatePosEP12CylinderClsn(((char *)this), 0);
        lower = unk_324;
        y = mPosY;
        upper = lower + 0x14a000;
        if (y < lower)
            goto Lzero;
        if (y <= upper) {
            in = 1;
            goto Lchk;
        }
Lzero:
        in = 0;
Lchk:
        if (in != 0)
            goto tail;
        {
            s32 v = lower;
            if (y >= lower) {
                s32 t = upper;
                if (y <= upper)
                    t = y;
                v = t;
            }
            mPosY = v;
        }

        {
            u8 *tog = (u8 *)(((int)((char *)this) + 0x32e) & 0xFFFFFFFFFFFFFFFF);
            u8 tv = *tog;
            *tog = tv ^ 1;
            {
                u8 *tbl_t = data_ov065_0211c0d4;
                u8 *tbl_s = data_ov065_0211c0d0;
                int idx = data_0209f2c0;
                u8 t = mMoveDir;
                unk_32c = *(u16 *)(&tbl_t[idx * 16] + t * 8);
                t = mMoveDir;
                unk_0a8 = *(s32 *)(&tbl_s[idx * 16] + t * 8);
                if (data_0209f2c0 != 2)
                    goto tail;
                {
                    u32 r = (u32)RandomIntInternal(&data_0209e650);
                    unk_32c = (u16)((r % 6) * 0x14 + 0xa);
                }
            }
        }

    }

tail:
    _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
ret1:
    return 1;
}
