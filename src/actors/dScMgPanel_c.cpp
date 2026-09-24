//cpp
/* Puzzle Panic. Flip the touched panel and its neighbours until every
 * face matches the deal.
 *
 * Each helper keeps its own extern "C" declarations: decl_common.h
 * disagrees with seven of them. The class methods are not extern "C",
 * so their callees are declared once above the methods.
 *
 * Leftover: func_ov006_021042e8, func_ov006_02104354, func_ov006_0210446c,
 *   func_ov006_02104558, `PanelPart *p = (PanelPart *)(scene + 0x46a8)`
 *   with p->x / p->show / p++, DIFF. The 0x18 byte step stays.
 * Leftover: func_ov006_02104580 and func_ov006_02104870, `PanelScroll *`
 *   at 0x4694, DIFF. func_ov006_021048b0's one `on` store matches.
 * Leftover: func_ov006_021051dc, `(u16 *)(int)&mDelay` then an s16 compare,
 *   DIFF, 25 words. func_ov006_02105854, that same form, DIFF.
 */

#pragma defer_codegen off /* per-function opt pragmas, and .text in source order */

#include "types.h"
#include "dScMgPanel_c.h"
#include "private/ov006_m8c.h"

#include "Sound.h"

/* Background scroll just under the particles. phase 0 shakes, then y runs
   up to 32.0 while sparks ride the edge. func_ov006_021048b0 is the one
   store this spelling matches. */
struct PanelScroll {
    int x;       /* 0x4694 */
    int y;       /* 0x4698 */
    int z;       /* 0x469c */
    int vel;     /* 0x46a0 */
    u8 on;       /* 0x46a4 */
    u8 phase;    /* 0x46a5 */
    u8 timer;    /* 0x46a6 */
    u8 gap;      /* 0x46a7 */
};
typedef char PanelScroll_size_must_be_0x14[sizeof(PanelScroll) == 0x14 ? 1 : -1];

namespace G2   { char *GetBG2ScrPtr(); }
namespace G2S  { char *GetBG2ScrPtr(); char *GetBG3CharPtr(); }
namespace GX   { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }
namespace GXS  { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }

// @symbol _ZN12dScMgPanel_cD1Ev
/* The complete-object destructor (D1).  Nothing this class owns needs
   destroying, so the whole 0x24 bytes are the compiler's own-vtable write and
   the tail call into ~dScMgBase_c -- the same empty body dScMgCurling_c has. */
dScMgPanel_c::~dScMgPanel_c()
{
}

// @symbol _ZN12dScMgPanel_cD0Ev
/* dScMgPanel_c::~dScMgPanel_c (D0, deleting destructor).  There is no second
   definition to write: mwcc emits D0 from the ONE definition above, and
   dScMgBase_c's own operator delete covers the deallocation.  Under
   `#pragma defer_codegen off` the variants come out D1, D0, D2, which is the
   cartridge order; the trailing D2 is homeless and is deadstripped. */

// @symbol func_ov006_021042e8
extern "C" {
/* Draw the live particles: 64 slots of 0x18 bytes, sprite chosen by the
   slot's kind byte and positioned from its Fix12 x/y. */
void func_ov006_021042e8(char *scene)
{
    extern void func_ov004_020afdd0(int a, int b, int scene, int d, int e);
    extern int data_ov006_0213def0[];
    int i;
    for (i = 0; i < 0x40; i++) {
        if (*(u8 *)(scene + 0x4000 + 0x6bd) != 0) {
            func_ov004_020afdd0(
                data_ov006_0213def0[*(u8 *)(scene + 0x4000 + 0x6ba)],
                *(int *)(scene + 0x4000 + 0x6a8) >> 12,
                *(int *)(scene + 0x4000 + 0x6ac) >> 12,
                -1, -1);
        }
        scene += 0x18;
    }
}
}

// @symbol func_ov006_02104354
/* Advance the 64 particles one frame: gravity on the free ones, a spin/fade
   counter on all of them, and retire a slot once its counter runs out. */
#pragma push
#pragma opt_common_subs off
extern "C" {
void func_ov006_02104354(char *scene)
{
    int i;
    for (i = 0; i < 0x40; i++, scene += 0x18) {
        if (*(u8 *)(scene + 0x46b8) != 0) {
            if (*(u8 *)(scene + 0x46b9) == 0) {
                *(int *)(scene + 0x46a8) += *(int *)(scene + 0x46b0);
                *(int *)(scene + 0x46ac) += *(int *)(scene + 0x46b4);
                *(int *)(scene + 0x46b4) -= 0x300;
                if (*(int *)(scene + 0x46b0) > 0)
                    *(int *)(scene + 0x46b0) += 0xc00;
                else
                    *(int *)(scene + 0x46b0) -= 0xc00;
            }
            *(u8 *)(scene + 0x46bc) += 1;
            if (*(u8 *)(scene + 0x46bc) >= 4) {
                *(u8 *)(scene + 0x46bb) += 1;
                if (*(u8 *)(scene + 0x46bb) >= 5) {
                    *(u8 *)(scene + 0x46b8) = 0;
                    *(u8 *)(scene + 0x46bd) = 0;
                }
            }
        }
    }
}
}
#pragma pop

// @symbol func_ov006_0210446c
/* Spawn one particle in the first free slot, at (x, y).  `mode` picks the
   fixed-drift variant over the random one. */
#pragma push
#pragma opt_common_subs off
extern "C" {
void func_ov006_0210446c(char *scene, int x, int y, int mode)
{
    extern int RandomIntInternal(int *seed);
    extern int data_0209d4b8;
    int i;
    char *q = scene;
    for (i = 0; i < 0x40; i++, q += 0x18) {
        if (*(u8 *)(q + 0x46b8) == 0) {
            int off = i * 0x18;
            int *w;
            *(u8 *)(scene + off + 0x46b8) = 1;
            *(u8 *)(scene + off + 0x46bd) = 1;
            *(u8 *)(scene + off + 0x46bc) = 0;
            *(u8 *)(scene + off + 0x46ba) = 0;
            *(u8 *)(scene + off + 0x46bb) = 0;
            *(u8 *)(scene + off + 0x46b9) = mode;
            /* The `(int)` launder keeps mwcc from common-subexpressioning this
               field address with the one taken three lines down; without it 16
               words of this function move. */
            *(int *)((char *)(((int)scene + 0x46b0)) + off) = 0;
            *(int *)(scene + off + 0x46b4) = 0;
            *(int *)(scene + off + 0x46a8) = x;
            *(int *)(scene + off + 0x46ac) = y;
            w = (int *)((char *)(((int)scene + 0x46b0)) + off);
            if (mode != 0) {
                *(u8 *)(scene + off + 0x46be) = 0;
            } else {
                int rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
                rnd = (int)((((u32)rnd) << 1) >> 15);
                *(u8 *)(scene + off + 0x46be) = rnd;
            }
            if (x >= 0x80000)
                *w = -0x100;
            else
                *w = 0x100;
            return;
        }
    }
}
}
#pragma pop

// @symbol func_ov006_02104558
extern "C" {
/* Retire every particle in the 64-slot pool. */
void func_ov006_02104558(char *scene)
{
    int i;
    for (i = 0; i < 0x40; i++) {
        *(u8 *)(scene + 0x46b8) = 0;
        *(u8 *)(scene + 0x46bd) = 0;
        scene += 0x18;
    }
}
}

// @symbol func_ov006_02104580
#pragma push
#pragma opt_common_subs off
extern "C" {
void func_ov006_02104580(char *scene)
{
    extern int RandomIntInternal(int *seed);
    extern int data_0209d4b8;
    extern void func_ov006_0210446c(char *scene, int x, int y, int mode);
    extern void SetBg2Offset(int x, int y);
    extern void SetSubBg2Offset(int x, int y);
    int x;
    int y;
    u32 r;
    u32 r2;

    if (*(u8 *)(scene + 0x46a5) == 0) {
        *(u8 *)(scene + 0x46a6) += 1;
        if (((*(u8 *)(scene + 0x46a6) >> 1) & 1) != 0)
            *(int *)(scene + 0x4694) = 0x2000;
        else
            *(int *)(scene + 0x4694) = -0x2000;

        if (*(u8 *)(scene + 0x46a7) != 0) {
            *(u8 *)(scene + 0x46a7) -= 1;
        } else {
            r = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            x = ((r * 7) >> 15) * 5;
            r = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            y = ((r << 4) >> 15) + 0xb0;
            func_ov006_0210446c(scene, x << 12, y << 12, 0);

            r = (u32)RandomIntInternal(&data_0209d4b8);
            r2 = (u32)RandomIntInternal(&data_0209d4b8);
            x = (((r >> 16 & 0x7fff) * 7) >> 15) * 5;
            y = ((((r2 >> 16) & 0x7fff) << 4) >> 15) + 0xb0;
            func_ov006_0210446c(scene, (0x100 - x) << 12, y << 12, 0);
            *(u8 *)(scene + 0x46a7) = 3;
        }

        if (*(u8 *)(scene + 0x46a6) >= 0x3c) {
            *(u8 *)(scene + 0x46a5) += 1;
            *(u8 *)(scene + 0x46a6) = 0;
            *(u8 *)(scene + 0x46a7) = 0;
            *(int *)(scene + 0x4694) = 0;
            *(int *)(scene + 0x46a0) = 0x1000;
        }
    } else if (*(int *)(scene + 0x4698) < 0x200000) {
        *(int *)(scene + 0x4698) += *(int *)(scene + 0x46a0);
        *(int *)(scene + 0x46a0) += 0x200;
        if (*(int *)(scene + 0x4698) >= 0x200000)
            *(int *)(scene + 0x4698) = 0x200000;

        if (*(u8 *)(scene + 0x46a7) != 0) {
            *(u8 *)(scene + 0x46a7) -= 1;
        } else {
            y = *(int *)(scene + 0x4698) >> 12;
            if (y >= 0 && y <= 0xc0)
                y = (0xc0 - y) << 12;
            else if (y >= 0x100)
                y = (-0x20 - (y - 0x100)) << 12;

            x = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 7) >> 15) * 5;
            func_ov006_0210446c(scene, x << 12, y, 1);
            x = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 7) >> 15) * 5;
            func_ov006_0210446c(scene, (0x100 - x) << 12, y, 1);
            *(u8 *)(scene + 0x46a7) = 2;
        }
    }

    x = *(int *)(scene + 0x4694) >> 12;
    y = *(int *)(scene + 0x4698) >> 12;
    SetBg2Offset(x, y);
    if (y >= 0x100)
        y = 0x100;
    SetSubBg2Offset(x, y);
}
}
#pragma pop

