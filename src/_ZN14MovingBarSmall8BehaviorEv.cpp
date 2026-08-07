//cpp
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
extern "C" {
u8 DecIfAbove0_Byte(u8 *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *c, void *cc);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
int Vec3_Dist(const void *a, const void *b);
u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 d, const void *v, u32 e);
void func_020393a4(int *p, int v);
void func_02039394(int *p, int v);
void func_ov015_021123c8(char *c);
}
extern "C" int _ZN14MovingBarSmall8BehaviorEv(char *c)
{
    int v = *(int *)(c + 8) & 0xff;
    if (v == 1) {
        if (DecIfAbove0_Byte((u8 *)(c + 0x390)) == 0) {
            *(s16 *)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFFLL) += 0x8000;
            *(u8 *)(c + 0x390) = 0x87;
        }
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x150000, 0x1000))
            _ZN8Platform19UpdateClsnPosAndRotEv(c);
        goto done;
    }
    if (v == 2) {
        if (DecIfAbove0_Byte((u8 *)(c + 0x390)) == 0 && *(u8 *)(c + 0x392) != 0) {
            _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);

            s32 y, lim; int b;
            lim = *(s32 *)(c + 0x37c);
            y = *(s32 *)(c + 0x60);
            b = (y >= lim);
            if (b == false) {
                *(s32 *)(c + 0x60) = (y < lim) ? lim : y;
                *(s32 *)(c + 0xa8) = 0xa000;
                if (*(u8 *)(c + 0x391) == 0) *(u8 *)(c + 0x392) = 0;
            } else {
                lim = *(s32 *)(c + 0x380);
                b = y <= lim;
                if (b == false) {
                    *(s32 *)(c + 0x60) = (y > lim) ? lim : y;
                    *(s32 *)(c + 0xa8) = -0xa000;
                    *(u8 *)(c + 0x390) = 0x5a;
                }
            }

        }
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x150000, 0x1000))
            _ZN8Platform19UpdateClsnPosAndRotEv(c);
        if (Vec3_Dist(c + 0x5c, c + 0x68) != 0) {
            *(u32 *)(c + 0x38c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
                *(u32 *)(c + 0x38c), 3, 0x82, c + 0x74, 0);
        }
        *(u8 *)(c + 0x391) = 0;
        goto done;
    }
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x150000, 0x1000);
done:
    func_020393a4((int *)(c + 0x124), 0x150000);
    func_02039394((int *)(c + 0x124), 0x1000);
    func_ov015_021123c8(c);
    return 1;
}
