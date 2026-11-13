//cpp
/* GX -- VRAM bank control (the Nitro SDK's gx_vramcnt.c). Owns arm9 .text
 * 0x02053e1c..0x02054d38: the LCDC mapper, the GX_SetBankFor* setters, the
 * reset and disable families, the bank getters and the bank-size helper.
 * mwccarm emits .text in reverse source order, so the file reads top-down
 * from the mapper at 0x02054c80 to the size getters at 0x02053e1c.
 *
 * Banks are the GX_VRAM_A..GX_VRAM_I bits, and each setter's cases are the
 * SDK's names for the bank sets that user can take (nitro/gx/gx_vramcnt.h).
 * A bank's VRAMCNT byte is GX_VRAMCNT(mst, ofs): enabled, mapped to user
 * MST at slot OFS. MST 0 is the LCDC; the other MST numbers mean a
 * different user on each bank (1 is the main BG on A-D, the sub BG on H).
 *
 * data_020a6088 is the SDK's VRAM state block (gGXState): which banks each
 * user holds. Every bank not held by a user stays mapped to the LCDC, so a
 * setter first gives its old banks back to lcdc, takes its new ones out,
 * then remaps whatever lcdc now holds. The block itself and its clearing
 * (src/GX_InitGXState.c) live in the file below this one.
 *
 * The setters, the reset and disable families and the getters carry pret's
 * names (pokediamond arm9/lib/NitroSDK/src/GX_vramcnt.c, nitro/gx/gx_vramcnt.h)
 * and, as there, appear in the SDK's source order. The three helpers are the
 * SDK's statics, GX_VRAMCNT_SetLCDC_, resetBankForX_ and disableBankForX_
 * (the same names in pret and in ntrtwl/NitroSDK libraries/gx/src/
 * gx_vramcnt.c); here they keep external C linkage. pret has no
 * GX_GetSizeOf*; GX_GetSizeOfTex and GX_GetSizeOfTexPltt are named from
 * ntrtwl/NitroSDK libraries/gx/src/gx_vramcnt.c, which ends the file with
 * them over the static GX_GetSizeOfX_.
 */

#include "types.h"
#include "nitro/hw/registers.h"
#include "private/gx_state.h"
#include "nitro/gx/g3x.h"
#include "nitro/gx/gx_vramcnt.h"

/* VRAMCNT's fields: the enable bit, the bank's user (MST) and its slot
 * within that user's space (OFS). */
#define REG_GX_VRAMCNT_E_MASK    0x80
#define REG_GX_VRAMCNT_OFS_SHIFT 3
#define REG_GX_VRAMCNT_MST_SHIFT 0
#define GX_VRAMCNT(mst, ofs)                                                                      \
    (REG_GX_VRAMCNT_E_MASK | ((ofs) << REG_GX_VRAMCNT_OFS_SHIFT) |                              \
     ((mst) << REG_GX_VRAMCNT_MST_SHIFT))



void GX_VRAMCNT_SetLCDC_(int mask) {
    if (mask & GX_VRAM_A)
        REG_VRAMCNT_A = GX_VRAMCNT(0, 0);
    if (mask & GX_VRAM_B)
        REG_VRAMCNT_B = GX_VRAMCNT(0, 0);
    if (mask & GX_VRAM_C)
        REG_VRAMCNT_C = GX_VRAMCNT(0, 0);
    if (mask & GX_VRAM_D)
        REG_VRAMCNT_D = GX_VRAMCNT(0, 0);
    if (mask & GX_VRAM_E)
        REG_VRAMCNT_E = GX_VRAMCNT(0, 0);
    if (mask & GX_VRAM_F)
        REG_VRAMCNT_F = GX_VRAMCNT(0, 0);
    if (mask & GX_VRAM_G)
        REG_VRAMCNT_G = GX_VRAMCNT(0, 0);
    if (mask & GX_VRAM_H)
        REG_VRAMCNT_H = GX_VRAMCNT(0, 0);
    if (mask & GX_VRAM_I)
        REG_VRAMCNT_I = GX_VRAMCNT(0, 0);
}

