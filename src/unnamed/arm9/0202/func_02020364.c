extern unsigned char GetPlayerFlagByte(int player, int byte);
/* Bit 3 of player's link settings byte 0: the ready bit
   func_02020334 sets. */
unsigned char func_02020364(int player) {
    return GetPlayerFlagByte(player, 0) & 8;
}
