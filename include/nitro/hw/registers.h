#ifndef NITRO_HW_REGISTERS_H
#define NITRO_HW_REGISTERS_H

#include "types.h"

/*
 * Nintendo DS ARM9 memory-mapped I/O used by the current decomp.
 *
 * Keep these as volatile lvalues or simple address expressions. Some matched
 * functions depend on the exact source expression used to materialize an MMIO
 * address, so higher-level helpers belong beside the functions that need them.
 */

/*
 * Every address below was read out of the cartridge, not looked up: the object
 * file's relocation says which word of a function carries the register address,
 * and the retail word at that offset holds it. See notes/phantom-references.md.
 */

/* 2D display engines */
#define REG_DISPCNT      (*(volatile u32 *)0x04000000)
#define REG_DISPCNT_SUB  (*(volatile u32 *)0x04001000)

/* Display capture control (engine A only). */
#define REG_DISPCAPCNT (*(volatile u32 *)0x04000064)

#define DISPLAY_CONTROL_BG_EXT_PALETTE  (1u << 30)
#define DISPLAY_CONTROL_OBJ_EXT_PALETTE (1u << 31)

/* DISPCNT bits 8-12 enable BG0-BG3 and OBJ. The game keeps each engine's
   enabled set shifted down to bit 0 (gMainScreenLayers for engine A,
   gSubScreenLayers for engine B) and copies it in with `<< 8`, so these are
   the bits of that byte. */
#define DISPLAY_LAYER_BG0 (1u << 0)
#define DISPLAY_LAYER_BG1 (1u << 1)
#define DISPLAY_LAYER_BG2 (1u << 2)
#define DISPLAY_LAYER_BG3 (1u << 3)
#define DISPLAY_LAYER_OBJ (1u << 4)

/* BG control: engine A's BG0CNT..BG3CNT are 0x04000008..0x0400000e, engine B's
   0x04001008..0x0400100e. Bits 0-1 are the priority and bit 6 the mosaic enable. */
#define REG_BG0CNT    (*(volatile u16 *)0x04000008)
#define REG_BG1CNT    (*(volatile u16 *)0x0400000a)
#define REG_BG2CNT    (*(volatile u16 *)0x0400000c)
#define REG_BG3CNT    (*(volatile u16 *)0x0400000e)
#define REG_DB_BG0CNT (*(volatile u16 *)0x04001008)
#define REG_DB_BG1CNT (*(volatile u16 *)0x0400100a)
#define REG_DB_BG2CNT (*(volatile u16 *)0x0400100c)
#define REG_DB_BG3CNT (*(volatile u16 *)0x0400100e)

#define BG_CONTROL_PRIORITY_MASK 3u
#define BG_CONTROL_MOSAIC (1u << 6)

/* Colour special effects (alpha blending and brightness), one per engine. */
#define REG_BLDCNT    (*(volatile u16 *)0x04000050)
#define REG_DB_BLDCNT (*(volatile u16 *)0x04001050)
#define REG_BLDALPHA    (*(volatile u16 *)0x04000052)
#define REG_DB_BLDALPHA (*(volatile u16 *)0x04001052)

/* Master brightness: fades the whole engine toward white or black. */
#define REG_MASTER_BRIGHT    (*(volatile u16 *)0x0400006c)
#define REG_DB_MASTER_BRIGHT (*(volatile u16 *)0x0400106c)

/* DISPSTAT bit 1 is the HBlank flag; VCOUNT is the line being drawn. */
#define REG_DISPSTAT (*(volatile u16 *)0x04000004)
#define DISPSTAT_HBLANK (1u << 1)
#define REG_VCOUNT (*(volatile u16 *)0x04000006)

/* The 3D engine's display control, DISP3DCNT (engine A only). */
#define REG_DISP3DCNT (*(volatile u16 *)0x04000060)

/* BG0HOFS is 0x04000010 and BG0VOFS 0x04000012; the ROM sets the pair with one
   32-bit store, so this is typed to match that access rather than split in two. */
