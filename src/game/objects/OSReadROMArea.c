#include "types.h"

/* Blocking, polled read of a byte range out of the cartridge ROM.
 *
 * The card can only be read in 0x200-byte blocks, so the transfer starts at the
 * block containing romAddr and words that fall outside [0, len) are discarded.
 *
 * Registers touched (all addresses are the documented DS gamecard I/O ports):
 *   0x040001a1  high byte of the AUXSPI control register (0x80 = slot enabled)
 *   0x040001a4  gamecard ROM control: bit31 start/busy, bit23 word ready,
 *               bits 24-26 block size (1 = 0x200 bytes)
 *   0x040001a8  8-byte command buffer; 0xb7 is the "read data" command
 *   0x04100010  gamecard data-in port
 *   0x027ffe60  copy of the cartridge header in main RAM, +0x60 = the value to
 *               program into 0x040001a4 for normal commands
 */
void OSReadROMArea(u32 romAddr, void *dst, s32 len)
{
    u8 *out = (u8 *)dst;
    u32 ctrl = (*(u32 *)0x27ffe60 & ~0x7000000) | 0xa1000000;
    s32 i = -(s32)(romAddr & 0x1ff);

    while (*(volatile u32 *)0x40001a4 & 0x80000000)
        ;
    *(volatile u8 *)0x40001a1 = 0x80;

    romAddr += i;

    while (i < len) {
        u32 status;

        *(volatile u8 *)0x40001a8 = 0xb7;
        *(volatile u8 *)0x40001a9 = (u8)(romAddr >> 24);
        *(volatile u8 *)0x40001aa = (u8)(romAddr >> 16);
        *(volatile u8 *)0x40001ab = (u8)(romAddr >> 8);
        *(volatile u8 *)0x40001ac = (u8)romAddr;
        *(volatile u8 *)0x40001ad = 0;
        *(volatile u8 *)0x40001ae = 0;
        *(volatile u8 *)0x40001af = 0;

        *(volatile u32 *)0x40001a4 = ctrl;
        do {
            status = *(volatile u32 *)0x40001a4;
            if (status & 0x800000) {
                u32 data = *(volatile u32 *)0x4100010;
                if (i >= 0 && i < len)
                    *(u32 *)(out + i) = data;
                i += 4;
            }
        } while (status & 0x80000000);

        romAddr += 0x200;
    }
}
