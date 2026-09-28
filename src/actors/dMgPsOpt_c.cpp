//cpp
/* dMgPsOpt_c -- the minigame pause and options touch panel, ov004.
 *
 * Eight buttons on the touch screen. Top row is the sound mode (previous,
 * the mode graphic, next, and a language-sized caption). Middle row is the
 * backlight (on, off, and its caption). The bottom button closes the panel.
 * mActive is 1 while it takes touches, 2 while the close delay runs, 0 when
 * shut. dScMgBase_c owns the one instance, mTouchOptions.
 *
 * Source order is the reverse of the ROM's. mwccarm emits .text backwards.
 * Do not reorder.
 *
 * deslop leftovers:
 * - func_ov004_020b8f78: Sound::Play2D(2, id) grows it 0x284 to 0x298 and
 *   retargets the calls to _ZN5Sound6Play2DEjj. The ROM calls func_02012790,
 *   whose body is that Play2D.
 * - func_ov004_020b8dc0: one GetGameLanguage() for both half extents shrinks
 *   it 0x120 to 0x108. Each caption calls it twice.
 * - TouchIcon_c::Render: a plain s16 xy[2] shrinks it 0x1a8 to 0x198, so the
 *   pair stays volatile. Dropping the mHalfW reload in the column loop
 *   shrinks it 0x1a8 to 0x1a4.
 * - dMgPsOpt_c(): dropping the store of 8 before the store of 0 shrinks it
 *   0x16c to 0x164.
 */

#include "types.h"
#include "dMgPsOpt_c.h"

/* The blitter's 2x2 matrix. This panel always passes none. */
struct M;

/* {halfW, halfH} records laid down by __sinit_ov004_020b9b24, stride 4.
   Width and height are two linker symbols two bytes apart, and the ROM
   loads each base on its own. */
#define HALF_AT(base, i) (*(short *)((char *)(base) + (i) * 4))

/* dThIcon_c leaves these unk_. The other derived icons do not agree on
   names, so the names stay in this file.
     unk_004 mX, unk_006 mY          center, pixels
     unk_008 mHalfW, unk_00a mHalfH  hit box and the BG highlight
     unk_00c mBlink                  countdown; 0 means settled
     unk_010 mOn                     lit / pressed
     unk_011 mReady                  accepts a touch
     unk_01c mStyle                  which graphic
     unk_020 mVariant                sound mode, or 0 */
#define mX unk_004
#define mY unk_006
#define mHalfW unk_008
#define mHalfH unk_00a
#define mBlink unk_00c
#define mOn unk_010
#define mReady unk_011
#define mStyle unk_01c
#define mVariant unk_020

enum {
    kIconSoundPrev = 0,
    kIconSoundNext = 1,
    kIconSoundMode = 2,
    kIconLightOn = 3,
    kIconLightOff = 4,
    kIconBack = 5,
    kIconSoundLabel = 6,
    kIconLightLabel = 7,
    kIconCount = 8
};

enum {
    kStyleSoundMode = 0,
    kStyleLight = 1,
    kStyleSoundPrev = 2,
    kStyleSoundNext = 3,
    kStyleSoundLabel = 4,
    kStyleLightLabel = 5,
    kStyleBack = 6
};

enum {
    kPanelShut = 0,
    kPanelOpen = 1,
    kPanelClosing = 2
};

/* 0x64 mode changed, 0x65 back, 0x66 backlight changed, 0x67 rejected. */
enum {
    kSndMode = 0x64,
    kSndBack = 0x65,
    kSndLight = 0x66,
    kSndReject = 0x67
};

/* Sub-screen BG enable shadow. Value 2 is this panel. */
enum { kSubBgBit = 2 };