// @symbol func_ov006_02104870
extern "C" {
void func_ov006_02104870(char *scene)
{
    *(u8 *)(scene + 0x46a4) = 1;
    *(int *)(scene + 0x4694) = 0;
    *(int *)(scene + 0x4698) = 0;
    *(int *)(scene + 0x469c) = 0;
    *(int *)(scene + 0x46a0) = 0;
    *(u8 *)(scene + 0x46a6) = 0;
    *(u8 *)(scene + 0x46a7) = 0;
    *(u8 *)(scene + 0x46a5) = 0;
    Sound::PlayBank2_2D(0x1fd);
}
}

// @symbol func_ov006_021048b0
extern "C" {
void func_ov006_021048b0(char *scene)
{
    extern void SetBg2Offset(int a, int b);
    extern void SetSubBg2Offset(int a, int b);
    ((PanelScroll *)(scene + 0x4694))->on = 0;
    SetBg2Offset(0, 0);
    SetSubBg2Offset(0, 0);
}
}

// @symbol func_ov006_021048e4
extern "C" {
void func_ov006_021048e4(u8 *scene)
{
    extern u8 data_0209d454[];
    if (scene[0x4693] != 0) return;
    scene[0x4693] = 1;
    *(u16 *)(scene + 0x4600 + 0x90) = 0;
    data_0209d454[0] |= 1;
}
}

// @symbol func_ov006_02104920
extern "C" {
void func_ov006_02104920(char *scene, int index)
{
    extern void SetSubBg0Offset(int a, int b);
    extern void func_02012790(int a);
    extern u8 data_0209d454;
    extern int data_ov006_0212ed00[];
    int n = index * 0xc;
    char *base = scene + 0x4690;
    int cnt;
    *(u16 *)(base + n) = *(u16 *)(base + n) + 1;
    cnt = *(u16 *)(base + n);
    if ((u32)cnt >= 0x20) {
        *(u16 *)(base + n) = 0;
        *(u8 *)(scene + n + 0x4000 + 0x693) = 0;
        SetSubBg0Offset(0, 0);
        data_0209d454 &= ~1;
        func_02012790(0x12f);
        *(u8 *)(scene + 0x4fe2) -= 1;
        return;
    }
    *(int *)(scene + 0x468c + n) = data_ov006_0212ed00[cnt >> 3];
    {
        char *q = scene + n + 0x4600;
        u16 t = *(u16 *)(q + 0x90);
        if (t == 1 || t == 0x11)
            Sound::PlayBank2_2D(0x1fc);
    }
    SetSubBg0Offset(0, *(int *)(scene + 0x468c + n));
}
}

// @symbol func_ov006_02104a10
extern "C" {
void SetSubBg0Offset(int a, int b);

void func_ov006_02104a10(char *scene, int index)
{
    extern int data_ov006_0212ecec[];
    int m = index * 0xc;
    char *b = scene + 0x4690;
    u16 h = *(u16 *)(b + m);
    h = h + 1;
    *(u16 *)(b + m) = h;
    h = *(u16 *)(b + m);
    if (h >= 0x20) {
        *(s16 *)(b + m) = 0;
        *(u8 *)(scene + m + 0x4000 + 0x693) = 2;
        return;
    }
    {
        int t = data_ov006_0212ecec[h >> 3];
        int *dst = (int *)(scene + 0x468c + m);
        u16 st;
        *dst = t;
        st = *(u16 *)(scene + m + 0x4600 + 0x90);
        if (st == 1 || st == 0x11) Sound::PlayBank2_2D(0x1fc);
        SetSubBg0Offset(0, *dst);
    }
}
}

// @symbol func_ov006_02104ac0
extern "C" {
void func_ov006_02104ac0(void)
{
}
}

// @symbol func_ov006_02104ac4
extern "C" {
struct PanelC_ac4;
typedef void (PanelC_ac4::*PanelPmf_ac4)(int);
struct PanelEntry_ac4 { PanelPmf_ac4 pmf[1]; };
struct PanelC_ac4 {
    char pad[0x4692];
    u8 guard;
    u8 idx;
};
void func_ov006_02104ac4(PanelC_ac4 *c)
{
    extern PanelEntry_ac4 data_ov006_021427bc[];
    if (c->guard == 0) return;
    int j = c->idx;
    (c->*data_ov006_021427bc[j].pmf[0])(0);
}
}

// @symbol func_ov006_02104b24
extern "C" {
void func_ov006_02104b24(char *scene)
{
    *(char *)(scene + 0x4692) = 1;
    *(int *)(scene + 0x4688) = 0;
    *(int *)(scene + 0x468c) = 0;
    *(s16 *)(scene + 0x4690) = 0;
    *(char *)(scene + 0x4693) = 0;
}
}

// @symbol func_ov006_02104b4c
extern "C" {
void func_ov006_02104b4c(char *scene)
{
    *(char *)(scene + 0x4692) = 0;
}
}

// @symbol func_ov006_02104b5c
extern "C" {
void func_ov006_02104b5c(char *scene)
{
    void func_ov004_020af948(void *a, int b, int scene, int d);
    extern void *data_ov006_02136e2c[];
    if (*(u8 *)(scene + 0x4685) == 0) return;
    func_ov004_020af948(data_ov006_02136e2c[0],
                        *(int *)(scene + 0x4678) >> 12,
                        *(int *)(scene + 0x467c) >> 12, 0);
}
}

// @symbol func_ov006_02104bac
extern "C" {
void func_ov006_02104bac(void)
{
}
}

// @symbol func_ov006_02104bb0
extern "C" {
void func_ov006_02104bb0(char *scene)
{
    u16 *p = (u16 *)(scene + 0x4682);
    *p = *p + 1;
    if (*(u16 *)(scene + 0x4600 + 0x82) < 8) return;
    *(u16 *)(scene + 0x4600 + 0x82) = 0;
    {
        int *q = (int *)(scene + 0x467c);
        *q = *q + 0x4000;
    }
    *(u8 *)(scene + 0x4686) = 2;
}
}

// @symbol func_ov006_02104c08
extern "C" {
void func_ov006_02104c08(char *scene)
{
    u16 *h = (u16 *)(scene + 0x4682);
    *h = *h + 1;
    if (*(u16 *)(scene + 0x4600 + 0x82) < 4) return;
    *(u16 *)(scene + 0x4600 + 0x82) = 0;
    {
        int *w = (int *)(scene + 0x467c);
        *w = *w - 0x4000;
    }
    *(u8 *)(scene + 0x4000 + 0x686) = 1;
}
}

// @symbol func_ov006_02104c60
extern "C" {
struct PanelC_c60;
typedef void (PanelC_c60::*PanelPmf_c60)();
void func_ov006_02104c60(char *scene)
{
    void func_ov006_02104c60(char *scene);
    extern PanelPmf_c60 data_ov006_021427ec[];
    u16 *ip;
    if (*(u8 *)(scene + 0x4000 + 0x684) == 0) return;
    ip = (u16 *)(scene + 0x4680);
    *ip = *ip - 1;
    if (*(s16 *)(scene + 0x4600 + 0x80) <= 0) {
        *(s16 *)(scene + 0x4600 + 0x80) = 0;
        *(u8 *)(scene + 0x4000 + 0x684) = 0;
        *(u8 *)(scene + 0x4000 + 0x685) = 0;
        return;
    }
    {
        PanelC_c60 *cc = (PanelC_c60 *)scene;
        (cc->*data_ov006_021427ec[*(u8 *)(scene + 0x4000 + 0x686)])();
    }
}
}

// @symbol func_ov006_02104cfc
extern "C" {
void func_ov006_02104cfc(char *scene, int i)
{
    char *e = scene + (i << 2);
    *(u8 *)(scene + 0x4684) = 1;
    *(u8 *)(scene + 0x4685) = 1;
    *(int *)(scene + 0x4678) = *(int *)(e + 0x4cc4);
    *(int *)(scene + 0x467c) = *(int *)(e + 0x4d54) + 0x8000;
    *(s16 *)(scene + 0x4680) = 0x30;
    *(s16 *)(scene + 0x4682) = 0;
    *(u8 *)(scene + 0x4686) = 0;
}
}

// @symbol func_ov006_02104d44
extern "C" {
void func_ov006_02104d44(char *scene)
{
    void Hud_RenderSprite(void *a, int b, int scene, int d, int e);
    extern void *data_ov006_02136d40[];
    if (*(u8 *)(scene + 0x4fe3) == 0) return;
    Hud_RenderSprite(data_ov006_02136d40[*(u8 *)(scene + 0x4fe2)],
                        0xf0, 0x24, -1, -1);
}
}

