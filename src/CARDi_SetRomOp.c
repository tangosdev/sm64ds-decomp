/* NitroSDK CARDi_SetRomOp (card_rom.c): wait out the previous card command,
 * select ROM mode with the slot and its interrupt enabled, and load the two
 * command words into the 8-byte command buffer, most significant byte first. */
#include "nitro/hw/registers.h"

void CARDi_SetRomOp(u32 cmd1, u32 cmd2) {
    while ((REG_CARD_CNT & CARD_START) != 0) {
    }
    REG_CARD_MASTERCNT = CARDMST_SEL_ROM | CARDMST_ENABLE | CARDMST_IF_ENABLE;
    {
        vu8 *const p_cmd = &REG_CARD_CMD;
        p_cmd[0] = (u8)(cmd1 >> (8 * 3));
        p_cmd[1] = (u8)(cmd1 >> (8 * 2));
        p_cmd[2] = (u8)(cmd1 >> (8 * 1));
        p_cmd[3] = (u8)(cmd1 >> (8 * 0));
        p_cmd[4] = (u8)(cmd2 >> (8 * 3));
        p_cmd[5] = (u8)(cmd2 >> (8 * 2));
        p_cmd[6] = (u8)(cmd2 >> (8 * 1));
        p_cmd[7] = (u8)(cmd2 >> (8 * 0));
    }
}
