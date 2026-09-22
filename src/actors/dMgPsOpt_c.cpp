//cpp
/* The minigame pause/options touch panel -- ov004/dMgPsOpt_c.
 *
 * dMgPsOpt_c is the eight-button options strip dScMgBase_c embeds at self+0xf4
 * as `dMgPsOpt_c mTouchOptions`. Each button is a dMgPsOpt_c::TouchIcon_c, a
 * dThIcon_c with its own Render override, and the panel drives all eight from a
 * small state byte: 1 while it is taking input, 2 while the close delay runs
 * down, 0 when it is shut.
 *
 * CLASS IDENTITY. The outer class carries no type-info of its own, so a probe
 * for an unnested type-string finds nothing. The nested type's record is the
 * cartridge's proof: ov004:0x020bca84 holds the 28-byte string
 * "N10dMgPsOpt_c11TouchIcon_cE", whose Itanium length prefixes decode to
 * dMgPsOpt_c::TouchIcon_c. Both names are the cartridge's own, not coined. The
 * type-info record at 0x020bca68 names dThIcon_c as the single public base in
 * its third word (0x020ad478, ov001), and the two-slot table at 0x020bca74
 * keeps dThIcon_c::Behavior in slot 0 while slot 1 is this class's own Render.
 * Two slots and no destructor slot is what makes TouchIcon_c's destructor
 * nonvirtual.
 *
 * TU BOUNDARY. One contiguous run, 0x020b8c18..0x020b944c, eleven functions,
 * closing out ov004's .text. Only five carried a class label; the six
 * func_ov004_* members are unlabelled simply because nothing in the cartridge
 * names them. Each takes a dMgPsOpt_c or a TouchIcon_c as its first argument,
 * and a referrer census over every relocs.txt under config/ finds every
 * reference from outside the run coming from dScMgBase_c passing self+0xf4.
 * The two labelled members the cartridge does name are called from outside the
 * run too, so an external caller does not argue a function out of this TU --
 * the test that discriminates is the `this` pointer, and all eleven pass it.
 *
 * Those six keep their ROM symbol names and the parameter spelling the rest of
 * the tree already declares for them, so they stay `extern "C"` and take the
 * panel as a raw pointer; each takes a typed local on its first line instead.
 *
 * EMISSION ORDER IS THE REVERSE OF THE ROM'S. mwccarm 2004/b56 emits one .text
 * section per function in the reverse of source order, so the highest-address
 * ROM function is written first here. Do not reorder.
 */
#include "types.h"
#include "dMgPsOpt_c.h"

/* Opaque to this TU: only ever passed through to the shared blitter. */
struct M;

#define SH(base,i) (*(short*)((char*)(base) + (i)*4))

extern "C" {
extern u16 *_ZN3G2S12GetBG1ScrPtrEv(void);
extern void func_ov004_020aea78(void *self, int a1, int a2, struct M *a3);
extern u16 data_ov004_020bca58[];
extern u16 data_ov004_020bca60[];
extern int GetGameLanguage(void);
extern void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);
extern short data_ov004_020bfe74[];
extern short data_ov004_020bfe76[];
extern short data_ov004_020bfe88[];
extern short data_ov004_020bfe8a[];
extern short data_ov004_020bfe9c[];
extern short data_ov004_020bfe9e[];
extern "C" int func_ov004_020b8f18(void *p);
int TouchArea_Update(void *p, int f);
void func_ov001_020ab3f0(void *p);
void func_02012790(int a);
int GetSoundMode(void);
void SetSoundMode(int mode);
void func_ov001_020ab41c(void *c);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
u8 DecIfAbove0_Byte(u8 *p);
void func_ov004_020b91fc(char *p);
extern unsigned char data_0209d454;
extern unsigned char data_0208ee3c;
void func_ov004_020b8dc0(char *p, int style, int palette, short x, short y);
extern s16 data_ov004_020bfe74[];
extern s16 data_ov004_020bfe88[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN10dMgPsOpt_c11TouchIcon_cC1Ev, 0x020b9430, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dMgPsOpt_c11TouchIcon_cC1Ev
dMgPsOpt_c::TouchIcon_c::TouchIcon_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN10dMgPsOpt_cC1Ev, 0x020b92c4, size 0x16c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dMgPsOpt_cC1Ev
dMgPsOpt_c::dMgPsOpt_c()
{
    func_ov004_020b8dc0((char *)&mIcons[2], 0, GetSoundMode(), 0xac, 0x38);
    func_ov004_020b8dc0((char *)&mIcons[0], 2, 0, 0x64, 0x38);
    func_ov004_020b8dc0((char *)&mIcons[1], 3, 0, 0xf4, 0x38);
    func_ov004_020b8dc0((char *)&mIcons[6], 4, 0,
        data_ov004_020bfe74[GetGameLanguage() * 2] + 0x18, 0x38);
    func_ov004_020b8dc0((char *)&mIcons[3], 1, 0, 0x8c, 0x68);
    func_ov004_020b8dc0((char *)&mIcons[4], 1, 0, 0xcc, 0x68);
    func_ov004_020b8dc0((char *)&mIcons[7], 5, 0,
        data_ov004_020bfe88[GetGameLanguage() * 2] + 0x18, 0x68);
    func_ov004_020b8dc0((char *)&mIcons[5], 6, 0, 0x80, 0xa8);

    mSelectedIcon = 8;
    mSelectedIcon = 0;
    mActive = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN10dMgPsOpt_cD1Ev, 0x020b929c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dMgPsOpt_cD1Ev
dMgPsOpt_c::~dMgPsOpt_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN10dMgPsOpt_c11TouchIcon_cD1Ev, 0x020b9280, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dMgPsOpt_c11TouchIcon_cD1Ev
dMgPsOpt_c::TouchIcon_c::~TouchIcon_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov004_020b9220, 0x020b9220, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov004_020b9220
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov004_020b9220(char *p)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)p;

    self->mIcons[2].unk_010 = 1;
    if (data_0208ee3c != 0) {
        self->mIcons[3].unk_010 = 1;
        self->mIcons[4].unk_010 = 0;
    } else {
        self->mIcons[3].unk_010 = 0;
        self->mIcons[4].unk_010 = 1;
    }
    self->mIcons[5].unk_010 = 0;
    data_0209d454 |= 2;
    self->mSelectedIcon = 8;
    self->mActive = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov004_020b91fc, 0x020b91fc, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov004_020b91fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov004_020b91fc(char *p)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)p;

    data_0209d454 &= ~2;
    self->mSelectedIcon = 0;
    self->mActive = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov004_020b8f78, 0x020b8f78, size 0x284 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov004_020b8f78