// @symbol func_ov006_02104d94
extern "C" {
void func_ov006_02104d94(char *scene)
{
    extern void func_ov004_020b2444(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
    extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
    extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
    extern void *data_ov006_02135054[];
    extern void *data_ov006_02136de8[];
    if (*(u8 *)(scene + 0x4000 + 0x676) == 0) return;
    if (*(u8 *)(scene + 0x4000 + 0x675) != 4) {
        int a = *(int *)(scene + 0x4000 + 0x660) >> 12;
        int b = *(int *)(scene + 0x4000 + 0x664) >> 12;
        int index;
        func_ov004_020b2444(a, b, *(u8 *)(scene + 0x4000 + 0xfde), -1, -1, 0, 0);
        index = 1;
        if (*(u8 *)(scene + 0x4000 + 0xfde) >= 2) index = 0;
        func_ov004_020afdd0(data_ov006_02135054[index], a + 0x2c, b, -1, -1);
    } else {
        int x = *(u8 *)(scene + 0x4000 + 0x677);
        Hud_RenderSprite(data_ov006_02136de8[x],
                            *(int *)(scene + 0x4000 + 0x660) >> 12,
                            *(int *)(scene + 0x4000 + 0x664) >> 12,
                            -1, -1);
    }
}
}

// @symbol func_ov006_02104e70
extern "C" {
void func_ov006_02104e70(u8 *scene)
{
    scene += 0x4000;
    scene[0x677] = scene[0xfde];
}
}

// @symbol func_ov006_02104e80
extern "C" {
// If the byte flag at self+0x4677 is set, decrement it. The read folds to add+ldrb
// (offset > 0xfff), while the laundered RMW pool-loads the offset, matching the ROM's
// predicated ldrbne/subne/strbne tail.
void func_ov006_02104e80(char *scene)
{
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    if (s->mSlideLeft)
        s->mSlideLeft -= 1;
}
}

// @symbol func_ov006_02104ea8
extern "C" {
void func_ov006_02104ea8(char *scene)
{
    ((dScMgPanel_c *)scene)->mSlideFlag = 0;
}
}

// @symbol func_ov006_02104eb8
extern "C" {
void func_ov006_02104eb8(char *scene)
{
    ((dScMgPanel_c *)scene)->mSlideFlag = 1;
}
}

// @symbol func_ov006_02104ec8
extern "C" {
void func_ov006_02104ec8(void)
{
}
}

// @symbol func_ov006_02104ecc
extern "C" {
void func_ov006_02104ecc(char *scene)
{
    extern void FreeGfxSlotsById(int arg);
    extern void func_ov006_02106048(char *scene);
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlide += s->mSlideVel;
    s->mSlideVel -= 0x400;
    if ((s->mSlide >> 12) > -0x40) return;
    s->mIntro++;
    s->mSlideStep = 4;
    s->mSlide = 0x10000;
    s->mSlideY = 0x24000;
    FreeGfxSlotsById(0x1d);
    if (s->mPromptBlinkCount == 0) {
        s->mPromptEnabled = 1;
        s->mPromptBlinkCount = 1;
        s->mPromptBlinkTimer = 0;
    }
    *(u8 *)(scene + 0x4fe3) = 1;
    s->mSlideLeft = s->mFlipCount;
    s->mHold = 0;
    func_ov006_02106048(scene);
    Sound::PlayBank2_2D(0x1fb);
}
}

// @symbol func_ov006_02104fb4
extern "C" {
void func_ov006_02104fb4(u8 *scene)
{
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    if (s->mHold != 0) {
        u16 *p = &s->mHold;
        *p = *p - 1;
        if (*(s16 *)&s->mHold < 0) s->mHold = 0;
    } else {
        s->mSlideVel = -0x3000;
        s->mSlideStep = 3;
    }
}
}

// @symbol func_ov006_0210500c
extern "C" {
void func_ov006_0210500c(char *scene)
{
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlide += s->mSlideVel;
    s->mSlideVel -= 0x400;
    if (s->mSlide >> 12 > 0x58) return;
    s->mSlide = 0x58000;
    s->mSlideStep = 2;
    s->mHold = 0x80;
}
}

// @symbol func_ov006_0210508c
extern "C" {
void func_ov006_0210508c(char *scene)
{
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlide = 0x110000;
    s->mSlideY = 0x60000;
    s->mSlideVel = -0x3000;
    s->mSlideFlag = 1;
    s->mSlideStep = 1;
}
}

// @symbol func_ov006_021050bc
extern "C" {
struct PanelC_0bc;
typedef void (PanelC_0bc::*PanelPmf_0bc)();
struct PanelEntry_0bc { PanelPmf_0bc pmf; };
struct PanelC_0bc { char pad[0x4674]; u8 guard; u8 idx; };
void func_ov006_021050bc(PanelC_0bc *c)
{
    extern PanelEntry_0bc data_ov006_02142860[];
    if (!c->guard) return;
    (c->*(data_ov006_02142860[c->idx].pmf))();
}
}

// @symbol func_ov006_02105118
extern "C" {
void func_ov006_02105118(char *scene)
{
    *(char *)(scene + 0x4674) = 1;
    *(char *)(scene + 0x4675) = 0;
    *(char *)(scene + 0x4677) = 0;
}
}

// @symbol func_ov006_02105134
extern "C" {
void func_ov006_02105134(char *scene)
{
    void func_02012790(int a);
    void func_ov006_021048e4(u8 *r0);
    void func_ov006_02105c1c(char *scene);
    if (*(u8 *)(scene + 0x4fe9) != 0) return;
    if (*(int *)(scene + 0x4ca8) == 6) return;
    if (*(u8 *)(scene + 0x4fe1) < *(u8 *)(scene + 0x4fde)) return;

    func_02012790(0xe);
    func_ov006_021048e4((u8 *)scene);

    if (*(u8 *)(scene + 0x4fe2) > 1) {
        *(int *)(scene + 0x4ca8) = 3;
        func_ov006_02105c1c(scene);
    } else {
        *(u8 *)(scene + 0x4fe6) = 0;
        func_ov006_02105c1c(scene);
        *(int *)(scene + 0x4ca8) = 6;
        *(s16 *)(scene + 0x4ec0) = 0x50;
    }
}
}

// @symbol func_ov006_021051dc
extern "C" {
void func_ov006_021051dc(char *scene)
{
    extern void func_ov006_02104cfc(char *scene, int i);
    int index;
    int w;
    int col;
    int row;
    int hq;
    int wq;
    int y;
    int x;

    if (*(u16 *)(scene + 0x4ec4) != 0) {
        (*(u16 *)(int)(scene + 0x4ec4))--;
        if (*(s16 *)(scene + 0x4ec4) < 0)
            *(u16 *)(scene + 0x4ec4) = 0;
        return;
    }

    if (*(u8 *)(scene + 0x4fe5) >= *(u8 *)(scene + 0x4fde)) {
        *(u16 *)(scene + 0x4ec0) = 0x50;
        *(int *)(scene + 0x4ca8) = 6;
        *(u8 *)(scene + 0x4fe6) = 0;
        return;
    }

    index = *(u8 *)(scene + *(u8 *)(scene + 0x4fe5) + 0x4fae);
    w = *(int *)(scene + 0x4cbc);
    hq = 3;
    wq = hq;

    col = index % w;
    row = index / w;

    if (col == 0 || col == w - 1)
        wq = 2;
    if (row == 0 || row == w - 1)
        hq = 2;

    if (--col < 0)
        col = 0;
    if (--row < 0)
        row = 0;

    for (y = 0; y < hq; y++) {
        for (x = 0; x < wq; x++) {
            int cell = *(int *)(scene + 0x4cbc) * (row + y) + (col + x);

            *(u8 *)(scene + cell + 0x4efa) = 1;
            (*(u8 *)(int)(scene + cell + 0x4f1e)) ^= 1;
            *(u16 *)(scene + cell * 2 + 0x4e78) = 8;
        }
    }

    func_ov006_02104cfc(scene, index);
    *(u16 *)(scene + index * 2 + 0x4e78) = 0;
    *(u16 *)(scene + 0x4ec4) = 0x30;
    (*(u8 *)(int)(scene + 0x4fe5))++;
}
}

// @symbol func_ov006_021053a8
extern "C" {
void func_ov006_021053a8(Ov006M8c *scene)
{
    extern u8 *data_ov006_0213e070[];
    extern u8 *data_ov006_0213ded0[];
    char *c = (char *)scene;
    int n;
    int found;
    int i;
    int j;
    int li;
    int nlights;
    int arr[36];
    int idx;
    int wq;
    int hq;
    int col;
    int row;
    int y;
    int x;
    int cell;
    u8 *p;

    if (((u16 *)(c + 0x4e00))[0x62] != 0) {
        (*(u16 *)(c + 0x4ec4))--;
        if (((s16 *)(c + 0x4e00))[0x62] < 0) {
            ((s16 *)(c + 0x4e00))[0x62] = 0;
        }
        return;
    }

    n = *(int *)(c + 0x4cb8);
    found = 0;
    i = found;
    if (n > 0) {
        do {
            if (*(u8 *)(c + i + 0x4efa) != 0) {
                found++;
                break;
            }
            i++;
        } while (i < n);
    }
    if (found) {
        return;
    }

    i = 0;
    do {
        arr[i] = 0;
        i++;
    } while (i < 0x24);
    j = 0;

    if (n > 0) {
        do {
            idx = *(int *)(c + 0x4cb4);
            if (scene->m8c() != 0) {
                arr[j] = data_ov006_0213ded0[idx][j];
            } else {
                arr[j] = data_ov006_0213e070[idx][j];
            }
            n = *(int *)(c + 0x4cb8);
            j++;
        } while (j < n);
    }

    nlights = *(u8 *)(c + 0x4fde);
    li = 0;
    if (nlights > 0) {
        do {
            idx = *(u8 *)(c + li + 0x4fae);
            wq = 3;
            hq = 3;
            col = idx % scene->width;
            row = idx / scene->width;
            if (col == 0 || col == scene->width - 1) {
                wq = 2;
            }
            if (row == 0 || row == scene->width - 1) {
                hq = 2;
            }
            if (--col < 0) {
                col = 0;
            }
            if (--row < 0) {
                row = 0;
            }
            y = 0;
            if (hq > 0) {
                do {
                    x = 0;
                    if (wq > 0) {
                        do {
                            cell = scene->width * (row + y) + (col + x);
                            arr[cell] ^= 1;
                            x++;
                        } while (x < wq);
                    }
                    y++;
                } while (y < hq);
            }
            li++;
        } while (li < nlights);
    }

    j = 0;
    if (n > 0) {
        do {
            p = (u8 *)(c + j + 0x4f1e);
            if (*p != arr[j]) {
                *(u8 *)(c + j + 0x4efa) = 1;
                *p ^= 1;
                *(u8 *)(c + j + 0x4f8a) = 1;
            }
            j++;
        } while (j < *(int *)(c + 0x4cb8));
    }

    ((u16 *)(c + 0x4e00))[0x62] = 0x40;
    *(u8 *)(c + 0x4fe0) = 3;
    *(u8 *)(c + 0x4fe5) = 0;
}
}

// @symbol func_ov006_02105670
extern "C" {
void func_ov006_02105670(char *scene)
{
    if (((u16 *)(scene + 0x4e00))[0x62] != 0) {
        (*(u16 *)(scene + 0x4ec4))--;
        if (((s16 *)(scene + 0x4e00))[0x62] < 0) {
            ((s16 *)(scene + 0x4e00))[0x62] = 0;
        }
        return;
    } else {
        int k;
        for (k = 0; k < *(int *)(scene + 0x4cb8); k++) {
            *(u8 *)(scene + k + 0x4efa) = 1;
            *(u8 *)(scene + k + 0x4f1e) ^= 1;
            *(u8 *)(scene + k + 0x4f8a) = 1;
        }
        *(u8 *)(scene + 0x4fe0) = 2;
        ((u16 *)(scene + 0x4e00))[0x62] = 0x20;
    }
}
}

// @symbol func_ov006_02105730
extern "C" {
typedef struct WarpEntry_730 {
    u8 a;
    u8 b;
    u8 c;
    u8 d;
} WarpEntry_730;

void func_ov006_02105730(char *c)
{
    extern u8 data_020a0e40;
    extern WarpEntry_730 data_020a0de8[];
    u8 idx;
    s32 b;
    s32 dx;
    s32 dy;

    if (*(u8 *)(c + 0x4fe9) != 0) {
        return;
    }
    idx = data_020a0e40;
    b = 0;
    if (data_020a0de8[idx].a != 0) {
        b = (data_020a0de8[idx].b != 0);
    }
    if (b == 0) {
        return;
    }
    dx = data_020a0de8[idx].c - 0xe8;
    dy = data_020a0de8[idx].d - 0x10;
    if (dx < -15) {
        return;
    }
    if (dx > 15) {
        return;
    }
    if (dy < -12) {
        return;
    }
    if (dy > 12) {
        return;
    }
    *(u8 *)(c + 0x4fe0) = 1;
    *(u32 *)(c + 0x4ca8) = 5;
    *(u16 *)(c + 0x4ec4) = 0x20;
}
}

// @symbol func_ov006_021057f0
extern "C" {
struct PanelC_7f0;
typedef void (PanelC_7f0::*PanelPmf_7f0)();
struct PanelB_7f0 {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34();
    virtual int m();
};
struct PanelC_7f0 : PanelB_7f0 { char pad[0x4fdc]; u8 idx; };
void func_ov006_021057f0(PanelC_7f0 *c)
{
    extern PanelPmf_7f0 data_ov006_02142820[];
    extern void func_ov006_02104c60(PanelC_7f0 *);
    if (c->m() != 0) return;
    (c->*data_ov006_02142820[c->idx])();
    func_ov006_02104c60(c);
}
}

// @symbol func_ov006_02105854
extern "C" {
void func_ov006_02105854(char *scene)
{
    extern void func_ov006_02104eb8(char *p);
    extern void func_ov006_02104e70(u8 *p);
    if (*(u16 *)(scene + 0x4ec4) != 0) {
        (*(u16 *)(int)(scene + 0x4ec4))--;
        if (*(s16 *)(scene + 0x4ec4) < 0)
            *(u16 *)(scene + 0x4ec4) = 0;
        return;
    }

    {
        int found = 0;
        int i;
        int n = *(int *)(scene + 0x4cb8);
        for (i = 0; i < n; i++) {
            if (*(u8 *)(scene + i + 0x4efa) != 0) {
                found++;
                break;
            }
        }
        if (found)
            return;
    }

    if (*(u8 *)(scene + 0x4fe4) == 0xff) {
        *(u8 *)(scene + 0x4fe4) = 0;
        *(int *)(scene + 0x4ca8) = 2;
        func_ov006_02104eb8(scene);
        func_ov006_02104e70((u8 *)scene);
        return;
    }

    (*(u8 *)(int)(scene + 0x4fe4))--;

    if (*(u8 *)(scene + 0x4fe4) == 0xff) {
        *(u8 *)(scene + 0x4fe4) = 0;
        *(int *)(scene + 0x4ca8) = 2;
        *(u8 *)(scene + 0x4fe1) = 0;
        func_ov006_02104eb8(scene);
        func_ov006_02104e70((u8 *)scene);
        return;
    }

    {
        u8 slot = *(u8 *)(scene + 0x4fe4);
        int hq = 3;
        int index = *(u8 *)(scene + slot + 0x4fce);
        int w = *(int *)(scene + 0x4cbc);
        int wq = 3;
        int col = index % w;
        int row = index / w;
        int x, y;

        if (col == 0 || col == w - 1)
            wq = 2;
        if (row == 0 || row == w - 1)
            hq = 2;
        if (--col < 0)
            col = 0;
        if (--row < 0)
            row = 0;

        for (y = 0; y < hq; y++) {
            for (x = 0; x < wq; x++) {
                int cell = *(int *)(scene + 0x4cbc) * (row + y) + (col + x);
                *(u8 *)(scene + cell + 0x4efa) = 1;
                (*(u8 *)(int)(scene + cell + 0x4f1e)) ^= 1;
                *(u16 *)(scene + cell * 2 + 0x4e78) = 8;
                *(u8 *)(scene + cell + 0x4f8a) = 1;
            }
        }
        *(u16 *)(scene + index * 2 + 0x4e78) = 0;
        Sound::PlayBank2_2D(0x1fb);
    }
}
}

// @symbol func_ov006_02105ab4
extern "C" {
typedef struct PanelObj_ab4
{
    char _p0[0x4ca8];
    int mode;                                  /* 0x4ca8 */
    char _p1[0x4cb8 - 0x4ca8 - 4];
    int count;                                 /* 0x4cb8 */
    char _p2[0x4cc4 - 0x4cb8 - 4];
    int xarr[(0x4d54 - 0x4cc4) / 4];           /* 0x4cc4 */
    int yarr[(0x4de8 - 0x4d54) / 4];           /* 0x4d54 */
    u16 harr[(0x4efa - 0x4de8) / 2];/* 0x4de8 */
    u8 tarr[0x4f1e - 0x4efa];       /* 0x4efa */
    u8 karr[0x4f42 - 0x4f1e];       /* 0x4f1e */
    u8 parr[0x4f66 - 0x4f42];       /* 0x4f42 */
    u8 flagarr[0x4fe8 - 0x4f66];    /* 0x4f66 */
    u8 q;                           /* 0x4fe8 */
} PanelObj_ab4;

void func_ov006_02105ab4(PanelObj_ab4 *scene)
{
    extern void func_ov004_020af868(void *a0, int a1, int a2, int a3, int a4, void *a5);
    extern void DrawOamSprite(void *a0, int a1, int a2, int a3);
    extern void *data_ov006_021427d4[];
    extern u8 data_ov006_0213dd34[];
    int x;
    int y;
    int v;
    int t;
    int k;
    int index;
    int i;

    for (i = 0; i < scene->count; i++) {
        if (scene->flagarr[i] != 0) {
            v = 0;

            if (scene->mode == 6) {
                v = 1;
            }

            t = scene->tarr[i];
            k = scene->karr[i];
            x = scene->xarr[i] >> 12;
            y = scene->yarr[i] >> 12;

            index = k * 3;
            if (t == 1) {
                int h = scene->harr[i];
                index = data_ov006_0213dd34[k * 5 + h];
            }

            func_ov004_020af868(data_ov006_021427d4[index], x, y, -1, v, 0);

            if (scene->mode == 1) {
                int p = scene->parr[i];
                int q = scene->q;
                DrawOamSprite(data_ov006_021427d4[p * 3 + q], x, y, 0);
            } else {
                int p = scene->parr[i];
                DrawOamSprite(data_ov006_021427d4[p * 3], x, y, 0);
            }
        }
    }
}
}

// @symbol func_ov006_02105c1c
extern "C" {
void func_ov006_02105c1c(char *scene)
{
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int i = 0;

    if (s->mCount <= 0) return;
    do {
        if (s->mFace[i] != s->mShown[i]) {
            s->mKind[i] = 2;
            *(u16 *)(scene + (int)(((long long)i)) * 2 + 0x4e30) = 0;
        }
        i++;
    } while (i < s->mCount);
}
}

// @symbol func_ov006_02105c88
extern "C" {
void func_ov006_02105c88(char *scene)
{
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int cnt;
    int i;
    cnt = 0;
    if (s->mBusy != 0) return;

    for (i = 0; i < s->mCount; i++) {
        if (s->mFace[i] != s->mShown[i]) {
            cnt++;
            break;
        }
    }
    if (cnt != 0) return;

    s->mState = 6;
    *(u8 *)(scene + 0x4fe6) = 1;
    *(s16 *)(scene + 0x4ec0) = 0x40;
}
}

// @symbol func_ov006_02105d20
extern "C" {
void func_ov006_02105d20(char *scene)
{
    int found;
    int i;
    int n;
    int tmp;
    if (*(u8 *)(scene + 0x4fe9) == 0) return;
    if (*(u8 *)(scene + 0x4feb) == 0) return;
    /* Laundered volatile round trip: the `(int)` cast and the volatile are both
       load-bearing -- they are what turns this decrement into the ROM's
       ldrb/sub/strb instead of a folded one. */
    tmp = *(volatile u8 *)(int *)(((int)scene + 0x4feb));
    --tmp;
    *(volatile u8 *)(int *)(((int)scene + 0x4feb)) = tmp;
    if (*(u8 *)(scene + 0x4feb) != 0) return;
    found = 0;
    i = 0;
    n = *(int *)(scene + 0x4cb8);
    while (i < n) {
        if (*(u8 *)(scene + i + 0x4f1e) != *(u8 *)(scene + i + 0x4f42)) {
            found++;
            break;
        }
        i++;
    }
    if (found != 0) return;
    Sound::PlayBank2_2D(0x1cd);
}
}

// @symbol func_ov006_02105de4
extern "C" {
/* Find the cursor slot whose fixed-point position is
 * within 16px of the level's target cell, mark it captured, flip a 2x2 or 3x3
 * quad of grid cells around it and start their flash timers.
 */
typedef struct PanelObj_de4 {
    char _p0[0x4cb8];
    int count;                       /* 0x4cb8 */
    int w;                           /* 0x4cbc */
    char _p1[4];
    int xarr[(0x4d54 - 0x4cc4) / 4]; /* 0x4cc4 */
    int yarr[(0x4de8 - 0x4d54) / 4]; /* 0x4d54 */
} PanelObj_de4;

void func_ov006_02105de4(char *scene)
{
    extern u8 data_020a0e40;
    extern u8 data_020a0de8[];
    extern u8 data_020a0de9[];
    extern void func_ov006_02104e80(char *);
    u8 lvl;
    int n;
    int i;
    int tx, ty;

    if (*(u8 *)(scene + 0x4fe9) != 0)
        return;

    lvl = data_020a0e40;
    n = 0;
    if (data_020a0de8[lvl * 4] != 0)
        n = data_020a0de9[lvl * 4] != 0;
    if (n == 0)
        return;

    i = 0;
    n = ((PanelObj_de4 *)scene)->count;
    if (n <= 0)
        return;

    {
        u8 *e = &data_020a0de8[lvl * 4];
        tx = e[2];
        ty = e[3];
    }

    do {
        int dx = tx - (((PanelObj_de4 *)scene)->xarr[i] >> 12);
        int dy = ty - (((PanelObj_de4 *)scene)->yarr[i] >> 12);
        if (dx >= -0x10 && dx <= 0x10 && dy >= -0x10 && dy <= 0x10) {
            int wq = 3;
            int hq = 3;
            int w, col, row;

            *(u8 *)(scene + *(u8 *)(scene + 0x4fe4) + 0x4fce) = i;
            (*(u8 *)(int)(scene + 0x4fe4))++;
            w = *(int *)(scene + 0x4cbc);
            col = i % w;
            row = i / w;

            if (col == 0 || col == w - 1)
                wq = 2;
            if (row == 0 || row == w - 1)
                hq = 2;

            if (--col < 0)
                col = 0;
            if (--row < 0)
                row = 0;

            {
                int y;
                for (y = 0; y < hq; y++) {
                    int x;
                    for (x = 0; x < wq; x++) {
                        int index = *(int *)(scene + 0x4cbc) * (row + y) + (col + x);
                        *(u8 *)(scene + index + 0x4efa) = 1;
                        (*(u8 *)(int)(scene + index + 0x4f1e)) ^= 1;
                        *(u16 *)(scene + index * 2 + 0x4e78) = 8;
                    }
                }
            }

            *(u16 *)(scene + i * 2 + 0x4e78) = 0;
            *(u8 *)(scene + 0x4feb) = 0x28;
            (*(u8 *)(int)(scene + 0x4fe1))++;
            Sound::PlayBank2_2D(0x1fa);
            func_ov006_02104e80(scene);
            return;
        }
    } while (++i < n);
}
}

// @symbol func_ov006_02106048
extern "C" {
/* Mark all live panels as settled. */
void func_ov006_02106048(char *scene)
{
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    int i;
    for (i = 0; i < s->mCount; i++) {
        s->mSettled[i] = 1;
    }
}
}

// @symbol func_ov006_02106080
extern "C" {
/* Flip the faces in the 3x3 neighbourhood centred on panel `idx`, clipped at
   the board edges.  The same walk appears in func_ov006_021051dc,
   func_ov006_02105854 and func_ov006_02105de4; only this one touches the face
   table alone. */
void func_ov006_02106080(char *scene, int index)
{
    extern int __aeabi_idiv(int a, int b);
    int w = *(int *)(scene + 0x4cbc);
    int wq = 3;
    int hq = 3;
    int col = index % w;
    int row = index / w;
    if (col == 0 || col == w - 1) wq = 2;
    if (row == 0 || row == w - 1) hq = 2;
    col = col - 1;
    if (col < 0) col = 0;
    row = row - 1;
    if (row < 0) row = 0;
    int y, x;
    for (y = 0; y < hq; y++) {
        for (x = 0; x < wq; x++) {
            u8 *p = (u8 *)((((int)scene + ((*(int *)(scene + 0x4cbc)) * (row + y) + (col + x))) + 0x4f1e));
            *p ^= 1;
        }
    }
}
}

// @symbol func_ov006_02106168
extern "C" {
/* Layout pass: reads the panel layout (x and y from the layout tables,
   face from the face table picked by Virtual8C), draws mFlips random picks
   through func_ov006_02106080, and retries the whole thing until
   func_ov006_02106664 accepts it, at least one panel face differs from its
   copy, and no two picks are the same panel.

   Register shape: the loop-1 counter and the loop-2 counter are DIFFERENT
   locals (i, j); a single `i` for both loops colours out of the callee-saved
   band (r5) no matter where it is declared. With them split, the declaration
   order i, v, cnt2, j, cnt1, pd, pe is what puts i and j on r7, v on sb, cnt2
   on r8, cnt1 on r6 and the ded0 table pointer on r5; every other slot for j
   rotates loop 2 or loop 1. The pd/pe pointer locals keep the byte tables
   hoisted above the do-loop and the [idx4] reload inside it. */

void func_ov006_02106168(dScMgPanel_c *scene)
{
    int RandomIntInternal(int *seed);
    extern int data_0209d4b8;
    void func_ov006_02106080(dScMgPanel_c *scene, int x);
    int func_ov006_02106664(dScMgPanel_c *scene);
    extern u16 *data_ov006_0213dd4c[];
    extern u16 *data_ov006_0213dd58[];
    extern u8 *data_ov006_0213ded0[];
    extern u8 *data_ov006_0213e070[];
    int i;
    int v;
    int cnt2;
    int j;
    int cnt1;
    u8 **pd;
    u8 **pe;
    int mismatch, dup, a, b, k;
    do {
        scene->mFlipCount = 0;
        for (i = 0; i < (cnt1 = scene->mCount); i++) {
            int m = scene->mWidth - 4;
            int idx4 = scene->mFaceSet;
            scene->mX[i] = data_ov006_0213dd4c[m][i] << 12;
            scene->mY[i] = data_ov006_0213dd58[m][i] << 12;
            pe = data_ov006_0213e070;
            pd = data_ov006_0213ded0;
            if (scene->Virtual8C()) {
                scene->mFace[i] = pd[idx4][i];
                scene->mShown[i] = pd[idx4][i];
            } else {
                scene->mFace[i] = pe[idx4][i];
                scene->mShown[i] = pe[idx4][i];
            }
        }
        cnt2 = scene->mFlips;
        for (j = 0; j < cnt2; j++) {
            u32 rnd = (u32)RandomIntInternal(&data_0209d4b8);
            u32 s = (rnd >> 16) & 0x7fff;
            v = (cnt1 * s) >> 15;
            func_ov006_02106080(scene, v);
            scene->mFlipAt[scene->mFlipCount] = v;
            scene->mFlipCount++;
        }
        mismatch = 0;
        for (k = 0; k < scene->mCount; k++) {
            if (scene->mFace[k] != scene->mShown[k]) {
                mismatch++;
                break;
            }
        }
        dup = 0;
        for (a = 0; a < cnt2; a++) {
            for (b = a + 1; b < cnt2; b++) {
                if (scene->mFlipAt[a] == scene->mFlipAt[b]) {
                    dup++;
                    break;
                }
            }
        }
        if (dup != 0) mismatch = 0;
    } while (func_ov006_02106664(scene) == 0 || mismatch == 0);
}
}

// @symbol func_ov006_021063a0
#pragma push
#pragma opt_common_subs off
extern "C" {
/* Address launder and the 15-bit random draw, both used a dozen times below.
   The launder is what stops mwcc folding the field address into the
   surrounding expression; func_ov006_0210446c spells out the same lever. */
#define PANEL_AT(p, off) ((void *)(int)((char *)(p) + (off)))
#define PANEL_RAND ((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff)

struct PanelO_3a0
{
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34();
    virtual int m8c();
};

void func_ov006_021063a0(char *scene)
{
    extern int RandomIntInternal(int *seed);
    extern int data_0209d4b8;
    extern u8 data_ov006_0213dd64[];
    extern u8 data_ov006_0213dd74[];
    extern u8 data_ov006_0213dd94[];
    extern u8 data_ov006_0213de28[];
    extern u8 data_ov006_0213de44[];
    extern u8 data_ov006_0213de60[];
    int t = *(int *)(scene + 0xbc);
    if (((PanelO_3a0 *)scene)->m8c() != 0) {
        int nv;
        if (t >= 0xf) {
            t = (PANEL_RAND * 0xf) >> 15;
            if (t == *(int *)(scene + 0x4de4)) {
                t += ((PANEL_RAND * 0xe) >> 15) + 1;
                if (t >= 0xf) t -= 0xf;
            }
        }
        if (data_ov006_0213dd64[t] != 0 || *(int *)(scene + 0x4cb0) == 0) {
            if (*(int *)(scene + 0x4cb0) == 0) *(int *)PANEL_AT(scene, 0x4cb0) += 1;
            nv = (PANEL_RAND * 8) >> 15;
            if (nv == *(int *)(scene + 0x4cb4)) {
                nv = (nv + (((PANEL_RAND * 7) >> 15) + 1)) & 7;
            }
            *(int *)(scene + 0x4cb4) = nv;
        }
        *(int *)(scene + 0x4cbc) = data_ov006_0213dd94[t];
        { int q = *(int *)(scene + 0x4cbc); *(int *)(scene + 0x4cb8) = q * q; }
        *(int *)(scene + 0x4cc0) = data_ov006_0213dd74[t];
        *(int *)(scene + 0x4de4) = t;
    } else {
        int nv;
        int k;
        u8 *p;
        while (t >= 0x19) t -= 0x19;
        if (data_ov006_0213de28[t] != 0 || *(int *)(scene + 0x4cb0) == 0) {
            if (*(int *)(scene + 0x4cb0) == 0) *(int *)PANEL_AT(scene, 0x4cb0) += 1;
            nv = (PANEL_RAND * 4) >> 15;
            k = data_ov006_0213de60[t];
            p = &data_ov006_0213de60[t];
            if (k == 5) {
                nv = ((PANEL_RAND * 5) >> 15) + 4;
            }
            if (nv == *(int *)(scene + 0x4cb4)) {
                if (*p == 4) {
                    nv = (nv + (((PANEL_RAND * 3) >> 15) + 1)) & 3;
                } else {
                    nv = nv + (((PANEL_RAND * 4) >> 15) + 1);
                    if (nv >= 9) nv -= 5;
                }
            }
            *(int *)(scene + 0x4cb4) = nv;
        }
        *(int *)(scene + 0x4cbc) = data_ov006_0213de60[t];
        { int q = *(int *)(scene + 0x4cbc); *(int *)(scene + 0x4cb8) = q * q; }
        *(int *)(scene + 0x4cc0) = data_ov006_0213de44[t];
    }
}
#undef PANEL_AT
#undef PANEL_RAND
}
#pragma pop

// @symbol func_ov006_02106664
#pragma push
#pragma opt_strength_reduction off
extern "C" {
int func_ov006_02106664(char *scene)
{
    int i, bad, j;
    char *row = scene;

    /* Refuse a pick set that repeats any of the last five. */
    for (i = 0; i < 5; i++) {
        bad = 0;
        for (j = 0; j < 0xa; j++) {
            if (*(u8 *)(scene + 0x4fae + j) != *(u8 *)(row + 0x4ec8 + j)) {
                bad = 1;
                break;
            }
        }
        if (bad == 0) return 0;
        row += 0xa;
    }
    /* Shift the history down one row, then record this pick set as row 0. */
    {
        int r, k;
        char *dst;
        char *src = scene;
        for (r = 0; r < 4; r++) {
            dst = scene + (r + 1) * 0xa;
            for (k = 0; k < 0xa; k++) {
                *(u8 *)(dst + 0x4ec8) = *(u8 *)(src + 0x4ec8 + k);
                dst++;
            }
            src += 0xa;
        }
    }
    for (i = 0; i < 0xa; i++) {
        *(u8 *)(scene + 0x4ec8 + i) = *(u8 *)(scene + 0x4fae + i);
    }
    return 1;
}
}
#pragma pop

// @symbol func_ov006_02106758
extern "C" {
void func_ov006_02106758(char *scene)
{
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 0xa; j++) {
            *(u8 *)(scene + j + 0x4ec8) = 0xff;
        }
        scene += 0xa;
    }
}
}

