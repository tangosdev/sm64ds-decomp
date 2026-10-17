//cpp
/**
 * Course-entry / results scene.
 *
 * ov075 hosts ENTRY(6) and RESULT(7); both spawn dScEntry_c. Nested
 * icon_c (nine dThIcon_c leaves) and graphCallback_c (dGraph_c::callback_c
 * leaf). GraphCallback2 writes sub BG2 priority, decompresses the screen,
 * then steps every live icon through Render. icon_c::Behavior bobs the
 * selected character; icon_c::Render draws course, arrow and character
 * OAM.
 *
 * dScEntry_c_classInit_ENTRY / RESULT are reconstructed (RTTI dScEntry_c,
 * ENTRY/RESULT registry). Retail does not store those spellings. RESULT
 * is in d_s_entry_methods.cpp, this unit's upper part past the hole.
 * ENTRY (d_s_entry_entry.cpp) opens the next unit.
 *
 * deslop
 * Leftover: helpers stay func_ov075_*. This TU's GraphCallback2 calls
 *   021160dc; 02115e8c / 0211601c / 02116028 / 02116030 are the icon
 *   and graphCallback writers. Naming belongs with recovered members,
 *   not coined here.
 * Leftover: func_ov001_020ab5b0 is still the linker name of the ov001
 *   dThIcon field writer (kind, x, y, w, h). This TU's 02115e8c calls
 *   it. Naming belongs in ov001.
 * Leftover: func_0200f0bc is still the linker name of the arm9 language
 *   mapper (GetOwnerLanguage, then 2..5 -> 1..4). This TU's 02115e8c
 *   indexes the width/height pairs with it.
 * Leftover: func_0203d974 is still the arm9 vs/region gate
 *   (0x27ffc40 == 2, else data_020a0f10). This TU's icon_c::Render
 *   case 13. Naming belongs in arm9.
 * Leftover: ApproachLinear2 stays mangled. No header. This TU's
 *   icon_c::Behavior.
 * Leftover: G2S::GetBG2ScrPtr stays mangled. No header. GraphCallback2
 *   and icon_c::Render.
 * Leftover: OAM::Render Fix12-by-value stays mangled (OAM.h, wall 6az).
 *   This TU's icon_c::Render.
 * Leftover: data_ov075_0211cb64 / 0211c720 / 0211c730 / 0211c954 are
 *   OAM. 0211d72c / 0211d72e / 0211d740 / 0211d742 / 0211d754 /
 *   0211d756 / 0211d948 / 0211d94a are language- or type-indexed
 *   width/height pairs (4-byte stride). Overlay .data/.bss this TU
 *   does not own.
 * Leftover: data_0209b2e4 is still the linker name of the selected
 *   character. This TU's icon_c::Behavior. Naming belongs in arm9.
 * Leftover: g_profile_ENTRY / g_profile_RESULT live outside this TU
 *   (S14). leftover return new belongs with the factories.
 *
 * Source order IS the ROM's: `#pragma defer_codegen off` below makes
 * mwccarm emit .text in source order instead of reverse. Do not reorder.
 * Under deferred codegen the out-of-line destructor emitted D0 before D1,
 * which is why the pair used to stay in its own files. Written first in
 * this ascending file, it emits D1, D0, D2 -- the cartridge's order --
 * and the unreferenced D2 is dropped at link. The destructor is the key
 * function, so this file also emits the vtable and the typeinfo chain;
 * the cartridge's copies stay canonical.
 *
 * .text 0x02115ab8..0x021165b0, 15 functions. The unit goes on to
 * 0x0211a854 (82 functions): 58 remaining func_ov075_* helpers, each still its own
 * file, then d_s_entry_methods.cpp (0x0211a210..0x0211a854) with the six
 * scene methods, the RESULT factory and the OamAnimation and icon_c
 * constructors. A licensed range cannot step over the helpers.
 */

#pragma defer_codegen off

#include "dScEntry_c.h"
#include "OAM.h"

