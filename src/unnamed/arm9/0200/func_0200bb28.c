#include "types.h"
// @symbol func_0200bb28
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
extern u16 data_0209f49c[];
extern u16 data_0209f49e[];

extern u32 func_02012790(u32 a);
extern s32 ApproachAngle(s16* cur, s16 target, s32 div, s32 band, s32 step);
extern u16 DecIfAbove0_Short(u16* p);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern void Vec3_RotateYAndTranslate(s32* out, s32* in, s16 ang, s32* src);
extern s32 Math_Function_0203b14c(s32* p, s32 tgt, s32 rate, s32 lim, s32 step);
extern s32 _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(s32* v, s32* t, s32 rate);

#define L0(p) ((s32)(p))
#define L1(p) ((s32)(p))
#define L2(p) ((s32)((p) | 0LL))
#define L3(p) ((s32)(p))

/* File-local views (the class headers are C++): only the fields this state
 * touches whose role is shown. Camera names follow include/dCamera_c.h; the
 * player's follow include/dActor_c.h. The yaw words 0x180..0x184 and 0x19e,
 * the flag 0x1a0, and 0x144 and 0x164 stay raw: this function is their only
 * reader here, and it does not settle what each one is. */
typedef struct {
    u8 pad_000[0x8e];
    s16 mAngleY; /* 0x08e */
} PlayerView;

typedef struct {
    u8 pad_000[0x80];
    struct Vector3 lookAt; /* 0x080 */
    struct Vector3 pos;    /* 0x08c */
    /* func_020089f8 copies mTargetPlayer->mPos here every frame. */
    struct Vector3 mTargetPos; /* 0x098 */
    u8 pad_0a4[0x6c];
    PlayerView *mTargetPlayer; /* 0x110 */
    u8 pad_114[0x40];
    u32 mFlags; /* 0x154 */
    u8 pad_158[0x24];
    s16 mAngleY; /* 0x17c */
    u8 pad_17e[0x18];
    /* A countdown the states arm and run down: here the delay before a held
       turn input repeats. */
    u16 mStateTimer; /* 0x196 */
} CameraView;

#pragma opt_common_subs off
void func_0200bb28(char* c, char* a1)
{
    CameraView *cam = (CameraView *)c;
    s32 flag4000;
    s32 sp_speed;
    struct Vector3 vsrc;
    struct Vector3 vin;
    struct Vector3 vout;
    s32* tbl;
    s32 comp;
    s32 m;

    cam->mFlags |= 0x1000;

    m = gActivePlayerSlot * 0x18;
    flag4000 = *(u16*)((char*)data_0209f49c + m) & 0x4000;

    if (flag4000 != 0 || *(u16*)(c + 0x1a0) != 0) {
        if (*(u16*)((char*)data_0209f49e + m) & 0x4000) {
            func_02012790(0x1a);
        }
        if (flag4000 != 0) {
            *(s16 *)(c + 0x19e) = (s16)(cam->mTargetPlayer->mAngleY + 0x8000);
        }
        *(u16*)(c + 0x1a0) = (ApproachAngle((s16*)(c + 0x180), *(s16*)(c + 0x19e), 4, 0x400, 0x100) != 0);
        *(s16*)(c + 0x184) = (s16)(*(s16*)(c + 0x180) - *(s16*)(c + 0x182));
        *(s16*)(c + 0x184) = (s16)((*(s16*)(c + 0x184) + 0x1000) & -0x2000);
        cam->mFlags |= 0x60;
    } else {
        if (a1 != 0) {
            if (*(u8*)a1 == 2 && a1 != *(char**)(c + 0x144)) {
                *(s16*)(c + 0x182) = (s16)(*(s16*)(a1 + 0xa) + 0x8000);
                if (*(u8*)(a1 + 1) == 0) {
                    *(s16*)(c + 0x180) = *(s16*)(c + 0x182);
                    cam->mFlags |= 4;
                }
                *(s16*)(c + 0x184) = 0;
                *(char**)(c + 0x144) = a1;
            }
        } else {
            *(char**)(c + 0x144) = 0;
        }
        if (!(cam->mFlags & 0x60)) {
            s32 m2;
            s32 f1;
            DecIfAbove0_Short(&cam->mStateTimer);
            m2 = gActivePlayerSlot * 0x18;
            f1 = *(u16*)((char*)data_0209f49c + m2) & 0x300;
            if (f1 != 0) {
                s32 f2 = *(u16*)((char*)data_0209f49e + m2) & 0x300;
                if (f2 != 0) {
                    *(u32*)(c + 0x164) = f2;
                } else if (!(*(u32*)(c + 0x164) & f1)) {
                    f2 = f1;
                    *(u32*)(c + 0x164) = f1;
                }
                if (f2 != 0 || cam->mStateTimer == 0) {
                    u32 dir = *(u32*)(c + 0x164);
                    if (dir & 0x200) {
                        *(s16*)L0(c + 0x184) -= 0x2000;
                        cam->mFlags |= 0x20;
                    } else if (dir & 0x100) {
                        *(s16*)L0(c + 0x184) += 0x2000;
                        cam->mFlags |= 0x40;
                    }
                    cam->mStateTimer = 0x14;
                }
            }
        }
        _Z14ApproachLinearRsss((s16*)(c + 0x180), (s16)(*(s16*)(c + 0x182) + *(s16*)(c + 0x184)), 0x800);
    }

    tbl = (cam->mFlags & 2) ? data_02086d50 : data_02086cc8;
    sp_speed = tbl[1];
    func_0200c9e0(c, &sp_speed, &sp_speed);
    comp = FX_MUL(tbl[0], sp_speed);
    vsrc.x = 0;
    vsrc.z = 0;
    vsrc.y = comp;
    {
        s32 t0 = tbl[0];
        vsrc.z = _ZN4cstd4sqrtEy((s64)t0 * t0 - (s64)comp * comp);
    }
    {
        s32 tz = cam->mTargetPos.z;
        s32 ty = cam->lookAt.y;
        s32 tx = cam->mTargetPos.x;
        vin.x = tx;
        vin.y = ty;
        vin.z = tz;
    }
    Vec3_RotateYAndTranslate(&vout.x, &vin.x, *(s16*)(c + 0x180), &vsrc.x);
    if (cam->mFlags & 4) {
        cam->pos.x = vout.x;
        cam->pos.y = vout.y;
        cam->pos.z = vout.z;
    }
    Math_Function_0203b14c(&cam->pos.y, vout.y, 0x300, 0x18000, 0x100);
    vout.y = cam->pos.y;
    if (_Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&cam->pos.x, &vout.x,
                                                 flag4000 != 0 ? 0xc0000 : 0x40000) != 0 &&
        flag4000 == 0) {
        cam->mFlags &= ~0x60;
    }
    cam->mAngleY = *(s16 *)(c + 0x180);
}