// @symbol func_ov006_021067a4
/* Clear the whole board back to its start-of-round state: the 36 parallel
   panel arrays, the state word, the pick list (0xff = no pick) and the
   cursor, then hand off to the three subsystem resets. */
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_021067a4(char *scene)
{
    extern void func_ov006_02104b4c(char *p);
    extern void func_ov006_021048b0(char *o);
    extern void func_ov006_02104558(char *p);
    int i;
    for (i = 0; i < 0x24; i++) {
        *(int *)(scene + i * 4 + 0x4cc4) = 0;
        *(int *)(scene + i * 4 + 0x4d54) = 0;
        *(s16 *)(scene + i * 2 + 0x4de8) = 0;
        *(s16 *)(scene + i * 2 + 0x4e30) = 0;
        *(s16 *)(scene + i * 2 + 0x4e78) = 0;
        *(char *)(scene + i + 0x4f1e) = 0;
        *(char *)(scene + i + 0x4efa) = 0;
        *(char *)(scene + i + 0x4f42) = 0;
        *(char *)(scene + i + 0x4f66) = 0;
        *(char *)(scene + i + 0x4f8a) = 0;
    }
    *(int *)(scene + 0x4ca8) = 0;
    *(char *)(scene + 0x4fe9) = 0;
    *(s16 *)(scene + 0x4ec0) = 0;
    *(s16 *)(scene + 0x4ec2) = 0;
    *(char *)(scene + 0x4fde) = 0;
    *(char *)(scene + 0x4fe1) = 0;
    *(char *)(scene + 0x4fe2) = 0;
    for (i = 0; i < 0x20; i++) {
        *(u8 *)(scene + i + 0x4fae) = 0xff;
    }
    for (i = 0; i < 0x10; i++) {
        *(char *)(scene + i + 0x4fce) = 0;
    }
    *(char *)(scene + 0x4fe4) = 0;
    *(char *)(scene + 0x4fe0) = 0;
    dScMgPanel_c *s = (dScMgPanel_c *)scene;
    s->mSlide = 0;
    s->mSlideY = 0;
    s->mSlideVel = 0;
    *(int *)(scene + 0x466c) = 0;
    s->mHold = 0;
    s->mSlideStep = 0;
    s->mSlideFlag = 0;
    s->mSlideOn = 0;
    *(int *)(scene + 0x4678) = 0;
    *(int *)(scene + 0x467c) = 0;
    *(s16 *)(scene + 0x4680) = 0;
    *(char *)(scene + 0x4684) = 0;
    *(char *)(scene + 0x4685) = 0;
    *(char *)(scene + 0x4686) = 0;
    *(char *)(scene + 0x4fe6) = 0;
    *(s16 *)(scene + 0x4ec6) = 0;
    func_ov006_02104b4c(scene);
    func_ov006_021048b0(scene);
    func_ov006_02104558(scene);
}
}
#pragma pop