void GX_SetBankForBG(int bg) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.bg) & ~bg);
    data_020a6088.bg = bg;
    switch (bg) {
    case GX_VRAM_BG_NONE:
        break;
    case GX_VRAM_BG_128_D:
        REG_VRAMCNT_D = GX_VRAMCNT(1, 0);
        break;
    case GX_VRAM_BG_256_CD:
        REG_VRAMCNT_D = GX_VRAMCNT(1, 1);
        // fallthrough
    case GX_VRAM_BG_128_C:
        REG_VRAMCNT_C = GX_VRAMCNT(1, 0);
        break;
    case GX_VRAM_BG_384_BCD:
        REG_VRAMCNT_D = GX_VRAMCNT(1, 2);
        // fallthrough
    case GX_VRAM_BG_256_BC:
        REG_VRAMCNT_C = GX_VRAMCNT(1, 1);
        // fallthrough
    case GX_VRAM_BG_128_B:
        REG_VRAMCNT_B = GX_VRAMCNT(1, 0);
        break;
    case GX_VRAM_BG_512_ABCD:
        REG_VRAMCNT_D = GX_VRAMCNT(1, 3);
        // fallthrough
    case GX_VRAM_BG_384_ABC:
        REG_VRAMCNT_C = GX_VRAMCNT(1, 2);
        // fallthrough
    case GX_VRAM_BG_256_AB:
        REG_VRAMCNT_B = GX_VRAMCNT(1, 1);
        // fallthrough
    case GX_VRAM_BG_128_A:
        REG_VRAMCNT_A = GX_VRAMCNT(1, 0);
        break;
    case GX_VRAM_BG_384_ABD:
        REG_VRAMCNT_A = GX_VRAMCNT(1, 0);
        REG_VRAMCNT_B = GX_VRAMCNT(1, 1);
        REG_VRAMCNT_D = GX_VRAMCNT(1, 2);
        break;
    case GX_VRAM_BG_384_ACD:
        REG_VRAMCNT_D = GX_VRAMCNT(1, 2);
        // fallthrough
    case GX_VRAM_BG_256_AC:
        REG_VRAMCNT_A = GX_VRAMCNT(1, 0);
        REG_VRAMCNT_C = GX_VRAMCNT(1, 1);
        break;
    case GX_VRAM_BG_256_AD:
        REG_VRAMCNT_A = GX_VRAMCNT(1, 0);
        REG_VRAMCNT_D = GX_VRAMCNT(1, 1);
        break;
    case GX_VRAM_BG_256_BD:
        REG_VRAMCNT_B = GX_VRAMCNT(1, 0);
        REG_VRAMCNT_D = GX_VRAMCNT(1, 1);
        break;
    case GX_VRAM_BG_96_EFG:
        REG_VRAMCNT_G = GX_VRAMCNT(1, 3);
        // fallthrough
    case GX_VRAM_BG_80_EF:
        REG_VRAMCNT_F = GX_VRAMCNT(1, 2);
        // fallthrough
    case GX_VRAM_BG_64_E:
        REG_VRAMCNT_E = GX_VRAMCNT(1, 0);
        break;
    case GX_VRAM_BG_80_EG:
        REG_VRAMCNT_G = GX_VRAMCNT(1, 2);
        REG_VRAMCNT_E = GX_VRAMCNT(1, 0);
        break;
    case GX_VRAM_BG_32_FG:
        REG_VRAMCNT_G = GX_VRAMCNT(1, 1);
        // fallthrough
    case GX_VRAM_BG_16_F:
        REG_VRAMCNT_F = GX_VRAMCNT(1, 0);
        break;
    case GX_VRAM_BG_16_G:
        REG_VRAMCNT_G = GX_VRAMCNT(1, 0);
        break;
    }
    GX_VRAMCNT_SetLCDC_(data_020a6088.lcdc);
}