extern "C" u8 func_ov004_020b8f78(char *p)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)p;
    u8 state = self->mActive;

    switch (state) {
    case 1:
        if (TouchArea_Update(&self->mIcons[5], -1)) {
            /* The back button: start the close delay and let state 2 run it down. */
            func_ov001_020ab3f0(&self->mIcons[5]);
            self->mCloseTimer = 0x14;
            self->mActive = 2;
            func_02012790(0x65);
        } else {
            if (self->mIcons[2].unk_011 != 0) {
                /* Sound mode: icons 1, 2 and 6 step forward, icon 0 steps back. */
                int changed = 0;
                int mode = GetSoundMode();

                if (TouchArea_Update(&self->mIcons[2], -1)
                    || TouchArea_Update(&self->mIcons[1], -1)
                    || TouchArea_Update(&self->mIcons[6], -1)) {
                    if (mode == 2) mode = 0;
                    else mode = (mode + 1) & 0xff;
                    self->mIcons[1].unk_010 = 0;
                    func_ov001_020ab3f0(&self->mIcons[1]);
                    changed = 1;
                } else if (TouchArea_Update(&self->mIcons[0], -1)) {
                    if (mode == 0) mode = 2;
                    else mode = (mode - 1) & 0xff;
                    self->mIcons[0].unk_010 = 0;
                    func_ov001_020ab3f0(&self->mIcons[0]);
                    changed = 1;
                }

                if (changed != 0) {
                    SetSoundMode(mode);
                    self->mIcons[2].unk_020 = mode;
                    func_ov001_020ab41c(&self->mIcons[2]);
                    func_02012790(0x64);
                }
            }

            if (self->mIcons[3].unk_011 != 0 && self->mIcons[4].unk_011 != 0) {
                /* Backlight: icon 3 turns it on, icon 4 off, icon 7 toggles. */
                int dir = 0;

                if (TouchArea_Update(&self->mIcons[3], -1)) {
                    dir = -1;
                } else if (TouchArea_Update(&self->mIcons[4], -1)) {
                    dir = 1;
                } else if (TouchArea_Update(&self->mIcons[7], -1)) {
                    dir = (self->mIcons[3].unk_010 != 0) ? 1 : -1;
                }

                if (dir < 0) {
                    if (self->mIcons[3].unk_010 != 0) {
                        func_02012790(0x67);
                    } else {
                        TurnBacklightOn();
                        func_02012790(0x66);
                    }
                    func_ov001_020ab41c(&self->mIcons[3]);
                    self->mIcons[4].unk_010 = 0;
                } else if (dir > 0) {
                    if (self->mIcons[4].unk_010 != 0) {
                        func_02012790(0x67);
                    } else {
                        TurnBacklightOff();
                        func_02012790(0x66);
                    }
                    self->mIcons[3].unk_010 = 0;
                    func_ov001_020ab41c(&self->mIcons[4]);
                }
            }
        }
        break;

    case 2:
        if (DecIfAbove0_Byte(&self->mCloseTimer) == 0)
            func_ov004_020b91fc((char *)self);
        break;
    }

    {
        int count = self->mSelectedIcon;
        int i = 0;

        if (count > 0) {
            dMgPsOpt_c::TouchIcon_c *icon = self->mIcons;
            do {
                icon->Behavior();
                i++;
                icon++;
            } while (i < self->mSelectedIcon);
        }
    }

    return self->mActive;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov004_020b8f18, 0x020b8f18, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov004_020b8f18
