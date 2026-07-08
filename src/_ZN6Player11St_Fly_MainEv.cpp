//cpp
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef long long s64;

struct State;

struct Player {
    int St_Fly_Main();
    void ChangeState(State &);
    void SetAnim(unsigned int, int, int, unsigned int);
    int IsAnim(unsigned int);
    int FinishedAnim();
    unsigned char GetBodyModelID(unsigned int, bool) const;
};

extern "C" int func_ov002_020d674c(char *c);
extern "C" void func_ov002_020c2f64(void *c);
extern "C" void func_ov002_020df8f0(void *c);
extern "C" int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32, u32, s32, s32, s32, void *, void *);
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(u32, void *);
extern "C" void func_ov002_020e25f0(char *c, int a);
extern "C" void MulVec3Mat4x3(void *dst, void *src, void *out);
extern "C" void Vec3_MulScalarInPlace(s32 *v, s32 s);
extern "C" int func_02012194(char *c, int a1, int a2, int a3, int s0, void *s1, int s2);
extern "C" void func_ov002_020bedd4(char *self);

extern State _ZN6Player10ST_SWALLOWE;
extern State _ZN6Player15ST_GROUND_POUNDE;
extern State _ZN6Player7ST_FALLE;
extern State _ZN6Player7ST_DIVEE;
extern u8 data_020a0e40;
extern u16 data_0209f49e;
extern u16 data_0209f49c;
extern s16 SINE_TABLE[];

struct Vec16 { s16 x, y, z; };
struct Vec32 { s32 x, y, z; };

#define c ((char *)this)

int Player::St_Fly_Main()
{
    *(s32 *)(c + 0x684) = *(s32 *)(c + 0x60);

    if ((*(u16 *)((char *)&data_0209f49e + (&data_020a0e40)[0] * 0x18) & 0x400) == 0) {
        goto skip;
    }
    if (func_ov002_020d674c(c) != 0) {
        this->ChangeState(_ZN6Player10ST_SWALLOWE);
    } else {
        this->ChangeState(_ZN6Player15ST_GROUND_POUNDE);
    }
    return 1;

skip:
    if (*(u8 *)(c + 0x6ff) == 0) {
        this->ChangeState(_ZN6Player7ST_FALLE);
        return 1;
    }
    if (*(u8 *)(c + 0x6de) == 0) {
        func_ov002_020c2f64(c);
        this->ChangeState(_ZN6Player7ST_DIVEE);
        this->SetAnim(0x43, 0x40000000, 0x1000, 0);
        *(s32 *)(c + 0xa8) = 0;
        return 1;
    }

    if (this->IsAnim(0x4a) != 0) {
        if (*(s32 *)(c + 0xa8) > 0x20000) {
            *(s32 *)(c + 0x9c) = -0x3400;
            if (*(s32 *)(c + 0xa8) >= 0) {
                if ((*(u16 *)((char *)&data_0209f49c + (&data_020a0e40)[0] * 0x18) & 2) == 0) {
                    *(s32 *)(c + 0x9c) = -0x8000;
                }
            } else {
                *(s32 *)(c + 0x9c) = -0x2000;
                *(s32 *)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFF) = *(s32 *)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFF) + 0xe00;
            }
            if (this->FinishedAnim() != 0) {
                *(s32 *)(c + 0x9c) = 0;
                *(s32 *)(c + 0xa0) = -0x4b000;
                this->SetAnim(0x49, 0, 0x1000, 0);
            }
        } else {
            *(s32 *)(c + 0x9c) = 0;
            *(s32 *)(c + 0xa0) = -0x4b000;
            func_ov002_020df8f0(c);
        }
    } else {
        func_ov002_020df8f0(c);
    }

    Vec16 vec16;
    Vec32 vec32;
#define spC vec16.x
#define spE vec16.y
#define sp10 vec16.z
#define sp14 vec32.x
#define sp18 vec32.y
#define sp1C vec32.z
    {
        s16 n = SINE_TABLE[(*(u16 *)(c + 0x92) >> 4) * 2 + 1];
        s32 r0 = (s16)-n;
        s16 t = SINE_TABLE[(*(u16 *)(c + 0x8e) >> 4) * 2];
        spC = (s16)(((s64)r0 * t + 0x800) >> 0xc);
        spE = SINE_TABLE[(*(u16 *)(c + 0x8c) >> 4) * 2];
        s16 t2 = SINE_TABLE[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];
        sp10 = (s16)(((s64)r0 * t2 + 0x800) >> 0xc);
    }

    if (*(s32 *)(c + 0x640) >= 0x28000) {
        *(s32 *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32 *)(c + 0x628), 0xd1, *(s32 *)(c + 0x5c), *(s32 *)(c + 0x60), *(s32 *)(c + 0x64), &vec16, 0);
    }

    if (*(s32 *)(c + 0x640) >= 0x30000) {
        if (*(s16 *)(c + 0x92) > 0x800) {
            if (*(u8 *)(c + 0x70c) == 0) {
                _ZN5Sound9PlayBank0EjRK7Vector3(0xb9, c + 0x74);
                func_ov002_020e25f0(c, 2);
                *(u8 *)(((int)c + 0x70c) & 0xFFFFFFFFFFFFFFFF) = *(u8 *)(((int)c + 0x70c) & 0xFFFFFFFFFFFFFFFF) + 1;
            }
            {
                char *m = *(char **)(c + this->GetBodyModelID(*(u32 *)(c + 8) & 0xff, 0) * 4 + 0xdc);
                s32 r5 = *(s32 *)(m + 0x14) + 0x2a0;
                char *m2 = *(char **)(c + this->GetBodyModelID(*(u32 *)(c + 8) & 0xff, 0) * 4 + 0xdc);
                MulVec3Mat4x3((void *)(r5 + 0x24), m2 + 0x1c, &vec32);
                Vec3_MulScalarInPlace(&vec32.x, 0x8000);
                *(s32 *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(u32 *)(c + 0x62c), 0xd0, sp14, sp18, sp1C, &vec16, 0);
            }
            {
                char *m = *(char **)(c + this->GetBodyModelID(*(u32 *)(c + 8) & 0xff, 0) * 4 + 0xdc);
                s32 r5 = *(s32 *)(m + 0x14) + 0x210;
                char *m2 = *(char **)(c + this->GetBodyModelID(*(u32 *)(c + 8) & 0xff, 0) * 4 + 0xdc);
                MulVec3Mat4x3((void *)(r5 + 0x24), m2 + 0x1c, &vec32);
                Vec3_MulScalarInPlace(&vec32.x, 0x8000);
                *(s32 *)(c + 0x630) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(u32 *)(c + 0x630), 0xd0, sp14, sp18, sp1C, &vec16, 0);
            }
        }
    } else {
        *(u8 *)(c + 0x70c) = 0;
    }

    {
        s32 v = *(s32 *)(c + 0x640);
        s32 r1 = (v / 0x1000) * 3;
        if (r1 >= 0x100) {
            r1 = 0x100;
        }
        *(s32 *)(c + 0x620) = func_02012194(*(char **)(c + 0x620), 0, 0x104, 3, r1, c + 0x74, 0);
    }
    func_ov002_020bedd4(c);
    return 1;
}