void GX_SetBankForOBJ(int obj) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.obj) & ~obj);
    data_020a6088.obj = obj;
    switch (obj) {
    case GX_VRAM_OBJ_NONE:
        break;
    case GX_VRAM_OBJ_256_AB:
        REG_VRAMCNT_B = GX_VRAMCNT(2, 1);
        // fallthrough
    case GX_VRAM_OBJ_128_A:
        REG_VRAMCNT_A = GX_VRAMCNT(2, 0);
        break;
    case GX_VRAM_OBJ_128_B:
        REG_VRAMCNT_B = GX_VRAMCNT(2, 0);
        break;
    case GX_VRAM_OBJ_96_EFG:
        REG_VRAMCNT_G = GX_VRAMCNT(2, 3);
        // fallthrough
    case GX_VRAM_OBJ_80_EF:
        REG_VRAMCNT_F = GX_VRAMCNT(2, 2);
        // fallthrough
    case GX_VRAM_OBJ_64_E:
        REG_VRAMCNT_E = GX_VRAMCNT(2, 0);
        break;
    case GX_VRAM_OBJ_80_EG:
        REG_VRAMCNT_G = GX_VRAMCNT(2, 2);
        REG_VRAMCNT_E = GX_VRAMCNT(2, 0);
        break;
    case GX_VRAM_OBJ_32_FG:
        REG_VRAMCNT_G = GX_VRAMCNT(2, 1);
        // fallthrough
    case GX_VRAM_OBJ_16_F:
        REG_VRAMCNT_F = GX_VRAMCNT(2, 0);
        break;
    case GX_VRAM_OBJ_16_G:
        REG_VRAMCNT_G = GX_VRAMCNT(2, 0);
        break;
    }
    GX_VRAMCNT_SetLCDC_(data_020a6088.lcdc);
}

/* GX_SetBankForBGExtPltt: the main BG extended palettes, slots 0-3 on E or
 * F/G; DISPCNT bit 30 turns the extended palettes on with them. */
void GX_SetBankForBGExtPltt(int bgExtPltt) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.bgExtPltt) & ~bgExtPltt);
    data_020a6088.bgExtPltt = (u16)bgExtPltt;
    switch (bgExtPltt) {
    case GX_VRAM_BGEXTPLTT_NONE:
        REG_DISPCNT &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
        break;
    case GX_VRAM_BGEXTPLTT_0123_E:
        REG_DISPCNT |= DISPLAY_CONTROL_BG_EXT_PALETTE;
        REG_VRAMCNT_E = GX_VRAMCNT(4, 0);
        break;
    case GX_VRAM_BGEXTPLTT_23_G:
        REG_DISPCNT |= DISPLAY_CONTROL_BG_EXT_PALETTE;
        REG_VRAMCNT_G = GX_VRAMCNT(4, 1);
        break;
    case GX_VRAM_BGEXTPLTT_0123_FG:
        REG_VRAMCNT_G = GX_VRAMCNT(4, 1);
        // fallthrough
    case GX_VRAM_BGEXTPLTT_01_F:
        REG_VRAMCNT_F = GX_VRAMCNT(4, 0);
        REG_DISPCNT |= DISPLAY_CONTROL_BG_EXT_PALETTE;
        break;
    }
    GX_VRAMCNT_SetLCDC_(data_020a6088.lcdc);
}

