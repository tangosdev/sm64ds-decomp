typedef int s32;
typedef short s16;
typedef long long s64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12 a, int b);
extern int AngleDiff(int a, int b);
extern void ApproachAngle(s16* cur, s16 target, int divisor, int band, int maxStep);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern int _ZN6Player7IsStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020bf30c(void* c, int a);
extern int func_ov002_020bf224(void* c, int a, int b);

extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern s16 data_02082214[];
extern int data_ov002_0211055c[];

void func_ov002_020e28d4(char* c, int a, int b)
{
    int r4;

    if (*(int*)(c + 8) == 1)
        a <<= 1;

    if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        if (*(u8*)(c + 0x6e9) & 2) {
            int t = _ZN4cstd5atan2E5Fix12IiES1_(*(int*)(c + 0x560), *(int*)(c + 0x568));
            if (AngleDiff((s16)(t + 0x8000), *(s16*)(c + 0x94)) < 0x2000) {
                if (*(int*)(c + 0x98) >= 0x3000)
                    *(int*)(c + 0x98) = 0x3000;
            }
        }
        if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_0211055c)) {
            r4 = func_ov002_020bf30c(c, 0x3c000);
            s16 e = AngleDiff(*(s16*)(c + 0x6d2), *(s16*)(c + 0x8e));
            e = e + 0x4000;
            s16 tv = data_02082214[((u16)e >> 4) * 2];
            r4 = (int)(((s64)r4 * tv + 0x800) >> 12);
            if (r4 < 0)
                r4 = -r4;
            if (r4 < 0xa000)
                r4 = 0xa000;
        } else {
            r4 = func_ov002_020bf224(c, func_ov002_020bf30c(c, 0x21000), func_ov002_020bf30c(c, 0xa000));
        }

        s16 r7 = *(s16*)(c + 0x6d2);
        if (*(int*)(c + 0x98) != 0) {
            if (AngleDiff(*(s16*)(c + 0x94), r7) > 0x6000) {
                r7 = r7 + 0x8000;
                r4 = -r4;
            }
            ApproachAngle((s16*)(c + 0x94), r7, 0x10, 0x100, 0);
        } else {
            *(s16*)(c + 0x94) = r7;
        }
    } else {
        r4 = 0;
        a = b;
    }

    _Z14ApproachLinearRiii((int*)(c + 0x98), r4, a);

    if (AngleDiff(*(s16*)(c + 0x94), *(s16*)(c + 0x8e)) < 0x4000)
        return;

    if (*(int*)(c + 0x98) >= 0x10000)
        *(int*)(((long long)(int)(c + 0x98)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x2000;
}