#define REG_BG0OFS (*(volatile u32 *)0x04000010)
#define REG_BG1OFS (*(volatile u32 *)0x04000014)
#define REG_BG2OFS (*(volatile u32 *)0x04000018)
#define REG_BG3OFS (*(volatile u32 *)0x0400001c)
#define REG_DB_BG0OFS (*(volatile u32 *)0x04001010)
#define REG_DB_BG1OFS (*(volatile u32 *)0x04001014)
#define REG_DB_BG2OFS (*(volatile u32 *)0x04001018)
#define REG_DB_BG3OFS (*(volatile u32 *)0x0400101c)

/* The BG2 and BG3 affine parameters' first word, PA. G2x_SetBGyAffine walks
   PA/PB/PC/PD from the address it is handed. */
#define REG_BG2PA    (*(volatile u16 *)0x04000020)
#define REG_BG3PA    (*(volatile u16 *)0x04000030)
#define REG_DB_BG2PA (*(volatile u16 *)0x04001020)
#define REG_DB_BG3PA (*(volatile u16 *)0x04001030)
#define REG_BG2PD    (*(volatile u16 *)0x04000026)
#define REG_BG3PD    (*(volatile u16 *)0x04000036)
#define REG_DB_BG2PD (*(volatile u16 *)0x04001026)
#define REG_DB_BG3PD (*(volatile u16 *)0x04001036)

/* Windows: WIN0H/WIN1H are the x ranges, WIN0V/WIN1V the y ranges, WININ and
   WINOUT the layers shown inside and outside them. */
#define REG_WIN0H     (*(volatile u16 *)0x04000040)
#define REG_WIN1H     (*(volatile u16 *)0x04000042)
#define REG_WIN0V     (*(volatile u16 *)0x04000044)
#define REG_WIN1V     (*(volatile u16 *)0x04000046)
#define REG_WININ     (*(volatile u16 *)0x04000048)
#define REG_WINOUT    (*(volatile u16 *)0x0400004a)
#define REG_DB_WIN0H  (*(volatile u16 *)0x04001040)
#define REG_DB_WIN1H  (*(volatile u16 *)0x04001042)
#define REG_DB_WIN0V  (*(volatile u16 *)0x04001044)
#define REG_DB_WIN1V  (*(volatile u16 *)0x04001046)
#define REG_DB_WININ  (*(volatile u16 *)0x04001048)
#define REG_DB_WINOUT (*(volatile u16 *)0x0400104a)

/* MOSAIC's two bytes, written one at a time: the BG mosaic size, then the OBJ
   mosaic size. */
#define REG_MOSAIC_BG     (*(volatile u8 *)0x0400004c)
#define REG_MOSAIC_OBJ    (*(volatile u8 *)0x0400004d)
#define REG_DB_MOSAIC_BG  (*(volatile u8 *)0x0400104c)
#define REG_DB_MOSAIC_OBJ (*(volatile u8 *)0x0400104d)
/* The same register as one halfword (the SDK's reg_G2_MOSAIC). */
#define REG_MOSAIC (*(volatile u16 *)0x0400004c)

/* The geometry command FIFO port: a command word, then its parameter words.
   Packed and immediate-mode senders (nitro/gx/g3.h opcodes) write here. */
#define REG_GXFIFO       (*(volatile u32 *)0x04000400)

/* 3D geometry commands: the material colours, and a light's vector and colour
   (bits 30-31 of each pick the light). */
#define REG_DIF_AMB      (*(volatile u32 *)0x040004c0)
#define REG_SPE_EMI      (*(volatile u32 *)0x040004c4)
#define REG_LIGHT_VECTOR (*(volatile u32 *)0x040004c8)
#define REG_LIGHT_COLOR  (*(volatile u32 *)0x040004cc)

/* 3D geometry commands: matrix stack, polygon setup and the vertex list. The
   texture-parameter pair (0x040004a8, 0x040004ac) is in nitro/gx/g3imm.h. */