/* No texture banks also switches texturing off in DISP3DCNT. */
void GX_SetBankForTex(int tex) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.tex) & ~tex);
    data_020a6088.tex = tex;
    if (tex == GX_VRAM_TEX_NONE) {
        REG_DISP3DCNT &= (u16) ~(REG_G3X_DISP3DCNT_TME_MASK | REG_G3X_DISP3DCNT_RO_MASK |
                                 REG_G3X_DISP3DCNT_GO_MASK);
    } else {
        REG_DISP3DCNT =
            (u16)((REG_DISP3DCNT & ~(REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK)) |
                  REG_G3X_DISP3DCNT_TME_MASK);
        switch (tex) {
        case GX_VRAM_TEX_NONE:
            break;
        case GX_VRAM_TEX_01_AC:
            REG_VRAMCNT_A = GX_VRAMCNT(3, 0);
            REG_VRAMCNT_C = GX_VRAMCNT(3, 1);
            break;
        case GX_VRAM_TEX_01_AD:
            REG_VRAMCNT_A = GX_VRAMCNT(3, 0);
            REG_VRAMCNT_D = GX_VRAMCNT(3, 1);
            break;
        case GX_VRAM_TEX_01_BD:
            REG_VRAMCNT_B = GX_VRAMCNT(3, 0);
            REG_VRAMCNT_D = GX_VRAMCNT(3, 1);
            break;
        case GX_VRAM_TEX_012_ABD:
            REG_VRAMCNT_A = GX_VRAMCNT(3, 0);
            REG_VRAMCNT_B = GX_VRAMCNT(3, 1);
            REG_VRAMCNT_D = GX_VRAMCNT(3, 2);
            break;
        case GX_VRAM_TEX_012_ACD:
            REG_VRAMCNT_A = GX_VRAMCNT(3, 0);
            REG_VRAMCNT_C = GX_VRAMCNT(3, 1);
            REG_VRAMCNT_D = GX_VRAMCNT(3, 2);
            break;
        case GX_VRAM_TEX_0_D:
            REG_VRAMCNT_D = GX_VRAMCNT(3, 0);
            break;
        case GX_VRAM_TEX_01_CD:
            REG_VRAMCNT_D = GX_VRAMCNT(3, 1);
            // fallthrough
        case GX_VRAM_TEX_0_C:
            REG_VRAMCNT_C = GX_VRAMCNT(3, 0);
            break;
        case GX_VRAM_TEX_012_BCD:
            REG_VRAMCNT_D = GX_VRAMCNT(3, 2);
            // fallthrough
        case GX_VRAM_TEX_01_BC:
            REG_VRAMCNT_C = GX_VRAMCNT(3, 1);
            // fallthrough
        case GX_VRAM_TEX_0_B:
            REG_VRAMCNT_B = GX_VRAMCNT(3, 0);
            break;
        case GX_VRAM_TEX_0123_ABCD:
            REG_VRAMCNT_D = GX_VRAMCNT(3, 3);
            // fallthrough
        case GX_VRAM_TEX_012_ABC:
            REG_VRAMCNT_C = GX_VRAMCNT(3, 2);
            // fallthrough
        case GX_VRAM_TEX_01_AB:
            REG_VRAMCNT_B = GX_VRAMCNT(3, 1);
            // fallthrough
        case GX_VRAM_TEX_0_A:
            REG_VRAMCNT_A = GX_VRAMCNT(3, 0);
            break;
        }
    }
    GX_VRAMCNT_SetLCDC_(data_020a6088.lcdc);
}

void GX_SetBankForTexPltt(int texPltt) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.texPltt) & ~texPltt);
    data_020a6088.texPltt = texPltt;
    switch (texPltt) {
    case GX_VRAM_TEXPLTT_0_G:
        REG_VRAMCNT_G = GX_VRAMCNT(3, 0);
        break;
    case GX_VRAM_TEXPLTT_01_FG:
        REG_VRAMCNT_G = GX_VRAMCNT(3, 1);
        // fallthrough
    case GX_VRAM_TEXPLTT_0_F:
        REG_VRAMCNT_F = GX_VRAMCNT(3, 0);
        break;
    case GX_VRAM_TEXPLTT_012345_EFG:
        REG_VRAMCNT_G = GX_VRAMCNT(3, 3);
        // fallthrough
    case GX_VRAM_TEXPLTT_01234_EF:
        REG_VRAMCNT_F = GX_VRAMCNT(3, 2);
        // fallthrough
    case GX_VRAM_TEXPLTT_0123_E:
        REG_VRAMCNT_E = GX_VRAMCNT(3, 0);
        // fallthrough
    case GX_VRAM_TEXPLTT_NONE:
        break;
    }
    GX_VRAMCNT_SetLCDC_(data_020a6088.lcdc);
}

/* GX_SetBankForLCDC: hand banks straight to the LCDC. */
void GX_SetBankForLCDC(int lcdc) {
    data_020a6088.lcdc |= lcdc;
    GX_VRAMCNT_SetLCDC_(lcdc);
}

void GX_SetBankForSubBG(int subBg) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.sub_bg) & ~subBg);
    data_020a6088.sub_bg = subBg;
    switch (subBg) {
    case GX_VRAM_SUB_BG_NONE:
        break;
    case GX_VRAM_SUB_BG_128_C:
        REG_VRAMCNT_C = GX_VRAMCNT(4, 0);
        break;
    case GX_VRAM_SUB_BG_48_HI:
        REG_VRAMCNT_I = GX_VRAMCNT(1, 0);
        // fallthrough
    case GX_VRAM_SUB_BG_32_H:
        REG_VRAMCNT_H = GX_VRAMCNT(1, 0);
        break;
    }
    GX_VRAMCNT_SetLCDC_(data_020a6088.lcdc);
}

