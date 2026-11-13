/* GXS -- 2D VRAM uploads (the Nitro SDK's gx_load2d.c).
 * Owns .text 0x02055dec..0x020568cc: the VRAM load family -- BG character and
 * screen loads, the OBJ/OAM/palette loads, and the extended-palette banked
 * upload family (GXS_EndLoadOBJExtPltt .. GXS_BeginLoadBGExtPltt). The file
 * holds both engines' loaders, GX_ and GXS_ alike, under pret's names
 * (pokediamond arm9/lib/NitroSDK/src/GX_load2d.c, nitro/gx/gx_load.h) and in
 * its source order. The 3D uploads above 0x020568cc are gx_load3d.c
 * (src/engine/gx/GX.cpp).
 */
#include "types.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/hw/mmap_global.h"
#include "private/gxdma.h"

/* OBJ character VRAM has a fixed base, so the SDK's getters
 * (G2_/G2S_GetOBJCharPtr, nitro/gx/gx_bgcnt.h) are inlines, not calls.
 * LoadOBJ reads the base through one, which keeps the base in a register
 * where a literal would fold into each copy's address add. */

/* GX sub-screen extended palette VRAM bases */
#define GXS_OBJ_EXT_PLTT 0x068a0000u
#define GXS_BG_EXT_PLTT  0x06898000u

extern int GXi_DmaId;     // RENDER_DMA_CHANNEL: DMA channel number, -1 if none
extern u32 data_020a60a4; // BG ext-palette bank saved by BeginLoadBG, restored by EndLoadBG
extern u32 data_020a60a8; // OBJ ext-palette bank saved by BeginLoadOBJ, restored by EndLoadOBJ

void GX_LoadBGPltt(const void *pSrc, u32 offset, u32 szByte) {
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(HW_BG_PLTT + offset), szByte);
}

void GXS_LoadBGPltt(const void *pSrc, u32 offset, u32 szByte) {
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(HW_DB_BG_PLTT + offset), szByte);
}

void GX_LoadOBJPltt(const void *pSrc, u32 offset, u32 szByte) {
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(HW_OBJ_PLTT + offset), szByte);
}

void GXS_LoadOBJPltt(const void *pSrc, u32 offset, u32 szByte) {
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(HW_DB_OBJ_PLTT + offset), szByte);
}

void GX_LoadOAM(const void *pSrc, u32 offset, u32 szByte) {
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(HW_OAM + offset), szByte);
}

void GXS_LoadOAM(const void *pSrc, u32 offset, u32 szByte) {
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(HW_DB_OAM + offset), szByte);
}

void GX_LoadOBJ(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2_GetOBJCharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GXS_LoadOBJ(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2S_GetOBJCharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GX_LoadBG0Scr(const void *pSrc, int offset, int szByte) {
    u32 base = (u32)G2_GetBG0ScrPtr();
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GXS_LoadBG0Scr(const void *pSrc, int offset, int szByte) {
    u32 base = (u32)G2S_GetBG0ScrPtr();
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GX_LoadBG1Scr(const void *pSrc, int offset, int szByte) {
    u32 base = (u32)G2_GetBG1ScrPtr();
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GXS_LoadBG1Scr(const void *pSrc, int offset, int szByte) {
    u32 base = (u32)G2S_GetBG1ScrPtr();
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GX_LoadBG2Scr(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2_GetBG2ScrPtr();
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GXS_LoadBG2Scr(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2S_GetBG2ScrPtr();
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GX_LoadBG3Scr(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2_GetBG3ScrPtr();
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GXS_LoadBG3Scr(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2S_GetBG3ScrPtr();
    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GX_LoadBG0Char(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2_GetBG0CharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GXS_LoadBG0Char(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2S_GetBG0CharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GX_LoadBG1Char(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2_GetBG1CharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GXS_LoadBG1Char(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2S_GetBG1CharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GX_LoadBG2Char(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2_GetBG2CharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GXS_LoadBG2Char(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2S_GetBG2CharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GX_LoadBG3Char(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2_GetBG3CharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

void GXS_LoadBG3Char(const void *pSrc, u32 offset, u32 szByte) {
    u32 base = (u32)G2S_GetBG3CharPtr();
    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(base + offset), szByte);
}

// Clears the sub-engine BG ext-palette enable bit and unmaps the bank back to
// LCDC (GX_ResetBankForSubBGExtPltt); the result is held for
// GXS_EndLoadBGExtPltt.
void GXS_BeginLoadBGExtPltt() {
    data_020a60a4 = GX_ResetBankForSubBGExtPltt();
}

void GXS_LoadBGExtPltt(const void *pSrc, u32 destSlotAddr, u32 szByte) {
    GXi_DmaCopy32Async(GXi_DmaId, pSrc, (void *)(GXS_BG_EXT_PLTT + destSlotAddr), szByte, 0, 0);
}

void GXS_EndLoadBGExtPltt(void) {
    GXi_WaitDma(GXi_DmaId);
    GX_SetBankForSubBGExtPltt(data_020a60a4);
    data_020a60a4 = 0;
}

// The OBJ counterpart (GX_ResetBankForSubOBJExtPltt); the result is held for
// GXS_EndLoadOBJExtPltt.
void GXS_BeginLoadOBJExtPltt() {
    data_020a60a8 = GX_ResetBankForSubOBJExtPltt();
}

void GXS_LoadOBJExtPltt(const void *pSrc, u32 destSlotAddr, u32 szByte) {
    GXi_DmaCopy32Async(GXi_DmaId, pSrc, (void *)(GXS_OBJ_EXT_PLTT + destSlotAddr), szByte, 0, 0);
}

void GXS_EndLoadOBJExtPltt(void) {
    GXi_WaitDma(GXi_DmaId);
    GX_SetBankForSubOBJExtPltt(data_020a60a8);
    data_020a60a8 = 0;
}

// extern "C"
