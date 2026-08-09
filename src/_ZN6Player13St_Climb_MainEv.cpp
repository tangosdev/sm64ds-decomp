//cpp
// @symbol _ZN6Player13St_Climb_MainEv
/* Player::St_Climb_Main (ov002:0x020cb5bc, 0x778).
 *
 * func_ov002_020cc05c takes the pad-held halfword as a second argument it
 * never reads (decl_common.h declares (void*, unsigned short); the matched
 * St_Headstand_Main passes it too). Dropping it recolors the whole
 * case-1/tail scratch web down one register (r2 -> r1, 9 words); passing it
 * keeps the data_0209f49c load live in r1 through both call sites, which is
 * the ROM's coloring.
 *
 * Honest residue (plan-cpp-language-mode section 3, principle 2):
 *  - SetAnim stays a mangled extern: it takes Fix12<int> BY VALUE, the
 *    audit's excluded-by-value class, and Player.h does not declare it.
 *  - ChangeState stays a mangled extern: Player::State is not declared yet.
 *  - The object at this+0x37c (the ground/platform node) has no real header
 *    class; its slot-2 virtual (returns the position vector) is called
 *    through a raw function-pointer read, same spelling St_Climb_Init uses,
 *    rather than a local shadow struct.
 *  - Fields are raw offsets off (char*)this; Player.h names are still
 *    placeholders below 0x768 and renaming cannot change codegen.
 */
#include "decl_common.h"
#include "Player.h"

/* slot 2 of the ground node's vtable: s32* GetPos(void) thiscall */
typedef s32* (*GroundPosFn)(void*);

extern "C" {
extern void Vec3_RotateYAndTranslate(s32* out, s32* in, s16 angle, s32* src);
extern void func_0200d5c0(void* cam, u8 playerID);
extern void func_0200d5fc(void* cam, s32 playerID);
extern void ApproachAngle(s16* cur, s16 target, s32 divisor, s32 band, s32 maxStep);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, s32 a, s32 b, u32 d);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020cb354(void* c);
extern s16 GetAngleToCamera(u8 i);
extern void _Z14ApproachLinearRsss(s16* cur, s16 target, s16 step);
extern void func_ov002_020bf340(void* c, s32* p, s32 delta, s32 limit);
extern void func_ov002_020cb400(void* c);
extern void func_ov002_020cb474(void* c);
extern int func_ov002_020cbea8(void* c);
extern void func_ov002_020bedd4(void* c);
extern void* data_0209f318;
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern u16 data_0209f49c[];
extern s16 data_02082214[];
extern int data_ov002_021101b4[];
extern int data_ov002_021106f4[];
extern u8 data_0209f4ac[];
extern int data_ov002_0211013c[];
extern int data_ov002_0211070c[];
}

