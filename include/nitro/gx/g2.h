#ifndef NITRO_GX_G2_H
#define NITRO_GX_G2_H

#include "types.h"
#include "nitro/hw/registers.h"

/* gx_g2.c, under the SDK's names with C linkage (ntrtwl/NitroSDK
 * libraries/gx/src/g2.c). Parameters unnamed: a named parameter here
 * renumbers every includer's @NNN.
 *
 * The SDK passes each register block as a u32 address; this tree passes
 * the `volatile unsigned short *` its callers already hold. MtxFx22 is
 * Matrix2x2. G2x_SetBlendBrightnessExt_ is not in this ROM. */

struct Matrix2x2;

#ifdef __cplusplus
extern "C" {
#endif
void G2x_SetBGyAffine_(volatile unsigned short *, struct Matrix2x2 *, int, int, int, int);
void G2x_SetBlendAlpha_(volatile unsigned short *, int, int, int, int);
void G2x_SetBlendBrightness_(volatile unsigned short *, int, int);
void G2x_ChangeBlendBrightness_(volatile unsigned short *, int);
#ifdef __cplusplus
}
#endif

/* The SDK's public wrappers: each passes one engine's register block to the
 * x_ worker above (G2_ for engine A, G2S_ for engine B). The SDK writes them
 * as static inlines; they are macros here because an inline body in a shared
 * header renumbers every includer's @NNN symbols. The expansion is the same
 * call either way. */
#define G2_SetBG2Affine(mtx, centerX, centerY, x1, y1)     G2x_SetBGyAffine_(&REG_BG2PA, (mtx), (centerX), (centerY), (x1), (y1))
#define G2_SetBG3Affine(mtx, centerX, centerY, x1, y1)     G2x_SetBGyAffine_(&REG_BG3PA, (mtx), (centerX), (centerY), (x1), (y1))
#define G2S_SetBG2Affine(mtx, centerX, centerY, x1, y1)     G2x_SetBGyAffine_(&REG_DB_BG2PA, (mtx), (centerX), (centerY), (x1), (y1))
#define G2S_SetBG3Affine(mtx, centerX, centerY, x1, y1)     G2x_SetBGyAffine_(&REG_DB_BG3PA, (mtx), (centerX), (centerY), (x1), (y1))

#define G2_SetBlendAlpha(plane1, plane2, ev1, ev2)     G2x_SetBlendAlpha_(&REG_BLDCNT, (plane1), (plane2), (ev1), (ev2))
#define G2S_SetBlendAlpha(plane1, plane2, ev1, ev2)     G2x_SetBlendAlpha_(&REG_DB_BLDCNT, (plane1), (plane2), (ev1), (ev2))
#define G2_SetBlendBrightness(plane, brightness)     G2x_SetBlendBrightness_(&REG_BLDCNT, (plane), (brightness))
#define G2S_SetBlendBrightness(plane, brightness)     G2x_SetBlendBrightness_(&REG_DB_BLDCNT, (plane), (brightness))
#define G2_ChangeBlendBrightness(brightness)     G2x_ChangeBlendBrightness_(&REG_BLDCNT, (brightness))
#define G2S_ChangeBlendBrightness(brightness)     G2x_ChangeBlendBrightness_(&REG_DB_BLDCNT, (brightness))

/* Blending off: BLDCNT's effect field back to none. */
#define G2_BlendNone() (REG_BLDCNT = 0)
#define G2S_BlendNone() (REG_DB_BLDCNT = 0)

/* A background's scroll offset: horizontal in BGxOFS bits 0-8, vertical in
   bits 16-24, written as one word. */
