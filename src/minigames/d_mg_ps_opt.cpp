//cpp
/* dMgPsOpt_c -- the minigame pause and options touch panel, ov004.
 *
 * Eight buttons on the touch screen. Top row is the sound mode (previous,
 * the mode graphic, next, and a language-sized caption). Middle row is the
 * backlight (on, off, and its caption). The bottom button closes the panel.
 * mActive is 1 while it takes touches, 2 while the close delay runs, 0 when
 * shut. dScMgBase_c owns the one instance, mTouchOptions.
 */
// leftovers: Update calls the func_02012790 thunk, not Sound::Play2D
// (the member call grows it 0x284 to 0x298 and retargets the callees).
// PsOpt_PlaceIcon calls GetGameLanguage() twice per caption (one call shrinks
// it 0x120 to 0x108). TouchIcon_c::Render keeps the volatile s16 xy pair
// and the mHalfW reload (plain spellings shrink it 0x1a8 to 0x198/0x1a4).
// The ctor keeps the store of 8 before the store of 0 (dropping it shrinks
// it 0x16c to 0x164).

#include "types.h"
#include "dMgPsOpt_c.h"
#include "Sound.h"
#include "nitro/gx/gx_bgcnt.h"
#include "math/CalcLib.h"
namespace s20aea78 {
struct S;
struct M;
void Mg_DrawSpriteListEntry(S *, int, int, M *);
}
using s20aea78::Mg_DrawSpriteListEntry;
namespace s20ad674 {
s32 GetGameLanguage(void);
}
using s20ad674::GetGameLanguage;

/* The blitter's 2x2 matrix. This panel always passes none. */
struct M;

/* {halfW, halfH} records, stride 4. The ROM reads the width through the
   array's own symbol and the height through a second linker symbol two
   bytes in (sPsOptIconPosAY and friends), and loads each base on its own.
   The width reads are plain sPsOptIconPosA[i].mHalfW; the height reads
   keep the byte stride: sPsOptIconPosAY[i * 2] changes PsOpt_PlaceIcon's size
   (measured). */
#define HALF_H_AT(base, i) (*(short *)((char *)(base) + (i) * 4))

/* dThIcon_c leaves these two unk_. The other derived icons do not agree
   on names, so the names stay in this file.
     unk_01c mStyle                  which graphic
     unk_020 mVariant                sound mode, or 0 */
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

enum { kPanelShut = 0, kPanelOpen = 1, kPanelClosing = 2 };

/* 0x64 mode changed, 0x65 back, 0x66 backlight changed, 0x67 rejected. */
enum { kSndMode = 0x64, kSndBack = 0x65, kSndLight = 0x66, kSndReject = 0x67 };

/* Sub-screen BG enable shadow. Value 2 is this panel. */
enum { kSubBgBit = 2 };

/* The two arrow sprites: one 16x16 OAM entry each (tile 0x49 previous,
   0x47 next, palette 13) and a 0xffff terminator. Mg_DrawSpriteListEntry
   copies the entry and draws it. */
extern u16 sPsOptSoundPrevSprite[];

extern u16 sPsOptSoundNextSprite[];
extern void func_ov001_020ab5b0(char *icon, int kind, short x, short y, short halfW, short halfH);
extern int TouchArea_Update(void *icon, int touch);
extern /* Toggles mOn, drops mReady, and reloads mPressTimer from mPressTime. */
    void
    func_ov001_020ab3f0(void *icon);
/* Sound::Play2D(2, id). The ROM calls this thunk, not Play2D. */
extern "C" int GetSoundMode(void);
extern "C" {
extern void SetSoundMode(int mode);
extern void TurnBacklightOn(void);
extern void TurnBacklightOff(void);
}

/* Sub BG enable bits. dScMgBase_c saves them in mSavedSubBgBits. */
extern unsigned char gSubScreenLayers;

/* 1 while the backlight is on. TurnBacklightOn / Off write it. */
extern unsigned char data_0208ee3c;

void PsOpt_PlaceIcon(dMgPsOpt_c::TouchIcon_c *icon, int style, int variant, short x, short y);

/* One icon's half extents. The destructor is the shared empty arm9 one
   (NoOpDestructor, 0x0203b98c), so it is declared here and not defined. */
struct PsOptHalfSize {
    PsOptHalfSize(short w, short h) : mHalfW(w), mHalfH(h) {}
    ~PsOptHalfSize();

    short mHalfW;
    short mHalfH;
};

/* Half extents per language: the sound-mode caption (sPsOptIconPosA), the
   backlight caption (sPsOptIconPosB), and per style for every other icon
   (sPsOptStylePos). mwcc builds and registers each element in this file's
   static initializer (retail 0x020b9b24). */