// @symbol func_ov006_021068d8
extern "C" {
void func_ov006_021068d8(char *scene)
{
    int i;
    for (i = 0; i < *(int *)(scene + 0x4cbc); i++) {
        *(u8 *)(scene + i + 0x4efa) = 3;
    }
}
}

// @symbol func_ov006_02106910
extern "C" {
void func_ov006_02106910(char *scene, int index)
{
    int old = ((int *)(scene + 0x4d54))[index];
    ((int *)(scene + 0x4d54))[index] = old + 0x10000;
    if ((*(int *)(scene + index * 4 + 0x4d54) >> 12) >= 0xd0) {
        *(u8 *)(scene + index + 0x4efa) = 0;
        *(u8 *)(scene + index + 0x4f66) = 0;
    }

    /* Hand the fall on to the panel directly below, unless this one is already
       on the bottom row or the one below has not come far enough down yet.
       The row/column split is a subtract loop, not a divide, and the nested
       blocks are what keep it that way -- flattening them costs 18 words. */
    {
        u8 *flag = (u8 *)(scene + 0x4f8a + index);
        if (*flag != 0) return;
        {
            int col = index;
            int row = 0;
            int div = *(int *)(scene + 0x4cbc);
            if (index >= div) {
                do {
                    col -= div;
                    row++;
                } while (col >= div);
            }
            if (row == div - 1) {
                *flag = 1;
                return;
            }
            {
                int nxt = (row + 1) * div + col;
                if (((*(int *)(scene + nxt * 4 + 0x4d54) - *(int *)(scene + (u32)index * 4 + 0x4d54)) >> 12) > 0) return;
                *flag = 1;
                *(u8 *)(scene + nxt + 0x4efa) = 3;
            }
        }
    }
}
}

