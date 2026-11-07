//cpp
/* GX -- VRAM bank control (the Nitro SDK's gx_vramcnt.c). Owns arm9 .text
 * 0x02053e1c..0x02054d38: the LCDC mapper, the GX::SetBankFor* setters, the
 * reset and disable families, the bank getters and the bank-size helper.
 * mwccarm emits .text in reverse source order, so the file reads top-down
 * from the mapper at 0x02054c80 to the size getters at 0x02053e1c.
 *
 * Banks are named by bit: A 0x001, B 0x002, C 0x004, D 0x008, E 0x010,
 * F 0x020, G 0x040, H 0x080, I 0x100. A VRAMCNT byte is 0x80 (enable) |
 * MST in bits 0-2 | OFS in bits 3-4; 0x80 alone hands the bank to the LCDC.
 *
 * data_020a6088 is the SDK's VRAM state block (gGXState): which banks each
 * user holds. Every bank not held by a user stays mapped to the LCDC, so a
 * setter first gives its old banks back to lcdc, takes its new ones out,
 * then remaps whatever lcdc now holds. The block itself and its clearing
 * (func_02053d9c) live in the file below this one.
 *
 * Callers reach the address-named members under C linkage; those keep their
 * ROM names here.
 */

#include "types.h"
#include "nitro/hw/registers.h"
#include "private/gx_state.h"

#define REG_VRAMCNT_A (*(volatile u8 *)0x04000240)
#define REG_VRAMCNT_B (*(volatile u8 *)0x04000241)
#define REG_VRAMCNT_C (*(volatile u8 *)0x04000242)
#define REG_VRAMCNT_D (*(volatile u8 *)0x04000243)
#define REG_VRAMCNT_E (*(volatile u8 *)0x04000244)
#define REG_VRAMCNT_F (*(volatile u8 *)0x04000245)
#define REG_VRAMCNT_G (*(volatile u8 *)0x04000246)
#define REG_VRAMCNT_H (*(volatile u8 *)0x04000248)
#define REG_VRAMCNT_I (*(volatile u8 *)0x04000249)

#define REG_DISP3DCNT (*(volatile u16 *)0x04000060)

extern "C" {

void Vram__Map(int mask) {
    if (mask & 0x001)
        REG_VRAMCNT_A = 0x80;
    if (mask & 0x002)
        REG_VRAMCNT_B = 0x80;
    if (mask & 0x004)
        REG_VRAMCNT_C = 0x80;
    if (mask & 0x008)
        REG_VRAMCNT_D = 0x80;
    if (mask & 0x010)
        REG_VRAMCNT_E = 0x80;
    if (mask & 0x020)
        REG_VRAMCNT_F = 0x80;
    if (mask & 0x040)
        REG_VRAMCNT_G = 0x80;
    if (mask & 0x080)
        REG_VRAMCNT_H = 0x80;
    if (mask & 0x100)
        REG_VRAMCNT_I = 0x80;
}

} // extern "C"

