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
 * and the retail word at that offset holds it.
 */

/* 2D display engines */
#define REG_DISPCNT      (*(volatile u32 *)0x04000000)
#define REG_DISPCNT_SUB  (*(volatile u32 *)0x04001000)

#define DISPLAY_CONTROL_BG_EXT_PALETTE  (1u << 30)
#define DISPLAY_CONTROL_OBJ_EXT_PALETTE (1u << 31)

#define REG_VCOUNT (*(volatile u16 *)0x04000006)

/* BG0HOFS is 0x04000010 and BG0VOFS 0x04000012; the ROM sets the pair with one
   32-bit store, so this is typed to match that access rather than split in two. */
#define REG_BG0OFS (*(volatile u32 *)0x04000010)

/* Engine B's BG3 affine parameters. Taken by address, not assigned: G2x_SetBGyAffine
   walks PA/PB/PC/PD from the pointer it is handed. */
#define REG_DB_BG3PA ((volatile u16 *)0x04001030)

/* Timer 0. The counter and the control word share an address: reads see the live
   count, writes set the reload value. */
#define REG_TM0CNT_L (*(volatile u16 *)0x04000100)

/* Memory control */
#define REG_EXMEMCNT (*(volatile u16 *)0x04000204)
#define REG_WRAMCNT  (*(volatile u8  *)0x04000247)

/* Hardware square root. SQRTCNT bit 15 is the busy flag; the ROM spins on it
   through a pointer, so the address form is what that loop needs. */
#define REG_SQRTCNT     (*(volatile u16 *)0x040002b0)
#define REG_SQRT_RESULT (*(volatile s32 *)0x040002b4)

#define SQRT_CONTROL_BUSY (1u << 15)

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

/* Interrupt controller */
#define REG_IE (*(volatile u32 *)0x04000210)
#define REG_IF (*(volatile u32 *)0x04000214)

#define IRQ_GAME_CARD (1u << 19)

#endif /* NITRO_HW_REGISTERS_H */
