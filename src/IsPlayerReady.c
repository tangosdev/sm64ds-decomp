extern unsigned char GetLinkByte(int slot, int byte);
/* Bit 3 of player's link settings byte 0: the ready bit
   func_02020334 sets. */
unsigned char IsPlayerReady(int player) {
    return GetLinkByte(player, 0) & 8;
}