namespace GX {

void SetBankForBG(int bg) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.bg) & ~bg);
    data_020a6088.bg = bg;
    switch (bg) {
    case 0x00:
        break;
    case 0x08:
        REG_VRAMCNT_D = 0x81;
        break;
    case 0x0c:
        REG_VRAMCNT_D = 0x89;
        // fallthrough
    case 0x04:
        REG_VRAMCNT_C = 0x81;
        break;
    case 0x0e:
        REG_VRAMCNT_D = 0x91;
        // fallthrough
    case 0x06:
        REG_VRAMCNT_C = 0x89;
        // fallthrough
    case 0x02:
        REG_VRAMCNT_B = 0x81;
        break;
    case 0x0f:
        REG_VRAMCNT_D = 0x99;
        // fallthrough
    case 0x07:
        REG_VRAMCNT_C = 0x91;
        // fallthrough
    case 0x03:
        REG_VRAMCNT_B = 0x89;
        // fallthrough
    case 0x01:
        REG_VRAMCNT_A = 0x81;
        break;
    case 0x0b:
        REG_VRAMCNT_A = 0x81;
        REG_VRAMCNT_B = 0x89;
        REG_VRAMCNT_D = 0x91;
        break;
    case 0x0d:
        REG_VRAMCNT_D = 0x91;
        // fallthrough
    case 0x05:
        REG_VRAMCNT_A = 0x81;
        REG_VRAMCNT_C = 0x89;
        break;
    case 0x09:
        REG_VRAMCNT_A = 0x81;
        REG_VRAMCNT_D = 0x89;
        break;
    case 0x0a:
        REG_VRAMCNT_B = 0x81;
        REG_VRAMCNT_D = 0x89;
        break;
    case 0x70:
        REG_VRAMCNT_G = 0x99;
        // fallthrough
    case 0x30:
        REG_VRAMCNT_F = 0x91;
        // fallthrough
    case 0x10:
        REG_VRAMCNT_E = 0x81;
        break;
    case 0x50:
        REG_VRAMCNT_G = 0x91;
        REG_VRAMCNT_E = 0x81;
        break;
    case 0x60:
        REG_VRAMCNT_G = 0x89;
        // fallthrough
    case 0x20:
        REG_VRAMCNT_F = 0x81;
        break;
    case 0x40:
        REG_VRAMCNT_G = 0x81;
        break;
    }
    Vram__Map(data_020a6088.lcdc);
}

void SetBankForOBJ(int obj) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.obj) & ~obj);
    data_020a6088.obj = obj;
    switch (obj) {
    case 0x00:
        break;
    case 0x03:
        REG_VRAMCNT_B = 0x8a;
        // fallthrough
    case 0x01:
        REG_VRAMCNT_A = 0x82;
        break;
    case 0x02:
        REG_VRAMCNT_B = 0x82;
        break;
    case 0x70:
        REG_VRAMCNT_G = 0x9a;
        // fallthrough
    case 0x30:
        REG_VRAMCNT_F = 0x92;
        // fallthrough
    case 0x10:
        REG_VRAMCNT_E = 0x82;
        break;
    case 0x50:
        REG_VRAMCNT_G = 0x92;
        REG_VRAMCNT_E = 0x82;
        break;
    case 0x60:
        REG_VRAMCNT_G = 0x8a;
        // fallthrough
    case 0x20:
        REG_VRAMCNT_F = 0x82;
        break;
    case 0x40:
        REG_VRAMCNT_G = 0x82;
        break;
    }
    Vram__Map(data_020a6088.lcdc);
}

} // namespace GX

extern "C" {

/* GX_SetBankForBGExtPltt: the main BG extended palettes, slots 0-3 on E or
 * F/G; DISPCNT bit 30 turns the extended palettes on with them. */
void func_02054748(int bgExtPltt) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.bgExtPltt) & ~bgExtPltt);
    data_020a6088.bgExtPltt = (u16)bgExtPltt;
    switch (bgExtPltt) {
    case 0x00:
        REG_DISPCNT &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
        break;
    case 0x10:
        REG_DISPCNT |= DISPLAY_CONTROL_BG_EXT_PALETTE;
        REG_VRAMCNT_E = 0x84;
        break;
    case 0x40:
        REG_DISPCNT |= DISPLAY_CONTROL_BG_EXT_PALETTE;
        REG_VRAMCNT_G = 0x8c;
        break;
    case 0x60:
        REG_VRAMCNT_G = 0x8c;
        // fallthrough
    case 0x20:
        REG_VRAMCNT_F = 0x84;
        REG_DISPCNT |= DISPLAY_CONTROL_BG_EXT_PALETTE;
        break;
    }
    Vram__Map(data_020a6088.lcdc);
}

} // extern "C"