// @symbol func_ov006_02106a08
extern "C" {
void func_ov006_02106a08(char *c, int idx)
{
    u16 *timers = (u16 *)(int)(c + 0x4e30);
    u8 *q;
    timers[idx] = timers[idx] + 1;
    if ((((int)*(u16 *)(c + idx * 2 + 0x4e30) >> 3) & 1) != 0) {
        q = (u8 *)(int)(c + 0x4f66);
        q[idx] = 1;
    } else {
        q = (u8 *)(int)(c + 0x4f66);
        q[idx] = 0;
    }
    {
        /* The `(long long)` round trip on the index is load-bearing here. */
        int idxL = (int)((long long)idx);
        char *b = c + idxL * 2;
        b += 0x4e00;
        if (((u16 *)b)[0x18] < 0x40)
            return;
        ((u16 *)b)[0x18] = 0;
        *(u8 *)(c + idx + 0x4efa) = 0;
        q[idx] = 1;
    }
}
}

// @symbol func_ov006_02106aa8
#pragma push
#pragma opt_common_subs off
extern "C" {
/*
 * Per-slot tick for one of the tracked entries (idx).
 * Bumps the global tick byte at +0x4fe9.  If the slot's hold counter
 * (+0x4e78[idx]) is running, count it down and clamp at zero; otherwise
 * advance the slot's frame counter (+0x4e30[idx]) and, once it reaches
 * the limit (4 while the +0x4f8a flag is set, else 8), reset it, step the
 * slot's phase (+0x4de8[idx]) and wrap the phase past 5 with the +0x4efa
 * flag cleared.
 *
 * Shape notes: the pool-constant + register-offset form is what the
 * RMW sites (`*p = *p +- 1`) produce, the split base+0xNN00/#off form is
 * what the plain reads produce, and opt_common_subs off keeps the
 * else-branch recomputing base+idx*2 (its own extended basic block) the
 * way the ROM does.  The frame counter must be bumped before the flag
 * byte is read, and the limit compare is unsigned.
 */

void func_ov006_02106aa8(char *scene, int index)
{
    u8 *cnt = (u8 *)(scene + 0x4fe9);

    *cnt = *cnt + 1;

    if (*(u16 *)(scene + index * 2 + 0x4e78) != 0) {
        u16 *q = (u16 *)(scene + 0x4e78 + index * 2);
        *q = *q - 1;
        if (*(s16 *)(scene + index * 2 + 0x4e78) < 0) {
            *(u16 *)(scene + index * 2 + 0x4e78) = 0;
        }
    } else {
        u16 *r = (u16 *)(scene + 0x4e30 + index * 2);
        u8 flag;
        u32 lim;
        *r = *r + 1;
        flag = *(u8 *)(scene + index + 0x4000 + 0xf8a);
        lim = (flag != 0) ? 4 : 8;
        if (*(u16 *)(scene + index * 2 + 0x4e30) < lim) {
            return;
        }
        *(u16 *)(scene + index * 2 + 0x4e30) = 0;
        {
            u16 *s = (u16 *)(scene + 0x4de8 + index * 2);
            *s = *s + 1;
        }
        if (*(u16 *)(scene + index * 2 + 0x4d00 + 0xe8) >= 5) {
            *(u16 *)(scene + index * 2 + 0x4d00 + 0xe8) = 0;
            *(u8 *)(scene + index + 0x4000 + 0xefa) = 0;
        }
    }
}
}
#pragma pop

