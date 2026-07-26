//cpp
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;

extern "C" {
void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
u16 DecIfAbove0_Short(void *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *c, void *cc);
int RandomIntInternal(int *seed);

extern u8 data_0209f2c0;
extern int data_0209e650;
extern u8 data_ov065_0211c0d4[];
extern u8 data_ov065_0211c0d0[];
}

class TtcRotatingGear {
public:
    int Behavior();
};

int TtcRotatingGear::Behavior()
{
    char *c = (char *)this;

    if (data_0209f2c0 == 3) {
        *(s32 *)(c + 0x60) = *(s32 *)(c + 0x324) + 0x14a000;
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
            _ZN8Platform19UpdateClsnPosAndRotEv(c);
        goto ret1;
    }

    if (DecIfAbove0_Short(c + 0x32c) == 0) {
        s32 lower, upper, y, in;
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        lower = *(s32 *)(c + 0x324);
        y = *(s32 *)(c + 0x60);
        upper = lower + 0x14a000;
        if (y < lower)
            goto setzero;
        if (y <= upper) {
            in = 1;
            goto check;
        }
setzero:
        in = 0;
check:
        if (in != 0)
            goto tail;
        {
            s32 v = lower;
            if (y >= lower) {
                s32 v2 = upper;
                if (y <= upper)
                    v2 = y;
                v = v2;
            }
            *(s32 *)(c + 0x60) = v;
        }
        {
            u8 *tog = (u8 *)(((int)c + 0x32e) & 0xFFFFFFFFFFFFFFFFLL);
            *tog ^= 1;
            *(u16 *)(c + 0x32c) = *(u16 *)(&data_ov065_0211c0d4[data_0209f2c0 * 16] + *(u8 *)(c + 0x32e) * 8);
            *(s32 *)(c + 0xa8) = *(s32 *)(&data_ov065_0211c0d0[data_0209f2c0 * 16] + *(u8 *)(c + 0x32e) * 8);
            if (data_0209f2c0 != 2)
                goto tail;
            {
                u32 r = (u32)RandomIntInternal(&data_0209e650);
                *(u16 *)(c + 0x32c) = (u16)((r % 6) * 0x14 + 0xa);
            }
        }
    }

tail:
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
ret1:
    return 1;
}
