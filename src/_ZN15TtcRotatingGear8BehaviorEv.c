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

int _ZN15TtcRotatingGear8BehaviorEv(char *c)
{
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
            *(s32 *)(c + 0x60) = v;
        }

        {
            u8 *tog = (u8 *)(((int)c + 0x32e));
            u8 tv = *tog;
            *tog = tv ^ 1;
            {
                u8 *tbl_t = data_ov065_0211c0d4;
                u8 *tbl_s = data_ov065_0211c0d0;
                int idx = data_0209f2c0;
                u8 t = *(u8 *)(c + 0x32e);
                *(u16 *)(c + 0x32c) = *(u16 *)(&tbl_t[idx * 16] + t * 8);
                t = *(u8 *)(c + 0x32e);
                *(s32 *)(c + 0xa8) = *(s32 *)(&tbl_s[idx * 16] + t * 8);
                if (data_0209f2c0 != 2)
                    goto tail;
                {
                    u32 r = (u32)RandomIntInternal(&data_0209e650);
                    *(u16 *)(c + 0x32c) = (u16)((r % 6) * 0x14 + 0xa);
                }
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
