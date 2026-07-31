//cpp
// @symbol _ZN6Camera8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Camera.h"
#include "decl_Particle.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Camera.h"
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;
typedef s32 Fix12i;

enum Bool { FALSE, TRUE };

extern "C" {
extern int _ZN6Camera11ChangeStateEPNS_5StateE(void *thiz, void *state);
extern short Vec3_HorzAngle(const void *v0, const void *v1);
extern short Vec3_VertAngle(const void *v1, const void *v0);
extern void _Z14ApproachLinearRiii(int &dst, int target, int step);
extern int _Z15ApproachLinear2Rsss(short &dst, short target, short step);
extern void MulVec3Mat4x3(void *dst, void *src, void *out);
extern int *Vec3_LslInPlace(int *v, int sh);
}

extern s32 data_0209fc48;
extern u8 data_0209f2c4;
extern u8 data_0209f20c;
extern u8 data_0209f294;
extern s32 data_0209b454;
extern s8 data_0209f2f8;
extern s32 data_0209f32c;
extern s16 data_02082214[];
extern char data_0209b41c;

int Camera::Behavior()
{
    char *c = (char *)((void *)this);
    s32 spVec[3];
    s32 temp_r1;
    s32 r5;

    *(s32 *)(c + 0xc8) = *(s32 *)(c + 0x80);
    *(s32 *)(c + 0xcc) = *(s32 *)(c + 0x84);
    *(s32 *)(c + 0xd0) = *(s32 *)(c + 0x88);
    *(s32 *)(c + 0xd4) = *(s32 *)(c + 0x8c);
    *(s32 *)(c + 0xd8) = *(s32 *)(c + 0x90);
    *(s32 *)(c + 0xdc) = *(s32 *)(c + 0x94);
    *(u32 *)(((int)c + 0x154)) &= ~0x1000u;
    if (*(u32 *)(c + 0x140) != (u32)&data_0208733c) {
        *(u32 *)(((int)c + 0x154)) &= ~0x60u;
    }
    temp_r1 = *(s32 *)(c + 0x154);
    if (!(temp_r1 & 8)) {
        enum Bool b = (enum Bool)(data_0209fc48 != 0);
        if (b != FALSE) goto block_7;
    } else {
block_7:
        if (temp_r1 & 0xc000) {
            *(u32 *)((long long)(int)(c + 0x154)) &= ~0xc000u;
            _ZN6Camera11ChangeStateEPNS_5StateE(c, &data_0209b008);
        }
        *(s16 *)(c + 0x17c) = Vec3_HorzAngle(c + 0x80, c + 0x8c);
        *(s16 *)(c + 0x17e) = Vec3_VertAngle(c + 0x80, c + 0x8c);
        func_020089f8(c);
    }

    if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0 && !(data_0209b454 & 0x40000000)) {
        enum Bool b2 = (enum Bool)(data_0209fc48 != 0);
        if (b2 == FALSE) {
            if (*(s32 *)(c + 0x154) & 0x2000) {
                Math_Function_0203b0fc((int *)(c + 0x168), 0x1000, 0x88, 0x7fffffff);
            } else {
                Math_Function_0203b0fc((int *)(c + 0x168), 0, 0x88, 0x7fffffff);
            }
            func_0200ca50(c);
            if (*(s32 *)(c + 0x134) != 0) {
                _Z14ApproachLinearRiii(*(int *)(c + 0x134), 0, 0x1000);
                *(s32 *)(c + 0x134) = 0 - *(s32 *)(c + 0x134);
            }
            if (*(s16 *)(c + 0x18c) != 0) {
                if (_Z15ApproachLinear2Rsss(*(short *)(c + 0x18c), 0, 0x10) != 0) {
                    *(s16 *)(c + 0x18a) = 0;
                } else {
                    *(s16 *)(((long long)(int)(c + 0x18a))) =
                        (s16)(*(s16 *)(((long long)(int)(c + 0x18a))) + 0x3000);
                }
            }
        }
        if (*(s16 *)(c + 0x18e) != 0) {
            *(s16 *)(((long long)(int)(c + 0x192))) =
                (s16)(*(s16 *)(((long long)(int)(c + 0x192))) + *(s16 *)(c + 0x194));
            _Z15ApproachLinear2Rsss(*(short *)(((long long)(int)(c + 0x18e))), 0, *(s16 *)(c + 0x190));
        }
        {
            int condA = 1;
            if (data_0209f2f8 != 0xa && data_0209f2f8 != 0x13) condA = 0;
            if (condA != 0) {
                r5 = 0x112;
            } else {
                u8 t = (u8)data_0209f2f8 + 0xf8;
                enum Bool ip = FALSE;
                if ((u32)t <= 0x16) {
                    if ((1 << t) & 0x402401) {
                        ip = TRUE;
                    }
                }
                if (ip != FALSE) {
                    r5 = 0xbc;
                } else if (data_0209f2f8 == 0x31) {
                    r5 = 0x86;
                } else {
                    r5 = -1;
                }
            }
        }
        if (r5 >= 0) {
            MulVec3Mat4x3(&CAM_SPACE_CAM_POS_ASR_3, &data_0209b41c, spVec);
            Vec3_LslInPlace(spVec, 3);
            int flag;
            if (r5 != 0xbc) {
                flag = 1;
            } else {
                flag = _ZNK6Camera12IsUnderwaterEv(c);
            }
            *(u32 *)(c + 0x15c) = _ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj(
                *(u32 *)(c + 0x15c), (u32)r5, spVec[0], spVec[1], spVec[2], 0, flag);
            if (r5 == 0x86) {
                *(u32 *)(c + 0x160) = _ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj(
                    *(u32 *)(c + 0x160), 0x87, spVec[0], spVec[1], spVec[2], 0, 1);
            }
        }
    }

    {
        char *m = *(char **)(c + 0x110);
        enum Bool isBf = (enum Bool)(*(u16 *)(m + 0xc) == 0xbf);
        if (isBf != FALSE) {
            s32 t2 = data_0209f32c - (data_02082214[(*(u16 *)(c + 0x17e) >> 4) * 2] * 0x10);
            if (!(*(s32 *)(c + 0x154) & 1)) {
                if (*(u8 *)(m + 0x706) != 0 || *(u8 *)(m + 0x707) != 0) {
                    if (*(s32 *)(c + 0x90) <= t2) {
                        *(u32 *)((long long)(int)(c + 0x154)) |= 1u;
                    }
                }
            } else if (*(s32 *)(c + 0x90) > t2) {
                *(u32 *)((long long)(int)(c + 0x154)) &= ~1u;
            }
        }
    }
    {
        u32 *pf = (u32 *)(((long long)(int)(c + 0x154)));
        *pf &= 0xfff8fdfbu;
        if (*(s32 *)(c + 0x118) != 0) {
            *(s32 *)(c + 0x118) = 0;
            *pf |= 4u;
        }
    }
    *(s32 *)(c + 0x114) = 0;
    func_0203dafc((short)(*(s16 *)(c + 0x17c) + *(s16 *)(c + 0x188)));
    _Z15ApproachLinear2Rsss(*(short *)(c + 0x188), 0, 0x400);
    return 1;
}