namespace GX {

/* No texture banks also switches texturing off in DISP3DCNT. */
void SetBankForTex(int tex) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.tex) & ~tex);
    data_020a6088.tex = tex;
    if (tex == 0) {
        REG_DISP3DCNT &= 0xcffe;
    } else {
        REG_DISP3DCNT = (u16)((REG_DISP3DCNT & ~0x3000) | 1);
        switch (tex) {
        case 0:
            break;
        case 0x05:
            REG_VRAMCNT_A = 0x83;
            REG_VRAMCNT_C = 0x8b;
            break;
        case 0x09:
            REG_VRAMCNT_A = 0x83;
            REG_VRAMCNT_D = 0x8b;
            break;
        case 0x0a:
            REG_VRAMCNT_B = 0x83;
            REG_VRAMCNT_D = 0x8b;
            break;
        case 0x0b:
            REG_VRAMCNT_A = 0x83;
            REG_VRAMCNT_B = 0x8b;
            REG_VRAMCNT_D = 0x93;
            break;
        case 0x0d:
            REG_VRAMCNT_A = 0x83;
            REG_VRAMCNT_C = 0x8b;
            REG_VRAMCNT_D = 0x93;
            break;
        case 0x08:
            REG_VRAMCNT_D = 0x83;
            break;
        case 0x0c:
            REG_VRAMCNT_D = 0x8b;
            // fallthrough
        case 0x04:
            REG_VRAMCNT_C = 0x83;
            break;
        case 0x0e:
            REG_VRAMCNT_D = 0x93;
            // fallthrough
        case 0x06:
            REG_VRAMCNT_C = 0x8b;
            // fallthrough
        case 0x02:
            REG_VRAMCNT_B = 0x83;
            break;
        case 0x0f:
            REG_VRAMCNT_D = 0x9b;
            // fallthrough
        case 0x07:
            REG_VRAMCNT_C = 0x93;
            // fallthrough
        case 0x03:
            REG_VRAMCNT_B = 0x8b;
            // fallthrough
        case 0x01:
            REG_VRAMCNT_A = 0x83;
            break;
        }
    }
    Vram__Map(data_020a6088.lcdc);
}

void SetBankForTexPltt(int texPltt) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.texPltt) & ~texPltt);
    data_020a6088.texPltt = texPltt;
    switch (texPltt) {
    case 0x40:
        REG_VRAMCNT_G = 0x83;
        break;
    case 0x60:
        REG_VRAMCNT_G = 0x8b;
        // fallthrough
    case 0x20:
        REG_VRAMCNT_F = 0x83;
        break;
    case 0x70:
        REG_VRAMCNT_G = 0x9b;
        // fallthrough
    case 0x30:
        REG_VRAMCNT_F = 0x93;
        // fallthrough
    case 0x10:
        REG_VRAMCNT_E = 0x83;
        // fallthrough
    case 0x00:
        break;
    }
    Vram__Map(data_020a6088.lcdc);
}

} // namespace GX

extern "C" {

/* GX_SetBankForLCDC: hand banks straight to the LCDC. */
void func_02054430(int lcdc) {
    data_020a6088.lcdc |= lcdc;
    Vram__Map(lcdc);
}

} // extern "C"

namespace GX {

void SetBankForSubBG(int subBg) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.sub_bg) & ~subBg);
    data_020a6088.sub_bg = subBg;
    switch (subBg) {
    case 0x000:
        break;
    case 0x004:
        REG_VRAMCNT_C = 0x84;
        break;
    case 0x180:
        REG_VRAMCNT_I = 0x81;
        // fallthrough
    case 0x080:
        REG_VRAMCNT_H = 0x81;
        break;
    }
    Vram__Map(data_020a6088.lcdc);
}