#define REG_MTX_MODE     (*(volatile u32 *)0x04000440)
#define REG_MTX_PUSH     (*(volatile u32 *)0x04000444)
#define REG_MTX_POP      (*(volatile u32 *)0x04000448)
#define REG_MTX_STORE    (*(volatile u32 *)0x0400044c)
#define REG_MTX_IDENTITY (*(volatile u32 *)0x04000454)
#define REG_MTX_SCALE    (*(volatile u32 *)0x0400046c)
#define REG_MTX_TRANS    (*(volatile u32 *)0x04000470)
#define REG_COLOR        (*(volatile u32 *)0x04000480)
#define REG_NORMAL       (*(volatile u32 *)0x04000484)
#define REG_TEXCOORD     (*(volatile u32 *)0x04000488)
#define REG_VTX_16       (*(volatile u32 *)0x0400048c)
#define REG_VTX_10       (*(volatile u32 *)0x04000490)
#define REG_VTX_XZ       (*(volatile u32 *)0x04000498)
#define REG_VTX_YZ       (*(volatile u32 *)0x0400049c)
#define REG_POLYGON_ATTR (*(volatile u32 *)0x040004a4)
#define REG_BEGIN_VTXS   (*(volatile u32 *)0x04000500)
#define REG_END_VTXS     (*(volatile u32 *)0x04000504)
#define REG_SWAP_BUFFERS (*(volatile u32 *)0x04000540)
#define REG_VIEWPORT     (*(volatile u32 *)0x04000580)

/* The 3D rendering engine: the clear colour (with its alpha, polygon ID and
   fog flag in the upper bits), clear depth and clear-image offset, the fog
   colour and depth offset, and the alpha-test reference. EDGE_COLOR (eight
   u16s), FOG_TABLE (32 bytes) and TOON_TABLE (32 u16s) are tables, filled
   from their first address. */
#define REG_EDGE_COLOR_ADDR 0x04000330
#define REG_ALPHA_TEST_REF  (*(volatile u16 *)0x04000340)
#define REG_CLEAR_COLOR     (*(volatile u32 *)0x04000350)
#define REG_CLEAR_DEPTH     (*(volatile u16 *)0x04000354)
#define REG_CLRIMAGE_OFFSET (*(volatile u16 *)0x04000356)
#define REG_FOG_COLOR       (*(volatile u32 *)0x04000358)
#define REG_FOG_OFFSET      (*(volatile u16 *)0x0400035c)
#define REG_FOG_TABLE_ADDR  0x04000360
#define REG_TOON_TABLE_ADDR 0x04000380

/* 3D geometry command: the shininess table, written 32 words in a row. */
#define REG_SHININESS (*(volatile u32 *)0x040004d0)

/* The geometry engine's status: matrix-stack levels and error flag, the busy
   flag and the FIFO interrupt condition (bit masks in nitro/gx/g3x.h). */
#define REG_GXSTAT (*(volatile u32 *)0x04000600)

/* Timer 0. The counter and the control word share an address: reads see the live
   count, writes set the reload value. */
#define REG_TM0CNT_L (*(volatile u16 *)0x04000100)

/* VRAM bank control, one byte per bank. VRAMCNT is A-D as one word and
   VRAM_HI_CNT is H-I as one halfword; WRAMCNT sits between G and H. */
#define REG_VRAMCNT     (*(volatile u32 *)0x04000240)
#define REG_VRAMCNT_A   (*(volatile u8 *)0x04000240)
#define REG_VRAMCNT_B   (*(volatile u8 *)0x04000241)
#define REG_VRAMCNT_C   (*(volatile u8 *)0x04000242)
#define REG_VRAMCNT_D   (*(volatile u8 *)0x04000243)
#define REG_VRAMCNT_E   (*(volatile u8 *)0x04000244)
#define REG_VRAMCNT_F   (*(volatile u8 *)0x04000245)
#define REG_VRAMCNT_G   (*(volatile u8 *)0x04000246)
#define REG_VRAM_HI_CNT (*(volatile u16 *)0x04000248)
#define REG_VRAMCNT_H   (*(volatile u8 *)0x04000248)
#define REG_VRAMCNT_I   (*(volatile u8 *)0x04000249)

/* Game card bus: the slot's master control byte (high byte of MCCNT0), the
   ROM transfer control word (bit 31 start/busy, bit 23 word ready), the
   8-byte command buffer, and the data-in port a transfer's words arrive
   through. Spelling follows pret's NitroSDK card_rom.c and OS_reset.c. */
#define REG_CARD_MASTERCNT (*(volatile u8  *)0x040001a1)
#define REG_CARD_CNT       (*(volatile u32 *)0x040001a4)
#define REG_CARD_CMD       (*(volatile u8  *)0x040001a8)
#define REG_CARD_DATA      (*(volatile u32 *)0x04100010)

