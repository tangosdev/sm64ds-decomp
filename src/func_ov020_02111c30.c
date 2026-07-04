typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef long long s64;

extern int func_ov020_02111418(char *c);
extern void _ZN9Animation7AdvanceEv(void *);
extern int _Z14ApproachLinearRiii(s32 *, int, int);
extern int _ZN9Animation8FinishedEv(void *);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, int);
extern void func_ov020_021112b0(char *c);
extern int _Z14ApproachLinearRsss(s16 *, short, short);

extern void *data_ov020_02114ab0[];
extern s16 data_02082214[];

#define A8C (*(u16 *)((char *)c + 0x8c))
#define A8E (*(u16 *)((char *)c + 0x8e))

void func_ov020_02111c30(char *c)
{
    if (func_ov020_02111418(c))
        return;

    _ZN9Animation7AdvanceEv(c + 0x160);

    if (_Z14ApproachLinearRiii((s32 *)(c + 0x98), 0xa000, 0xa00) == 0)
        return;

    if (_ZN9Animation8FinishedEv(c + 0x160)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov020_02114ab0[1], 0, 0x1000, 0);
        *(s32 *)(c + 0x424) = 3;
        *(s32 *)(c + 0x98) = 0;
        func_ov020_021112b0(c);

        *(s16 *)(c + 0x92) = -*(s16 *)(c + 0x444);
        *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x92);
        *(s16 *)(c + 0x94) = *(s16 *)(c + 0x446);
        *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);

        {
            s32 r = (s32)(((s64)data_02082214[(A8C >> 4) * 2 + 1] * 0x32000 + 0x800) >> 12);
            *(s32 *)(c + 0xa4) = (s32)(((s64)r * data_02082214[(A8E >> 4) * 2] + 0x800) >> 12);
            *(s32 *)(c + 0xa8) = (s32)(((s64)data_02082214[(A8C >> 4) * 2] * -0x32000 + 0x800) >> 12);
            *(s32 *)(c + 0xac) = (s32)(((s64)r * data_02082214[(A8E >> 4) * 2 + 1] + 0x800) >> 12);
        }
        return;
    }

    (*(u16 *)(((int)c + 0x100) & 0xFFFFFFFFFFFFFFFF))++;
    if (*(u16 *)(((int)c + 0x100) & 0xFFFFFFFFFFFFFFFF) < 5)
        return;

    func_ov020_021112b0(c);
    _Z14ApproachLinearRsss((s16 *)(c + 0x8e), *(s16 *)(c + 0x446), 0x7d0);
    _Z14ApproachLinearRsss((s16 *)(c + 0x8c), -*(s16 *)(c + 0x444), 0x7d0);

    if (*(u16 *)(c + 0x100) < 9)
        return;

    _Z14ApproachLinearRsss((s16 *)(c + 0x90), *(s16 *)(c + 0x448), 0x7d0);

    if (*(u16 *)(c + 0x100) < 0x13)
        return;

    _Z14ApproachLinearRiii((s32 *)(c + 0x44c), 0x1800, 0x19a);
    *(s32 *)(c + 0x220) = *(s32 *)(c + 0x44c) * 0x32;
    *(s32 *)(c + 0x224) = *(s32 *)(c + 0x44c) * 0x64;
    *(s32 *)(c + 0x43c) = *(s32 *)(c + 0x44c) * -0x32;
    {
        s32 v = *(s32 *)(c + 0x44c);
        *(s32 *)(c + 0x80) = v;
        *(s32 *)(c + 0x84) = v;
        *(s32 *)(c + 0x88) = v;
    }
}