extern "C" {
unsigned int _ZN3G2S12GetBG2ScrPtrEv();
void DecompressLZ16(void *, void *);
void Deallocate(void *);

int _Z15ApproachLinear2Rsss(short *, short, short);
extern unsigned char data_0209b2e4;

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int a, void *oam, int b, int c, int d, int e, int f, int g, int h, int i);
extern int func_0203d974(void);

int func_0200f0bc(void);
void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);

extern int data_ov075_0211cb64[];
extern char data_ov075_0211c720[];
extern char data_ov075_0211c730[];
extern void *data_ov075_0211c954[];
extern short data_ov075_0211d72c[];
extern short data_ov075_0211d72e[];
extern short data_ov075_0211d740[];
extern short data_ov075_0211d742[];
extern short data_ov075_0211d754[];
extern short data_ov075_0211d756[];
extern short data_ov075_0211d948[];
extern short data_ov075_0211d94a[];
}

#define SH(base,i) (*(short*)((char*)(base) + (i)*4))

extern "C" void func_ov075_021160dc(char* c);

/* Written first so D1 and D0 are emitted first, as in the cartridge. The
 * body is empty: tearing down the four OamAnimations and the nine icon_c
 * leaves in reverse declaration order, then chaining to dScene_c, is all
 * compiler-generated. */
// @symbol _ZN10dScEntry_cD1Ev
dScEntry_c::~dScEntry_c()
{
}

// @symbol _ZN10dScEntry_cD0Ev
/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

// @symbol _ZN10dScEntry_c6icon_cD1Ev
dScEntry_c::icon_c::~icon_c()
{
}

// @symbol _ZN12OamAnimationD1Ev
OamAnimation::~OamAnimation()
{
}

// @symbol _ZN10dScEntry_c6icon_c6RenderEv
void dScEntry_c::icon_c::Render()
{
    volatile s16 xy[2];
    u16 *p;
    int mask;
    int row;
    int col;
    int w;
    int h;
    void *oam;
    int y;
    int x;

    xy[0] = unk_004;
    xy[1] = unk_006;

    switch (unk_01c) {
    case 7:
    {
        int idx;
        void *ptr;
        int v;

        idx = unk_020;
        if (unk_010 == 0) {
            if (unk_011 != 0)
                idx += 4;
        }
        ptr = (void *)data_ov075_0211cb64[idx];

        v = unk_00c;
        if (v <= 0) {
            v = 0x1000;
        } else if (v < 4) {
            v = (v << 8) + 0x1000;
        } else {
            v = ((8 - v) << 8) + 0x1000;
        }

        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, ptr, xy[0], xy[1], -1, -1, v, v, 0, -1);
        return;
    }

    case 0:
    case 1:
    case 2:
        mask = (unk_010 != 0) ? 0x6000 : 0x5000;
        /* The ROM's case 0..2 block branches into case 8..11's fill loop.
         * A shared inline helper (void or int-returning), one merged
         * case, a do/while(0) that breaks out of a merged case, a fill
         * tail after the switch and a fill duplicated into both cases
         * all miss it: mwccarm merges no tails, and merging the cases
         * adds an unk_01c compare. `if (0) { case 8: ... }' matches,
         * but it is this jump spelled less plainly, so the jump stays. */
        goto fill;

    case 8:
    case 9:
    case 10:
    case 11:
        mask = (unk_010 != 0) ? 0x2000 : 0x1000;
    fill:
        w = unk_008;
        p = (u16 *)_ZN3G2S12GetBG2ScrPtrEv();
        x = unk_004;
        h = unk_00a;
        x -= w;
        x >>= 3;
        p += x;
        y = unk_006;
        y -= h;
        p += (y >> 3) << 5;
        for (row = 0; row < unk_00a >> 2; row++) {
            for (col = 0; col < w >> 2; col++) {
                p[col] = (u16)(mask | (p[col] & 0xfff));
                w = unk_008;
            }
            p += 0x20;
        }
        return;

    case 3:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] - 2);
        oam = data_ov075_0211c720;
        break;

    case 4:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] + 2);
        oam = data_ov075_0211c730;
        break;

    case 13:
        if (func_0203d974() != 0 && unk_020 >= 4)
            return;
        oam = data_ov075_0211c954[unk_020];
        break;

    case 5:
    case 6:
    case 12:
    default:
        return;
    }

    OAM::RenderSub((OamAttr *)oam, xy[0], xy[1]);
}

