#ifndef NITRO_GX_GX_BGCNT_H
#define NITRO_GX_GX_BGCNT_H

/* NitroSDK's OBJ character base getter (gx_bgcnt.h): a static inline that
 * returns the sub engine's OBJ VRAM. Read through the inline, the base is not
 * folded with an offset added to it, and the ROM builds 0x06602000 as
 * `mov #0x6600000 / add #0x2000` (func_ov002_020f39ec) where a literal
 * address would load it from the pool. */

/* The two engines' OBJ character VRAM, under the SDK's memory-map names. */
#define HW_OBJ_VRAM 0x06400000
#define HW_DB_OBJ_VRAM 0x06600000

#ifdef __cplusplus
extern "C" {
#endif

/* The BG base-address getters, defined in src/engine/gx/GX_BgCnt.c. Each
 * returns the layer's VRAM address, or null where the current mode gives the
 * layer no such base. */
void *G2_GetBG0ScrPtr(void);
void *G2S_GetBG0ScrPtr(void);
void *G2_GetBG1ScrPtr(void);
void *G2S_GetBG1ScrPtr(void);
void *G2_GetBG2ScrPtr(void);
void *G2S_GetBG2ScrPtr(void);
void *G2_GetBG3ScrPtr(void);
void *G2S_GetBG3ScrPtr(void);
void *G2_GetBG0CharPtr(void);
void *G2S_GetBG0CharPtr(void);
void *G2_GetBG1CharPtr(void);
void *G2S_GetBG1CharPtr(void);
void *G2_GetBG2CharPtr(void);
void *G2S_GetBG2CharPtr(void);
void *G2_GetBG3CharPtr(void);
void *G2S_GetBG3CharPtr(void);

#ifdef __cplusplus
}
#endif

/* The two engines' OBJ character bases. The address is fixed, so the SDK
 * reads it through an inline, not a call. Read through the getter, the ROM
 * builds 0x06401280 as `mov #0x6400000 / add #0x1280` where a literal
 * address would load it from the pool. */
static inline void *G2_GetOBJCharPtr(void) {
    return (void *)HW_OBJ_VRAM;
}

static inline void *G2S_GetOBJCharPtr(void) {
    return (void *)HW_DB_OBJ_VRAM;
}

#endif
