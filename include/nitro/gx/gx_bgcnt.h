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

static inline void *G2S_GetOBJCharPtr(void) {
    return (void *)HW_DB_OBJ_VRAM;
}

#endif