// @symbol _ZN10dScEntry_c6icon_c8BehaviorEv
void dScEntry_c::icon_c::Behavior()
{
    if (unk_01c == 0xd) {
        if (unk_020 == data_0209b2e4) {
            _Z15ApproachLinear2Rsss(&unk_006, 0x14, 8);
        } else {
            if (_Z15ApproachLinear2Rsss(&unk_006, -0x24, 8) != 0)
                unk_020 = data_0209b2e4;
        }
    }
    dThIcon_c::Behavior();
}

extern "C" {

// @symbol func_ov075_02115e8c
void func_ov075_02115e8c(char* self, int a, int b, short c, short e)
{
    int kind;
    int i, j;
    *(int*)(self + 0x1c) = a;
    *(int*)(self + 0x20) = b;
    kind = (*(int*)(self + 0x1c) < 7) ? 2 : (*(int*)(self + 0x1c) < 0xd) ? 1 : 0;
    if (a == 5) {
        i = func_0200f0bc();
        j = func_0200f0bc();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d72c, i), SH(data_ov075_0211d72e, j));
    } else if (a == 6) {
        i = func_0200f0bc();
        j = func_0200f0bc();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d740, i), SH(data_ov075_0211d742, j));
    } else if (a == 0xb) {
        i = func_0200f0bc();
        j = func_0200f0bc();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d754, i), SH(data_ov075_0211d756, j));
    } else {
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d948, a), SH(data_ov075_0211d94a, a));
    }
}

// @symbol func_ov075_0211601c
void func_ov075_0211601c(char* c, int a, int b) {
    dScEntry_c::graphCallback_c *cb = (dScEntry_c::graphCallback_c *)c;
    cb->entryScene = (void *)a;
    cb->bg2Priority = b;
}

// @symbol func_ov075_02116028
void func_ov075_02116028(int *p, int v)
{
    ((dScEntry_c::graphCallback_c *)p)->bg2Priority = v;
}

// @symbol func_ov075_02116030
void func_ov075_02116030(void *c, int v)
{
    dScEntry_c::graphCallback_c *cb = (dScEntry_c::graphCallback_c *)c;
    if (cb->compressedBg2Screen == 0)
        cb->compressedBg2Screen = (void *)v;
}

}

// @symbol _ZN10dScEntry_c15graphCallback_c14GraphCallback2Ev
int dScEntry_c::graphCallback_c::GraphCallback2()
{
    volatile unsigned short *reg = (volatile unsigned short *)0x400100c;
    unsigned short value = (unsigned short)(*reg & ~0x1f00);
    *reg = value | (bg2Priority << 8);

    if (compressedBg2Screen != 0) {
        unsigned short *screen = (unsigned short *)_ZN3G2S12GetBG2ScrPtrEv();
        DecompressLZ16(compressedBg2Screen, screen);
        Deallocate(compressedBg2Screen);
        compressedBg2Screen = 0;
    }

    if (entryScene != 0)
        func_ov075_021160dc((char *)entryScene);

    return 1;
}

// @symbol _ZN10dScEntry_c15graphCallback_cC1Ev
dScEntry_c::graphCallback_c::graphCallback_c()
    : compressedBg2Screen(0), entryScene(0)
{
}

// @symbol func_ov075_021160dc
extern "C" void func_ov075_021160dc(char* c) {
    dScEntry_c *scene = (dScEntry_c *)c;
    int count = scene->mIconCount;
    int i = 0;
    if (count <= 0) return;
    dScEntry_c::icon_c *elem = scene->mIcons;
    do {
        elem->Render();
        count = scene->mIconCount;
        i++;
        elem++;
    } while (i < count);
}