PsOptHalfSize sPsOptIconPosA[5] = {
    PsOptHalfSize(0x20, 0x10), PsOptHalfSize(0x20, 0x10), PsOptHalfSize(0x18, 0x10),
    PsOptHalfSize(0x1c, 0x10), PsOptHalfSize(0x1c, 0x10),
};
PsOptHalfSize sPsOptIconPosB[5] = {
    PsOptHalfSize(0x30, 0x10), PsOptHalfSize(0x18, 0x10), PsOptHalfSize(0x30, 0x10),
    PsOptHalfSize(0x30, 0x10), PsOptHalfSize(0x30, 0x10),
};
PsOptHalfSize sPsOptStylePos[7] = {
    PsOptHalfSize(0x3c, 0x10), PsOptHalfSize(0x1c, 0x10), PsOptHalfSize(0x08, 0x10),
    PsOptHalfSize(0x08, 0x10), PsOptHalfSize(0x18, 0x10), PsOptHalfSize(0x28, 0x10),
    PsOptHalfSize(0x20, 0x10),
};
/* The heights, two bytes into each array. */
extern short sPsOptIconPosAY[];
extern short sPsOptIconPosBY[];
extern short sPsOptStylePosY[];

dMgPsOpt_c::TouchIcon_c::TouchIcon_c() {}

dMgPsOpt_c::dMgPsOpt_c() {
    PsOpt_PlaceIcon(&mIcons[kIconSoundMode], kStyleSoundMode, GetSoundMode(), 0xac, 0x38);
    PsOpt_PlaceIcon(&mIcons[kIconSoundPrev], kStyleSoundPrev, 0, 0x64, 0x38);
    PsOpt_PlaceIcon(&mIcons[kIconSoundNext], kStyleSoundNext, 0, 0xf4, 0x38);
    PsOpt_PlaceIcon(&mIcons[kIconSoundLabel], kStyleSoundLabel, 0,
                    sPsOptIconPosA[GetGameLanguage()].mHalfW + 0x18, 0x38);
    PsOpt_PlaceIcon(&mIcons[kIconLightOn], kStyleLight, 0, 0x8c, 0x68);
    PsOpt_PlaceIcon(&mIcons[kIconLightOff], kStyleLight, 0, 0xcc, 0x68);
    PsOpt_PlaceIcon(&mIcons[kIconLightLabel], kStyleLightLabel, 0,
                    sPsOptIconPosB[GetGameLanguage()].mHalfW + 0x18, 0x68);
    PsOpt_PlaceIcon(&mIcons[kIconBack], kStyleBack, 0, 0x80, 0xa8);

    /* The ROM stores the full count and then clears it. Both stores stay. */
    mIconCount = kIconCount;
    mIconCount = 0;
    mActive = kPanelShut;
}

dMgPsOpt_c::~dMgPsOpt_c() {}

dMgPsOpt_c::TouchIcon_c::~TouchIcon_c() {}

/* Open. Lights the current sound and backlight choices and sets sub BG
   value 2. Called from the pause menu (Mg_UpdateMenu). */
void dMgPsOpt_c::Open() {

    mIcons[kIconSoundMode].mOn = 1;
    if (data_0208ee3c != 0) {
        mIcons[kIconLightOn].mOn = 1;
        mIcons[kIconLightOff].mOn = 0;
    } else {
        mIcons[kIconLightOn].mOn = 0;
        mIcons[kIconLightOff].mOn = 1;
    }
    mIcons[kIconBack].mOn = 0;
    gSubScreenLayers |= kSubBgBit;
    mIconCount = kIconCount;
    mActive = kPanelOpen;
}

/* Shut. Clears sub BG value 2. The close delay calls this, and so does
   dScMgBase_c::CloseMenu. */
void dMgPsOpt_c::Close() {

    gSubScreenLayers &= ~kSubBgBit;
    mIconCount = 0;
    mActive = kPanelShut;
}

/* One frame of input. Returns mActive; the pause menu
   (Mg_UpdateMenu) yields the frame while that is nonzero. */