void GX_SetBankForSubOBJ(int subObj) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.sub_obj) & ~subObj);
    data_020a6088.sub_obj = subObj;
    switch (subObj) {
    case GX_VRAM_SUB_OBJ_NONE:
        break;
    case GX_VRAM_SUB_OBJ_128_D:
        REG_VRAMCNT_D = GX_VRAMCNT(4, 0);
        break;
    case GX_VRAM_SUB_OBJ_16_I:
        REG_VRAMCNT_I = GX_VRAMCNT(2, 0);
        break;
    }
    GX_VRAMCNT_SetLCDC_(data_020a6088.lcdc);
}

void GX_SetBankForSubBGExtPltt(int subBgExtPltt) {
    data_020a6088.lcdc = (u16)((data_020a6088.lcdc | data_020a6088.sub_bgExtPltt) & ~subBgExtPltt);
    data_020a6088.sub_bgExtPltt = subBgExtPltt;
    if (subBgExtPltt != GX_VRAM_SUB_BGEXTPLTT_NONE) {
        if (subBgExtPltt == GX_VRAM_SUB_BGEXTPLTT_0123_H) {
            REG_DISPCNT_SUB |= DISPLAY_CONTROL_BG_EXT_PALETTE;
            REG_VRAMCNT_H = GX_VRAMCNT(2, 0);
        }
    } else {
        REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
    }
    GX_VRAMCNT_SetLCDC_(data_020a6088.lcdc);
}

void GX_SetBankForSubOBJExtPltt(int subObjExtPltt) {
    data_020a6088.lcdc =
        (u16)((data_020a6088.lcdc | data_020a6088.sub_objExtPltt) & ~subObjExtPltt);
    data_020a6088.sub_objExtPltt = subObjExtPltt;
    if (subObjExtPltt != GX_VRAM_SUB_OBJEXTPLTT_NONE) {
        if (subObjExtPltt == GX_VRAM_SUB_OBJEXTPLTT_0_I) {
            REG_DISPCNT_SUB |= DISPLAY_CONTROL_OBJ_EXT_PALETTE;
            REG_VRAMCNT_I = GX_VRAMCNT(3, 0);
        }
    } else {
        REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_OBJ_EXT_PALETTE;
    }
    GX_VRAMCNT_SetLCDC_(data_020a6088.lcdc);
}

/* Give a user's banks back to the LCDC. Returns the banks it held. The
 * GX_ResetBankFor* wrappers that call it follow, BG first. */
u16 resetBankForX_(u16 *bank) {
    u16 mask = *bank;
    *bank = 0;
    data_020a6088.lcdc |= mask;
    GX_VRAMCNT_SetLCDC_(mask);
    return mask;
}

/* The eight GX_ResetBankFor* wrappers: each releases exactly one gGXState
 * field. */
u16 GX_ResetBankForBG(void) {
    return resetBankForX_(&data_020a6088.bg);
}

u16 GX_ResetBankForOBJ(void) {
    return resetBankForX_(&data_020a6088.obj);
}

u16 GX_ResetBankForTex(void) {
    return resetBankForX_(&data_020a6088.tex);
}

u16 GX_ResetBankForTexPltt(void) {
    return resetBankForX_(&data_020a6088.texPltt);
}

u16 GX_ResetBankForSubBG(void) {
    return resetBankForX_(&data_020a6088.sub_bg);
}

u16 GX_ResetBankForSubOBJ(void) {
    return resetBankForX_(&data_020a6088.sub_obj);
}

u16 GX_ResetBankForSubBGExtPltt(void) {
    REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
    return resetBankForX_(&data_020a6088.sub_bgExtPltt);
}

u16 GX_ResetBankForSubOBJExtPltt(void) {
    REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_OBJ_EXT_PALETTE;
    return resetBankForX_(&data_020a6088.sub_objExtPltt);
}

/* Switch a user's banks off altogether. Returns the banks it held. The
 * GX_DisableBankFor* wrappers that call it follow, BG first. */