// @symbol func_ov075_02116128
/* recovered: dScEntry_c recolours the border of one VS/wireless entry-menu slot box.
 *
 * G2::GetBG1ScrPtr gives the BG1 screen map and func_02030958 maps the slot index to
 * the entry's palette row. The box for `slot` starts at row 4 + 2n, column 4, with a
 * row pitch of 5 + n rows, where n = 4 - data_0209fc50 (the player count), so the
 * entry offset is slot * (32n + 0xa0) + (64n + 0x84).
 *
 * The border is written as four single entries - the left and right columns of rows 1
 * and 2 - followed by a 24-wide sweep over rows 0 and 3, which also covers the corners.
 * Each write replaces the 4-bit palette field (bits 12..15) of the map entry and leaves
 * the tile number and flip bits alone. `dim` selects the palette four rows earlier,
 * which is how an inactive slot is drawn.
 *
 * The palette is a u16 and the field is spelled `(u16)(pal << 12)`: mwccarm narrows
 * that as `lsl #0x1c` in place on the palette's own register, then `lsr #16` folded
 * into each `orr`, with the loop's copy hoisted - exactly the ROM's register use.
 * Spelling the shift through separate named temporaries gives the same schedule but
 * a rotated register assignment.
 */

extern "C" {
void *_ZN2G212GetBG1ScrPtrEv();
int func_02030958(int val);
}
extern u8 data_0209fc50;

static inline void SetPalette(u16 *entry, u16 pal) {
    *entry = (*entry & 0xfff) | (u16)(pal << 12);
}

extern "C" void func_ov075_02116128(int unused, int slot, int dim) {
    int players = data_0209fc50;
    u16 *map = (u16 *)_ZN2G212GetBG1ScrPtrEv();
    u16 pal = func_02030958(slot) + 10;
    u16 *p;
    int i;
    int n = 4 - players;
    int offset = slot * (n * 0x20 + 0xa0) + (n * 0x40 + 0x84);

    p = map + offset;
    if (dim)
        pal -= 4;

    SetPalette(&p[0x20], pal);
    SetPalette(&p[0x40], pal);
    SetPalette(&p[0x37], pal);
    SetPalette(&p[0x57], pal);
    for (i = 0; i < 24; i++) {
        SetPalette(&p[0], pal);
        SetPalette(&p[0x60], pal);
        p++;
    }
}

// @symbol func_ov075_0211621c
/* recovered: dScEntry_c draws the per-player rows of the VS/wireless entry menu.
 *
 * One row per joined player (data_0209fc50 of them). With d = 4 - players, the rows
 * start at x = data_ov075_0211c6e8[d] and step by 16d + 0x38, which is how the layout
 * spreads two players wider apart than four.
 *
 * Each row emits three things through OAM::Render: the player icon at y = 0x98, the
 * character-index sprite at x + 0x1c, and a three-digit score at y = 0xb0. The score is
 * the shared base at data_0209ee90+0x1d4 plus this player's delta from data_0209f358,
 * split by / 100, / 10 and % 10. Leading zeros are suppressed and the pen advances by 8
 * for a drawn digit and 4 for a skipped one, so the number stays centred. Where a call
 * returns an OAM attribute block, the low ten bits of the second word are the tile
 * index, and the digit or character index is ADDED to it rather than stored - that is
 * the `(val & ~0x3ff) | ((n + (val << 22 >> 22)) & 0x3ff)` in each block.
 *
 * Two spellings carry the register assignment and are load-bearing:
 *   - `rem` (the n % 100 remainder) is declared INSIDE the digit block, after `dx`.
 *     Giving that web a name and that rank is what hands the strength-reduced row
 *     offset r7 and the remainder r8; with the remainder anonymous the two swap, and
 *     with `rem` declared in any outer scope it lands in sb instead.
 *   - in the LAST digit block only, the attribute field is added as
 *     `(val << 22 >> 22) + digit`, not `digit + (val << 22 >> 22)`. The three earlier
 *     blocks read their digit from a value that is already in hand, so either order
 *     colours the same; here the digit is divided out at the point of use and the
 *     operand order decides which of r0/r1 holds the shifted attribute word.
 */