u8 dMgPsOpt_c::Update() {
    u8 state = mActive;

    switch (state) {
    case kPanelOpen:
        if (TouchArea_Update(&mIcons[kIconBack], -1)) {
            /* Back: pulse the button, then let state 2 run the delay down. */
            func_ov001_020ab3f0(&mIcons[kIconBack]);
            mCloseTimer = 0x14;
            mActive = kPanelClosing;
            func_02012790(kSndBack);
        } else {
            if (mIcons[kIconSoundMode].mReady != 0) {
                /* Sound mode wraps 0, 1, 2. Next, the mode graphic, and the
                   caption step forward; previous steps back. */
                int changed = 0;
                int mode = GetSoundMode();

                if (TouchArea_Update(&mIcons[kIconSoundMode], -1) ||
                    TouchArea_Update(&mIcons[kIconSoundNext], -1) ||
                    TouchArea_Update(&mIcons[kIconSoundLabel], -1)) {
                    if (mode == 2)
                        mode = 0;
                    else
                        mode = (mode + 1) & 0xff;
                    mIcons[kIconSoundNext].mOn = 0;
                    func_ov001_020ab3f0(&mIcons[kIconSoundNext]);
                    changed = 1;
                } else if (TouchArea_Update(&mIcons[kIconSoundPrev], -1)) {
                    if (mode == 0)
                        mode = 2;
                    else
                        mode = (mode - 1) & 0xff;
                    mIcons[kIconSoundPrev].mOn = 0;
                    func_ov001_020ab3f0(&mIcons[kIconSoundPrev]);
                    changed = 1;
                }

                if (changed != 0) {
                    SetSoundMode(mode);
                    mIcons[kIconSoundMode].mVariant = mode;
                    mIcons[kIconSoundMode].Select();
                    func_02012790(kSndMode);
                }
            }

            if (mIcons[kIconLightOn].mReady != 0 && mIcons[kIconLightOff].mReady != 0) {
                /* On turns the backlight on, off turns it off, the caption toggles. */
                int dir = 0;

                if (TouchArea_Update(&mIcons[kIconLightOn], -1)) {
                    dir = -1;
                } else if (TouchArea_Update(&mIcons[kIconLightOff], -1)) {
                    dir = 1;
                } else if (TouchArea_Update(&mIcons[kIconLightLabel], -1)) {
                    dir = (mIcons[kIconLightOn].mOn != 0) ? 1 : -1;
                }

                if (dir < 0) {
                    if (mIcons[kIconLightOn].mOn != 0) {
                        func_02012790(kSndReject);
                    } else {
                        TurnBacklightOn();
                        func_02012790(kSndLight);
                    }
                    mIcons[kIconLightOn].Select();
                    mIcons[kIconLightOff].mOn = 0;
                } else if (dir > 0) {
                    if (mIcons[kIconLightOff].mOn != 0) {
                        func_02012790(kSndReject);
                    } else {
                        TurnBacklightOff();
                        func_02012790(kSndLight);
                    }
                    mIcons[kIconLightOn].mOn = 0;
                    mIcons[kIconLightOff].Select();
                }
            }
        }
        break;

    case kPanelClosing:
        if (DecIfAbove0_Byte(&mCloseTimer) == 0)
            Close();
        break;
    }

    {
        int count = mIconCount;
        int i = 0;

        if (count > 0) {
            TouchIcon_c *icon = mIcons;
            do {
                icon->Behavior();
                i++;
                icon++;
            } while (i < mIconCount);
        }
    }

    return mActive;
}

/* Draw every live icon. Returns 1 while the panel is up, which makes
   Mg_DrawMenu skip the rest of the touch-screen UI. */
int dMgPsOpt_c::Draw() {
    int i;

    if (mActive == kPanelShut)
        return 0;

    for (i = 0; i < mIconCount; i++) {
        mIcons[i].Render();
    }

    return 1;
}

/* 1 when no live icon is still counting down a press. BeforeBehavior uses
   that as the gate before CloseMenu closes the panel. */
int dMgPsOpt_c::IsIdle() {
    TouchIcon_c *icon = mIcons;
    int i;

    for (i = 0; i < mIconCount; i++) {
        if (icon->mPressTimer != 0)
            return 0;
        icon++;
    }

    return 1;
}

/* Place one icon. Styles below the back button are an edge-triggered hit
   (kind 2); the back button is a held hit (kind 1). TouchArea_Update is
   what reads that kind. The two captions take their half extents from the
   per-language records; every other style shares the flat table. */
void PsOpt_PlaceIcon(dMgPsOpt_c::TouchIcon_c *icon, int style, int variant, short x, short y) {
    int kind;
    int lang, lang2;

    icon->mStyle = style;
    kind = (style < kStyleBack) ? 2 : 1;
    icon->mVariant = variant;

    if (style == kStyleSoundLabel) {
        lang = GetGameLanguage();
        lang2 = GetGameLanguage();
        func_ov001_020ab5b0((char *)icon, kind, x, y, sPsOptIconPosA[lang].mHalfW,
                            HALF_H_AT(sPsOptIconPosAY, lang2));
    } else if (style == kStyleLightLabel) {
        lang = GetGameLanguage();
        lang2 = GetGameLanguage();
        func_ov001_020ab5b0((char *)icon, kind, x, y, sPsOptIconPosB[lang].mHalfW,
                            HALF_H_AT(sPsOptIconPosBY, lang2));
    } else {
        func_ov001_020ab5b0((char *)icon, kind, x, y, sPsOptStylePos[style].mHalfW,
                            HALF_H_AT(sPsOptStylePosY, style));
    }
}

/* Styles 2 and 3 draw an OAM arrow and nudge it two pixels while held.
   Styles 0, 1 and 6 repaint BG1 screen entries: style 0 first stamps a
   fresh 13-wide run for the current sound mode, then all three of them
   set or clear the palette bits over the icon's rectangle. The captions
   (styles 4 and 5) draw nothing of their own. */
void dMgPsOpt_c::TouchIcon_c::Render() {
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
        sprites = sPsOptSoundPrevSprite;
        break;

    case kStyleSoundNext:
        if (mOn != 0)
            xy[0] = (s16)(xy[0] + 2);
        sprites = sPsOptSoundNextSprite;
        break;

    case kStyleSoundMode:
        scr = (u16 *)((char *)G2S_GetBG1ScrPtr() + 0x19e);
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
        scr = (u16 *)G2S_GetBG1ScrPtr();
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

    Mg_DrawSpriteListEntry((s20aea78::S *)sprites, xy[0], xy[1], 0);
}