u16 disableBankForX_(u16 *bank) {
    u16 mask = *bank;
    *bank = 0;
    if (mask & GX_VRAM_A)
        REG_VRAMCNT_A = 0;
    if (mask & GX_VRAM_B)
        REG_VRAMCNT_B = 0;
    if (mask & GX_VRAM_C)
        REG_VRAMCNT_C = 0;
    if (mask & GX_VRAM_D)
        REG_VRAMCNT_D = 0;
    if (mask & GX_VRAM_E)
        REG_VRAMCNT_E = 0;
    if (mask & GX_VRAM_F)
        REG_VRAMCNT_F = 0;
    if (mask & GX_VRAM_G)
        REG_VRAMCNT_G = 0;
    if (mask & GX_VRAM_H)
        REG_VRAMCNT_H = 0;
    if (mask & GX_VRAM_I)
        REG_VRAMCNT_I = 0;
    return mask;
}

/* The thirteen GX_DisableBankFor* wrappers and the four GX_GetBankFor*
 * queries. */
u16 GX_DisableBankForBG(void) {
    return disableBankForX_(&data_020a6088.bg);
}

u16 GX_DisableBankForOBJ(void) {
    return disableBankForX_(&data_020a6088.obj);
}

u16 GX_DisableBankForBGExtPltt(void) {
    REG_DISPCNT &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
    return disableBankForX_(&data_020a6088.bgExtPltt);
}

u16 GX_DisableBankForOBJExtPltt(void) {
    REG_DISPCNT &= ~DISPLAY_CONTROL_OBJ_EXT_PALETTE;
    return disableBankForX_(&data_020a6088.objExtPltt);
}

u16 GX_DisableBankForTex(void) {
    return disableBankForX_(&data_020a6088.tex);
}

u16 GX_DisableBankForTexPltt(void) {
    return disableBankForX_(&data_020a6088.texPltt);
}

u16 GX_DisableBankForClearImage(void) {
    return disableBankForX_(&data_020a6088.clrImg);
}

u16 GX_DisableBankForARM7(void) {
    return disableBankForX_(&data_020a6088.arm7);
}

u16 GX_DisableBankForLCDC(void) {
    return disableBankForX_(&data_020a6088.lcdc);
}

u16 GX_DisableBankForSubBG(void) {
    return disableBankForX_(&data_020a6088.sub_bg);
}

u16 GX_DisableBankForSubOBJ(void) {
    return disableBankForX_(&data_020a6088.sub_obj);
}

u16 GX_DisableBankForSubBGExtPltt(void) {
    REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
    return disableBankForX_(&data_020a6088.sub_bgExtPltt);
}

u16 GX_DisableBankForSubOBJExtPltt(void) {
    REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_OBJ_EXT_PALETTE;
    return disableBankForX_(&data_020a6088.sub_objExtPltt);
}

/* The banks a user currently holds. */
int GX_GetBankForTex(void) {
    return data_020a6088.tex;
}

int GX_GetBankForTexPltt(void) {
    return data_020a6088.texPltt;
}

int GX_GetBankForSubBG(void) {
    return data_020a6088.sub_bg;
}

int GX_GetBankForSubOBJ(void) {
    return data_020a6088.sub_obj;
}

/* The size in bytes of a set of banks: A-D are 128 KiB, E 64 KiB, F, G
 * and I 16 KiB, H 32 KiB. Of the SDK's GX_GetSizeOf* wrappers this game
 * links only Tex and TexPltt (below). */
int GX_GetSizeOfX_(int mask) {
    int size = 0;
    if (mask & GX_VRAM_A)
        size += 0x20000;
    if (mask & GX_VRAM_B)
        size += 0x20000;
    if (mask & GX_VRAM_C)
        size += 0x20000;
    if (mask & GX_VRAM_D)
        size += 0x20000;
    if (mask & GX_VRAM_E)
        size += 0x10000;
    if (mask & GX_VRAM_F)
        size += 0x4000;
    if (mask & GX_VRAM_G)
        size += 0x4000;
    if (mask & GX_VRAM_H)
        size += 0x8000;
    if (mask & GX_VRAM_I)
        size += 0x4000;
    return size;
}

int GX_GetSizeOfTex(void) {
    return GX_GetSizeOfX_(data_020a6088.tex);
}

int GX_GetSizeOfTexPltt(void) {
    return GX_GetSizeOfX_(data_020a6088.texPltt);
}

 // extern "C"