extern "C" {
extern u8 data_ov075_0211c6e8[];
extern char data_ov075_0211c7c0[];
extern char data_ov075_0211c8b0[];
extern char data_ov075_0211c870[];
extern s8 data_0209f310[];
extern s16 data_0209f358[];
extern char data_0209ee90[];
// local extern: OAM::Render takes Fix12<int> by value (measured wall 6az).
extern OamAttr *_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int a, OamAttr *attr, int x, int y, int pal,
                                                        int prio, int scale, int rot);
}

static inline int EntryPlayerIndex(int arg0) {
    return func_02030958(arg0);
}

extern "C" void func_ov075_0211621c(char *c) {
    int count = data_0209fc50;
    int stride;
    int tens;
    int hundreds;
    int i = 0;
    int d = 4 - count;
    int xbase;
    (void)c;
    xbase = data_ov075_0211c6e8[d];
    stride = (d * 16) + 0x38;
    {
        if (count > 0) {
            int minus1 = -1;
            int zA;
            int yIcon;
            int zC;
            int zB;
            int off14;
            int zD;
            int zE;
            int zF;
            int scale;
            int yNum;
            zE = i;
            zD = i;
            zB = i;
            off14 = 0x14;
            zF = i;
            zA = i;
            yIcon = 0x98;
            zC = i;
            scale = 0x1000;
            yNum = 0xb0;
            do {
                int *p;
                int pid = EntryPlayerIndex(i);
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(zA, data_ov075_0211c7c0,
                                                            xbase + i * stride, yIcon, minus1,
                                                            minus1, scale, scale, zA, minus1);
                p = (int *)_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                    zC, (OamAttr *)data_ov075_0211c8b0, xbase + 0x1c + i * stride, yIcon, minus1,
                    minus1, scale, zC);
                if (p != 0) {
                    int *q = (int *)((int)(p + 1));
                    int val = *q;
                    *q = (val & (~0x3ff)) |
                         ((data_0209f310[pid] + (((u32)(val << 22)) >> 22)) & 0x3ff);
                }
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(zB, data_ov075_0211c870,
                                                            xbase + i * stride, yNum, minus1,
                                                            minus1, scale, scale, zB, minus1);
                {
                    int n;
                    int dx = off14;
                    int rem;
                    n = (*((int *)(data_0209ee90 + 0x1d4))) + data_0209f358[pid];
                    hundreds = n / 100;
                    rem = n % 100;
                    tens = rem / 10;
                    if (hundreds != 0) {
                        p = (int *)_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                            zD, (OamAttr *)data_ov075_0211c8b0, xbase + 0x14 + i * stride, yNum,
                            minus1, minus1, scale, zD);
                        if (p != 0) {
                            int *q = (int *)((int)(p + 1));
                            int val = *q;
                            *q = (val & (~0x3ff)) |
                                 ((hundreds + (((u32)(val << 22)) >> 22)) & 0x3ff);
                        }
                        dx = dx + 8;
                    } else {
                        dx = dx + 4;
                    }
                    if ((tens != 0) || (hundreds != 0)) {
                        p = (int *)_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                            zE, (OamAttr *)data_ov075_0211c8b0, xbase + dx + i * stride, yNum,
                            minus1, minus1, scale, zE);
                        if (p != 0) {
                            int *q = (int *)((int)(p + 1));
                            int val = *q;
                            *q = (val & (~0x3ff)) | ((tens + (((u32)(val << 22)) >> 22)) & 0x3ff);
                        }
                        dx = dx + 8;
                    } else {
                        dx = dx + 4;
                    }
                    p = (int *)_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                        zF, (OamAttr *)data_ov075_0211c8b0, xbase + dx + i * stride, yNum, minus1,
                        minus1, scale, zF);
                    if (p != 0) {
                        int *q = (int *)((int)(p + 1));
                        int val = *q;
                        *q = (val & (~0x3ff)) | (((((u32)(val << 22)) >> 22) + (rem % 10)) & 0x3ff);
                    }
                }
                i++;
            } while (i < count);
        }
    }
}