int Player::St_Climb_Main()
{
    char* c = (char*)this;
    s32 vbuf[6];

    *(s32*)(c + 0x684) = *(s32*)(c + 0x60);

    {
        char* gr = *(char**)(c + 0x37c);
        s32* p = (*(GroundPosFn*)(*(char**)gr + 8))(gr);
        vbuf[0] = p[0];
        vbuf[1] = p[1];
        vbuf[2] = p[2];
    }
    if (*(s32*)(c + 8) == 2) {
        char* gr;
        vbuf[4] = 0;
        vbuf[3] = 0;
        vbuf[5] = -0x10000;
        gr = *(char**)(c + 0x37c);
        Vec3_RotateYAndTranslate(&vbuf[0], (*(GroundPosFn*)(*(char**)gr + 8))(gr), *(s16*)(c + 0x8e), &vbuf[3]);
    }
    *(s32*)(c + 0x5c) = vbuf[0];
    *(s32*)(c + 0x64) = vbuf[2];

    {
        void* cam = data_0209f318;
        if (*(u32*)(*(char**)(c + 0x37c) + 0x18) & 0x1000000) {
            func_0200d5c0(cam, *(u8*)(c + 0x6d8));
        } else {
            s32 v = (s32)(((s64)(*(s32*)(*(char**)(c + 0x37c) + 8)) * 0xb33 + 0x800) >> 12);
            if (v <= *(s32*)(c + 0x688)) {
                func_0200d5c0(cam, *(u8*)(c + 0x6d8));
            } else {
                func_0200d5fc(cam, *(u8*)(c + 0x6d8));
            }
        }
    }

    switch (*(u8*)(c + 0x6e3)) {
    case 0:
        ApproachAngle((s16*)(c + 0x69c), 0, 8, 0x2000, 0x200);
        if (FinishedAnim()) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x26, 0, 0x1000, 0);
            *(u8*)(c + 0x6e3) = 1;
        }
        goto tail;

    case 1: {
        s16 v;
        u32 idx = (u32)data_020a0e40 * 0x18;
        u16 flagsE = *(u16*)((char*)data_0209f49e + idx);
        u16 flagsC = *(u16*)((char*)data_0209f49c + idx);
        if (flagsE & 0x400) {
            s16 ang = (s16)(*(s16*)(c + 0x8e) + 0x8000);
            s32 t = (u16)ang;
            s32 ti = (t >> 4) * 2;
            s16 cosv = data_02082214[ti];
            *(s32*)(((long long)(int)(c + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL) += (s32)(((s64)cosv * 0x64000 + 0x800) >> 12);
            s16 sinv = data_02082214[ti + 1];
            *(s32*)(((long long)(int)(c + 0x64)) & 0xFFFFFFFFFFFFFFFFLL) += (s32)(((s64)sinv * 0x64000 + 0x800) >> 12);
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
            return 1;
        }

        {
            v = *(s16*)((char*)data_0209f4a4 + idx);
            if (v < -0x400) {
                func_ov002_020cb354(c);
                *(s16*)(c + 0x69c) = 0;
                _Z14ApproachLinearRsss((s16*)(c + 0x8e), GetAngleToCamera(*(u8*)(c + 0x6d8)), 0x200);
                if (*(s32*)(*(char**)(c + 0x37c) + 8) <= *(s32*)(c + 0x688)) {
                    if ((*(u32*)(*(char**)(c + 0x37c) + 0x18) & 0x2000000) &&
                        *(s32*)(c + 0xa8) == 0 &&
                        (s32)(*(s16*)((char*)data_0209f4a4 + (u32)data_020a0e40 * 0x18)) < -0xc00 &&
                        *(u8*)(c + 0x6e5) == 1) {
                        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021106f4);
                    }
                    *(s32*)(c + 0x688) = *(s32*)(*(char**)(c + 0x37c) + 8);
                    *(s32*)(c + 0xa8) = 0;
                } else {
                    u32 idx2 = (u32)data_020a0e40 * 0x18;
                    s32 mag;
                    if (*((u8*)data_0209f4ac + idx2) == 0) {
                        mag = (*(u16*)((char*)data_0209f49c + idx2) & 0x800) ? 0xc000 : 0x6660;
                    } else {
                        mag = 0xc000;
                    }
                    s16 av = *(s16*)((char*)data_0209f4a4 + idx2);
                    if (av < 0) av = -av;
                    s32 fx = (s32)(((s64)mag * av + 0x800) >> 12);
                    func_ov002_020bf340(c, (s32*)(c + 0xa8), 0x800, fx);
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x24, 0, 0x1000, 0);
                    s32 fx2 = (s32)(((s64)fx * 0x500 + 0x800) >> 12);
                    if (fx2 >= 0x4000) fx2 = 0x4000;
                    {
                        s32 modelIdx = GetBodyModelID((u8)(*(s32*)(c + 8)), 0);
                        char* p2 = (char*)(((long long)(int)(*(char**)(c + modelIdx * 4 + 0xdc) + 0x50)) & 0xFFFFFFFFFFFFFFFFLL);
                        *(s32*)(p2 + 0xc) = fx2;
                    }
                    *(u8*)(c + 0x6e5) = 0;
                }
                goto end;
            } else if (v > 0x200) {
                func_ov002_020cb400(c);
                *(s16*)(c + 0x6a4) = 0x1e;
                {
                    u32 idx3 = (u32)data_020a0e40 * 0x18;
                    s32 mag;
                    if (*((u8*)data_0209f4ac + idx3) == 0) {
                        mag = (*(u16*)((char*)data_0209f49c + idx3) & 0x800) ? 0x1e000 : 0xfff0;
                    } else {
                        mag = 0x1e000;
                    }
                    s16 val = *(s16*)((char*)data_0209f4a4 + idx3);
                    s32 fx = (s32)(((s64)mag * val + 0x800) >> 12);
                    func_ov002_020bf340(c, (s32*)(c + 0xa8), -0x800, fx);
                }
                *(s16*)(((long long)(int)(c + 0x69c)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x40;
                {
                    u32 idx4 = (u32)data_020a0e40 * 0x18;
                    s16 av = *(s16*)((char*)data_0209f4a4 + idx4);
                    if (av < 0) av = -av;
                    if (*(s16*)(c + 0x69c) <= av) av = *(s16*)(c + 0x69c);
                    *(s16*)(c + 0x69c) = av;
                }
                if (!IsAnim(0x25)) {
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x25, 0, 0x1000, 0);
                }
                if (*(s32*)(c + 0x688) < -0x64000) {
                    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
                    return 1;
                }
                goto end;
            } else {
                *(s16*)(c + 0x6a4) = 0x1e;
                *(s32*)(c + 0xa8) = 0;
                func_ov002_020cc05c(c, flagsC);
                if (!IsAnim(0x26)) {
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x26, 0, 0x1000, 0);
                }
                goto end;
            }
        }
    }

    default:
        goto tail;
    }

