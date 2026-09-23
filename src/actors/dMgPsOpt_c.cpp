//cpp
/* ov004/dMgPsOpt_c: the minigame pause and options touch panel.
 *
 * dScMgBase_c embeds one at 0xf4 as mTouchOptions. It is a strip of eight
 * TouchIcon_c buttons (dThIcon_c with its own Render) driven by mActive:
 * 1 while taking input, 2 while the close delay runs down, 0 when shut.
 * Both class names are the ROM's own, from the nested type's RTTI string
 * "N10dMgPsOpt_c11TouchIcon_cE".
 *
 * The six func_ov004_ functions are this class's methods (each takes the
 * panel or an icon as its first argument), but nothing in the ROM names
 * them, so they keep their linker names and take the object as a raw
 * pointer. The icon fields are dThIcon_c's unk_ placeholders until that
 * header names them.
 *
 * Functions run in REVERSE of ROM order (highest address first); do not
 * reorder.
 */
#include "types.h"
#include "dMgPsOpt_c.h"

/* Only ever passed through to the shared blitter. */
struct M;

#define SH(base,i) (*(short*)((char*)(base) + (i)*4))

extern "C" {
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
}

namespace G2S { u16 *GetBG1ScrPtr(); }

// @symbol _ZN10dMgPsOpt_c11TouchIcon_cC1Ev
dMgPsOpt_c::TouchIcon_c::TouchIcon_c()
{
}

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

// @symbol _ZN10dMgPsOpt_cD1Ev
dMgPsOpt_c::~dMgPsOpt_c()
{
}

// @symbol _ZN10dMgPsOpt_c11TouchIcon_cD1Ev
dMgPsOpt_c::TouchIcon_c::~TouchIcon_c()
{
}

// @symbol func_ov004_020b9220
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov004_020b9220(char *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;

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

// @symbol func_ov004_020b91fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov004_020b91fc(char *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;

    data_0209d454 &= ~2;
    self->mSelectedIcon = 0;
    self->mActive = 0;
}
}

// @symbol func_ov004_020b8f78
extern "C" u8 func_ov004_020b8f78(char *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;
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

// @symbol func_ov004_020b8f18
int func_ov004_020b8f18(void *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;
    int i;

    if (self->mActive == 0)
        return 0;

    for (i = 0; i < self->mSelectedIcon; i++) {
        self->mIcons[i].Render();
    }

    return 1;
}

// @symbol func_ov004_020b8ee0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov004_020b8ee0(char *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;
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

// @symbol func_ov004_020b8dc0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov004_020b8dc0(char *raw, int style, int palette, short x, short y)
{
    dMgPsOpt_c::TouchIcon_c *icon = (dMgPsOpt_c::TouchIcon_c *)raw;
    int kind;
    int lang, lang2;

    icon->unk_01c = style;
    kind = (style < 6) ? 2 : 1;
    icon->unk_020 = palette;

    /* Styles 4 and 5 are the two captioned buttons, so their hit box comes out
       of a per-language table instead of the flat one the rest share. */
    if (style == 4) {
        lang = GetGameLanguage();
        lang2 = GetGameLanguage();
        func_ov001_020ab5b0(raw, kind, x, y,
                            SH(data_ov004_020bfe74, lang), SH(data_ov004_020bfe76, lang2));
    } else if (style == 5) {
        lang = GetGameLanguage();
        lang2 = GetGameLanguage();
        func_ov001_020ab5b0(raw, kind, x, y,
                            SH(data_ov004_020bfe88, lang), SH(data_ov004_020bfe8a, lang2));
    } else {
        func_ov001_020ab5b0(raw, kind, x, y,
                            SH(data_ov004_020bfe9c, style), SH(data_ov004_020bfe9e, style));
    }
}
}

// @symbol _ZN10dMgPsOpt_c11TouchIcon_c6RenderEv
/* Slot 1. Styles 2 and 3 hand a sprite table to the shared blitter and
   nudge x two pixels while the icon is held (unk_010); styles 0, 1 and 6
   recolour a rectangle of BG1 screen entries in place, style 0 first laying
   down a fresh 13-entry run. */
void dMgPsOpt_c::TouchIcon_c::Render()
{
    volatile s16 xy[2];
    void *sprites;
    u16 *scr;
    u16 tile;
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
        sprites = data_ov004_020bca58;
        break;

    case 3:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] + 2);
        sprites = data_ov004_020bca60;
        break;

    case 0:
        scr = (u16 *)((char *)G2S::GetBG1ScrPtr() + 0x19e);
        tile = (u16)((unk_020 << 6) + 0x13);
        for (i = 0; i < 13; i++) {
            scr[0] = tile;
            scr[0x20] = (u16)(tile + 0x20);
            scr++;
            tile++;
        }
        /* fallthrough */
    case 1:
    case 6:
        mask = (unk_010 != 0) ? 0 : 0x3000;
        w = unk_008;
        scr = G2S::GetBG1ScrPtr();
        x = unk_004;
        h = unk_00a;
        x -= w;
        x >>= 3;
        scr += x;
        y = unk_006;
        y -= h;
        scr += (y >> 3) << 5;
        for (row = 0; row < unk_00a >> 2; row++) {
            for (col = 0; col < w >> 2; col++) {
                scr[col] = (u16)(mask | (scr[col] & 0xfff));
                w = unk_008;
            }
            scr += 0x20;
        }
        return;

    default:
        return;
    }

    func_ov004_020aea78(sprites, xy[0], xy[1], 0);
}
