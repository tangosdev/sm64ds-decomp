extern unsigned char GetLinkByte(int slot, int byte);
/* Bit 4 of player's link settings byte 2: the rematch ready bit
   func_020201c0 sets. */
unsigned char IsPlayerReadyForRematch(int player) {
    return GetLinkByte(player, 2) & 0x10;
}
