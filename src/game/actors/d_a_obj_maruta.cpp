//cpp
/* daObjMaruta_c -- the abstract base of the two rolling logs, and the shared
 * implementation both of them forward to.
 *
 * Licensed .text run: ov080 0x02127058 .. 0x0212766c, eight functions.
 *
 * The class's two destructor variants sit just below this run, at 0x02126fbc
 * (D0) and 0x02127014 (D1), and stay in their own enrolled sources. The
 * cartridge orders D0 BELOW D1; with the destructor defined in the class body
 * -- which include/daObjMaruta_c.h requires, so that both descendants inline
 * the body instead of calling it -- mwccarm 2004/b56 emits D1 then D0, and no
 * admissible source form reverses that. This TU emits its own copies of both
 * because it owns the key function, and they are licensed as deadstripped
 * duplicates; the manifest entry records the measurement.
 *
 * Four of the eight members carry their placeholder func_ov080_ names. They
 * are the class's non-virtual shared implementation, called by name from the
 * two leaves in ov022 and ov030 (RollingLogLll and RollingLogTtm forward
 * InitResources, Behavior and CleanupResources straight into them), so the
 * symbols have to keep C linkage and these spellings.
 */

#include "daObjMaruta_c.h"
#include "Player.h"
#include "types.h"
#include "common.h"

extern "C" {
void func_02012694(int a, void *b);
void Matrix4x3_FromRotationXYZExt(void *, int, int, int);
int _ZN4dBgW9IsEnabledEv(void *);
void _ZN4dBgW7DisableEv(void *);
void _ZN13SharedFilePtr7ReleaseEv(void *);
int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(void *self, s16 a, s16 b, s16 c, int d);
int DecIfAbove0_Byte(u8 *p);
int Vec3_Dist(void *a, void *b);
short Vec3_HorzAngle(void *a, void *b);
int AngleDiff(int a, int b);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *a, void *b);
void Vec3_Sub(void *out, void *a, void *b);
int LenVec3(void *v);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, void *);
u16 DecIfAbove0_Short(u16 *p);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *, int, int);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
int _ZN5Model8LoadFileER13SharedFilePtr(void *f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *o, int bmd, int a, int b);
int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *o, int kcl, void *mtx, int fix, short s, void *clps);
void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
extern int _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern s16 data_02082214[];

/* This translation unit's own six free helpers, forward-declared because the
   members above them in ROM address order -- which are BELOW them in source
   order -- call them. */
