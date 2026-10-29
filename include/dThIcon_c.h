#ifndef DTHICON_C_H
#define DTHICON_C_H

#include "types.h"

/* Polymorphic touch-screen icon base. RTTI proves the dThIcon_c name and
 * its derived-class edges. ROM vtables prove the two virtual roles are
 * Behavior and Render; the destructor is nonvirtual. Its 0x24-byte
 * extent is fixed by the derived constructors/destructors and array
 * stride.
 *
 * Field names are coined from how the code uses them; the cartridge
 * carries none. unk_01c / unk_020 stay unk_ because each derived icon
 * gives them a different role: dScMgSlot1_c::betIcon_c the bet count and
 * the payout countdown, dScEntry_c::icon_c the icon type and index,
 * dMgPsOpt_c::TouchIcon_c the graphic style and variant.
 */
struct dThIcon_c {
    /* Centre and half extents in pixels. func_ov001_020ab5b0 stores them;
       TouchArea_Update tests the pen against centre +/- half extent, and
       the derived Renders draw or fill the BG at them. */
    s16 mX;     /* 0x004 */
    s16 mY;     /* 0x006 */
    s16 mHalfW; /* 0x008 */
    s16 mHalfH; /* 0x00a */
    /* Frames left in a press. func_ov001_020ab3f0 loads it from
       mPressTime; Behavior counts it down by data_0208ee44 and, at zero,
       flips mOn back and re-arms mReady. dScEntry_c::icon_c::Render
       scales its sprite by it. */
    s32 mPressTimer; /* 0x00c */
    /* Lit / pressed: func_ov001_020ab3f0 toggles it, Behavior toggles it
       back; the derived Renders pick the lit sprite or palette by it. */
    u8 mOn; /* 0x010 */
    /* TouchArea_Update returns 0 unless set. func_ov001_020ab3f0 clears
       it; func_ov001_020ab5b0 and Behavior set it when mTouchKind >= 1. */
    u8 mReady; /* 0x011 */
    /* TouchArea_Update stores whether the pen is inside this frame, so
       mTouchKind 2 fires only on entry. */
    u8 mPenInside; /* 0x012 */
    u8 pad_013[0x1];
    /* How TouchArea_Update fires: 1 on a new pen touch inside, 2 when the
       pen enters, 0 never. func_ov001_020ab5b0 stores it. */
    s32 mTouchKind; /* 0x014 */
    /* mPressTimer's reload: func_ov001_020ab5b0 stores 10 and
       SlotBet_Place 6. */
    s32 mPressTime; /* 0x018 */
    s32 unk_01c;
    s32 unk_020;

    dThIcon_c() {}
    ~dThIcon_c() {}
    /* Light an icon a menu has just picked: an unlit icon turns on, a lit
       one plays the press pulse. */
    void Select();
    virtual void Behavior();
    virtual void Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dThIcon_c_size_must_be_0x24[sizeof(dThIcon_c) == 0x24 ? 1 : -1];
#endif

#endif