end:
    if (*(u8*)(c + 0x6de) == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        return 1;
    }
    if (*(u16*)((char*)data_0209f49e + (u32)data_020a0e40 * 0x18) & 2) {
        *(s16*)(c + 0x94) = *(s16*)(c + 0x8e);
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211070c);
        return 1;
    }

tail:
    {
        char* gr = *(char**)(c + 0x37c);
        s32* p3 = (*(GroundPosFn*)(*(char**)gr + 8))(gr);
        s32 y = p3[1];
        if ((*(s32*)(c + 0x60) - y) < -0x64000) {
            *(s32*)(c + 0x60) = y - 0x64000;
        }
    }
    if (*(s32*)(c + 0x688) < -0x64000) {
        *(s32*)(c + 0x688) = -0x64000;
    }
    {
        s32 t2 = *(s32*)(*(char**)(c + 0x37c) + 8);
        if (t2 <= *(s32*)(c + 0x688)) {
            *(s32*)(c + 0x688) = t2;
        }
    }
    if (*(u16*)(c + 0x6a8) != 0 || (*(s32*)(c + 0xa8) != 0 && *(u8*)(c + 0x6e3) == 1)) {
        func_ov002_020cb474(c);
    }
    {
        s16* p8e = (s16*)(((long long)(int)(c + 0x8e)) & 0xFFFFFFFFFFFFFFFFLL);
        *p8e = (s16)(*p8e + *(s16*)(c + 0x69c));
    }
    {
        s32* p688 = (s32*)(((long long)(int)(c + 0x688)) & 0xFFFFFFFFFFFFFFFFLL);
        *p688 += *(s32*)(c + 0xa8);
    }
    if (func_ov002_020cbea8(c)) {
        return 1;
    }
    {
        char* gr = *(char**)(c + 0x37c);
        s32* p4 = (*(GroundPosFn*)(*(char**)gr + 8))(gr);
        *(s32*)(c + 0x60) = *(s32*)(c + 0x688) + p4[1];
    }
    if (*(s16*)((char*)data_0209f4a4 + (u32)data_020a0e40 * 0x18) == 0) {
        *(u8*)(c + 0x6e5) = 1;
    }
    func_ov002_020bedd4(c);
    return 1;
}