void func_ov080_02127094(char *t);
int func_ov080_021270dc(char *t, void **f);
int func_ov080_0212714c(char *c, int *p2);
int func_ov080_021274ac(char *self, char **arg);
void func_ov080_0212758c(char *a0, char *a1);
void func_ov080_02127658(void *a, void *b, void *c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov080_02127658, 0x02127658, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_02127658
/* Arg-shifting tail-call veneer, handed to the dBgW at 0x124 by
   func_ov080_021274ac below. Drops the first argument and forwards the next
   two; long_calls emits the pooled `ldr ip,[pc,#8]; bx ip` absolute
   tail-call. Bracketed closed immediately: long_calls is positional. */
#pragma long_calls on
extern "C" void func_ov080_02127658(void *a, void *b, void *c)
{
    func_ov080_0212758c((char *)b, (char *)c);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov080_0212758c, 0x0212758c, size 0xcc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_0212758c
extern "C" void func_ov080_0212758c(char *a0, char *a1)
{
    int t;
    int half;

    t = (*(volatile u16 *)(a1 + 0xc) == 0xbf);
    if (t != 0) {
        a1 = (char *)(int)(a1 + 0x5c);
        *(int *)(a0 + 0x32c) = *(int *)a1;
        *(int *)(a0 + 0x330) = *(int *)(a1 + 4);
        *(int *)(a0 + 0x334) = *(int *)(a1 + 8);
        *(unsigned char *)(a0 + 0x342) = 1;
        *(unsigned char *)(a0 + 0x33e) = 0x78;

        half = (*(int *)(a0 + 0x98)) >> 1;

        {
            int idx = (*(u16 *)(a0 + 0x8e)) >> 4;
            int sc = data_02082214[idx * 2];
            int v = *(int *)a1;
            *(int *)a1 = v + (int)(((long long)half * sc + 0x800) >> 12);
        }

        a1 = (char *)(int)(a1 + 8);
        {
            int idx = (*(u16 *)(a0 + 0x8e)) >> 4;
            int sc = data_02082214[idx * 2 + 1];
            int v = *(int *)a1;
            *(int *)a1 = v + (int)(((long long)half * sc + 0x800) >> 12);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov080_021274ac, 0x021274ac, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_021274ac
extern "C" int func_ov080_021274ac(char *self, char **arg)
{
    struct Vector3 d;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(arg[0]), 1, -1);
    func_ov080_02127094(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(self + 0x124, _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(arg[1]), self + 0x2ec, 0x1000, *(short *)(self + 0x8e), arg[2]);
    func_020393d4((int *)(self + 0x124), (int)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4((int *)(self + 0x124), (int)&func_ov080_02127658);
    *(int *)(self + 0x320) = *(int *)(self + 0x5c);
    *(int *)(self + 0x324) = *(int *)(self + 0x60);
    *(int *)(self + 0x328) = *(int *)(self + 0x64);
    Vec3_Sub(&d, self + 0x5c, self + 0x320);
    LenVec3(&d);
    *(int *)(self + 0x338) = AngleDiff(Vec3_HorzAngle(self + 0x5c, self + 0x320), *(short *)(self + 0x8e));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov080_0212714c, 0x0212714c, size 0x360 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_0212714c
extern "C" int func_ov080_0212714c(char *c, int *p2)
{
    int tmp[3];
    int len;
    int ang;
    s16 v;
    int horz;

    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(c, (s16)-0x1500, 0, 0, 0) != 0)
        return 1;
    if ((int)((*(int *)(c + 0xb0) & 8) != 0) != 0)
        return 1;

    if (*(u8 *)(c + 0x342) == 0) {
        if (DecIfAbove0_Byte((u8 *)((int)c + 0x33e))) {
            if (*(s16 *)((c + 0x300) + 0x3c) > 0) {
                s16 *p = (s16 *)(int)(c + 0x33c);
                *p = (s16)(*p - 0x10);
            } else {
                s16 *p = (s16 *)(int)(c + 0x33c);
                *p = (s16)(*p + 0x10);
            }
            v = *(s16 *)((c + 0x300) + 0x3c);
            if (v <= 0x10 && v >= (s16)-0x10)
                *(s16 *)((c + 0x300) + 0x3c) = 0;
        } else {
            v = *(s16 *)((c + 0x300) + 0x3c);
            if (v != 0x100) {
                if (v > 0x100) {
                    s16 *p = (s16 *)(int)(c + 0x33c);
                    *p = (s16)(*p - 0x10);
                } else {
                    s16 *p = (s16 *)(int)(c + 0x33c);
                    *p = (s16)(*p + 0x10);
                }
                v = *(s16 *)((c + 0x300) + 0x3c);
                if (v < 0x110 && v > 0xf0)
                    *(s16 *)((c + 0x300) + 0x3c) = 0x100;
            }
        }
        if (Vec3_Dist(c + 0x5c, c + 0x320) < 0x2000)
            *(s16 *)((c + 0x300) + 0x3c) = 0;
    } else {
        horz = Vec3_HorzAngle(c + 0x5c, c + 0x32c);
        AngleDiff(horz, *(s16 *)(c + 0x8e));
        ang = AngleDiff(horz, *(s16 *)(c + 0x8e));
        if (ang > 0x4000) {
            s16 *p = (s16 *)(int)(c + 0x33c);
            *p = (s16)(*p - 0x10);
        } else {
            s16 *p = (s16 *)(int)(c + 0x33c);
            *p = (s16)(*p + 0x10);
        }
        v = *(s16 *)((c + 0x300) + 0x3c);
        {
            int lim = 0x200;
            int nlim = -lim;
            if (v < nlim)
                v = (s16)nlim;
            else if (v > lim)
                v = (s16)lim;
        }
        *(s16 *)((c + 0x300) + 0x3c) = v;
    }

    {
        int d = *(s16 *)((c + 0x300) + 0x3c);
        int t = d + ((unsigned)(d >> 5) >> 26);
        *(int *)(c + 0x98) = (t >> 6) << 12;
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
    Vec3_Sub(tmp, c + 0x5c, c + 0x320);
    len = LenVec3(tmp);
    ang = Vec3_HorzAngle(c + 0x5c, c + 0x320);
    ang = AngleDiff(ang, *(s16 *)(c + 0x8e));
    if (ang >= 0x7f00)
        ang = 0x8000;
    if (ang <= 0x100)
        ang = 0;
    if (len > *p2) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
    }
    if (ang == *(int *)(c + 0x338) || ang == 0x8000) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
        *(u8 *)(c + 0x33e) = 0xa;
    }
    {
        s16 *px = (s16 *)(int)(c + 0x8c);
        *px = (s16)(*px + *(s16 *)((c + 0x300) + 0x3c));
    }
    {
        int mask = 0x1fff;
        if ((*(s16 *)(c + 0x8c) & mask) < 0x210 && *(s16 *)((c + 0x300) + 0x3c) != 0) {
            if (*(u16 *)((c + 0x300) + 0x40) == 0) {
                _ZN5Sound9PlayBank3EjRK7Vector3(0x25, c + 0x74);
                *(u16 *)((c + 0x300) + 0x40) = 0xe;
            }
        } else {
            DecIfAbove0_Short((u16 *)(c + 0x340));
        }
    }
    func_ov080_02127094(c);
    {
        int is66 = (int)(*(u16 *)(c + 0xc) == 0x66);
        if (is66 != 0) {
            if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0) != 0)
                _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
        } else {
            if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
                _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
        }
    }
    *(u8 *)(c + 0x342) = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13daObjMaruta_c6RenderEv, 0x02127124, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjMaruta_c6RenderEv
/* Dispatches through the Model at 0xd4 (dBgActor_c's mModel, inherited) --
   Model::Render(const Vector3 *) with a null scale, matching the ROM's
   literal-0 argument. */
s32 daObjMaruta_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov080_021270dc, 0x021270dc, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_021270dc
extern "C" int func_ov080_021270dc(char *t, void **f)
{
    if (_ZN4dBgW9IsEnabledEv(t + 0x124))
        _ZN4dBgW7DisableEv(t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(f[0]);
    _ZN13SharedFilePtr7ReleaseEv(f[1]);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- func_ov080_02127094, 0x02127094, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_02127094
extern "C" void func_ov080_02127094(char *t)
{
    Matrix4x3_FromRotationXYZExt(t + 0xf0, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN13daObjMaruta_c15OnHitByMegaCharER6Player, 0x02127058 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjMaruta_c15OnHitByMegaCharER6Player
/* Vtable slot 27, and this class's KEY FUNCTION: the destructor is inline in
   the class body, so the first out-of-line virtual declared anchors
   _ZTV13daObjMaruta_c and the RTTI chain, and this TU is where it is defined.
   dBgActor_c::KillByMegaChar is non-virtual, so the unqualified call is the
   direct `bl` the ROM has. */
void daObjMaruta_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    func_02012694(0x1e, &mCamSpacePosX);
    KillByMegaChar(player);
}

/* Below this point the compiler emits, unbidden, _ZN13daObjMaruta_cD1Ev and
   _ZN13daObjMaruta_cD0Ev in that order, plus the vtable and the RTTI chain --
   the price of owning the key function. Only the vtable and RTTI are wanted;
   the two destructor bodies are the cartridge's, in the cartridge's bytes, but
   in the opposite order, so they are deliberately outside the licensed run
   above and stay in the two enrolled sources that already hold them. */
