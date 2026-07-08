//cpp
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;

struct Vector3 { s32 x, y, z; };
struct State;

extern "C" void _Z14ApproachLinearRiii(s32 &, s32, s32);
extern "C" void func_ov002_020de968(void *);
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void *, State &);
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(u32, const Vector3 &);
extern "C" s16 _ZN4cstd5atan2E5Fix12IiES1_(s32, s32);
extern "C" int AngleDiff(s32, s16);
extern "C" void func_ov002_020de428(void *);
extern "C" void ApproachAngle(void *, s16, s16, s16, s32);
extern "C" s32 func_ov002_020bf224(void *, s32, s32);
extern "C" void _Z15ApproachLinear2Rsss(s16 &, s16, s16);
extern "C" void func_ov002_020de3d0(void *, s16 *, s16 *);
extern "C" void _ZN9Animation7AdvanceEv(void *);

extern "C" u8 data_020a0e40;
extern "C" s16 data_0209f49e[];
extern "C" s16 data_0209f4a0[];
extern "C" s16 SINE_TABLE[];
extern "C" State _ZN6Player7ST_FALLE;
extern "C" State _ZN6Player15ST_GROUND_POUNDE;

extern "C" s32 _ZN6Player15St_Balloon_MainEv(char *c)
{
    if (*(u8 *)(c + 0x70c) != 0) {
        (*(u8 *)(((int)c + 0x70c) & 0xFFFFFFFFFFFFFFFF))--;
    }
    *(s32 *)(c + 0x684) = *(s32 *)(c + 0x60);
    *(s32 *)(c + 0x9c) = 0;
    *(s32 *)(c + 0xa0) = -0xc000;
    _Z14ApproachLinearRiii(*(s32 *)(c + 0xa8), -0x6000, 0x800);

    if (*(u16 *)(c + 0x6c0) == 0) {
        func_ov002_020de968(c);
        _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player7ST_FALLE);
        return 1;
    }

    u16 flags = *(u16 *)((char *)&data_0209f49e + data_020a0e40 * 0x18);
    if (flags & 0x400) {
        func_ov002_020de968(c);
        _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player15ST_GROUND_POUNDE);
        return 1;
    }

    if ((flags & 2) && *(u16 *)(c + 0x6a6) == 0) {
        if (*(u16 *)(c + 0x6a4) == 0) {
            *(s32 *)(((int)c + 0xa8) & 0xFFFFFFFFFFFFFFFF) += 0xc000;
            if (*(s32 *)(c + 0xa8) < 0xc000)
                *(s32 *)(c + 0xa8) = 0xc000;
            if (*(s32 *)(c + 0xa8) > 0x10000)
                *(s32 *)(c + 0xa8) = 0x10000;
        } else {
            *(s32 *)(((int)c + 0xa8) & 0xFFFFFFFFFFFFFFFF) += 0x6000;
            if (*(s32 *)(c + 0xa8) < 0x6000)
                *(s32 *)(c + 0xa8) = 0x6000;
            if (*(s32 *)(c + 0xa8) > 0xc000)
                *(s32 *)(c + 0xa8) = 0xc000;
        }
        _ZN5Sound9PlayBank0EjRK7Vector3(0xbe, *(Vector3 *)(c + 0x74));
        *(u16 *)(c + 0x6a4) = 8;
    }

    if (*(u8 *)(c + 0x6e9) & 2) {
        s16 ang = _ZN4cstd5atan2E5Fix12IiES1_(*(s32 *)(c + 0x560), *(s32 *)(c + 0x568));
        s16 diff = AngleDiff(ang, *(s16 *)(c + 0x94));
        if ((s32)diff > 0x4000) {
            func_ov002_020de428(c);
            *(s16 *)(c + 0x94) = (s16)((ang * 2 + 0x8000) - *(s16 *)(c + 0x94));
            *(s32 *)(c + 0x98) = (s32)(((long long)*(s32 *)(c + 0x98) * 0x1400 + 0x800) >> 12);
            if (*(s32 *)(c + 0x98) > 0x14000)
                *(s32 *)(c + 0x98) = 0x14000;
            if (*(s32 *)(c + 0x98) < 0x4000)
                *(s32 *)(c + 0x98) = 0x4000;
            s32 idx = ((s32)(u16)diff >> 4) * 2;
            s16 t0 = SINE_TABLE[idx + 1];
            s32 v1 = (s32)(((long long)*(s32 *)(c + 0x98) * t0 + 0x800) >> 12);
            s32 v = (s32)(((long long)v1 * 0xa0 + 0x800) >> 12);
            s16 t1 = SINE_TABLE[idx];
            *(s16 *)(c + 0x69c) = (s16)(((long long)t1 * v + 0x800) >> 12);
            *(s16 *)(c + 0x69e) = (s16)(((long long)t0 * v + 0x800) >> 12);
        }
    }

    u8 mode = *(u8 *)(c + 0x6e9);
    if (mode & 1) {
        s32 g = *(s32 *)(c + 0x640);
        if (g < 0) {
            *(s32 *)(c + 0xa8) = -(s32)(((long long)g * 0x2800 + 0x800) >> 12);
            func_ov002_020de428(c);
        }
    } else if (mode & 4) {
        s32 g = *(s32 *)(c + 0x640);
        if (g > 0) {
            *(s32 *)(c + 0xa8) = -(s32)((((long long)g << 14) + 0x800) >> 12);
            *(u16 *)(c + 0x6a6) = 0x1e;
            func_ov002_020de428(c);
        }
    }

    if (*(s32 *)(c + 0xa8) >= 0xa000)
        *(s32 *)(c + 0xa8) = 0xa000;

    s32 r4 = 0;
    if (*(s16 *)((char *)&data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        ApproachAngle(c + 0x94, *(s16 *)(c + 0x6d2), 0x20, 0x2000, 0x10);
        r4 = 0x10000;
    }
    _Z15ApproachLinear2Rsss(*(s16 *)(c + 0x8e), *(s16 *)(c + 0x94), 0x200);
    _Z14ApproachLinearRiii(*(s32 *)(c + 0x98), func_ov002_020bf224(c, r4, 0), 0x100);
    func_ov002_020de3d0(c, (s16 *)(c + 0x90), (s16 *)(c + 0x69c));
    func_ov002_020de3d0(c, (s16 *)(c + 0x8c), (s16 *)(c + 0x69e));
    *(s32 *)(c + 0x640) = *(s32 *)(c + 0xa8);
    _ZN9Animation7AdvanceEv(c + 0x140);
    return 1;
}