// @symbol func_ov006_02106bac
extern "C" {
void func_ov006_02106bac(char *scene, int index)
{
    char *base = scene + index + 0x4000;
    base[0xf8a] = 0;
}
}

// @symbol func_ov006_02106bc0
extern "C" {
struct PanelPmfRec_bc0 { int off; int adj; };
typedef void (*PanelPmfFn_bc0)(void *, int);

void func_ov006_02106bc0(char *scene)
{
    extern struct PanelPmfRec_bc0 data_ov006_02142840[];
    extern void func_ov004_020b0a54(int arg);
    extern void func_ov006_02104ea8(char *scene);
    int i;
    for (i = 0; i < *(int *)(scene + 0x4cb8); i++) {
        u8 index = *(u8 *)(scene + i + 0x4efa);
        struct PanelPmfRec_bc0 *e = &data_ov006_02142840[index];
        int adj = e->adj;
        char *thisp = scene + (adj >> 1);
        PanelPmfFn_bc0 fn;
        if (adj & 1) {
            fn = *(PanelPmfFn_bc0 *)(*(char **)thisp + e->off);
        } else {
            fn = (PanelPmfFn_bc0)e->off;
        }
        fn(thisp, i);
    }
    if (*(u16 *)(scene + 0x4ec0) == 0) return;
    *(u16 *)(scene + 0x4ec0) -= 1;
    if (*(s16 *)(scene + 0x4ec0) > 0) return;
    *(u16 *)(scene + 0x4ec0) = 0;
    func_ov004_020b0a54(0x12);
    *(u8 *)(scene + 0xc3) = 0;
    *(u8 *)(scene + 0x4fe3) = 0;
    func_ov006_02104ea8(scene);
}
}

// @symbol func_ov006_02106ca4
#pragma push
#pragma opt_propagation off
extern "C" {
/* This function reads data_ov004_020beb68 as a char *; dScMgBase_c.h
   declares it void *.  Reinterpret rather than redeclare. */
#define PANEL_BEB68 (*(char **)&data_ov004_020beb68)
struct PanelC_ca4;
typedef void (PanelC_ca4::*PanelPmf_ca4)(int);

void func_ov006_02106ca4(char *scene)
{
    extern PanelPmf_ca4 data_ov006_02142840[];
    extern u8 data_020a0e40;
    extern u8 data_020a0de8[];
    extern u8 data_020a0de9[];
    void func_ov006_021050bc(void *scene);
    void func_ov006_02104580(void *scene);
    void func_ov006_02104870(void *scene);
    void func_ov006_02104ea8(void *scene);
    void func_ov006_021068d8(void *scene);
    void func_ov004_020b0a54(int a);
    void func_ov004_020adb1c(int a);
    int found;
    int i;
    int n;

    func_ov006_021050bc(scene);
    found = 0;
    i = found;
    n = *(int *)(scene + 0x4000 + 0xcb8);
    if (n > 0) {
        do {
            u8 *p = (u8 *)(scene + i + 0x4efa);
            (((PanelC_ca4 *)scene)->*data_ov006_02142840[*p])(i);
            i++;
            if (*p != 0)
                found++;
            n = *(int *)(scene + 0x4000 + 0xcb8);
        } while (i < n);
    }
    if (found != 0)
        return;

    if (*(u16 *)(scene + 0x4e00 + 0xc0) != 0) {
        int off;
        u16 *tp;
        u8 *flags;
        off = 0x4ec0;
        tp = (u16 *)(int)((long long)(int)(scene + off) & 0xFFFFFFFFFFFFFFFFLL);
        flags = (u8 *)(scene + 0x4000);
        *tp = *tp - 1;
        if (flags[0xfe6] == 0) {
            int hit;
            u8 index;
            hit = 0;
            index = data_020a0e40;
            if (data_020a0de8[index * 4] != 0) {
                if (data_020a0de9[index * 4] != 0)
                    hit = 1;
            }
            if (hit)
                *(u16 *)(scene + 0x4e00 + 0xc0) = 0;
        }
        if (*(s16 *)(scene + 0x4e00 + 0xc0) > 0)
            return;
        *(u16 *)(scene + 0x4e00 + 0xc0) = 0;
        if (*(u8 *)(scene + 0x4000 + 0xfe6) != 0) {
            char *g;
            func_ov006_02104870(scene);
            func_ov004_020b0a54(0);
            func_ov006_02104ea8(scene);
            *(u8 *)(scene + 0x4000 + 0xfe3) = 0;
            *(u8 *)(scene + 0xc3) = 0;
            g = PANEL_BEB68;
            if (g != 0) {
                if (*(int *)(g + 0xb4) < 9999)
                    (*(int *)(int)(g + 0xb4))++;
                if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                    *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
            }
            func_ov004_020adb1c(PANEL_BEB68 != 0 ? *(int *)(PANEL_BEB68 + 0xb4) : 0);
            return;
        }
        *(u16 *)(scene + 0x4e00 + 0xc0) = 0x10;
        *(int *)(scene + 0x4000 + 0xca8) = 7;
        func_ov006_021068d8(scene);
        return;
    }
    func_ov006_02104580(scene);
    {
        int inc;
        u16 *ip;
        inc = 0x4ec6;
        ip = (u16 *)(int)((long long)(unsigned)(scene + inc) & 0xFFFFFFFFFFFFFFFFLL);
        *ip = *ip + 1;
    }
}
#undef PANEL_BEB68
}
#pragma pop

// @symbol func_ov006_02106eb8
extern "C" {
struct PanelC_eb8;
typedef void (PanelC_eb8::*PanelPmf_eb8)(int);
void func_ov006_02106eb8(char *scene)
{
    extern PanelPmf_eb8 data_ov006_02142840[];
    extern void func_ov006_021050bc(void *scene);
    extern void func_ov006_021057f0(void *scene);
    int i;
    func_ov006_021050bc(scene);
    func_ov006_021057f0(scene);
    for (i = 0; i < *(int *)(scene + 0x4cb8); i++) {
        u8 index = *(u8 *)(scene + i + 0x4efa);
        (((PanelC_eb8 *)scene)->*data_ov006_02142840[index])(i);
    }
}
}

// @symbol func_ov006_02106f44
extern "C" {
struct PanelC_f44;
typedef void (PanelC_f44::*PanelPmf_f44)(int);
struct PanelC_f44 { char pad[0x8000]; };
void func_ov006_02106f44(char *scene)
{
    extern PanelPmf_f44 data_ov006_02142840[];
    extern void func_ov006_021050bc(void *scene);
    extern void func_ov006_02105854(void *scene);
    int i;
    func_ov006_021050bc(scene);
    *(u8 *)(scene + 0x4fe9) = 1;
    func_ov006_02105854(scene);
    for (i = 0; i < *(int *)(scene + 0x4cb8); i++) {
        u8 index = *(u8 *)(scene + i + 0x4efa);
        (((PanelC_f44 *)scene)->*data_ov006_02142840[index])(i);
    }
}
}

// @symbol func_ov006_02106fdc
extern "C" {
typedef void (dScMgPanel_c::*PanelPmf_fdc)(int);

void func_ov006_02106fdc(dScMgPanel_c *scene)
{
    extern PanelPmf_fdc data_ov006_02142840[];
    extern void func_ov006_021050bc(dScMgPanel_c *scene);
    u8 index;
    int found;
    int i;
    func_ov006_021050bc(scene);
    found = 0;
    scene->mBusy = 1;
    for (i = 0; i < scene->mCount; i++) {
        index = scene->mKind[i];
        (scene->*data_ov006_02142840[index])(i);
        if (index != 0)
            found++;
    }
    if (found == 0) {
        scene->mState = 4;
        scene->mClear = 1;
        scene->mDelay = 0x40;
    }
}
}