#define G2_SetBG0Offset(hOffset, vOffset) (REG_BG0OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2_SetBG1Offset(hOffset, vOffset) (REG_BG1OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2_SetBG2Offset(hOffset, vOffset) (REG_BG2OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2_SetBG3Offset(hOffset, vOffset) (REG_BG3OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2S_SetBG0Offset(hOffset, vOffset) (REG_DB_BG0OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2S_SetBG1Offset(hOffset, vOffset) (REG_DB_BG1OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2S_SetBG2Offset(hOffset, vOffset) (REG_DB_BG2OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))
#define G2S_SetBG3Offset(hOffset, vOffset) (REG_DB_BG3OFS = (u32)(((hOffset) & 0x1ff) | (((vOffset) << 16) & 0x1ff0000)))

/* The window planes and positions (WININ, WINOUT, WINnH/WINnV). The SDK's
 * GXWndPlane masks select BG0-BG3 and OBJ; `effect` enables the colour
 * special effect inside that region. The plane setters keep the SDK
 * inline's shape -- read into a temporary, OR the effect bit in, store --
 * and the position setters pack x1/y1 into the high byte, x2/y2 the low. */
#define GX_WND_PLANEMASK_NONE 0x00
#define GX_WND_PLANEMASK_BG0  0x01
#define GX_WND_PLANEMASK_BG1  0x02
#define GX_WND_PLANEMASK_BG2  0x04
#define GX_WND_PLANEMASK_BG3  0x08
#define GX_WND_PLANEMASK_OBJ  0x10

#define G2x_SetWndPlane_(reg, mask, shift, wnd, effect) \
    do { \
        u32 wndTmp_ = (u32)(((reg) & ~(mask)) | ((u32)(wnd) << (shift))); \
        if (effect) \
            wndTmp_ |= 0x20 << (shift); \
        (reg) = (u16)wndTmp_; \
    } while (0)
#define G2_SetWnd0InsidePlane(wnd, effect)    G2x_SetWndPlane_(REG_WININ, 0x3f, 0, (wnd), (effect))
#define G2_SetWnd1InsidePlane(wnd, effect)    G2x_SetWndPlane_(REG_WININ, 0x3f00, 8, (wnd), (effect))
#define G2_SetWndOutsidePlane(wnd, effect)    G2x_SetWndPlane_(REG_WINOUT, 0x3f, 0, (wnd), (effect))
#define G2_SetWndOBJInsidePlane(wnd, effect)  G2x_SetWndPlane_(REG_WINOUT, 0x3f00, 8, (wnd), (effect))
#define G2S_SetWnd0InsidePlane(wnd, effect)   G2x_SetWndPlane_(REG_DB_WININ, 0x3f, 0, (wnd), (effect))
#define G2S_SetWnd1InsidePlane(wnd, effect)   G2x_SetWndPlane_(REG_DB_WININ, 0x3f00, 8, (wnd), (effect))
#define G2S_SetWndOutsidePlane(wnd, effect)   G2x_SetWndPlane_(REG_DB_WINOUT, 0x3f, 0, (wnd), (effect))
#define G2S_SetWndOBJInsidePlane(wnd, effect) G2x_SetWndPlane_(REG_DB_WINOUT, 0x3f00, 8, (wnd), (effect))

#define G2x_SetWndPosition_(regH, regV, x1, y1, x2, y2) \
    ((regH) = (u16)((((x1) << 8) & 0xff00) | ((x2) & 0xff)), \
     (regV) = (u16)((((y1) << 8) & 0xff00) | ((y2) & 0xff)))
#define G2_SetWnd0Position(x1, y1, x2, y2)  G2x_SetWndPosition_(REG_WIN0H, REG_WIN0V, (x1), (y1), (x2), (y2))
#define G2_SetWnd1Position(x1, y1, x2, y2)  G2x_SetWndPosition_(REG_WIN1H, REG_WIN1V, (x1), (y1), (x2), (y2))
#define G2S_SetWnd0Position(x1, y1, x2, y2) G2x_SetWndPosition_(REG_DB_WIN0H, REG_DB_WIN0V, (x1), (y1), (x2), (y2))
#define G2S_SetWnd1Position(x1, y1, x2, y2) G2x_SetWndPosition_(REG_DB_WIN1H, REG_DB_WIN1V, (x1), (y1), (x2), (y2))

#endif