int func_ov004_020b8f18(void *p)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)p;
    int i;

    if (self->mActive == 0)
        return 0;

    for (i = 0; i < self->mSelectedIcon; i++) {
        self->mIcons[i].Render();
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov004_020b8ee0, 0x020b8ee0, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov004_020b8ee0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov004_020b8ee0(char *p)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)p;
    dMgPsOpt_c::TouchIcon_c *icon = self->mIcons;
    int i;

    for (i = 0; i < self->mSelectedIcon; i++) {
        if (icon->unk_00c != 0)
            return 0;
        icon++;
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- func_ov004_020b8dc0, 0x020b8dc0, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov004_020b8dc0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov004_020b8dc0(char *p, int style, int palette, short x, short y)
{
    dMgPsOpt_c::TouchIcon_c *icon = (dMgPsOpt_c::TouchIcon_c *)p;
    int kind;
    int i, j;

    icon->unk_01c = style;
    kind = (style < 6) ? 2 : 1;
    icon->unk_020 = palette;

    /* Styles 4 and 5 are the two captioned buttons, so their hit box comes out
       of a per-language table instead of the flat one the rest share. */
    if (style == 4) {
        i = GetGameLanguage();
        j = GetGameLanguage();
        func_ov001_020ab5b0(p, kind, x, y,
                            SH(data_ov004_020bfe74, i), SH(data_ov004_020bfe76, j));
    } else if (style == 5) {
        i = GetGameLanguage();
        j = GetGameLanguage();
        func_ov001_020ab5b0(p, kind, x, y,
                            SH(data_ov004_020bfe88, i), SH(data_ov004_020bfe8a, j));
    } else {
        func_ov001_020ab5b0(p, kind, x, y,
                            SH(data_ov004_020bfe9c, style), SH(data_ov004_020bfe9e, style));
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN10dMgPsOpt_c11TouchIcon_c6RenderEv, 0x020b8c18, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dMgPsOpt_c11TouchIcon_c6RenderEv
/* dMgPsOpt_c::TouchIcon_c::Render -- slot 1 of the nested touch icon's vtable.

   The icon draws itself one of two ways depending on unk_01c, the style word
   dScMgBase_c's menu code writes when it builds the eight icons: styles 2 and 3
   hand a sprite table to the shared blitter and nudge x by two pixels while the
   icon is held down (unk_010), and styles 0, 1 and 6 recolour a rectangle of BG1
   screen entries in place, style 0 first laying down a fresh 13-entry run.

   This is the class's own Render override: the cartridge's vtable holds
   0x020b8c18 in slot 1, where dThIcon_c's own table holds
   _ZN9dThIcon_c6RenderEv. Until the override was declared in
   include/dMgPsOpt_c.h, mwcc emitted the inherited address into that word and
   romdata_check scored _ZTVN10dMgPsOpt_c11TouchIcon_cE DIFFERS; declaring it
   and giving this body its mangled name is what closes the slot, and the
   two-slot table is byte-exact against the cartridge now.

   The seven fields it touches are dThIcon_c's own, so the raw `char *self`
   offsets the C source used are gone; nothing else about the body changed. */
void dMgPsOpt_c::TouchIcon_c::Render()
{
    volatile s16 xy[2];
    void *tbl;
    u16 *p;
    u16 v;
    int i;
    int w;
    int mask;
    int row;
    int col;
    int y;
    int h;
    int x;

    xy[0] = unk_004;
    xy[1] = unk_006;

    switch (unk_01c) {
    case 2:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] - 2);
        tbl = data_ov004_020bca58;
        break;

    case 3:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] + 2);
        tbl = data_ov004_020bca60;
        break;

    case 0:
        p = (u16 *)((char *)_ZN3G2S12GetBG1ScrPtrEv() + 0x19e);
        v = (u16)((unk_020 << 6) + 0x13);
        for (i = 0; i < 13; i++) {
            p[0] = v;
            p[0x20] = (u16)(v + 0x20);
            p++;
            v++;
        }
        /* fallthrough */
    case 1:
    case 6:
        mask = (unk_010 != 0) ? 0 : 0x3000;
        w = unk_008;
        p = _ZN3G2S12GetBG1ScrPtrEv();
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

    default:
        return;
    }

    func_ov004_020aea78(tbl, xy[0], xy[1], 0);
}