void SetBankForSubOBJ(int subObj) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.sub_obj) & ~subObj);
    data_020a6088.sub_obj = subObj;
    switch (subObj) {
    case 0x000:
        break;
    case 0x008:
        REG_VRAMCNT_D = 0x84;
        break;
    case 0x100:
        REG_VRAMCNT_I = 0x82;
        break;
    }
    Vram__Map(data_020a6088.lcdc);
}

void SetBankForSubBGExtPltt(int subBgExtPltt) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.sub_bgExtPltt) & ~subBgExtPltt);
    data_020a6088.sub_bgExtPltt = subBgExtPltt;
    if (subBgExtPltt != 0) {
        if (subBgExtPltt == 0x80) {
            REG_DISPCNT_SUB |= DISPLAY_CONTROL_BG_EXT_PALETTE;
            REG_VRAMCNT_H = 0x82;
        }
    } else {
        REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
    }
    Vram__Map(data_020a6088.lcdc);
}

void SetBankForSubOBJExtPltt(int subObjExtPltt) {
    data_020a6088.lcdc =
        (u16)((data_020a6088.lcdc | data_020a6088.sub_objExtPltt) & ~subObjExtPltt);
    data_020a6088.sub_objExtPltt = subObjExtPltt;
    if (subObjExtPltt != 0) {
        if (subObjExtPltt == 0x100) {
            REG_DISPCNT_SUB |= DISPLAY_CONTROL_OBJ_EXT_PALETTE;
            REG_VRAMCNT_I = 0x83;
        }
    } else {
        REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_OBJ_EXT_PALETTE;
    }
    Vram__Map(data_020a6088.lcdc);
}

} // namespace GX

extern "C" {

/* Give a user's banks back to the LCDC. Returns the banks it held. Named to
 * match its sibling DisableVramBanks below; the ResetBankFor* wrappers that
 * call it follow, BG first. */
u16 ResetVramBanks(u16 *bank) {
    u16 mask = *bank;
    *bank = 0;
    data_020a6088.lcdc |= mask;
    Vram__Map(mask);
    return mask;
}

} // extern "C"

/* The eight ResetBankFor* wrappers: each releases exactly one gGXState field,
 * which is what names it. The spellings follow the SetBankFor* setters above,
 * so the Sub forms stay in GX rather than moving to GXS. */
namespace GX {

u16 ResetBankForBG(void) {
    return ResetVramBanks(&data_020a6088.bg);
}

u16 ResetBankForOBJ(void) {
    return ResetVramBanks(&data_020a6088.obj);
}

u16 ResetBankForTex(void) {
    return ResetVramBanks(&data_020a6088.tex);
}

u16 ResetBankForTexPltt(void) {
    return ResetVramBanks(&data_020a6088.texPltt);
}

u16 ResetBankForSubBG(void) {
    return ResetVramBanks(&data_020a6088.sub_bg);
}

u16 ResetBankForSubOBJ(void) {
    return ResetVramBanks(&data_020a6088.sub_obj);
}

u16 ResetBankForSubBGExtPltt(void) {
    REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
    return ResetVramBanks(&data_020a6088.sub_bgExtPltt);
}

u16 ResetBankForSubOBJExtPltt(void) {
    REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_OBJ_EXT_PALETTE;
    return ResetVramBanks(&data_020a6088.sub_objExtPltt);
}

} // namespace GX

extern "C" {

/* Switch a user's banks off altogether. Returns the banks it held. The
 * DisableBankFor* wrappers that call it follow, BG first. */
u16 DisableVramBanks(u16 *bank) {
    u16 mask = *bank;
    *bank = 0;
    if (mask & 0x001)
        REG_VRAMCNT_A = 0;
    if (mask & 0x002)
        REG_VRAMCNT_B = 0;
    if (mask & 0x004)
        REG_VRAMCNT_C = 0;
    if (mask & 0x008)
        REG_VRAMCNT_D = 0;
    if (mask & 0x010)
        REG_VRAMCNT_E = 0;
    if (mask & 0x020)
        REG_VRAMCNT_F = 0;
    if (mask & 0x040)
        REG_VRAMCNT_G = 0;
    if (mask & 0x080)
        REG_VRAMCNT_H = 0;
    if (mask & 0x100)
        REG_VRAMCNT_I = 0;
    return mask;
}

} // extern "C"

