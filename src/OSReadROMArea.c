#include "types.h"
#include "nitro/hw/mmap_shared.h"

/* OSReadROMArea at 0x01ffdbd8 (ITCM) -- NitroSDK os_reset.c's static
 * OSi_ReadCardRom32: a blocking, polled read of [src, src + len) from the card
 * into dst, for OSi_ReloadRomData (src/OSi_ReloadRomData.c).
 *
 * The page-read command fetches 512-byte pages, so the transfer starts at the
 * page that holds src and drops the words that fall outside [0, len). The
 * control word is the one the boot loader left in the cartridge header copy
 * (HW_ROM_HEADER_BUF + 0x60), with its block-size field set to one page.
 */
void OSReadROMArea(u32 src, void *dst, int len) {
    vu8 *const reg_CARD_MASTERCNT = (vu8 *)(HW_REG_BASE + 0x1a1);
    vu8 *const reg_CARD_CMD = (vu8 *)(HW_REG_BASE + 0x1a8);
    vu32 *const reg_CARD_CNT = (vu32 *)(HW_REG_BASE + 0x1a4);
    vu32 *const reg_CARD_DATA = (vu32 *)(HW_REG_BASE + 0x100010);
    vu32 *const hdr_GAME_BUF = (vu32 *)(HW_ROM_HEADER_BUF + 0x60);

    enum {
        CARD_MASTER_SELECT_ROM = 0x00,
        CARD_MASTER_ENABLE = 0x80,
        CARD_CMD_READ_PAGE = 0xb7,
        CARD_CTRL_CMD_MASK = 0x07000000,
        CARD_CTRL_CMD_PAGE = 0x01000000,
        CARD_CTRL_READ = 0x00000000,
        CARD_CTRL_RESET_HI = 0x20000000,
        CARD_CTRL_START = 0x80000000,
        CARD_CTRL_READY = 0x00800000,
        CARD_ENUM_END
    };

    const u32 ctrl_start =
        (u32)((*hdr_GAME_BUF & ~CARD_CTRL_CMD_MASK) |
              (CARD_CTRL_CMD_PAGE | CARD_CTRL_READ | CARD_CTRL_START | CARD_CTRL_RESET_HI));

    int pos = -(int)(src & (512 - 1));

    while ((*reg_CARD_CNT & CARD_CTRL_START) != 0) {
    }
    *reg_CARD_MASTERCNT = (u32)(CARD_MASTER_SELECT_ROM | CARD_MASTER_ENABLE);

    for (src = (u32)(src + pos); pos < len; src += 512) {
        reg_CARD_CMD[0] = (u8)(CARD_CMD_READ_PAGE);
        reg_CARD_CMD[1] = (u8)(src >> 24);
        reg_CARD_CMD[2] = (u8)(src >> 16);
        reg_CARD_CMD[3] = (u8)(src >> 8);
        reg_CARD_CMD[4] = (u8)(src >> 0);
        reg_CARD_CMD[5] = (u8)(0);
        reg_CARD_CMD[6] = (u8)(0);
        reg_CARD_CMD[7] = (u8)(0);

        *reg_CARD_CNT = ctrl_start;
        for (;;) {
            u32 ctrl = *reg_CARD_CNT;

            if ((ctrl & CARD_CTRL_READY) != 0) {
                const u32 data = *reg_CARD_DATA;

                if ((pos >= 0) && (pos < len)) {
                    *(u32 *)((u32)dst + pos) = data;
                }

                pos += sizeof(u32);
            }

            if (!(ctrl & CARD_CTRL_START)) {
                break;
            }
        }
    }
}