extern "C" {
extern void func_ov004_020aea78(void *sprite, int x, int y, struct M *matrix);
/* 8-byte OAM records. func_ov004_020aea78 copies one and draws it. */
extern u16 data_ov004_020bca58[];
extern u16 data_ov004_020bca60[];
extern int GetGameLanguage(void);
/* Writes the icon center, half extents, touch kind, and clears the blink. */
extern void func_ov001_020ab5b0(char *icon, int kind, short x, short y, short halfW, short halfH);
extern short data_ov004_020bfe74[];
extern short data_ov004_020bfe76[];
extern short data_ov004_020bfe88[];
extern short data_ov004_020bfe8a[];
extern short data_ov004_020bfe9c[];
extern short data_ov004_020bfe9e[];
extern "C" int func_ov004_020b8f18(void *panel);
int TouchArea_Update(void *icon, int touch);
/* Toggles mOn, drops mReady, and reloads mBlink from the icon's +0x18. */
void func_ov001_020ab3f0(void *icon);
/* Sound::Play2D(2, id). The ROM calls this thunk, not Play2D. */
void func_02012790(int id);
int GetSoundMode(void);
void SetSoundMode(int mode);
/* If mOn is clear, sets it. Otherwise the same pulse as func_ov001_020ab3f0. */
void func_ov001_020ab41c(void *icon);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
u8 DecIfAbove0_Byte(u8 *p);
void func_ov004_020b91fc(char *panel);
/* Sub BG enable bits. dScMgBase_c saves them in mSavedSubBgBits. */
extern unsigned char data_0209d454;
/* 1 while the backlight is on. TurnBacklightOn / Off write it. */
extern unsigned char data_0208ee3c;
void func_ov004_020b8dc0(dMgPsOpt_c::TouchIcon_c *icon, int style, int variant, short x, short y);
}

namespace G2S { u16 *GetBG1ScrPtr(); }

// @symbol _ZN10dMgPsOpt_c11TouchIcon_cC1Ev
dMgPsOpt_c::TouchIcon_c::TouchIcon_c()
{
}