#define CARDMST_SEL_ROM   0x00
#define CARDMST_IF_ENABLE 0x40
#define CARDMST_ENABLE    0x80
#define CARD_DATA_READY   0x00800000
#define CARD_START        0x80000000

/* Memory control */
#define REG_EXMEMCNT (*(volatile u16 *)0x04000204)
#define REG_WRAMCNT  (*(volatile u8  *)0x04000247)

/* The divider and square-root coprocessor (NitroSDK's ioreg_CP.h). The
   operand and result registers are 64 bits wide and are read or written either
   whole or by their low word, so they are named by address; nitro/cp/divider.h
   and nitro/cp/sqrt.h are the accessors. */
#define REG_DIVCNT             (*(volatile u16 *)0x04000280)
#define REG_DIV_NUMER_ADDR     0x04000290
#define REG_DIV_DENOM_ADDR     0x04000298
#define REG_DIV_RESULT_ADDR    0x040002a0
#define REG_DIVREM_RESULT_ADDR 0x040002a8
#define REG_SQRTCNT            (*(volatile u16 *)0x040002b0)
#define REG_SQRT_RESULT        (*(volatile u32 *)0x040002b4)
#define REG_SQRT_PARAM_ADDR    0x040002b8

#define REG_CP_DIVCNT_BUSY_MASK  0x8000
#define REG_CP_DIVCNT_MODE_MASK  0x0003
#define REG_CP_SQRTCNT_BUSY_MASK 0x8000
#define REG_CP_SQRTCNT_MODE_MASK 0x0001

/* PAUSE (POSTFLG): bit 0 is set once the system has booted. The SDK's
   reg_OS_PAUSE and REG_OS_PAUSE_CHK_MASK (ntrtwl/NitroSDK
   nitro/hw/ARM9/ioreg_OS.h). */
#define REG_PAUSE (*(volatile u16 *)0x04000300)

#define REG_OS_PAUSE_CHK_MASK 0x0001

/* Power control: bit 0 is the master enable for the 2D/3D hardware. */
#define REG_POWCNT1 (*(volatile u16 *)0x04000304)

#define POWER_CONTROL_ENABLE (1u << 0)

/* DMA channels: source, destination, and control are three u32 registers each. */
#define REG_DMA_BASE ((volatile u32 *)0x040000b0)
#define REG_DMA_CNT_PTR(channel) \
    (REG_DMA_BASE + ((channel) * 3 + 2))
#define REG_DMA_FILL_ADDR(channel) \
    ((u32)0x040000e0 + ((channel) << 2))

#define DMA_CONTROL_SRC_FIXED (1u << 24)
#define DMA_CONTROL_32_BIT    (1u << 26)
#define DMA_CONTROL_ENABLE    (1u << 31)

/* Interrupt controller. IME is the master enable; NitroSDK's OS_DisableIrq
   and OS_EnableIrq read the previous value and store 0 or 1. */
#define REG_IME (*(volatile u16 *)0x04000208)
#define REG_IE (*(volatile u32 *)0x04000210)
#define REG_IF (*(volatile u32 *)0x04000214)

/* The IE/IF bit numbers (nitro/hw/ARM9/ioreg_OS.h); nitro/os/interrupt.h
   has the OS_IE_* masks. */
#define REG_OS_IE_VB_SHIFT  0
#define REG_OS_IE_HB_SHIFT  1
#define REG_OS_IE_VE_SHIFT  2
#define REG_OS_IE_T0_SHIFT  3
#define REG_OS_IE_T1_SHIFT  4
#define REG_OS_IE_T2_SHIFT  5
#define REG_OS_IE_T3_SHIFT  6
#define REG_OS_IE_D0_SHIFT  8
#define REG_OS_IE_D1_SHIFT  9
#define REG_OS_IE_D2_SHIFT  10
#define REG_OS_IE_D3_SHIFT  11
#define REG_OS_IE_K_SHIFT   12
#define REG_OS_IE_I_D_SHIFT 13
#define REG_OS_IE_A7_SHIFT  16
#define REG_OS_IE_IFE_SHIFT 17
#define REG_OS_IE_IFN_SHIFT 18
#define REG_OS_IE_MC_SHIFT  19
#define REG_OS_IE_MI_SHIFT  20
#define REG_OS_IE_GF_SHIFT  21

#endif /* NITRO_HW_REGISTERS_H */