// @symbol func_ov006_0210709c
extern "C" {
struct PanelC_09c;
typedef void (PanelC_09c::*PanelPmf_09c)(int);

void func_ov006_0210709c(PanelC_09c *scene)
{
    extern PanelPmf_09c data_ov006_02142840[];
    void func_ov006_02105de4(PanelC_09c *scene);
    void func_ov006_021050bc(PanelC_09c *scene);
    void func_ov006_02105d20(PanelC_09c *scene);
    void func_ov006_02105c88(PanelC_09c *scene);
    void func_ov006_02105134(PanelC_09c *scene);
    int i;
    char *b = (char *)scene;
    func_ov006_02105de4(scene);
    func_ov006_021050bc(scene);
    *(u8 *)(b + 0x4fe9) = 0;
    for (i = 0; i < *(int *)(b + 0x4cb8); i++) {
        u8 index = *(u8 *)(b + i + 0x4efa);
        (scene->*data_ov006_02142840[index])(i);
    }
    func_ov006_02105d20(scene);
    func_ov006_02105c88(scene);
    func_ov006_02105134(scene);
}
}

// @symbol func_ov006_0210713c
extern "C" {
void func_ov006_0210713c(char *scene)
{
    extern void func_ov006_021050bc(void *scene);
    if (*(int *)(scene + 0x4cac) == 0) {
        func_ov006_021050bc(scene);
        *(u8 *)(scene + 0x4fe7) = 0;
        *(u8 *)(scene + 0x4fe8) = 2;
        return;
    }
    *(u8 *)(scene + 0x4fe7) += 1;
    if (*(u8 *)(scene + 0x4fe7) >= 8) {
        *(u8 *)(scene + 0x4fe7) = 0;
        *(u8 *)(scene + 0x4fe8) -= 1;
    }
    if (*(u8 *)(scene + 0x4fe8) == 0)
    *(int *)(scene + 0x4ca8) = 2;
}
}

// @symbol func_ov006_021071d4
extern "C" {
void func_ov006_021071d4(char *scene)
{
    extern void func_ov006_02105118(char *p);
    func_ov006_02105118(scene);
    *(int *)(scene + 0x4ca8) = 1;
    *(int *)(scene + 0x4cac) = 0;
}
}

/* The four class methods can't sit in an `extern "C"` region, so a
 * declaration inside them would get C++ linkage and name a symbol the ROM
 * doesn't have. Their callees are declared here instead, below every helper,
 * so the helpers' own declarations are untouched. Data declarations are not
 * mangled, so those stay in the bodies. */
extern "C" {

extern void  Deallocate(void *);
extern int   GetOwnerLanguage(void);
extern int   LoadFile(int handle);
extern void  DecompressLZ16(int src, void *dst);
extern void  MultiStore16(u16 val, char *dst, int nbytes);
extern void  FreeGfxSlotsById(int arg);
extern void  SetSubBg0Offset(int, int);
extern int   func_02054d88(void);
extern void  func_020562b4(const void *, u32, u32);
extern void  func_02056314(void *, u32, u32);
extern void  func_02056374(const void *, u32, u32);
extern void  func_020563d4(const void *, u32, u32);
extern void  func_020564f4(const void *, int, int);
extern void  func_ov004_020adb1c(int);
extern void  func_ov004_020b04d0(int);
extern void  func_ov004_020b0cac(int, int, int, int, int, s16);
extern void  func_ov004_020b1e34(void *a, int b, int c, int d);

}  /* extern "C" */

// @symbol _ZN12dScMgPanel_c13OnYoshiTryEatEi
/* Vtable slot 18, an override of dScMgBase_c::OnYoshiTryEat(int).  The
   signature has to repeat the base declaration exactly or mwcc appends a new
   slot instead of overriding it. */

void dScMgPanel_c::OnYoshiTryEat(int flag)
{
    extern u8 data_0209d454;
    char *self = (char *)this;

    char *p;

    if (flag == 0) {
        int *q = (int *)(self + 0xbc);
        *q += 1;
        if (*(u32 *)(self + 0xbc) > 0x270e)
            *(int *)(self + 0xbc) = 0x270e;
    } else {
        mMark = 0;
        *(int *)(self + 0xbc) = 0;
        if (*(u32 *)(self + 0xbc) > 0x270e)
            *(int *)(self + 0xbc) = 0x270e;

        if (data_ov004_020beb68 != 0)
            *(int *)((char *)data_ov004_020beb68 + 0xb4) = 0;

        p = (char *)data_ov004_020beb68;
        func_ov004_020adb1c(p != 0 ? *(int *)(p + 0xb4) : 0);
    }

    FreeGfxSlotsById(0x1d);
    func_ov006_021067a4(self);
    func_ov006_021063a0(self);
    func_ov006_02106168(this);
    func_ov006_02104b24(self);

    mLives = 3;
    mState = 0;
    func_ov004_020b0cac(0xd, 0x80, 0x40, 0, -1, 0xd);
    SetSubBg0Offset(0, 0);

    data_0209d454 &= ~1;
}

// @symbol _ZN12dScMgPanel_c6RenderEv
/* Vtable slot 9. */
s32 dScMgPanel_c::Render()
{
    char *c = (char *)this;
    func_ov004_020b1e34(this, 0xe0, 0x14, 1);
    func_ov006_02104d44(c);
    func_ov006_02104d94(c);
    func_ov006_021042e8(c);
    func_ov006_02104b5c(c);
    func_ov006_02105ab4((PanelObj_ab4 *)c);
    return 1;
}

// @symbol _ZN12dScMgPanel_c8BehaviorEv
/* Vtable slot 6.  One frame of the current state, dispatched through the
   descriptor pair at data_ov006_02142888. */
/* One row of the state-dispatch table at data_ov006_02142888: `a` is either a
   function address or a vtable byte offset, and bit 0 of `b` says which. */
struct Ent_358 { int a; int b; };
s32 dScMgPanel_c::Behavior()
{
    extern Ent_358 data_ov006_02142888[];
    char *c = (char *)this;
    int idx = mState;
    Ent_358 *e = &data_ov006_02142888[idx];
    int adj = e->b;
    char *obj = c + (adj >> 1);
    int fn;
    if (adj & 1) {
        fn = *(int *)(*(int *)obj + e->a);
    } else {
        fn = e->a;
    }
    ((void (*)(void *))fn)(obj);
    func_ov006_02104ac4((PanelC_ac4 *)c);
    func_ov006_02104354(c);
    return 1;
}

// @symbol _ZN12dScMgPanel_c13InitResourcesEv
/* Vtable slot 0.  Brings up both screens' backgrounds, loads the panel and
   message graphics for the owner's language, then resets the board. */

s32 dScMgPanel_c::InitResources()
{
    extern u8 data_0209d45c;
    extern u8 data_0209d454;
    char *c = (char *)this;
    char *b;
    volatile u16 mainFill;
    volatile u16 subFill;
    int file;
    int msgFile;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    file = LoadFile(0x6a);
    DecompressLZ16(file, (void *)(func_02054d88() + 0x4000));
    Deallocate((void *)file);

    file = LoadFile(0x6b);
    GX::LoadBGPltt((const void *)file, 0x1a0, 0x60);
    Deallocate((void *)file);

    file = LoadFile(0x6c);
    func_02056314((void *)file, 0, 0x800);
    Deallocate((void *)file);

    data_0209d45c |= 4;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u32 *)0x4000018 = 0;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9410;

    b = G2::GetBG2ScrPtr();
    mainFill = 0xf361;
    MultiStore16(mainFill, b, 0x1000);

    file = LoadFile(0x1e6);
    func_020563d4((const void *)file, 0x800, 0x800);
    Deallocate((void *)file);

    msgFile = LoadFile(0xe5);
    if (GetOwnerLanguage() == 5) {
        msgFile = LoadFile(0xe7);
    } else if (GetOwnerLanguage() == 4) {
        msgFile = LoadFile(0xe4);
    } else if (GetOwnerLanguage() == 3) {
        msgFile = LoadFile(0xe3);
    } else if (GetOwnerLanguage() == 2) {
        msgFile = LoadFile(0xe2);
    }

    file = LoadFile(0xe6);
    DecompressLZ16(msgFile, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)file, 0, 0x100);

    data_0209d454 |= 8;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 1;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 1;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x210;

    {
        int subFile = LoadFile(0x6a);
        DecompressLZ16(subFile, (void *)(G2S::GetBG3CharPtr() + 0x4000));
        Deallocate((void *)subFile);

        subFile = LoadFile(0x6b);
        GXS::LoadBGPltt((const void *)subFile, 0x1a0, 0x60);
        Deallocate((void *)subFile);

        subFile = LoadFile(0x69);
        func_020562b4((const void *)subFile, 0, 0x800);
        Deallocate((void *)subFile);

        data_0209d454 |= 4;
        *(volatile u16 *)0x400100c &= ~0x40;
        *(volatile u32 *)0x4001018 = 0;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 1;
        *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x8410;

        b = G2S::GetBG2ScrPtr();
        subFill = 0xf361;
        MultiStore16(subFill, b, 0x1000);

        subFile = LoadFile(0x1e6);
        func_02056374((const void *)subFile, 0, 0x800);
        Deallocate((void *)subFile);

        *(volatile u16 *)0x4001008 &= ~3;
        *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x610;

        subFile = LoadFile(0x1e5);
        func_020564f4((const void *)subFile, 0, 0x800);
        Deallocate((void *)subFile);
    }

    DecompressLZ16(msgFile, (void *)0x6600000);
    GXS::LoadOBJPltt((const void *)file, 0, 0x100);
    Deallocate((void *)msgFile);
    Deallocate((void *)file);

    FreeGfxSlotsById(0x1d);
    func_ov006_021067a4(c);
    func_ov006_02106758(c);

    mDealt = 0;
    mMark = 0;
    mFaceSet = 0xff;

    func_ov006_021063a0(c);
    func_ov006_02106168(this);
    func_ov006_02105118(c);
    func_ov006_02104b24(c);

    mState = 1;
    mIntro = 0;
    mLives = 3;

    func_ov004_020b04d0(0x20);
    func_ov004_020b0cac(0xd, 0x80, 0x40, 0, -1, 0xd);
    *(int *)(c + 0xb4) = 0;
    return 1;
}