// @symbol _ZN10dMgPsOpt_cC1Ev
dMgPsOpt_c::dMgPsOpt_c()
{
    func_ov004_020b8dc0(&mIcons[kIconSoundMode], kStyleSoundMode, GetSoundMode(), 0xac, 0x38);
    func_ov004_020b8dc0(&mIcons[kIconSoundPrev], kStyleSoundPrev, 0, 0x64, 0x38);
    func_ov004_020b8dc0(&mIcons[kIconSoundNext], kStyleSoundNext, 0, 0xf4, 0x38);
    func_ov004_020b8dc0(&mIcons[kIconSoundLabel], kStyleSoundLabel, 0,
        data_ov004_020bfe74[GetGameLanguage() * 2] + 0x18, 0x38);
    func_ov004_020b8dc0(&mIcons[kIconLightOn], kStyleLight, 0, 0x8c, 0x68);
    func_ov004_020b8dc0(&mIcons[kIconLightOff], kStyleLight, 0, 0xcc, 0x68);
    func_ov004_020b8dc0(&mIcons[kIconLightLabel], kStyleLightLabel, 0,
        data_ov004_020bfe88[GetGameLanguage() * 2] + 0x18, 0x68);
    func_ov004_020b8dc0(&mIcons[kIconBack], kStyleBack, 0, 0x80, 0xa8);

    /* The ROM stores the full count and then clears it. Both stores stay. */
    mIconCount = kIconCount;
    mIconCount = 0;
    mActive = kPanelShut;
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
/* Open. Lights the current sound and backlight choices and sets sub BG
   value 2. Called from the pause menu (func_ov004_020aeb24). */
extern "C" void func_ov004_020b9220(char *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;

    self->mIcons[kIconSoundMode].mOn = 1;
    if (data_0208ee3c != 0) {
        self->mIcons[kIconLightOn].mOn = 1;
        self->mIcons[kIconLightOff].mOn = 0;
    } else {
        self->mIcons[kIconLightOn].mOn = 0;
        self->mIcons[kIconLightOff].mOn = 1;
    }
    self->mIcons[kIconBack].mOn = 0;
    data_0209d454 |= kSubBgBit;
    self->mIconCount = kIconCount;
    self->mActive = kPanelOpen;
}

// @symbol func_ov004_020b91fc
/* Shut. Clears sub BG value 2. The close delay calls this, and so does
   dScMgBase_c::OnHitFromUnderneath. */
extern "C" void func_ov004_020b91fc(char *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;

    data_0209d454 &= ~kSubBgBit;
    self->mIconCount = 0;
    self->mActive = kPanelShut;
}

// @symbol func_ov004_020b8f78
/* One frame of input. Returns mActive; the pause menu
   (func_ov004_020aeb24) yields the frame while that is nonzero. */
extern "C" u8 func_ov004_020b8f78(char *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;
    u8 state = self->mActive;

    switch (state) {
    case kPanelOpen:
        if (TouchArea_Update(&self->mIcons[kIconBack], -1)) {
            /* Back: pulse the button, then let state 2 run the delay down. */
            func_ov001_020ab3f0(&self->mIcons[kIconBack]);
            self->mCloseTimer = 0x14;
            self->mActive = kPanelClosing;
            func_02012790(kSndBack);
        } else {
            if (self->mIcons[kIconSoundMode].mReady != 0) {
                /* Sound mode wraps 0, 1, 2. Next, the mode graphic, and the
                   caption step forward; previous steps back. */
                int changed = 0;
                int mode = GetSoundMode();

                if (TouchArea_Update(&self->mIcons[kIconSoundMode], -1)
                    || TouchArea_Update(&self->mIcons[kIconSoundNext], -1)
                    || TouchArea_Update(&self->mIcons[kIconSoundLabel], -1)) {
                    if (mode == 2) mode = 0;
                    else mode = (mode + 1) & 0xff;
                    self->mIcons[kIconSoundNext].mOn = 0;
                    func_ov001_020ab3f0(&self->mIcons[kIconSoundNext]);
                    changed = 1;
                } else if (TouchArea_Update(&self->mIcons[kIconSoundPrev], -1)) {
                    if (mode == 0) mode = 2;
                    else mode = (mode - 1) & 0xff;
                    self->mIcons[kIconSoundPrev].mOn = 0;
                    func_ov001_020ab3f0(&self->mIcons[kIconSoundPrev]);
                    changed = 1;
                }

                if (changed != 0) {
                    SetSoundMode(mode);
                    self->mIcons[kIconSoundMode].mVariant = mode;
                    func_ov001_020ab41c(&self->mIcons[kIconSoundMode]);
                    func_02012790(kSndMode);
                }
            }

            if (self->mIcons[kIconLightOn].mReady != 0
                && self->mIcons[kIconLightOff].mReady != 0) {
                /* On turns the backlight on, off turns it off, the caption toggles. */
                int dir = 0;

                if (TouchArea_Update(&self->mIcons[kIconLightOn], -1)) {
                    dir = -1;
                } else if (TouchArea_Update(&self->mIcons[kIconLightOff], -1)) {
                    dir = 1;
                } else if (TouchArea_Update(&self->mIcons[kIconLightLabel], -1)) {
                    dir = (self->mIcons[kIconLightOn].mOn != 0) ? 1 : -1;
                }

                if (dir < 0) {
                    if (self->mIcons[kIconLightOn].mOn != 0) {
                        func_02012790(kSndReject);
                    } else {
                        TurnBacklightOn();
                        func_02012790(kSndLight);
                    }
                    func_ov001_020ab41c(&self->mIcons[kIconLightOn]);
                    self->mIcons[kIconLightOff].mOn = 0;
                } else if (dir > 0) {
                    if (self->mIcons[kIconLightOff].mOn != 0) {
                        func_02012790(kSndReject);
                    } else {
                        TurnBacklightOff();
                        func_02012790(kSndLight);
                    }
                    self->mIcons[kIconLightOn].mOn = 0;
                    func_ov001_020ab41c(&self->mIcons[kIconLightOff]);
                }
            }
        }
        break;

    case kPanelClosing:
        if (DecIfAbove0_Byte(&self->mCloseTimer) == 0)
            func_ov004_020b91fc((char *)self);
        break;
    }

    {
        int count = self->mIconCount;
        int i = 0;

        if (count > 0) {
            dMgPsOpt_c::TouchIcon_c *icon = self->mIcons;
            do {
                icon->Behavior();
                i++;
                icon++;
            } while (i < self->mIconCount);
        }
    }

    return self->mActive;
}

// @symbol func_ov004_020b8f18
/* Draw every live icon. Returns 1 while the panel is up, which makes
   func_ov004_020ae858 skip the rest of the touch-screen UI. */
int func_ov004_020b8f18(void *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;
    int i;

    if (self->mActive == kPanelShut)
        return 0;

    for (i = 0; i < self->mIconCount; i++) {
        self->mIcons[i].Render();
    }

    return 1;
}

// @symbol func_ov004_020b8ee0
/* 1 when no live icon is still counting down a press. BeforeBehavior uses
   that as the gate before OnHitFromUnderneath closes the panel. */
extern "C" int func_ov004_020b8ee0(char *raw)
{
    dMgPsOpt_c *self = (dMgPsOpt_c *)raw;
    dMgPsOpt_c::TouchIcon_c *icon = self->mIcons;
    int i;

    for (i = 0; i < self->mIconCount; i++) {
        if (icon->mBlink != 0)
            return 0;
        icon++;
    }

    return 1;
}

// @symbol func_ov004_020b8dc0
/* Place one icon. Styles below the back button are an edge-triggered hit
   (kind 2); the back button is a held hit (kind 1). TouchArea_Update is
   what reads that kind. The two captions take their half extents from the
   per-language records; every other style shares the flat table. */
extern "C" void func_ov004_020b8dc0(dMgPsOpt_c::TouchIcon_c *icon, int style, int variant,
                                    short x, short y)
{
    int kind;
    int lang, lang2;

    icon->mStyle = style;
    kind = (style < kStyleBack) ? 2 : 1;
    icon->mVariant = variant;

    if (style == kStyleSoundLabel) {
        lang = GetGameLanguage();
        lang2 = GetGameLanguage();
        func_ov001_020ab5b0((char *)icon, kind, x, y,
                            HALF_AT(data_ov004_020bfe74, lang),
                            HALF_AT(data_ov004_020bfe76, lang2));
    } else if (style == kStyleLightLabel) {
        lang = GetGameLanguage();
        lang2 = GetGameLanguage();
        func_ov001_020ab5b0((char *)icon, kind, x, y,
                            HALF_AT(data_ov004_020bfe88, lang),
                            HALF_AT(data_ov004_020bfe8a, lang2));
    } else {
        func_ov001_020ab5b0((char *)icon, kind, x, y,
                            HALF_AT(data_ov004_020bfe9c, style),
                            HALF_AT(data_ov004_020bfe9e, style));
    }
}

// @symbol _ZN10dMgPsOpt_c11TouchIcon_c6RenderEv
/* Styles 2 and 3 draw an OAM arrow and nudge it two pixels while held.
   Styles 0, 1 and 6 repaint BG1 screen entries: style 0 first stamps a
   fresh 13-wide run for the current sound mode, then all three of them
   set or clear the palette bits over the icon's rectangle. The captions
   (styles 4 and 5) draw nothing of their own. */
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

    xy[0] = mX;
    xy[1] = mY;

    switch (mStyle) {
    case kStyleSoundPrev:
        if (mOn != 0)
            xy[0] = (s16)(xy[0] - 2);
        sprites = data_ov004_020bca58;
        break;

    case kStyleSoundNext:
        if (mOn != 0)
            xy[0] = (s16)(xy[0] + 2);
        sprites = data_ov004_020bca60;
        break;

    case kStyleSoundMode:
        scr = (u16 *)((char *)G2S::GetBG1ScrPtr() + 0x19e);
        tile = (u16)((mVariant << 6) + 0x13);
        for (i = 0; i < 13; i++) {
            scr[0] = tile;
            scr[0x20] = (u16)(tile + 0x20);
            scr++;
            tile++;
        }
        /* fallthrough */
    case kStyleLight:
    case kStyleBack:
        /* Lit keeps palette 0. Dim sets screen-entry bits 0x3000. */
        mask = (mOn != 0) ? 0 : 0x3000;
        w = mHalfW;
        scr = G2S::GetBG1ScrPtr();
        x = mX;
        h = mHalfH;
        x -= w;
        x >>= 3;
        scr += x;
        y = mY;
        y -= h;
        scr += (y >> 3) << 5;
        for (row = 0; row < mHalfH >> 2; row++) {
            for (col = 0; col < w >> 2; col++) {
                scr[col] = (u16)(mask | (scr[col] & 0xfff));
                w = mHalfW;
            }
            scr += 0x20;
        }
        return;

    default:
        return;
    }

    func_ov004_020aea78(sprites, xy[0], xy[1], 0);
}