/* The thirteen DisableBankFor* wrappers and the four GetBankFor* queries,
 * named the same way as the reset family above. LCDC, ARM7 and ClearImage have
 * no SetBankFor* counterpart in the tree, so those three names are coined from
 * the gGXState field each one releases. */
namespace GX {

u16 DisableBankForBG(void) {
    return DisableVramBanks(&data_020a6088.bg);
}

u16 DisableBankForOBJ(void) {
    return DisableVramBanks(&data_020a6088.obj);
}

u16 DisableBankForBGExtPltt(void) {
    REG_DISPCNT &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
    return DisableVramBanks(&data_020a6088.bgExtPltt);
}

u16 DisableBankForOBJExtPltt(void) {
    REG_DISPCNT &= ~DISPLAY_CONTROL_OBJ_EXT_PALETTE;
    return DisableVramBanks(&data_020a6088.objExtPltt);
}

u16 DisableBankForTex(void) {
    return DisableVramBanks(&data_020a6088.tex);
}

u16 DisableBankForTexPltt(void) {
    return DisableVramBanks(&data_020a6088.texPltt);
}

u16 DisableBankForClearImage(void) {
    return DisableVramBanks(&data_020a6088.clrImg);
}

u16 DisableBankForARM7(void) {
    return DisableVramBanks(&data_020a6088.arm7);
}

u16 DisableBankForLCDC(void) {
    return DisableVramBanks(&data_020a6088.lcdc);
}

u16 DisableBankForSubBG(void) {
    return DisableVramBanks(&data_020a6088.sub_bg);
}

u16 DisableBankForSubOBJ(void) {
    return DisableVramBanks(&data_020a6088.sub_obj);
}

u16 DisableBankForSubBGExtPltt(void) {
    REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
    return DisableVramBanks(&data_020a6088.sub_bgExtPltt);
}

u16 DisableBankForSubOBJExtPltt(void) {
    REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_OBJ_EXT_PALETTE;
    return DisableVramBanks(&data_020a6088.sub_objExtPltt);
}

/* GX_GetBankFor*: the banks a user currently holds. */
int GetBankForTex(void) {
    return data_020a6088.tex;
}

int GetBankForTexPltt(void) {
    return data_020a6088.texPltt;
}

int GetBankForSubBG(void) {
    return data_020a6088.sub_bg;
}

int GetBankForSubOBJ(void) {
    return data_020a6088.sub_obj;
}

} // namespace GX

extern "C" {

/* The size in bytes of a set of banks: A-D are 128 KiB, E 64 KiB, F, G
 * and I 16 KiB, H 32 KiB. The SDK's static GX_GetSizeOfX_; of its
 * GX_GetSizeOf* wrappers this game links only Tex and TexPltt (below). */
int func_02053e4c(int mask) {
    int size = 0;
    if (mask & 0x001)
        size += 0x20000;
    if (mask & 0x002)
        size += 0x20000;
    if (mask & 0x004)
        size += 0x20000;
    if (mask & 0x008)
        size += 0x20000;
    if (mask & 0x010)
        size += 0x10000;
    if (mask & 0x020)
        size += 0x4000;
    if (mask & 0x040)
        size += 0x4000;
    if (mask & 0x080)
        size += 0x8000;
    if (mask & 0x100)
        size += 0x4000;
    return size;
}

// GX_GetSizeOfTex
int func_02053e34(void) {
    return func_02053e4c(data_020a6088.tex);
}

// GX_GetSizeOfTexPltt
int func_02053e1c(void) {
    return func_02053e4c(data_020a6088.texPltt);
}

} // extern "C"
